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
#include "AnonFunc.hpp"
#include "Algorithms.hpp"
#include "Struct.hpp"
#include "Parser.hpp"

// Merge this file with FuncPtr.cpp

#define LOOKUP(x,field) x.constStructPtr()[field].get(1)


Array AnonFuncConstructor(Interpreter* eval, const Tree & t) {
  StringVector variables;
  VariableReferencesList(t.second(),variables);
  StringVector arguments = IdentifierList(t.first());
  ArrayVector vals;
  StringVector fields;
  for (int i=0;i<variables.size();i++) {
    ArrayReference ptr(eval->getContext()->lookupVariable(variables[i]));
    if (ptr.valid()) {
      fields << variables[i];
      vals << *ptr;
    }
    //    } else if (!arguments.contains(variables[i]))
    //      throw Exception("variable " + variables[i] + " which is not an argument, must be defined when the anonymous function is declared.");
  }
  Array workspace(StructConstructor(fields,vals));
  Array args(CellArrayFromArray(CellArrayFromStringVector(arguments)));
  Array expr(Array(QString(t.text())));
  StringVector fieldnames;
  fieldnames << "args" << "workspace" << "expr";
  ArrayVector values;
  values << args << workspace << expr;
  Array ret(StructConstructor(fieldnames,values));
  ret.structPtr().setClassName("anonfunction");
  return ret;
}

QString AnonFuncToString(const Array &a)
{
  QString ret = " @(";
  StringVector argnames(StringVectorFromArray(LOOKUP(a,"args")));
  if (argnames.size() > 0) 
    ret += argnames[0];
  for (int j=1;j<argnames.size();j++)
    ret += "," + argnames[j];
  ret += ")  " + LOOKUP(a,"expr").asString();
  return ret;
}

//@@Signature
//sfunction @anonfunction:display AnonFuncDispFunction
//input x
//output none
ArrayVector AnonFuncDispFunction(int nargout, const ArrayVector& arg,
				 Interpreter *eval) {
  for (int i=0;i<arg.size();i++) {
    eval->outputMessage(AnonFuncToString(arg[i]));
    eval->outputMessage("\n");
  }
  return ArrayVector();
}

//@@Signature
//sfunction @anonfunction:subsref AnonFuncSubsrefFunction
//input x s
//output varargout
ArrayVector AnonFuncSubsrefFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  // We should be in our own scope currently...
  // Insert the variables from the workspace into the context
  const Array &var(LOOKUP(arg[0],"workspace"));
  const StructArray &sp(var.constStructPtr());
  Context *context = eval->getContext();
  // Borrow the callers scope name and detail... hope this doesn't
  // have unintended side effects... err...
  QString scopeName;
  QString scopeDetail;
  {
    ParentScopeLocker lock(context);
    scopeName = context->scopeName();
    scopeDetail = context->scopeDetailString();
  }
  context->pushScope(scopeName);
  context->setScopeDetailString(scopeDetail);
  for (int i=0;i<sp.fieldCount();i++)
    context->insertVariableLocally(sp.fieldName(i),sp[i].get(1));
  // Retrieve the arguments...
  ArrayVector argSet;
  if (arg.size() == 2) {
    if (LOOKUP(arg[1],"type").asString() != "()")
      throw Exception("for anonymous functions, only a(x,...,y) is defined");
    const Array &sub(LOOKUP(arg[1],"subs"));
    const BasicArray<Array> &rp(sub.constReal<Array>());
    for (index_t i=1;i<=rp.length();i++)
      argSet.push_back(rp[i]);
  }
  // Assign the arguments to internal variables.
  StringVector args(StringVectorFromArray(LOOKUP(arg[0],"args")));
  for (int i=0;i<qMin(argSet.size(),args.size());i++) 
    context->insertVariableLocally(args[i],argSet[i]);
  // We need to build a parse tree
  const Tree & expTree(ParseExpressionString(LOOKUP(arg[0],"expr").asString()));
  // Excecute the tree
  ArrayVector outputs;
  try {
    eval->multiexpr(expTree,outputs,1,true);
  } catch (InterpreterBreakException& ) {
  } catch (InterpreterContinueException& ) {
  } catch (InterpreterReturnException& ) {
  }
  context->popScope();
  return outputs;
}


//@@Signature
//sfunction @anonfunction:feval AnonFuncFevalFunction
//input x varargin
//output varargout
ArrayVector AnonFuncFevalFunction(int nargout, const ArrayVector& arg, Interpreter *eval) {
  if (arg.size() == 0) return ArrayVector();
  const Array &var(LOOKUP(arg[0],"workspace"));
  const StructArray &sp(var.constStructPtr());
  Context *context = eval->getContext();
  // Borrow the callers scope name and detail... hope this doesn't
  // have unintended side effects... err...
  QString scopeName;
  QString scopeDetail;
  {
    ParentScopeLocker lock(context);
    scopeName = context->scopeName();
    scopeDetail = context->scopeDetailString();
  }
  context->pushScope(scopeName);
  context->setScopeDetailString(scopeDetail);
  for (int i=0;i<sp.fieldCount();i++)
    context->insertVariableLocally(sp.fieldName(i),sp[i].get(1));
  // Assign the arguments to internal variables.
  StringVector argnames(StringVectorFromArray(LOOKUP(arg[0],"args")));
  for (int i=0;i<qMin(arg.size()-1,argnames.size());i++) 
    context->insertVariableLocally(argnames[i],arg[i+1]);
  // We need to build a parse tree
  const Tree & expTree(ParseExpressionString(LOOKUP(arg[0],"expr").asString()));
  // Excecute the tree
  ArrayVector outputs;
  try {
    eval->multiexpr(expTree,outputs);
  } catch (InterpreterBreakException& e) {
  } catch (InterpreterContinueException& e) {
  } catch (InterpreterReturnException& e) {
  }
  context->popScope();
  return outputs;
}

