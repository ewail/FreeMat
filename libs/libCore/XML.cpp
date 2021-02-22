/*
 * Copyright (c) 2002-2007 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include <QtCore>
#include <QtGui>
#include <QtXml>
#include <QtNetwork>

#include "Array.hpp"
#include "XML.hpp"
#include "Algorithms.hpp"

//@@Signature
//function xmlread XMLReadFunction
//inputs filename
//outputs p
//DOCBLOCK io_xmlread

//@@Signature
//function htmlread HTMLReadFunction
//inputs filename
//outputs p
//DOCBLOCK io_htmlread

// private (recursively called) function to convert a QDomElement into
// a FreeMat Array.  The structure of a node is a struct with:
//    Name: nodeName
//    Attributes: cell array of attributes, each is a struct with Name and Value pairs
//    Data: Not sure yet
//    Children: cell array of nodes

static Array cellScalar(Array t) {
  ArrayMatrix m;
  ArrayVector n;
  n << t;
  m << n;
  return CellConstructor(m);
}

static Array QDomElementToArray(QDomElement elem);

static Array QDomNodeToArray(QDomNode p) {
  if (p.nodeType() == QDomNode::ElementNode)
    return QDomElementToArray(p.toElement());
  StringVector fnames;
  fnames << "Name" << "Value";
  ArrayVector vals;
  vals << Array(p.nodeName());
  vals << Array(p.nodeValue());
  return StructConstructor(fnames,vals);
}

static Array QDomNamedNodeMapToArray(QDomNamedNodeMap attr) {
  ArrayVector nodes;
  for (size_t i=0;i<attr.length();i++)
    nodes << QDomNodeToArray(attr.item(i));
  return CellConstructor(ArrayMatrix() << nodes);
}

static Array QDomElementToArray(QDomElement elem) {
  StringVector fnames;
  fnames << "Name" << "Attributes" << "Data" << "Children";
  ArrayVector vals;
  vals << Array(elem.tagName());
  vals << cellScalar(QDomNamedNodeMapToArray(elem.attributes()));
  vals << EmptyConstructor();
  ArrayVector children;
  QDomNode n = elem.firstChild();
  while (!n.isNull()) {
    children << QDomNodeToArray(n);
    n = n.nextSibling();
  }
  vals << cellScalar(CellConstructor(ArrayMatrix() << children));
  return StructConstructor(fnames,vals);
}

ArrayVector XMLReadFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1) 
    throw Exception("xmlread requires at least one argument (the filename)");
  // Parse the thing
  QDomDocument doc("fmdoc");
  QString filename(arg[0].asString());
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly))
    return ArrayVector();
  QString errMsg;
  int errLine, errColumn;
  if (!doc.setContent(&file,true,&errMsg,&errLine,&errColumn)) {
    file.close();
    throw Exception(QString("error parsing ") + filename + ":" + errMsg + " Line: " + errLine + " Column: " + errColumn);
  }
  file.close();
  //  Walk the tree
  QDomElement docElem = doc.documentElement();
  return ArrayVector() << QDomElementToArray(docElem);
}

ArrayVector HTMLReadFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1) 
    throw Exception("htmlread requires at least one argument (the filename)");
  // Read the proposed filename
  QString filename(arg[0].asString());
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly))
    return ArrayVector();
  QString fileData;
  QTextStream tStream(&file);
  fileData = tStream.readAll();
  file.close();
  // Tricky...   
  // Parse the thing
  QTextDocument *tdoc = new QTextDocument;
  tdoc->setHtml(fileData);
  QString xhtml(tdoc->toHtml());

  QFile file2("xhtml.xml");
  file2.open(QIODevice::WriteOnly);
  QTextStream tStream2(&file2);
  tStream2 << xhtml;
  file2.close();

  delete tdoc;
  QDomDocument doc("fmdoc");
  QString errMsg;
  int errLine, errColumn;
  if (!doc.setContent(xhtml,true,&errMsg,&errLine,&errColumn)) {
    throw Exception(QString("error parsing ") + filename + ":" + errMsg + " Line: " + errLine + " Column: " + errColumn);
  }
  //  Walk the tree
  QDomElement docElem = doc.documentElement();
  return ArrayVector() << QDomElementToArray(docElem);
}

//@@Signature
//function urlwrite URLWriteFunction
//inputs url filename timeout
//outouts filename
//DOCBLOCK io_urlwrite

void URLRetriever::requestFinished(int id, bool err) {
  if (id != m_httpGetId)
    return;
  m_err = err;
}

void URLRetriever::run()
{
  m_http.setHost(m_url.host(), m_url.port() != -1 ? m_url.port() : 80);
  if (!m_url.userName().isEmpty())
    m_http.setUser(m_url.userName(), m_url.password());
  m_httpGetId = m_http.get(m_url.path(), m_file);
  QTimer::singleShot((int)m_timeout, &m_http, SLOT(abort()));
  connect(&m_http, SIGNAL(done(bool)), &m_event, SLOT(quit()));
  connect(&m_http, SIGNAL(requestFinished(int, bool)), this, SLOT(requestFinished(int, bool)));
  m_event.exec();
}

ArrayVector URLWriteFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 3) throw Exception("urlwrite requires 3 arguments: url, filename, timeout");
  QUrl url(arg[0].asString());
  if (!url.isValid()) throw Exception(QString("url is not valid:") + arg[0].asString());
  QString filename(arg[0].asString());
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly))
    throw Exception(QString("unable to open output file ") + filename + " for writing");
  double timeout(arg[2].asDouble());
  URLRetriever p_url(url,&file,timeout);
  p_url.run();
  if (p_url.error())
    throw Exception(QString("error retrieving url:") + arg[0].asString());
  return ArrayVector() << Array(filename);
}
