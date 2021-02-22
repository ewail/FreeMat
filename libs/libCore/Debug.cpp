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


//@@Signature
//sfunction dbdelete DbDeleteFunction
//inputs num
//outputs none
//DOCBLOCK debug_dbdelete
ArrayVector DbDeleteFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() < 1)
    throw Exception("dbdelete requires an argument (number of breakpoint to delete)");
  eval->deleteBreakpoint(arg[0].asInteger());
  return ArrayVector();
}

//@@Signature
//sfunction dblist DbListFunction
//inputs none
//outputs none
//DOCBLOCK debug_dblist
ArrayVector DbListFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  eval->listBreakpoints();
  return ArrayVector();
}

//DOCBLOCK debug_dbstep

//@@Signature
//sfunction dbstop DbStopFunction
//inputs funcname linenumber
//outputs none
//DOCBLOCK debug_dbstop
ArrayVector DbStopFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() < 2)
    throw Exception("dbstop function requires at least two arguments");
  if (!(arg[0].isString()))
    throw Exception("first argument to dbstop must be the name of routine where to stop");
  eval->addBreakpoint(arg[0].asString(),arg[1].asInteger());
  return ArrayVector();
}

//@@Signature
//sfunction fdump FdumpFunction
//inputs fname
//outputs none
//DOCBLOCK debug_fdump
ArrayVector FdumpFunction(int nargout, const ArrayVector& arg,Interpreter* eval){
  if (arg.size() == 0)
    throw Exception("fdump function requires at least one argument");
  if (!(arg[0].isString()))
    throw Exception("first argument to fdump must be the name of a function (i.e., a string)");
  QString fname = arg[0].asString();
  Context *context = eval->getContext();
  FuncPtr funcDef;
  if (!context->lookupFunction(fname,funcDef))
    throw Exception(QString("function ") + fname + " undefined!");
  funcDef->updateCode(eval);
  funcDef->printMe(eval);
  return ArrayVector();
}
