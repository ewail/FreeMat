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

#include "Array.hpp"
#include "Interpreter.hpp"
#include "Utils.hpp"
#include <stdio.h>
#include "System.hpp"
#include <QtCore>
#include "Algorithms.hpp"

//@@Signature
//sfunction cd ChangeDirFunction
//inputs path
//outputs none
//DOCBLOCK os_cd
ArrayVector ChangeDirFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() != 1)
    throw Exception("cd function requires exactly one argument");
  eval->changeDir(TildeExpand(arg[0].asString()));
  return ArrayVector();
}

static void TabledOutput(StringVector sysresult, Interpreter* eval) {
  int maxlen = 0;
  // Find the maximal length
  for (int i=0;i<(int)sysresult.size();i++) {
    int ellen(sysresult[i].size());
    maxlen = (maxlen < ellen) ? ellen : maxlen;
  }
  // Calculate the number of columns that fit..
  int outcolumns;
  int termwidth = eval->getTerminalWidth()-1;
  outcolumns = termwidth/(maxlen+1);
  if (outcolumns < 1) outcolumns = 1;
  int colwidth = termwidth/outcolumns;
  int entryCount = 0;
  while (entryCount < (int)sysresult.size()) {
    char buffer[4096];
    sprintf(buffer,"%s",qPrintable(sysresult[entryCount]));
    int wlen;
    wlen = strlen(buffer);
    for (int j=wlen;j<colwidth;j++)
      buffer[j] = ' ';
    buffer[colwidth] = 0;
    eval->outputMessage(buffer);
    entryCount++;
    if (entryCount % outcolumns == 0)
      eval->outputMessage("\n");
  }
  eval->outputMessage("\n");
}

//@@Signature
//sfunction what WhatFunction
//inputs folder
//outputs none
//DOCBLOCK inspection_what
ArrayVector WhatFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  QDir pdir(QDir::current());
  if (arg.size() > 0)
    pdir.cd(arg[0].asString());
  QFileInfoList foo;
  foo = pdir.entryInfoList(QStringList() << "*.m" << "*.M");
  eval->outputMessage("\n");
  if (foo.size() > 0)
    {
      eval->outputMessage(QString("M-Files in directory ") + pdir.currentPath() + "\n\n");
      QStringList out;
      for (int i=0;i<foo.size();i++) out << foo[i].baseName();
      TabledOutput(out,eval);
      eval->outputMessage("\n");
    }
  foo = pdir.entryInfoList(QStringList() << "*.mat" << "*.MAT");
  if (foo.size() > 0)
    {
      eval->outputMessage(QString("MAT-Files in directory ") + pdir.currentPath() + "\n\n");
      QStringList out;
      for (int i=0;i<foo.size();i++) out << foo[i].baseName();
      TabledOutput(out,eval);
      eval->outputMessage("\n");
    }
  foo = pdir.entryInfoList(QStringList() << "@*");
  if (foo.size() > 0)
    {
      eval->outputMessage(QString("Classes in directory ") + pdir.currentPath() + "\n\n");
      QStringList out;
      for (int i=0;i<foo.size();i++)
	{
	  QString t = foo[i].baseName();
	  t = t.right(t.size()-1);
	  out << t;
	}
      TabledOutput(out,eval);
      eval->outputMessage("\n");
    }
  return ArrayVector();
}


//@@Signature
//sfunction dir DirFunction
//inputs name
//outputs array
//DOCBLOCK os_dir
ArrayVector DirFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  // Check for the case that the given name 
  QString dirarg;
  if (arg.size() > 0)
    dirarg = arg[0].asString();
  QDir pdir(QDir::current());
  QFileInfoList foo;
  if (pdir.cd(dirarg))
    foo = pdir.entryInfoList();
  else {
    if (dirarg.lastIndexOf(QDir::separator()) == -1)
      // it must be a pattern
      foo = pdir.entryInfoList(QStringList() << dirarg);
    else {
      // its not a pattern - its a mixed directory and pattern
      // combination.
      int path_length(dirarg.lastIndexOf(QDir::separator()));
      if (pdir.cd(dirarg.left(path_length+1)))
	foo = pdir.entryInfoList(QStringList() << dirarg.right(dirarg.size()-path_length-1));
    }
  }
  if (nargout == 0) {
    StringVector filelist;
    for (int i=0;i<foo.size();i++)
      filelist.push_back(foo[i].fileName());
    TabledOutput(filelist,eval);
  } else {
    // Output is a structure array
    StringVector fileNames;
    StringVector dates;
    ArrayVector bytes;
    ArrayVector isdirs;
    for (int i=0;i<foo.size();i++) {
      fileNames << foo[i].fileName();
      dates << foo[i].lastModified().toString();
      bytes << Array(double(foo[i].size()));
      isdirs << Array(bool(foo[i].isDir()));
    }
    return ArrayVector() << StructConstructor(StringVector() 
					      << "name"
					      << "date"
					      << "bytes"
					      << "isdir",
					      ArrayVector() 
					      << CellArrayFromStringVector(fileNames)
					      << CellArrayFromStringVector(dates)
					      << CellConstructor(bytes)
					      << CellConstructor(isdirs));
  }
  return ArrayVector();
}


//@@Signature
//sfunction ls ListFilesFunction
//inputs varargin
//outputs none
//DOCBLOCK os_ls
ArrayVector ListFilesFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  StringVector sysresult;
  QString buffer;
  int i;

#ifdef WIN32
  buffer = "dir ";
  for (i=0;i<arg.size();i++)
    buffer += arg[i].asString();
  sysresult = DoSystemCallCaptured(buffer);
  for (i=0;i<sysresult.size();i++) {
    eval->outputMessage(sysresult[i]);
    eval->outputMessage("\n");
  }
#else
  buffer = "ls ";
  for (i=0;i<arg.size();i++) {
    QString fipath(TildeExpand(arg[i].asString()));
    buffer += fipath;
  }
  sysresult = DoSystemCallCaptured(buffer);
  TabledOutput(sysresult,eval);
#endif
  return ArrayVector();
}

//@@Signature
//function dirsep DirSepFunction
//inputs none
//outputs y
//DOCBLOCK os_dirsep
ArrayVector DirSepFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector(Array(QString(QDir::separator())));
}

//@@Signature
//function pwd PrintWorkingDirectoryFunction
//inputs none
//outputs y
//DOCBLOCK os_pwd
ArrayVector PrintWorkingDirectoryFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector(Array(QDir::toNativeSeparators(QDir::currentPath())));
}

//@@Signature
//function fileattrib FileAttribFunction
//inputs filename attribset userset recursiveflag
//outputs attribs
//DOCBLOCK os_fileattrib

static Array FileAttrib(QString filename)
{
  StringVector fields;
  fields << "Name" << "archive" << "system" << "hidden" << 
    "directory" << "UserRead" << "UserWrite" << "UserExecute" <<
    "GroupRead" << "GroupWrite" << "GroupExecute" <<
    "OtherRead" << "OtherWrite" << "OtherExecute";
  ArrayVector data;
  QFileInfo finfo(filename);
  data << Array(finfo.absoluteFilePath());
  data << Array(double(0));
  data << Array(double(0));
  data << Array(double(finfo.isHidden() ? 1 : 0));
  data << Array(double(finfo.isDir() ? 1 : 0));
  QFile::Permissions perm = finfo.permissions();
  data << Array(double((perm & QFile::ReadUser) ? 1 : 0));
  data << Array(double((perm & QFile::WriteUser) ? 1 : 0));
  data << Array(double((perm & QFile::ExeUser) ? 1 : 0));
  data << Array(double((perm & QFile::ReadGroup) ? 1 : 0));
  data << Array(double((perm & QFile::WriteGroup) ? 1 : 0));
  data << Array(double((perm & QFile::ExeGroup) ? 1 : 0));
  data << Array(double((perm & QFile::ReadOther) ? 1 : 0));
  data << Array(double((perm & QFile::WriteOther) ? 1 : 0));
  data << Array(double((perm & QFile::ExeOther) ? 1 : 0));
  return StructConstructor(fields,data);
}

static ArrayVector FileAttribFunctionNoChange(int nargout, const ArrayVector& arg) {
  QString filename(arg[0].asString());
  QFileInfo fname(filename);
  if (fname.exists())
    return FileAttrib(fname.fileName());
  else {
    Array ret(Struct);
    QFileInfoList foo(QDir::current().entryInfoList(QStringList() << filename));
    for (int i=0;i<foo.size();i++)
      {
	QFileInfo fileInfo = foo.at(i);
	ret.set(i+1,FileAttrib(fileInfo.fileName()));
      }
    return ret;
  }  
}

static QFile::Permissions MapAttribToPermission(QChar type, QChar group)
{
  if (type == 'r' && group == 'o') return QFile::ReadOther;
  if (type == 'w' && group == 'o') return QFile::WriteOther;
  if (type == 'x' && group == 'o') return QFile::ExeOther;
  if (type == 'r' && group == 'u') return QFile::ReadUser | QFile::ReadOwner;
  if (type == 'w' && group == 'u') return QFile::WriteUser | QFile::WriteOwner;
  if (type == 'x' && group == 'u') return QFile::ExeUser | QFile::ExeOwner;
  if (type == 'r' && group == 'g') return QFile::ReadGroup;
  if (type == 'w' && group == 'g') return QFile::WriteGroup;
  if (type == 'x' && group == 'g') return QFile::ExeGroup;
  if (type == 'r' && group == 'a') 
    return QFile::ReadGroup | QFile::ReadUser | QFile::ReadOther | QFile::ReadOwner;
  if (type == 'w' && group == 'a') 
    return QFile::WriteGroup | QFile::WriteUser | QFile::WriteOther | QFile::WriteOwner;
  if (type == 'x' && group == 'a') 
    return QFile::ExeGroup | QFile::ExeUser | QFile::ExeOther | QFile::ExeOwner;
  return 0;
}

static ArrayVector FileAttribChange(QString filename, QString addset,
				    QString subset, QString userset) {
  QFile p(filename);
  QFile::Permissions perm = p.permissions();
  for (int i=0;i<addset.size();i++)
    for (int j=0;j<userset.size();j++)
      perm |= MapAttribToPermission(addset[i],userset[j]);
  for (int i=0;i<subset.size();i++)
    for (int j=0;j<userset.size();j++)
      perm &= ~MapAttribToPermission(subset[i],userset[j]);
  return ArrayVector(Array(double(p.setPermissions(perm))));
}

static ArrayVector FileAttribChangeDirRecursive(QString filename, QString addset, QString subset,
						QString userset) {
  QDir dir(filename);
  dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
  QFileInfoList list = dir.entryInfoList();
  for (int i=0;i<list.size();i++) {
    QFileInfo fileInfo = list.at(i);
    if (fileInfo.isDir())
      FileAttribChangeDirRecursive(fileInfo.absoluteFilePath(),addset,subset,userset);
    else
      FileAttribChange(fileInfo.absoluteFilePath(),addset,subset,userset);
  }
  return ArrayVector(Array(double(1)));
}

static ArrayVector FileAttribFunctionChange(int nargout, const ArrayVector& arg,
				     QString addset, QString subset, 
				     QString userset, bool recursive) {
  QString filename(arg[0].asString());
  QFileInfo fname(filename);
  if (fname.exists() && !recursive)
    return FileAttribChange(fname.fileName(),addset,subset,userset);
  if (fname.exists() && fname.isDir() && recursive)
    return FileAttribChangeDirRecursive(fname.fileName(),addset,subset,userset);
  QFileInfoList foo(QDir::current().entryInfoList(QStringList() << filename));
  for (int i=0;i<foo.size();i++)
    {
      QFileInfo fname(foo.at(i));
      if (fname.exists() && !recursive)
	FileAttribChange(fname.fileName(),addset,subset,userset);
      else if (fname.exists() && fname.isDir() && recursive)
	FileAttribChangeDirRecursive(fname.fileName(),addset,subset,userset);
    }
  return Array(double(1));
}

ArrayVector FileAttribFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0) return ArrayVector();
  if (arg.size() == 1) return FileAttribFunctionNoChange(nargout,arg);
  // Get the set of attribute changes
  QString attributeSet = arg[1].asString();
  // Parse the attribute set
  QChar p;
  QString addset;
  QString subset;
  int state = 0;
  for (int i=0;i<attributeSet.size();i++) {
    p = attributeSet[i];
    if (!p.isSpace())
      {
	if ((p == '+') && (state == 0)) {state = 1;}
	else if ((p == '-') && (state == 0)) {state = 2;}
	else if ((p == 'w') && (state == 1)) {addset += p; state = 0;}
	else if ((p == 'r') && (state == 1)) {addset += p; state = 0;}
	else if ((p == 'x') && (state == 1)) {addset += p; state = 0;}
	else if ((p == 'w') && (state == 2)) {subset += p; state = 0;}
	else if ((p == 'r') && (state == 2)) {subset += p; state = 0;}
	else if ((p == 'x') && (state == 2)) {subset += p; state = 0;}
	else
	  throw Exception("Malformed list of attribute changes:" + attributeSet);
      }
  }
  if (state != 0)
    throw Exception("Malformed list of attribute changes:" + attributeSet);
  qDebug() << "addset: " << addset;
  qDebug() << "subset: " << subset;
  QString userset;
  if (arg.size() >= 3)
    {
      QString userlist = arg[2].asString();
      for (int i=0;i<userlist.size();i++) {
	p = userlist[i];
	if (!p.isSpace())
	  {
	    if (p == 'u') userset += p;
	    else if (p == 'o') userset += p;
	    else if (p == 'g') userset += p;
	    else if (p == 'a') userset += p;
	    else
	      throw Exception("Malformed list of users:" + userlist);
	  }
      }
      if (userset.size() == 0) userset += 'u';
    }
  else
    userset += 'u';
  qDebug() << "userset: " << userset;
  bool recursive = false;
  if (arg.size() >= 4)
    {
      QString sflag = arg[3].asString();
      if (sflag == "s") recursive = true;
      else if (sflag == "") recursive = false;
      else
	throw Exception("Last argument must be either 's' or ''");
    }
  qDebug() << "recursive: " << recursive;
  return FileAttribFunctionChange(nargout,arg,addset,subset,userset,recursive);
}


//@@Signature
//function rmdir RMDirFunction
//inputs dirname flag
//outputs none
//DOCBLOCK os_rmdir
inline void RemoveDirectory(QString dirname) {
  QDir::current().rmdir(dirname);
}

inline void RemoveDirectoryRecursive(QString dirname) {
  QDir dir(dirname);
  dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
  QFileInfoList list = dir.entryInfoList();
  for (int i=0;i<list.size();i++) {
    QFileInfo fileInfo = list.at(i);
    if (fileInfo.isDir())
      RemoveDirectoryRecursive(fileInfo.absoluteFilePath());
    else
      dir.remove(fileInfo.absoluteFilePath());
  }
  QDir::current().rmdir(dirname);
}

ArrayVector RMDirFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    throw Exception("rmdir requires at least one argument (the directory to remove)");
  if (arg.size() == 1)
    RemoveDirectory(arg[0].asString());
  else if (arg.size() == 2) {
    QString arg1 = arg[1].asString();
    if ((arg1 == "s") || (arg1 == "S"))
      RemoveDirectoryRecursive(arg[0].asString());
    else
      throw Exception("rmdir second argment must be a 's' to do a recursive delete");
  }
  return ArrayVector();
}

// See mkdir.m for documentation
//@@Signature
//function mkdir_core MKDirCoreFunction
//inputs dir
//outputs flag
ArrayVector MKDirCoreFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    throw Exception("mkdir requires at least one argument (the directory to create)");
  if (QDir::current().mkpath(arg[0].asString()))
    return ArrayVector(Array(bool(true)));
  else
    return ArrayVector(Array(bool(false)));
}

//@@Signature
//function fileparts FilePartsFunction
//inputs filename
//outputs path name extension version
//DOCBLOCK os_fileparts
ArrayVector FilePartsFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    throw Exception("fileparts requires a filename");
  QFileInfo fi(arg[0].asString());
  Array path(fi.path());
  Array name(fi.completeBaseName());
  Array suffix;
  if (fi.suffix().size() > 0)
    suffix = Array("." + fi.suffix());
  else
    suffix = Array(QString(""));
  return ArrayVector() << path
		       << name
		       << suffix
		       << Array(StringArray);
}

//@@Signature
//function delete DeleteFunction
//inputs filename
//outputs none
//DOCBLOCK os_delete
ArrayVector DeleteFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    throw Exception("delete requires at least one argument");
  QString filename(arg[0].asString());
  QFileInfo fname(filename);
  if (fname.exists())
    fname.dir().remove(fname.fileName());
  else {
    QFileInfoList foo(QDir::current().entryInfoList(QStringList() << filename));
    for (int i=0;i<foo.size();i++)
      foo[i].dir().remove(foo[i].fileName());
  }
  return ArrayVector();
}

//@@Signature
//function copyfile CopyFileFunction
//inputs pattern directory_out forceflag
//outputs none
//DOCBLOCK os_copyfile
static void CopyFile(QString source, QString dest, bool override) {
  QFileInfo destInfo(dest);
  if (destInfo.exists() && (override || destInfo.isWritable()))
    QFile::remove(dest);
  QFile::copy(source,dest);
}

static void CopyDirectoryRecursive(QString srcdir, QString destdir, bool override) {
  QDir dir(srcdir);
  dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
  QFileInfoList list = dir.entryInfoList();
  for (int i=0;i<list.size();i++) {
    QFileInfo fileInfo = list.at(i);
    if (fileInfo.isDir())
      CopyDirectoryRecursive(fileInfo.absoluteFilePath(),
			     destdir+QDir::separator()+fileInfo.fileName(),
			     override);
    else 
      if (QDir::current().mkpath(destdir))
	CopyFile(fileInfo.absoluteFilePath(),
		 destdir+QDir::separator()+fileInfo.fileName(),override);
  }
}

ArrayVector CopyFileFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 2)
    throw Exception("copyfile requires two arguments - source and destination");
  bool override = ((arg.size() == 3) && (arg[2].asString().toUpper() == "F"));
  QString source(arg[0].asString());
  QString dest(arg[1].asString());
  QFileInfo source_info(source);
  QFileInfo dest_info(dest);
  // Case 1 - source is a file, and dest is a file
  if (source_info.isFile() && !dest_info.isDir()) {
    // Simple copy of files
    CopyFile(source,dest,override);
  } else if (source_info.isFile() && dest_info.isDir()) {
    CopyFile(source,dest + QDir::separator() + source_info.fileName(),override);
  } else if (source_info.isDir() && dest_info.isFile()) {
    throw Exception("cannot copy a directory to a file");
  } else if (source_info.isDir() && (dest_info.isDir() || !dest_info.exists())) {
    // Copy the contents of a directory 
    CopyDirectoryRecursive(source,dest,override);
  } else if ((source.count("*") > 0) && (dest_info.isDir() || !dest_info.exists())) {
    // Try and get to the directory
    QDir sourcedir(source_info.path());
    if (!sourcedir.exists()) 
      return ArrayVector();
    sourcedir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    sourcedir.setNameFilters(QStringList() << source_info.fileName());
    QFileInfoList list = sourcedir.entryInfoList();
    for (int i=0;i<list.size();i++) {
      QFileInfo fileInfo = list.at(i);
      if (fileInfo.isDir())
	CopyDirectoryRecursive(fileInfo.absoluteFilePath(),
			       dest+QDir::separator()+fileInfo.fileName(),
			       override);
      else 
	if (QDir::current().mkpath(dest))
	  CopyFile(fileInfo.absoluteFilePath(),dest+QDir::separator()+fileInfo.fileName(),override);
    }
  } else
    throw Exception("unhandled case of copyfile function");
  return ArrayVector();
}
