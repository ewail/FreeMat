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
#include <QtCore>
#include "Algorithms.hpp"
#include "Struct.hpp"

//@@Signature
//function p_end EndFunction
//inputs x dim subindexes
//outputs y
//DOCBLOCK inspection_end
ArrayVector EndFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 3)
    throw Exception("End function requires 3 arguments, the array, the end index, and the number of subindexes");
  NTuple t(arg[0].dimensions());
  index_t enddim(arg[1].asDouble());
  index_t totalndxs(arg[2].asDouble());
  if (totalndxs == 1)
    return ArrayVector(Array(index_t(arg[0].length())));
  return ArrayVector(Array(index_t(t[int(enddim-1)])));
}

//@@Signature
//sfunction who WhoFunction
//inputs varargin
//outputs none
//DOCBLOCK inspection_who
ArrayVector WhoFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  StringVector names;
  Context *context = eval->getContext();
  // Bypass our context (the who(who) one)
  ParentScopeLocker lock(context);
  // Search upwards until we find an active scope
  int bypasscount = 0;
  while (!context->isScopeActive()) {
    bypasscount++;
    context->bypassScope(1);
  }
  if (arg.size() == 0)
    names = eval->getContext()->listAllVariables();
  else
    for (int i=0;i<arg.size();i++)
      names.push_back(arg[i].asString());
  qSort(names.begin(),names.end());
  eval->outputMessage("  Variable Name       Type   Flags             Size\n");
  for (int i=0;i<names.size();i++) {
    Array lookup;
    ArrayReference ptr;
    eval->outputMessage(names[i].rightJustified(15,' ',false));
    ptr = eval->getContext()->lookupVariable(names[i]);
    if (!ptr.valid())
      eval->outputMessage("   <undefined>");
    else {
      lookup = *ptr;
      eval->outputMessage(lookup.className().rightJustified(10,' ',false));
      if (lookup.isSparse())
	eval->outputMessage("   sparse");
      else
	eval->outputMessage("         ");	  
      if (eval->getContext()->isVariableGlobal(names[i])) {
	eval->outputMessage("  global ");
      } else if (eval->getContext()->isVariablePersistent(names[i])) {
	eval->outputMessage(" persist ");
      } else {
	eval->outputMessage("         ");
      }
      eval->outputMessage(QString("  [") + 
			  lookup.dimensions().toString() + 
			  QString("]"));
    }
    eval->outputMessage("\n");
  }
  context->restoreScope(bypasscount);
  return ArrayVector();
}

//@@Signature
//sfunction whos WhosFunction
//inputs varargin
//outputs none
//DOCBLOCK inspection_whos
ArrayVector WhosFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  StringVector names;
  Context *context = eval->getContext();
  // Bypass our context (the who(who) one)
  ParentScopeLocker lock(context);
  // Search upwards until we find an active scope
  int bypasscount = 0;
  while (!context->isScopeActive()) {
    bypasscount++;
    context->bypassScope(1);
  }
  if (arg.size() == 0)
    names = eval->getContext()->listAllVariables();
  else
    for (int i=0;i<arg.size();i++)
      names.push_back(arg[i].asString());
  qSort(names.begin(),names.end());
  eval->outputMessage("  Variable Name       Type   Flags             Size       Bytes\n");
  for (int i=0;i<names.size();i++) {
    Array lookup;
    ArrayReference ptr;
    eval->outputMessage(names[i].rightJustified(15,' ',false));
    ptr = eval->getContext()->lookupVariable(names[i]);
    if (!ptr.valid())
      eval->outputMessage("   <undefined>");
    else {
      lookup = *ptr;
      eval->outputMessage(lookup.className().rightJustified(10,' ',false));
      if (lookup.isSparse())
	eval->outputMessage("   sparse");
      else
	eval->outputMessage("         ");	  
      if (eval->getContext()->isVariableGlobal(names[i])) {
	eval->outputMessage("  global ");
      } else if (eval->getContext()->isVariablePersistent(names[i])) {
	eval->outputMessage(" persist ");
      } else {
	eval->outputMessage("         ");
      }
      QString txt(QString("  [") + 
		  lookup.dimensions().toString() + 
		  QString("]"));
      eval->outputMessage(txt.leftJustified(15,' ',false));
      eval->outputMessage(QString("   %1").arg(lookup.bytes()));
    }
    eval->outputMessage("\n");
  }
  context->restoreScope(bypasscount);
  return ArrayVector();
}


//@@Signature
//function fieldnames FieldNamesFunction
//inputs y
//outputs x
//DOCBLOCK inspection_fieldnames
ArrayVector FieldNamesFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("fieldnames function requires at least one argument");
  if (arg[0].dataClass() != Struct)
    return ArrayVector(Array(CellArray,NTuple(0,0)));
  StringVector names(FieldNames(arg[0]));
  ArrayMatrix m;
  for (int i=0;i<names.size();i++)
    m.push_back(ArrayVector(Array(names.at(i))));
  return ArrayVector(CellConstructor(m));
}


//@@Signature
//sfunction where WhereFunction
//inputs none
//outputs none
//DOCBLOCK inspection_where
ArrayVector WhereFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  eval->stackTrace(1);
  return ArrayVector();
}

ArrayVector AddrFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector(Array(arg[0].address()));
}

#define LOOKUP(x,field) x.constStructPtr()[field].get(1)

//@@Signature
//sfunction nargin NarginFunction
//inputs funcname
//outputs count
//DOCBLOCK functions_nargin
ArrayVector NarginFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() == 0)
    {
      Context *ctxt = eval->getContext();
      ParentScopeLocker lock(ctxt);
      int nargin = ctxt->scopeNargin();
      return ArrayVector() << Array(double(nargin));
    }
  Array a = arg[0];
  QString txt;
  if (a.className() == "functionpointer")
    txt = LOOKUP(a,"name").asString();
  else
    txt = arg[0].asString();
  FuncPtr val;
  if (!eval->lookupFunction(txt,val))
    throw Exception("Unable to resolve " + txt + " to a function call");
  return Array(double(val->inputArgCount()));
}

//@@Signature
//sfunction nargout NargoutFunction
//inputs func
//outputs count
//DOCBLOCK functions_nargout
ArrayVector NargoutFunction(int, const ArrayVector&arg, Interpreter* eval) {
  if (arg.size() == 0)
    {
      Context *ctxt = eval->getContext();
      ParentScopeLocker lock(ctxt);
      int nargout = ctxt->scopeNargout();
      return ArrayVector() << Array(double(nargout));
    }
  Array a = arg[0];
  QString txt;
  if (a.className() == "functionpointer")
    txt = LOOKUP(a,"name").asString();
  else
    txt = arg[0].asString();
  FuncPtr val;
  if (!eval->lookupFunction(txt,val))
    throw Exception("Unable to resolve " + txt + " to a function call");
  return Array(double(val->outputArgCount()));  
}


//@@Signature
//sfunction which WhichFunction
//inputs functionname
//outputs location
//DOCBLOCK inspection_which
ArrayVector WhichFunction(int nargout, const ArrayVector& arg, 
			  Interpreter* eval) {
  if (arg.size() != 1)
    throw Exception("which function takes one string argument (the name of the function to look up)");
  QString fname = arg[0].asString();
  bool isFun;
  FuncPtr val;
  isFun = eval->lookupFunction(fname,val);
  Array ret(Double,NTuple(0,0));
  if (isFun) {
    if (val->type() == FM_M_FUNCTION) {
      MFunctionDef *mptr;
      mptr = (MFunctionDef *) val;
      try {
	mptr->updateCode(eval);
      } catch (Exception &e) {}
      if (mptr->pcodeFunction) {
	if (mptr->scriptFlag) {
	  if (nargout == 0) {
	    eval->outputMessage("Function "+fname+", P-code script\n");
	  }
	} else {
	  if (nargout == 0) {
	    eval->outputMessage("Function "+fname+", P-code function\n");
	  }
	}
      } else {
	if (mptr->scriptFlag) {
	  if (nargout == 0) {
	    eval->outputMessage("Function "+fname+", M-File script in file '"+mptr->fileName+"'\n");
	  } else 
	    ret = Array(mptr->fileName);
	} else {
	  if (nargout == 0) {
	    eval->outputMessage("Function "+fname+", M-File function in file '"+mptr->fileName+"'\n");
	  } else
	    ret = Array(mptr->fileName);
	}
      }
    } else if ((val->type() == FM_BUILT_IN_FUNCTION) || 
	       (val->type() == FM_SPECIAL_FUNCTION) ) {
      if (nargout == 0) {
	eval->outputMessage("Function "+fname+" is a built in function\n");
      }
    } else {
      if (nargout == 0) {
	eval->outputMessage("Function "+fname+" is an imported function\n");
      }
    }
  } else {
    if (nargout == 0) {
      eval->outputMessage("Function "+fname+" is unknown!\n");
    }
  }
  if (nargout > 0)
    return ArrayVector(ret);
  else
    return ArrayVector();
}


//@@Signature
//sfunction mfilename MFilenameFunction
//inputs none
//outputs filename
//DOCBLOCK freemat_mfilename
ArrayVector MFilenameFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  return ArrayVector(Array(QFileInfo(eval->getMFileName()).fileName()));
}

//@@Signature
//function computer ComputerFunction
//inputs none
//outputs str
//DOCBLOCK freemat_computer
ArrayVector ComputerFunction(int nargout, const ArrayVector& arg) {
#ifdef WIN32
  return ArrayVector(Array(QString("PCWIN")));
#elif defined(__APPLE__)
  return ArrayVector(Array(QString("MAC")));
#else
  return ArrayVector(Array(QString("UNIX")));
#endif
}

