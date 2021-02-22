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

#ifndef __FunctionDef_hpp__
#define __FunctionDef_hpp__

#include "Array.hpp"
#include "Tree.hpp"
#include "DynLib.hpp"
#include "mex.h"
#include <QSharedData>
#include <QDateTime>
#include "Scope.hpp"

#include "ffi.h"

using namespace std;

typedef enum {
  FM_M_FUNCTION,
  FM_BUILT_IN_FUNCTION,
  FM_SPECIAL_FUNCTION,
  FM_IMPORTED_FUNCTION,
  FM_MEX_FUNCTION
} FunctionType;

class Interpreter;

typedef ArrayVector (*BuiltInFuncPtr) (int,const ArrayVector&);
typedef ArrayVector (*SpecialFuncPtr) (int,const ArrayVector&,Interpreter*);

void VariableReferencesList(const Tree & t, StringVector& idents);
StringVector IdentifierList(const Tree & t);

/** Base class for the function types
 * A FunctionDef class is a base class for the different types
 * of function pointers used.  There are three types of functions
 * available:
 *    - M-functions - these are functions or scripts written by the
 *      user in the interpreted language.
 *    - Built-in functions - these are functions coded in C++ that
 *      implement functionality too difficult/impossible to do in
 *      the language itself.
 *    - Special functions - these are functions coded in C++ that
 *      require access to the internals of the interpreter object.
 * All of these functions have in common a name, a script classification
 * (is it a script or not), a well defined number of input arguments,
 * a well defined number of output arguments, and some means of 
 * being evaluated.
 */

class FunctionDef {
public:
  /**
   * The name of the function - must follow identifier rules.
   */
  QString name;
  /**
   * The reference count for this functiondef
   */
  int refcount;
  /**
   * Is this a script?
   */
  bool scriptFlag;
  /**
   * Is this a temporary function?
   */
  bool temporaryFlag;
  /**
   * If a function requires access to the graphics subsystem,
   * it must declare itself as such.  Such functions are executed
   * through the main thread.
   */
  bool graphicsFunction;
  /**
   * The list of functions that may be called from JIT-ted code
   * is limited.  Set this flag to true if the function can be called
   * from JIT-ted code.
   */
  bool jitsafe;
  /**
   * The names of the arguments to the fuction (analogous to returnVals).
   * Should have "varargin" as the last entry for variable argument
   * functions.
   */
  StringVector arguments;
  /**
   * The constructor.
   */
  FunctionDef();
  /** 
   * The virtual destructor
   */
  virtual ~FunctionDef();
  /**
   * Increment the reference count
   */
  void lock();
  /**
   * Decrement the reference count
   */
  void unlock();
  /**
   * Returns true if the current function has references
   */
  bool referenced();
  /**
   * The type of the function (FM_M_FUNCTION, FM_BUILT_IN_FUNCTION,
   * FM_SPECIAL_FUNCTION, FM_IMPORTED_FUNCTION).
   */
  virtual const FunctionType type() = 0;
  /**
   * Returns the name of the function
   */
  virtual QString functionName() = 0;
  /**
   * Returns the "detailed" name of the function -- a full path for example
   */
  virtual QString detailedName() = 0;
  /**
   * Print a description of the function
   */
  virtual void printMe(Interpreter* io) = 0;
  /**
   * The number of inputs required by this function (-1 if variable).
   */
  virtual int inputArgCount() = 0;
  /**
   * The number of outputs returned by this function (-1 if variable).
   */
  virtual int outputArgCount() = 0;
  /**
   * Evaluate the function and return its output.
   */
  virtual ArrayVector evaluateFunc(Interpreter *, ArrayVector& , int, 
				   VariableTable * Workspace = 0) = 0;
  /**
   * Compile the function (if it is required).  We guarantee that this
   * function will be called at least once before evaluateFunction is called.
   * Return true if the updateCode call did anything.
   */
  virtual bool updateCode(Interpreter *) {return false;}
};


/**
 * An MFunctionDef is a FunctionDef for an interpreted function.  The
 * function pointer stores the name of the file where the function is
 * located, and a time stamp as to when the function was last modified.
 * When the updateCode() member function is called, the contents of
 * the file are parsed (if necessary), and the resulting AST code tree
 * is stored.  The number of input and output arguments are computed
 * based on the contents of the returnVals and arguments StringVectors.
 */
class MFunctionDef : public FunctionDef {
public:
  /**
   * The names of the return values - this is a vector of strings with
   * one entry for each return value in the declared function.  Thus,
   * if the function is declared as "function [a,b] = foo(x)", then
   * returnVals contains two entries: "a", and "b".  For variable
   * return functions, the last entry should be "varargout".
   */
  StringVector returnVals;
  /**
   * The AST for the code that defines the function (only the body of the
   * function is contained in this AST, not the function declaration itself).
   */
  Tree code;
  /**
   * Flag to indicate if the function has been compiled.
   */
  bool functionCompiled;
  /**
   * Location of the function's defining file in the current filesystem.
   */
  QString fileName;
  /**
   * Time function was last modified.
   */
  QDateTime timeStamp;
  /**
   * Set to true for all of the localFunctions.  
   */
  bool localFunction;
  bool pcodeFunction;
  bool nestedFunction;
  /**
   * The help text.
   */
  StringVector helpText;
  /**
   * The variable access list - used as a hinting mechanism to try and
   * resolve scope rules for nested functions.
   */
  StringVector variablesAccessed;
  /**
   * The constructor.
   */
  MFunctionDef();
  /**
   * The destructor
   */
  ~MFunctionDef();
  /** The type of the function
   */
  virtual const FunctionType type() {return FM_M_FUNCTION;}
  virtual QString functionName() {return fileName;}
  virtual QString detailedName() {return name;}
  /** Print a description of the function
   */
  virtual void printMe(Interpreter* io);
  /** 
   * The number of inputs required by this function, which is the number of 
   * elements in arguments unless the last element is the keyword "varargin"
   * in which case the answer is -1.
   */
  virtual int inputArgCount();
  /** 
   * The number of outputs returned by this function, which is the number of 
   * elements in returnVals unless the last element is the keyword "varargout"
   * in which case the answer is -1.
   */
  virtual int outputArgCount();
  /**
   * Evaluate the function and return the outputs.
   * Throws an Exception if
   *   - the special variable 'varargout' is not defined in the body of the
   *     of the function as promised by the function declaration.
   *   - the variable 'varargout' contains too few elements to satisfy the
   *     number of return values in the call
   *   - the variable 'varargout' is the wrong type.
   */
  virtual ArrayVector evaluateFunc(Interpreter *, ArrayVector &, int, VariableTable *);
  /**
   * Check the timestamp on the file, and if necessary, recompile the 
   * function.  Throws an exception if a syntax error occurs in the
   * parsing of the file (i.e., it cannot be classified as either a
   * script or a function definition).
   */
  virtual bool updateCode(Interpreter *m_eval);
  /**
   * Find the line number closest to the requested one
   */
  unsigned ClosestLine(unsigned line);
};


class BuiltInFunctionDef : public FunctionDef {
public:
  /** The number of return args for this function (-1 for variable).
   */
  int retCount;
  /** The number of input args for this function (-1 for variable).
   */
  int argCount;
  /**
   * The pointer to (address of) the function.
   */
  BuiltInFuncPtr fptr;
  /**
   * Default constructor.
   */
  BuiltInFunctionDef();
  /**
   * Default destructor.
   */
  ~BuiltInFunctionDef();
  /**
   * The type of the function is FM_BUILT_IN_FUNCTION.
   */
  virtual const FunctionType type() {return FM_BUILT_IN_FUNCTION;}
  virtual QString functionName() {return name;}
  virtual QString detailedName() {return "builtin";}
  /** Print a description of the function
   */
  virtual void printMe(Interpreter *io);
  /**
   * The number of inputs required by this function.
   */
  virtual int inputArgCount();
  /**
   * The number of outputs returned by this function.
   */
  virtual int outputArgCount();
  /** 
   * Evaluate the function and return the values.
   */
  virtual ArrayVector evaluateFunc(Interpreter *, ArrayVector &, int, VariableTable *);
};

class SpecialFunctionDef : public FunctionDef {
public:
  /**
   * The number of return args for this function (-1 for variable).
   */
  int retCount;
  /** The number of input args for this function (-1 for variable).
   */
  int argCount;
  /**
   * The pointer to (address of) the function.
   */    
  SpecialFuncPtr fptr;
  /**
   * Default constructor.
   */
  SpecialFunctionDef();
  /**
   * Default destructor.
   */
  ~SpecialFunctionDef();
  /**
   * The type of the function is FM_SPECIAL_FUNCTION.
   */
  virtual const FunctionType type() {return FM_SPECIAL_FUNCTION;}
  virtual QString functionName() {return name;}
  virtual QString detailedName() {return "builtin";}
  /** Print a description of the function
   */
  virtual void printMe(Interpreter *);
  /**
   * The number of inputs required by this function.
   */
  virtual int inputArgCount() {return argCount;}
  /**
   * The number of outputs returned by this function.
   */
  virtual int outputArgCount() {return retCount;}
  /** 
   * Evaluate the function and return the values.
   */
  virtual ArrayVector evaluateFunc(Interpreter *, ArrayVector& , int, VariableTable *);
};

typedef void (*GenericFuncPointer)();
  
class ImportedFunctionDef : public FunctionDef {
public:
  /**
   * The number of return args for this function (either 0 or 1).
   */
  int retCount;
  /**
   * The number of input args for this function (cannot be variable).
   */
  int argCount;
  /**
   * The pointer to the function to be called.
   */
  GenericFuncPointer address;
  /**
   * The call interface object
   */
  ffi_cif cif;
  /**
   * The types of each argument
   */
  StringVector types;
  /**
   * The guard expressions associated with each argument
   */
  TreeList sizeCheckExpressions;
  /**
   * The return type of the function
   */
  QString retType;
  /**
   * Default constructor
   */
  ImportedFunctionDef(GenericFuncPointer address_arg,
		      StringVector types_arg,
		      StringVector arguments_arg,
		      TreeList sizeChecks,
		      QString retType_arg,
		      QString name);
  /**
   * Default destructor
   */
  ~ImportedFunctionDef();
  /**
   * The type of the function is FM_IMPORTED_FUNCTION.
   */
  virtual const FunctionType type() {return FM_IMPORTED_FUNCTION;}
  virtual QString functionName() {return name;}
  virtual QString detailedName() {return "imported";}
  /** Print a description of the function
   */
  virtual void printMe(Interpreter *);
  /**
   * The number of inputs required by this function.
   */
  virtual int inputArgCount() {return argCount;}
  /**
   * The number of outputs returned by this function.
   */
  virtual int outputArgCount() {return retCount;}
  /** 
   * Evaluate the function and return the values.
   */
  virtual ArrayVector evaluateFunc(Interpreter *, ArrayVector& , int, VariableTable *);    
  /**
   * Helper function -- is the ith argument passed as a pointer
   */
  bool isPassedAsPointer(int arg);
};

typedef void (*mexFuncPtr)(int, mxArray**, int, const mxArray**);

class MexFunctionDef : public FunctionDef {
public:
  /**
   * The full name of the library to link to
   */
  QString fullname;
  /**
   * The dynamic library object
   */
  DynLib *lib;
  /**
   * The following flag is set to true if the library is
   * successfully imported
   */
  bool importSuccess;
  /**
   * The pointer to the function to be called.
   */
  mexFuncPtr address;
  /**
   * Default constructor
   */
  MexFunctionDef(QString fullpathname);
  /**
   * Default destructor
   */
  ~MexFunctionDef();
  bool LoadSuccessful();
  /**
   * The type of the function is FM_MEX_FUNCTION.
   */
  virtual const FunctionType type() {return FM_MEX_FUNCTION;}
  virtual QString functionName() {return name;}
  virtual QString detailedName() {return "mex";}

  /** Print a description of the function
   */
  virtual void printMe(Interpreter *);
  /**
   * The number of inputs required by this function.
   */
  virtual int inputArgCount() {return -1;}
  /**
   * The number of outputs returned by this function.
   */
  virtual int outputArgCount() {return -1;}
  /** 
   * Evaluate the function and return the values.
   */
  virtual ArrayVector evaluateFunc(Interpreter *, ArrayVector& , int, VariableTable*);    
};

// This used to be a simple typedef to a pointer of a functiondef
// Now, it is a reference counted class.
class FuncPtr {
private:
  FunctionDef* d;
public:
  FuncPtr() : d(NULL) {}
  ~FuncPtr() {
    if (d) {
      d->unlock();
      if (!d->referenced()) delete d;
    }
  }
  FuncPtr(SpecialFunctionDef* ptr) {
    d = ptr;
    if (d)
      d->lock();
  }
  FuncPtr(BuiltInFunctionDef* ptr) {
    d = ptr;
    if (d)
      d->lock();
  }
  FuncPtr(FunctionDef* ptr) {
    d = ptr;
    if (d)
      d->lock();
  }
  FuncPtr(const FuncPtr &copy) {
    d = copy.d;
    if (d)
      d->lock();
  }
  FuncPtr& operator=(const FuncPtr &copy) {
    if (copy.d == d)
      return *this;
    if (d) {
      d->unlock();
      if (!d->referenced()) delete d;
    }
    d = copy.d;
    if (d)
      d->lock();
    return *this;
  }
  FunctionDef* operator->() const {
    return d;
  }
  FunctionDef& operator*() const {
    return *d;
  }
  bool operator!() const {
    return (d == NULL);
  }
  operator FunctionDef* () const {return d;}
  operator MFunctionDef* () const {return ((MFunctionDef*)d);}
};


#endif
