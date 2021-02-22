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
#include "PathSearch.hpp"
#include <QtCore>

static int ExistBuiltinFunction(QString fname, Interpreter* eval) {    
  bool isDefed;
  FuncPtr d;
  isDefed = eval->getContext()->lookupFunction(fname,d);
  if (isDefed && ((d->type() == FM_BUILT_IN_FUNCTION) ||
		  (d->type() == FM_SPECIAL_FUNCTION)))
    return 5;
  else
    return 0;
  return 0;
}

static int ExistDirFunction(QString fname, Interpreter* eval) {
  // Check for extra termination
  int flen = fname.size();
  if ((fname[flen-1] == '/') ||
      (fname[flen-1] == '\\'))
    fname[flen-1] = 0;
  QFileInfo filestat(fname);
  if (!filestat.exists()) return 0;
  if (filestat.isDir()) return 7;
  return 0;
}

static int ExistFileFunction(QString fname, Interpreter* eval) {
  PathSearcher src(eval->getPath());
  QString path = src.ResolvePath(fname);
  if( !path.isNull() )
	return 2;

  bool isDefed;
  FuncPtr d;
  isDefed = eval->getContext()->lookupFunction(fname,d);
  if (isDefed && (d->type() == FM_M_FUNCTION))
    return 2;
  return 0;
}

static int ExistVariableFunction(QString fname, Interpreter* eval) {
  ParentScopeLocker lock(eval->getContext());
  bool isDefed = (eval->getContext()->lookupVariable(fname).valid());
  if (isDefed)
    return 1;
  else
    return 0;
}

static int ExistAllFunction(QString fname, Interpreter* eval) {
  int ret;
  ret = ExistVariableFunction(fname,eval);
  if (ret) return ret;
  ret = ExistFileFunction(fname,eval);
  if (ret) return ret;
  ret = ExistDirFunction(fname,eval);
  if (ret) return ret;
  ret = ExistBuiltinFunction(fname,eval);
  if (ret) return ret;
  return 0;
}

//@@Signature
//sfunction exist ExistFunction
//inputs item kind
//outputs y
//DOCBLOCK inspection_exist
ArrayVector ExistFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() < 1)
    throw Exception("exist function takes at least one argument - the name of the object to check for");
  QString fname = arg[0].asString();
  QString stype;
  if (arg.size() > 1) {
    stype = arg[1].asString();
  } else {
    stype = "all";
  }
  int retval;
  if (stype=="all")
    retval = ExistAllFunction(fname,eval);
  else if (stype=="builtin")
    retval = ExistBuiltinFunction(fname,eval);
  else if (stype=="dir")
    retval = ExistDirFunction(fname,eval);
  else if (stype=="file")
    retval = ExistFileFunction(fname,eval);
  else if (stype=="var")
    retval = ExistVariableFunction(fname,eval);
  else throw Exception("Unrecognized search type for function 'exist'");
  return ArrayVector(Array(double(retval)));
}
