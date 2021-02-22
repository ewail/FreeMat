/*
 * Copyright (c) 2009 Samit Basu
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

#include "Interpreter.hpp"
#include "Array.hpp"
#include "helpwidget.hpp"
#include <QtCore>
#include <QApplication>
#include "PathSearch.hpp"
#include "MainApp.hpp"

extern MainApp *m_app;
bool inBundleMode();
QString GetRootPath();


//@@Signature
//sfunction help HelpFunction
//inputs function
//outputs none
//DOCBLOCK freemat_help
ArrayVector HelpFunction(int nargout, const ArrayVector& arg, Interpreter* eval)
{
  PathSearcher psearch(GetRootPath()+"/help/text");

  if (arg.size() != 1)
    throw Exception("help function requires a single argument (the function or script name)");
  QString fname = arg[0].asString();
  bool mdcexists = !(psearch.ResolvePath(fname+".mdc").isNull());
  bool isFun;
  FuncPtr val;
  isFun = eval->getContext()->lookupFunction(fname,val);
  if (isFun && (val->type() == FM_M_FUNCTION) && !mdcexists) {
    MFunctionDef *mptr;
    mptr = (MFunctionDef *) val;
    mptr->updateCode(eval);
    for (int i=0;i<(int)mptr->helpText.size();i++)
      eval->outputMessage(mptr->helpText[i]);
    return ArrayVector();
  } else {
    // Check for a mdc file with the given name
    QString mdcname = fname + ".mdc";
    mdcname = psearch.ResolvePath(mdcname);
    if( mdcname.isNull() )
      throw Exception("no help available on " + fname);
    
    QFile fp(mdcname);
    if (!fp.open(QIODevice::ReadOnly))
      throw Exception(QString("No help available on ") + fname);
    QTextStream io(&fp);
    while (!io.atEnd()) {
      QString cp = io.readLine();
      eval->outputMessage("\n       ");
      eval->outputMessage(cp);
    }
    return ArrayVector();
  }
  throw Exception("no help for that topic");
}

static HelpWindow *m_helpwin=0;

//@@Signature
//sgfunction helpwin HelpWinFunction
//inputs functionname
//outputs none
//DOCBLOCK freemat_helpwin
ArrayVector HelpWinFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {  
  QDir dir(GetRootPath()+"/help/html");
  if (!m_helpwin){
    m_helpwin = new HelpWindow(dir.canonicalPath());
    QObject::connect(m_helpwin,SIGNAL(EvaluateText(QString)),m_app->GetKeyManager(),SLOT(QueueMultiString(QString)));
  }
  if (arg.size() == 0) {
    m_helpwin->show();
    m_helpwin->raise();
  }
  else if (arg.size() == 1) {
    QString fulltext = arg[0].asString();
    m_helpwin->helpText(fulltext);
    m_helpwin->show();
    m_helpwin->raise();
  }
  else
    throw Exception("helpwin function accepts at most 1 argument.");
  return ArrayVector();
}
