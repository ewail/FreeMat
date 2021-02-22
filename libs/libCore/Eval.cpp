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
#include "Algorithms.hpp"

//@@Signature
//sfunction eval EvalFunction
//inputs try_clause catch_clause
//outputs varargout
//DOCBLOCK freemat_eval
static QString PrePendCallVars(QString line, int nargout) {
  QString gp;
  if (nargout > 1)
    gp += "[";
  for (int i=0;i<nargout-1;i++)
    gp += QString("t___%1,").arg(i);
  gp += QString("t___%1").arg(nargout-1);
  if (nargout > 1)
    gp += "] = " + line + ";\n";
  else
    gp += " = " + line + ";\n";
  return gp;
}

static ArrayVector RetrieveCallVars(Interpreter *eval, int nargout) {
  ArrayVector retval;
  for (int i=0;i<nargout;i++) {
    QString tname = QString("t___%1").arg(i);
    Array tval;
    ArrayReference ptr = eval->getContext()->lookupVariable(tname);
    if (!ptr.valid())
      tval = EmptyConstructor();
    else
      tval = *ptr;
    eval->getContext()->deleteVariable(tname);
    retval.push_back(tval);
  }
  return retval;
}


static ArrayVector EvalTryFunction(int nargout, Interpreter* eval, QString try_buf, 
				   QString catch_buf, bool retrieveVars, int popSpec) {
  ArrayVector retval;
  bool autostop;
  autostop = eval->AutoStop();
  eval->setAutoStop(false);
  bool save_trycatch_flag(eval->getTryCatchActive());
  eval->setTryCatchActive(true);
  Context *context = eval->getContext();
  PopContext saver(context,popSpec);
  int eval_depth = context->scopeDepth();
  try {
    eval->evaluateString(try_buf,true);
    if (retrieveVars)
      retval = RetrieveCallVars(eval,nargout);
  } catch (Exception &e) {
    while (context->scopeDepth() < eval_depth) context->restoreScope(1);
    while (context->scopeDepth() > eval_depth) context->popScope();
    eval->evaluateString(catch_buf,false);
    if (retrieveVars)
      retval = RetrieveCallVars(eval,nargout);
  }
  eval->setTryCatchActive(save_trycatch_flag);
  eval->setAutoStop(autostop);
  return retval;
}

static ArrayVector EvalTryFunction(int nargout, const ArrayVector& arg, Interpreter* eval, int popSpec) {
  if (nargout > 0) {
    QString try_line = arg[0].asString();
    QString try_buf = PrePendCallVars(try_line,nargout);
    QString catch_line = arg[1].asString();
    QString catch_buf = PrePendCallVars(catch_line,nargout);
    return EvalTryFunction(nargout,eval,try_buf,catch_buf,true,popSpec);
   } else {
    QString try_line = arg[0].asString();
    QString catch_line = arg[1].asString();
    QString try_buf = try_line + "\n";
    QString catch_buf = catch_line + "\n";
    return EvalTryFunction(nargout,eval,try_buf,catch_buf,false,popSpec);
  }
}

ArrayVector TraceFunction(int nargout, const ArrayVector& arg, Interpreter* eval);

static ArrayVector EvalNoTryFunction(int nargout, const ArrayVector& arg, Interpreter* eval, int popSpec) {
  if (nargout > 0) {
    QString line = arg[0].asString();
    QString buf = PrePendCallVars(line,nargout);
    PopContext saver(eval->getContext(),popSpec);
    eval->evaluateString(buf);
    return RetrieveCallVars(eval,nargout);
  } else {
    QString line = arg[0].asString();
    QString buf = line + "\n";
    PopContext saver(eval->getContext(),popSpec);
    eval->evaluateString(buf);
    return ArrayVector();
  }
}

ArrayVector EvalFunction(int nargout, const ArrayVector& arg,Interpreter* eval){
  eval->getContext()->deactivateCurrentScope(); // Make us invisible
  if (arg.size() == 0)
    throw Exception("eval function takes at least one argument - the string to execute");
  if (arg.size() == 2)
    return EvalTryFunction(nargout, arg, eval, 0);
  return EvalNoTryFunction(nargout, arg, eval, 0);
}

//@@Signature
//sfunction evalin EvalInFunction
//inputs varargin
//outputs x y z
//DOCBLOCK freemat_evalin
ArrayVector EvalInFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() < 2)
    throw Exception("evalin function requires a workspace (scope) specifier (either 'caller' or 'base') and an expression to evaluate");
  QString spec_str = arg[0].asString();
  int popspec = 0;
  if (spec_str=="base")
    popspec = -1;
  else if (spec_str=="caller")
    popspec = 2;
  else
    throw Exception("evalin function requires the first argument to be either 'caller' or 'base'");
  ArrayVector argcopy(arg);
  argcopy.pop_front();
  if (arg.size() == 3)
    return EvalTryFunction(nargout,argcopy,eval,popspec);
  else
    return EvalNoTryFunction(nargout,argcopy,eval,popspec);
}

//@@Signature
//sfunction assignin AssignInFunction
//inputs workspace variablename value
//outputs none
//DOCBLOCK freemat_assignin
ArrayVector AssignInFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() < 3)
    throw Exception("assignin function requires a workspace (scope) specifier (either 'caller' or 'base') a variable name and a value to assign");
  QString spec_str = arg[0].asString();
  int popspec = 0;
  if (spec_str=="base")
    popspec = -1;
  else if (spec_str=="caller") 
    popspec = 2;
  else
    throw Exception("assignin function requires the first argument to be either 'caller' or 'base'");
  QString varname = arg[1].asString();
  Array varvalue = arg[2];
  PopContext saver(eval->getContext(),popspec);
  eval->getContext()->insertVariable(varname,varvalue);
  return ArrayVector();
}


ArrayVector TraceFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  qDebug() << "**********************************************************************\n";
  // walk the trace of 
  while (eval->getContext()->activeScopeName() != "base") {
    qDebug() << "Scope is " << eval->getContext()->activeScopeName();
    qDebug() << "Variables " << eval->getContext()->listAllVariables();
    eval->getContext()->bypassScope(1);
  }
  qDebug() << "Scope is " << eval->getContext()->activeScopeName();
  qDebug() << "Variables " << eval->getContext()->listAllVariables();
  qDebug() << "**********************************************************************\n";
  eval->getContext()->restoreBypassedScopes();
  return ArrayVector();
}

//@@Signature
//sfunction feval FevalFunction
//inputs varargin
//outputs varargout
//DOCBLOCK freemat_feval
ArrayVector FevalFunction(int nargout, const ArrayVector& arg,Interpreter* eval){
  if (arg.size() == 0)
    throw Exception("feval function requires at least one argument");
  if (!arg[0].isString())
    throw Exception("first argument to feval must be the name of a function (i.e., a string) a function handle, or a user defined class");
  eval->getContext()->deactivateCurrentScope(); // Make feval call invisible
  FuncPtr funcDef;
  if (arg[0].isString()) {
    QString fname = arg[0].asString();
    if (!eval->lookupFunction(fname,funcDef)) {
      throw Exception(QString("function ") + fname + " undefined!");
    }
  } else 
    throw Exception("argument to feval must be a string");
  funcDef->updateCode(eval);
  if (funcDef->scriptFlag)
    throw Exception("cannot use feval on a script");
  ArrayVector newarg(arg);
  newarg.pop_front();
  return eval->doFunction(funcDef,newarg,nargout);
}
