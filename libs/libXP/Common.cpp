/*
 * Copyright (c) 2002-2006 Samit Basu
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
#include "Common.hpp"
#include <QtCore>
using namespace std;

QStringList GetRecursiveDirList(QString basedir) {
  QStringList ret;
  QFileInfo fi(basedir);
  if ((fi.baseName().left(1) == QString("@")) ||
      (fi.baseName().toLower() == QString("private")))
    return ret;
  ret << basedir;
  QDir dir(basedir);
  dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
  QFileInfoList list = dir.entryInfoList();
  for (int i=0;i<((int)list.size());i++) {
    QFileInfo fileInfo = list.at(i);
    ret += GetRecursiveDirList(fileInfo.absoluteFilePath());
  }
  return ret;
}

StringVector GetCompletionList(QString pattern) {
  StringVector completions;
  QString Pattern(QDir::fromNativeSeparators(pattern));
  QDir dir(QDir::current());
  if (Pattern.count("/") > 0) { //Qt uses universal directory separator '/'
    // Completion includes a directory name...  Have to split it out
    QFileInfo t(Pattern);
    if (!dir.cd(t.path())) return StringVector();
    QFileInfoList list = dir.entryInfoList(QStringList() << (t.fileName() + "*"));
    for (int i=0;i<((int)list.size());i++) {
      QFileInfo fileInfo = list.at(i);
      if (fileInfo.isDir()) 
	if (!t.path().endsWith("/")) 
	    completions.push_back(QDir::toNativeSeparators(t.path() + "/" + fileInfo.fileName() + "/"));
	else
	    completions.push_back(QDir::toNativeSeparators(t.path() + fileInfo.fileName() + "/"));
      else
	if (!t.path().endsWith("/")) 
	    completions.push_back(QDir::toNativeSeparators(t.path() + "/" + fileInfo.fileName()));
	else	
	    completions.push_back(QDir::toNativeSeparators(t.path() + fileInfo.fileName()));
    }
  } else {
    QFileInfoList list = dir.entryInfoList(QStringList() << (pattern + "*"));
    for (int i=0;i<((int)list.size());i++) {
      QFileInfo fileInfo = list.at(i);
      if (fileInfo.isDir())
	  completions.push_back(QDir::toNativeSeparators(fileInfo.fileName() + "/"));
      else
	  completions.push_back(QDir::toNativeSeparators(fileInfo.fileName()));
    }
  }
  return completions;
}
