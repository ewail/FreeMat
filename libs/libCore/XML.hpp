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
#ifndef __XML_hpp__
#define __XML_hpp__

#include <QObject>
#include <QUrl>
#include <QFile>
#include <QThread>
#include <QDebug>
#include <QHttp>
#include <QEventLoop>

class URLRetriever : public QObject
{
  Q_OBJECT

  QUrl m_url;
  QFile *m_file;
  double m_timeout;
  int m_httpGetId;
  QEventLoop m_event;
  QHttp m_http;
  bool m_err;
public:
  URLRetriever(QUrl url, QFile *file, double timeout) :
    m_url(url), m_file(file), m_timeout(timeout), m_err(true) {}
  bool error() {return m_err;}
  void run();
public slots:
  void requestFinished(int id, bool err);
};

#endif
