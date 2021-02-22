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

#include "FunctionDef.hpp"
#include "Interpreter.hpp"
#include "OctaveScanner.hpp"
#include "Parser.hpp"
#include "Exception.hpp"
#include <iostream>
#include <signal.h>
#include "SymbolTable.hpp"
#include "Types.hpp"
#include "MexInterface.hpp"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <sys/stat.h>
#include "MemPtr.hpp"
#include "Algorithms.hpp"
#include "FuncPtr.hpp"

#define MSGBUFLEN 2048

QMutex functiondefmutex;

StringVector IdentifierList(const Tree & t) {
  StringVector retval;
  for (int index=0;index<t.numChildren();index++) {
    if (t.child(index).is('&'))
      retval.push_back("&" + t.child(index).first().text());
    else
      retval.push_back(t.child(index).text());
  }
  return retval;
}

void VariableReferencesList(const Tree & t, StringVector& idents) {
  if (t.is(TOK_NEST_FUNC)) return;
  if (t.is(TOK_VARIABLE)) {
    bool exists = false;
    for (int i=0;(i<idents.size());i++) {
      exists = (idents[i] == t.first().text());
      if (exists) break;
    }
    if (!exists)
      idents.push_back(t.first().text());
  }
  for (int i=0;i<t.numChildren();i++)
    VariableReferencesList(t.child(i),idents);
}

MFunctionDef::MFunctionDef() {
  functionCompiled = false;
  localFunction = false;
  pcodeFunction = false;
  nestedFunction = false;
}

MFunctionDef::~MFunctionDef() {
}

int MFunctionDef::inputArgCount() {
  if (arguments.size() == 0) return 0;
  if (arguments[arguments.size()-1] == "varargin")
    return -1;
  else
    return arguments.size();
}

int MFunctionDef::outputArgCount() {
  if (returnVals.size() == 0) return 0;
  if (returnVals[returnVals.size()-1] == "varargout")
    return -1;
  else
    return returnVals.size();
}

void MFunctionDef::printMe(Interpreter*eval) {
  StringVector tmp;
  eval->outputMessage("Function name:" + name);
  eval->outputMessage("Function class: Compiled M function\n");
  eval->outputMessage("returnVals: ");
  tmp = returnVals;
  int i;
  for (i=0;i<tmp.size();i++)
    eval->outputMessage(tmp[i] + " ");
  eval->outputMessage("\n");
  eval->outputMessage("arguments: ");
  tmp = arguments;
  for (i=0;i<tmp.size();i++) 
    eval->outputMessage(tmp[i] + " ");
  eval->outputMessage("\ncode: \n");
  code.print();
}

ArrayVector MFunctionDef::evaluateFunc(Interpreter *walker, 
				       ArrayVector& inputs, 
				       int nargout,
				       VariableTable *workspace) {
  ArrayVector outputs;
  Context* context;
  bool warningIssued;
  int minCount;
    
  if (!code.valid()) return outputs;
  context = walker->getContext();
  context->setScopeNesting(nestedFunction);
  context->setVariablesAccessed(variablesAccessed);
  context->setLocalVariablesList(returnVals);
  if (workspace) {
    StringVector workspaceVars(workspace->getCompletions(""));
    for (int i=0;i<workspaceVars.size();i++)
      context->insertVariableLocally(workspaceVars[i],
				     *workspace->findSymbol(workspaceVars[i]));
  }
  // When the function is called, the number of inputs is
  // at sometimes less than the number of arguments requested.
  // Check the argument count.  If this is a non-varargin
  // argument function, then use the following logic:
  minCount = 0;
  if (inputArgCount() != -1) {
    minCount = (((int)inputs.size()) < arguments.size()) ? 
      inputs.size() : arguments.size();
    for (int i=0;i<minCount;i++) {
      QString arg(arguments[i]);
      if (arg[0] == '&')
	arg.remove(0,1);
      context->insertVariableLocally(arg,inputs[i]);
    }
    context->setScopeNargin(minCount);
  } else {
    // Count the number of supplied arguments
    int inputCount = inputs.size();
    context->setScopeNargin(inputCount);
    // Get the number of explicit arguments
    int explicitCount = arguments.size() - 1;
    // For each explicit argument (that we have an input for),
    // insert it into the scope.
    minCount = (explicitCount < inputCount) ? explicitCount : inputCount;
    int i;
    for (i=0;i<minCount;i++) {
      QString arg(arguments[i]);
      if (arg[0] == '&')
	arg.remove(0,1);
      context->insertVariableLocally(arg,inputs[i]);
    }
    inputCount -= minCount;
    // Put minCount...inputCount 
    ArrayVector varargin(inputs.mid(minCount,inputCount));
    context->insertVariableLocally(QString("varargin"),
				   CellArrayFromArrayVector(varargin,varargin.size()));
  }
  context->setScopeNargout(nargout);
  try {
    try {
      walker->block(code);
    } catch (InterpreterBreakException& e) {
    } catch (InterpreterContinueException& e) {
    } catch (InterpreterReturnException& e) {
    }
    warningIssued = false;
    if (outputArgCount() != -1) {
      // special case - if nargout == 0, and none of the
      // outputs are predefined, we don't do anything
      bool nonpredefed = true;
      for (int i=0;i<returnVals.size()&&nonpredefed;i++) {
	Array *ptr = context->lookupVariableLocally(returnVals[i]);
	nonpredefed = nonpredefed && (!ptr);
      }
      if ((nargout > 0) || 
	  ((nargout == 0) && (!nonpredefed))) {
	outputs = ArrayVector();
	for (int i=0;i<returnVals.size();i++) outputs.push_back(Array());
	//	outputs = ArrayVector(returnVals.size());
	for (int i=0;i<returnVals.size();i++) {
	  Array *ptr = context->lookupVariableLocally(returnVals[i]);
	  if (!ptr)
	    outputs[i] = EmptyConstructor();
	  else
	    outputs[i] = *ptr;
	  if (!ptr && (i < ((int)nargout)))
	    if (!warningIssued) {
	      walker->warningMessage("one or more outputs not assigned in call (1)");
	      warningIssued = true;
	    }
	}
      }
    } else {
      outputs = ArrayVector();
      for (int i=0;i<nargout;i++) outputs.push_back(Array());
      int explicitCount = returnVals.size() - 1;
      // For each explicit argument (that we have), insert it
      // into the scope.
      for (int i=0;i<explicitCount;i++) {
	Array *ptr = context->lookupVariableLocally(returnVals[i]);
	if (!ptr)
	  outputs[i] = EmptyConstructor();
	else
	  outputs[i] = *ptr;
	if (!ptr  && (i < nargout)) 
	  if (!warningIssued) {
	    walker->warningMessage("one or more outputs not assigned in call (2)");
	    warningIssued = true;
	  }
      }
      // Are there any outputs not yet filled?
      if (nargout > explicitCount) {
	Array varargout, *ptr;
	// Yes, get a pointer to the "vargout" variable that should be defined
	ptr = context->lookupVariableLocally("varargout");
	if (!ptr)
	  throw Exception("The special variable 'varargout' was not defined as expected");
	varargout = *ptr;
	if (varargout.dataClass() != CellArray)
	  throw Exception("The special variable 'varargout' was not defined as a cell-array");
	// Get the data pointer
	const BasicArray<Array> &dp(varargout.constReal<Array>());
	// Get the length
	int varlen = int(varargout.length());
	int toFill = nargout - explicitCount;
	if (toFill > varlen) 
	  throw Exception("Not enough outputs in varargout to satisfy call");
	for (int i=0;i<toFill;i++)
	  outputs[explicitCount+i] = dp[index_t(i+1)];
      }
      // Special case - nargout = 0, only variable outputs from function
      if ((nargout == 0) && (explicitCount == 0)) {
	Array varargout, *ptr;
	// Yes, get a pointer to the "vargout" variable that should be defined
	ptr = context->lookupVariableLocally("varargout");
	if (ptr) {
	  varargout = *ptr;
	  if (varargout.dataClass() != CellArray)
	    throw Exception("The special variable 'varargout' was not defined as a cell-array");
	  if (varargout.length() > 0) {
	    outputs << varargout.constReal<Array>().get(index_t(1));
	  }
	}	
      }
    }
    // Check for arguments that were passed by reference, and 
    // update their values.
    for (int i=0;i<minCount;i++) {
      QString arg(arguments[i]);
      if (arg[0] == '&')
	arg.remove(0,1);
      Array *ptr = context->lookupVariableLocally(arg);
      if (ptr)
	inputs[i] = *ptr;
    }
    // Check the outputs for function pointers
    CaptureFunctionPointers(outputs,walker,this);
    if (workspace) {
      StringVector workspaceVars(workspace->getCompletions(""));
      for (int i=0;i<workspaceVars.size();i++) {
	Array *ptr = context->lookupVariableLocally(workspaceVars[i]);
	workspace->insertSymbol(workspaceVars[i],*ptr);
      }
    }
    //    walker->popScope();
    return outputs;
  } catch (Exception& e) {
    if (workspace) {
      StringVector workspaceVars(workspace->getCompletions(""));
      for (int i=0;i<workspaceVars.size();i++) {
	Array *ptr = context->lookupVariableLocally(workspaceVars[i]);
	workspace->insertSymbol(workspaceVars[i],*ptr);
      }
    }
    throw;
  }
  catch (InterpreterRetallException& e) {
    if (workspace) {
      StringVector workspaceVars(workspace->getCompletions(""));
      for (int i=0;i<workspaceVars.size();i++) {
	Array *ptr = context->lookupVariableLocally(workspaceVars[i]);
	workspace->insertSymbol(workspaceVars[i],*ptr);
      }
    }
    throw;
  }
}
  
inline QString ReadFileIntoString(QString filename) {
  QFile fp(filename);
  if (!fp.open(QIODevice::ReadOnly)) 
    throw Exception(QString("Unable to open file :") + filename);
  QTextStream io(&fp);
  QString txt(io.readAll());
  if (!txt.endsWith("\n")) txt += "\n";
  return txt;
}

//MFunctionDef* ConvertParseTreeToMFunctionDef(tree t, string fileName) {
//  MFunctionDef *fp = new MFunctionDef;
//  fp->returnVals = IdentifierList(t.first());
//  fp->name = t.second().text();
//  fp->arguments = IdentifierList(t.child(2));
//  fp->code = t.child(3);
//  fp->fileName = fileName;
//  return fp;
//}
//
//MFunctionDef* ConvertParseTreeToMFunctionDefs(treeVector t, 
//					      string fileName) {
//  MFunctionDef* last = NULL;
//  for (int index = t.size()-1;index>=0;index--) {
//    MFunctionDef* rp = ConvertParseTreeToMFunctionDef(t[index],fileName);
//    if (index>0)
//      rp->localFunction = true;
//    else
//      rp->localFunction = false;
//    rp->nextFunction = last;
//    if (last)
//      last->prevFunction = rp;
//    last = rp;
//  }
//  return last;
//}

static void RegisterNested(const Tree & t, Interpreter *m_eval, MFunctionDef *parent) {
  if (t.is(TOK_NEST_FUNC)) {
    MFunctionDef *fp = new MFunctionDef;
    fp->localFunction = parent->localFunction;
    fp->nestedFunction = true;
    fp->returnVals = IdentifierList(t.first());
    fp->name = parent->name + "/" + t.second().text();
    fp->arguments = IdentifierList(t.child(2));
    fp->code = t.child(3); 
    VariableReferencesList(fp->code,fp->variablesAccessed);
    fp->fileName = parent->fileName;
    // Register any nested functions for the local functions
    m_eval->getContext()->insertFunction(fp,false);
    RegisterNested(fp->code,m_eval,fp);
  } else
    for (int i=0;i<t.numChildren();i++)
      RegisterNested(t.child(i),m_eval,parent);
}

static Tree ParseMFile(QString fileName) {
  // Read the file into a string
  QString fileText = ReadFileIntoString(fileName);
  if (!fileName.contains(QDir::separator()+QString("+octave")+QDir::separator()))
    {
      Scanner S(fileText,fileName);
      Parser P(S);
      return P.process();
    }
  else
    {
      OctaveScanner S(fileText,fileName);
      Parser P(S,true);
      Tree t = P.process();
      t.print();
      return t;
    }
}

// Compile the function...
bool MFunctionDef::updateCode(Interpreter *m_eval) {
  if (localFunction) return false;
  if (pcodeFunction) return false;
  if (nestedFunction) return false;
  // First, stat the file to get its time stamp
  QFileInfo filestat(fileName);
  if (!functionCompiled || (filestat.lastModified() != timeStamp)) {
    // Record the time stamp
    timeStamp = filestat.lastModified();
    QFile fp(fileName);
    if (!fp.open(QIODevice::ReadOnly))
      throw Exception(QString("Unable to open file :") + fileName);
    bool commentsOnly = true;
    helpText.clear();
    QTextStream io(&fp);
    QString cp;
    while (!io.atEnd() && commentsOnly) {
      cp = io.readLine();
      while ((cp.size() > 1) && (cp.at(0).isSpace()))
	cp.remove(0,1);
      if (cp == "\n" || cp.isEmpty()) continue;
      if (cp.at(0) != QChar('%'))
	commentsOnly = false;
      else {
	if ((cp.size() > 1) && (!cp.endsWith(QChar('\n'))))
	  helpText.push_back(cp + "\n");
	else
	  helpText.push_back(cp);
      }
    }
    if (helpText.size() == 0)
      helpText.push_back(cp);
    try {
      const Tree &pcode(ParseMFile(fileName));
      if (pcode.is(TOK_FUNCTION_DEFS)) {
	scriptFlag = false;
	// Get the main function..
	const Tree & MainFuncCode = pcode.first();
	returnVals = IdentifierList(MainFuncCode.first());
	// The name is mangled by the interpreter...  We ignore the
	// name as parsed in the function.
	//	name = MainFuncCode.second().text();
	arguments = IdentifierList(MainFuncCode.child(2));
	code = MainFuncCode.child(3);
	VariableReferencesList(code,variablesAccessed);
	// Register any nested functions
	RegisterNested(code,m_eval,this);
	localFunction = false;
	// Process the local functions
	for (int index = 1;index < pcode.numChildren();index++) {
	  const Tree & LocalFuncCode = pcode.child(index);
	  MFunctionDef *fp = new MFunctionDef;
	  fp->localFunction = true;
	  fp->returnVals = IdentifierList(LocalFuncCode.first());
	  fp->name = name + "/" + LocalFuncCode.second().text();
	  fp->arguments = IdentifierList(LocalFuncCode.child(2));
	  fp->code = LocalFuncCode.child(3); 
	  VariableReferencesList(fp->code,fp->variablesAccessed);
	  fp->fileName = fileName;
	  // Register any nested functions for the local functions
	  // local functions are not marked as temporary.  This yields
	  // clutter in the name space, but solves the troublesome
	  // issue of local functions being flushed by the CD command.
	  m_eval->getContext()->insertFunction(fp,false);
	  RegisterNested(fp->code,m_eval,this);
	}
	functionCompiled = true;
      } else {
	scriptFlag = true;
	functionCompiled = true;
	code = pcode.first();
      }
    } catch (Exception &e) {
      functionCompiled = false;
      throw;
    }
    return true;
  } 
  return false;
}

static bool StatementTypeNode(const Tree & t) {
  return (t.is('=') || t.is(TOK_MULTI) || t.is(TOK_SPECIAL) ||
	  t.is(TOK_FOR) || t.is(TOK_WHILE) || t.is(TOK_IF) ||
	  t.is(TOK_BREAK) || t.is(TOK_CONTINUE) || t.is(TOK_DBSTEP) ||
	  t.is(TOK_RETURN) || t.is(TOK_SWITCH) || t.is(TOK_TRY) || 
	  t.is(TOK_QUIT) || t.is(TOK_RETALL) || t.is(TOK_KEYBOARD) ||
	  t.is(TOK_GLOBAL) || t.is(TOK_PERSISTENT) || t.is(TOK_EXPR));
}

// Find the smallest line number larger than the argument
// if our line number is larger than the target, then we
// 
static void TreeLine(const Tree & t, unsigned &bestLine, unsigned lineTarget) {
  if (!t.valid()) return;
  // Nested functions are tracked separately - so that we do not
  // check them for line numbers
  if (t.is(TOK_NEST_FUNC)) return;
  if (StatementTypeNode(t)) {
    unsigned myLine = (LineNumber(t.context()));
    if ((myLine >= lineTarget) && (myLine < bestLine))
      bestLine = myLine;
  }
  for (int i=0;i<t.numChildren();i++)
    TreeLine(t.child(i),bestLine,lineTarget);
}


// Find the closest line number to the requested 
unsigned MFunctionDef::ClosestLine(unsigned line) {
  unsigned bestline;
  bestline = 1000000000;
  TreeLine(code,bestline,line);
  if (bestline == 1000000000)
    throw Exception(QString("Unable to find a line close to ") + 
		    QString::number(line) + 
		    QString(" in routine ") + name);
  return bestline;
}


BuiltInFunctionDef::BuiltInFunctionDef() {
}

BuiltInFunctionDef::~BuiltInFunctionDef() {
}

int BuiltInFunctionDef::inputArgCount() {
  return argCount;
}

int BuiltInFunctionDef::outputArgCount() {
  return retCount;
}

void BuiltInFunctionDef::printMe(Interpreter *eval) {
  eval->outputMessage(" Function name:" + name);
  eval->outputMessage(" Function class: Built in\n");
  eval->outputMessage(QString(" Return count: %1\n").arg(retCount));
  eval->outputMessage(QString(" Argument count: %1\n").arg(argCount));
}


ArrayVector BuiltInFunctionDef::evaluateFunc(Interpreter *walker,
					     ArrayVector& inputs, 
					     int nargout,
					     VariableTable*) {
  return fptr(nargout,inputs);
}

SpecialFunctionDef::SpecialFunctionDef() {
}

SpecialFunctionDef::~SpecialFunctionDef() {
}

ArrayVector SpecialFunctionDef::evaluateFunc(Interpreter *walker, 
					     ArrayVector& inputs, 
					     int nargout, 
					     VariableTable*) {
  return fptr(nargout,inputs,walker);
}

void SpecialFunctionDef::printMe(Interpreter *eval) {
}

FunctionDef::FunctionDef() {
  scriptFlag = false;
  graphicsFunction = false;
  temporaryFlag = false;
  refcount = 0;
}

void FunctionDef::lock() {
  QMutexLocker lockit(&functiondefmutex);
  refcount++;
}

void FunctionDef::unlock() {
  QMutexLocker lockit(&functiondefmutex);
  refcount--;
}

bool FunctionDef::referenced() {
  QMutexLocker lockit(&functiondefmutex);
  return (refcount>0);
}

FunctionDef::~FunctionDef() {
}

bool ImportedFunctionDef::isPassedAsPointer(int arg) {
  return (arguments[arg].startsWith("&") || (types[arg] == "string")
	  || sizeCheckExpressions[arg].valid());
}

static DataClass mapTypeNameToClass(QString name) {
  if (name == "uint8") return UInt8;
  if (name == "int8") return Int8;
  if (name == "uint16") return UInt16;
  if (name == "int16") return Int16;
  if (name == "uint32") return UInt32;
  if (name == "int32") return Int32;
  if (name == "uint64") return UInt64;
  if (name == "int64") return Int64;
  if (name == "float") return Float;
  if (name == "double") return Double;
  if (name == "string") return StringArray;
  if (name == "void") return Int32;
  throw Exception("unrecognized type " + name + " in imported function setup");
}

static ffi_type* mapTypeNameToFFIType(QString name) {
  if (name == "uint8") return &ffi_type_uint8;
  if (name == "int8") return &ffi_type_sint8;
  if (name == "uint16") return &ffi_type_uint16;
  if (name == "int16") return &ffi_type_sint16;
  if (name == "uint32") return &ffi_type_uint32;
  if (name == "int32") return &ffi_type_sint32;
  if (name == "uint64") return &ffi_type_uint64;
  if (name == "int64") return &ffi_type_sint64;
  if (name == "float") return &ffi_type_float;
  if (name == "double") return &ffi_type_double;
  if (name == "string") return &ffi_type_pointer;
  if (name == "void") return &ffi_type_void;
  throw Exception("unrecognized type " + name + " in imported function setup");
}

static int importCounter = 0;
ImportedFunctionDef::ImportedFunctionDef(GenericFuncPointer address_arg,
					 StringVector types_arg,
					 StringVector arguments_arg,
					 TreeList sizeChecks,
					 QString retType_arg,
					 QString name) {
  address = address_arg;
  types = types_arg;
  arguments = arguments_arg;
  sizeCheckExpressions = sizeChecks;
  retType = retType_arg;
  argCount = types_arg.size();
  if (retType == "void") 
    retCount = 0;
  else
    retCount = 1;
  // Build the ffi cif (call interface object)
  ffi_type **args;
  args = (ffi_type**) malloc(sizeof(ffi_type*)*argCount);
  for (int i=0;i<argCount;i++) {
    if (arguments[i][0] == '&' || types[i] == "string" ||
	sizeCheckExpressions[i].valid()) 
      args[i] = &ffi_type_pointer;
    else
      args[i] = mapTypeNameToFFIType(types[i]);
  }
  if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, argCount,
		   mapTypeNameToFFIType(retType), args) != FFI_OK)
    throw Exception("unable to import function through ffi!");
}

ImportedFunctionDef::~ImportedFunctionDef() {
}

void ImportedFunctionDef::printMe(Interpreter *) {
}


static QString TrimAmpersand(QString name) {
  if (!name.startsWith("&")) return name;
  name.remove(0,1);
  return name;
}

template <class T>
static Array DoCIFCall(ffi_cif cif, GenericFuncPointer address,
		       vector<void*> &values, DataClass rclass)
{
  T retval;
  if (values.size() > 0)
    ffi_call(&cif, address, &retval, &values[0]);
  else
    ffi_call(&cif, address, &retval, NULL);
  return Array::create(retval);
}

/**
 * Note: Pass-by-reference only really matters for strings, and that
 * is only because for strings, we convert them from Unicode to C strings
 * when passing down, and only bother converting them back if they
 * were passed by reference.
 */


ArrayVector ImportedFunctionDef::evaluateFunc(Interpreter *walker,
					      ArrayVector& inputs,
					      int nargout,
					      VariableTable*) {
  /**
   * To actually evaluate the function, we have to process each of
   * the arguments and get them into the right form.
   */
  int i;
  for (i=0;i<inputs.size();i++)
    inputs[i] = inputs[i].asDenseArray().toClass(mapTypeNameToClass(types[i]));
  /**
   * Next, we count how many of the inputs are to be passed by
   * reference.
   */
  int passByReference = 0;
  for (int j=0;j<inputs.size();j++)
    if ((arguments[j][0] == '&') || (types[j] == "string") ||
	(sizeCheckExpressions[j].valid()))
      passByReference++;
  /**
   * Strings are converted to C strings and stored here
   */
  std::vector<char*> string_store;
  for (i=0;i<inputs.size();i++)
    if (types[i] == "string")
      string_store.push_back(strdup(qPrintable(inputs[i].asString())));
    else
      string_store.push_back(NULL);
  /**
   * Next, we check to see if any bounds-checking expressions are
   * active.
   */
  bool boundsCheckActive = false;
  int m=0;
  while (m < inputs.size() && !boundsCheckActive)
    boundsCheckActive = (sizeCheckExpressions[m++].valid());
  if (boundsCheckActive) {
    /**
     * If the bounds-checking is active, we have to create a 
     * new context, and insert the defined arguments into the
     * context (much as for an M-function call).
     */
    Context* context = walker->getContext();
    context->pushScope("bounds_check",name);
    for (i=0;i<inputs.size();i++) {
      context->insertVariableLocally(TrimAmpersand(arguments[i]),inputs[i]);
    }
    /*
     * Next, evaluate each size check expression
     */
    for (i=0;i<inputs.size();i++) {
      if (sizeCheckExpressions[i].valid()) {
	Array ret(walker->expression(sizeCheckExpressions[i]));
	ret = ret.toClass(Int32);
	int len = ret.asInteger();
	if (len != (int)(inputs[i].length())) {
	  throw Exception("array input " + TrimAmpersand(arguments[i]) + 
			  " length different from computed bounds" + 
			  " check length");
	}
      }
    }
    context->popScope();
  }

  /**
   * Allocate an array of pointers to store for variables passed
   * by reference.
   */
  vector<void*> refPointers;
  refPointers.resize(passByReference);
  /** 
   * Allocate an array of value pointers...
   */
  vector<void*> values;
  values.resize(inputs.size());
  int ptr = 0;
  for (i=0;i<inputs.size();i++) {
    if (types[i] != "string") {
      if ((arguments[i][0] == '&') || (sizeCheckExpressions[i].valid())) {
	refPointers[ptr] = inputs[i].getVoidPointer();
	values[i] = &refPointers[ptr];
	ptr++;
      } else {
	values[i] = inputs[i].getVoidPointer();
      }
    } else {
      refPointers[ptr] = string_store[i];
      values[i] = &refPointers[ptr];
      ptr++;
    }
  }
  
  Array retArray;
  if (retType == "uint8") retArray = DoCIFCall<uint8>(cif,address,values,UInt8);
  else if (retType == "int8") retArray = DoCIFCall<int8>(cif,address,values,Int8);
  else if (retType == "uint16") retArray = DoCIFCall<uint16>(cif,address,values,UInt16);
  else if (retType == "int16") retArray = DoCIFCall<int16>(cif,address,values,Int16);
  else if (retType == "uint32") retArray = DoCIFCall<uint32>(cif,address,values,UInt32);
  else if (retType == "int32") retArray = DoCIFCall<int32>(cif,address,values,Int32);
  else if (retType == "uint64") retArray = DoCIFCall<uint64>(cif,address,values,UInt64);
  else if (retType == "int64") retArray = DoCIFCall<int64>(cif,address,values,Int64);
  else if (retType == "float") retArray = DoCIFCall<float>(cif,address,values,Float);
  else if (retType == "double") retArray = DoCIFCall<double>(cif,address,values,Double);
  else if (retType == "void") {
    DoCIFCall<int32>(cif,address,values,Int32);
    retArray = EmptyConstructor();
  } else
    throw Exception("Unsupported return tye " + retType + " in imported function call");

  // Strings that were passed by reference have to be
  // special-cased
  for (i=0;i<inputs.size();i++) {
    if (arguments[i].startsWith("&") && (types[i] == "string"))
      inputs[i] = Array(QString(string_store[i]));
  }
  for (i=0;i<inputs.size();i++)
    if (string_store[i]) free(string_store[i]);
  return ArrayVector(retArray);
}

MexFunctionDef::MexFunctionDef(QString fullpathname) {
  fullname = fullpathname;
  importSuccess = false;
  lib = new DynLib(fullname);
  try {
    address = (mexFuncPtr) lib->GetSymbol("mexFunction");  
    importSuccess = true;
  } catch (Exception& e) {
  }
}

bool MexFunctionDef::LoadSuccessful() {
  return importSuccess;
}
  
MexFunctionDef::~MexFunctionDef() {
}

void MexFunctionDef::printMe(Interpreter *) {
}
  
ArrayVector MexFunctionDef::evaluateFunc(Interpreter *walker, 
					 ArrayVector& inputs, 
					 int nargout,
					 VariableTable*) {
  // Convert arguments to mxArray
  mxArray** args = new mxArray*[inputs.size()];
  for (int i=0;i<inputs.size();i++)
    args[i] = MexArrayFromArray(inputs[i]);
  // Allocate output array vector
  int lhsCount = nargout;
  lhsCount = (lhsCount < 1) ? 1 : lhsCount;
  mxArray** rets = new mxArray*[lhsCount];
  try {
    address(lhsCount,rets,inputs.size(),(const mxArray**)args);
  } catch (QString &e) {
    throw Exception(e);
  }
  ArrayVector retvec;
  for (int i=0;i<lhsCount;i++) {
    retvec.push_back(ArrayFromMexArray(rets[i]));
    mxDestroyArray(rets[i]);
  }
  delete[] rets;
  return retvec;
}
