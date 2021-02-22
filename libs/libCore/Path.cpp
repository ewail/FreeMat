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

#include "Array.hpp"
#include "Interpreter.hpp"
#include "PathTool.hpp"
#include <QSettings>

//@@Signature
//sfunction getpath GetPathFunction
//inputs none
//outputs y
//DOCBLOCK os_getpath
ArrayVector GetPathFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  return ArrayVector(Array(eval->getPath()));
}

//@@Signature
//sfunction setpath SetPathFunction
//inputs y
//outputs none
//DOCBLOCK os_setpath
ArrayVector SetPathFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() != 1)
    throw Exception("setpath function requires exactly one string argument");
  QSettings settings("FreeMat",Interpreter::getVersionString());
  settings.setValue("interpreter/path",arg[0].asString());
  settings.sync();
  eval->setPath(arg[0].asString());
  eval->rescanPath();
  return ArrayVector();
}

//@@Signature
//sgfunction pathtool PathToolFunction
//inputs none
//outputs none
//DOCBLOCK freemat_pathtool
ArrayVector PathToolFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  PathTool p;
  p.exec();
  QSettings settings("FreeMat",Interpreter::getVersionString());
  QStringList userPath = settings.value("interpreter/path").toStringList();
  eval->setUserPath(userPath);
  eval->rescanPath();
  return ArrayVector();
}
