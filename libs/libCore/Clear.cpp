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
#include "Module.hpp"
#include "Class.hpp"

//@@Signature
//sfunction clear ClearFunction
//inputs varargin
//outputs none
//DOCBLOCK inspection_clear

static void ClearVariable(Interpreter* eval, QString name) {
  Context *context = eval->getContext();
  ParentScopeLocker lock(context);
  context->deleteVariable(name);
}

static void ClearAllFunction(Interpreter* eval) {
  ClearLibs(eval);
  Context *context = eval->getContext();
  StringVector names;
  {
    ParentScopeLocker lock(context);
    names = context->listAllVariables();
  }
  for (int i=0;i<(int)names.size();i++)
    ClearVariable(eval,names[i]);
}

static void ClearPersistent(Interpreter* eval) {
  ParentScopeLocker lock(eval->getContext());
  StringVector names = eval->getContext()->listGlobalVariables();
  for (int i=0;i<(int)names.size();i++) {
    if ((names[i].size() >= 1) && (names[i][0] == '_'))
      eval->getContext()->deleteGlobalVariable(names[i]);
  }
  eval->getContext()->clearPersistentVariableList();
}

static void ClearGlobal(Interpreter* eval) {
  ParentScopeLocker lock(eval->getContext());
  StringVector names = eval->getContext()->listGlobalVariables();
  for (int i=0;i<(int)names.size();i++) {
    if ((names[i].size() >= 1) && (names[i][0] != '_')) {
      eval->getContext()->deleteGlobalVariable(names[i]);
    }
  }
  eval->getContext()->clearGlobalVariableList();
}

ArrayVector ClearFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  StringVector names;
  if (arg.size() == 0) 
    names.push_back("all");
  else
    for (int i=0;i<arg.size();i++) 
      names.push_back(arg[i].asString());
  for (int i=0;i<(int)names.size();i++) {
    if (names[i] == "all")
      ClearAllFunction(eval);
    else if (names[i] == "libs")
      ClearLibs(eval);
    else if (names[i] == "persistent")
      ClearPersistent(eval);
    else if (names[i] == "global")
      ClearGlobal(eval);
    else if (names[i] == "classes")
      clearUserClasses();
    else 
      ClearVariable(eval,names[i]);
  }
  return ArrayVector();
}
