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
#include "Struct.hpp"
#include "MemPtr.hpp"
#include <QtCore>
#include "Algorithms.hpp"
#include "FuncPtr.hpp"
#include "AnonFunc.hpp"

//@@Signature
//function permute PermuteFunction jitsafe
//inputs x p
//outputs y
//DOCBLOCK array_permute
ArrayVector PermuteFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 2) throw Exception("permute requires 2 inputs, the array to permute, and the permutation vector");
  Array permutation(arg[1].asDenseArray().toClass(UInt32));
  const BasicArray<uint32> &perm_dp(permutation.constReal<uint32>());
  uint32 max_perm_value = MaxValue(perm_dp);
  uint32 min_perm_value = MinValue(perm_dp);
  if ((max_perm_value != permutation.length()) || (min_perm_value != 1))
    throw Exception("second argument is not a valid permutation");
  MemBlock<bool> p(max_perm_value);
  bool *d = &p;
  for (index_t i=1;i<=perm_dp.length();i++) 
    d[perm_dp[i]-1] = true;
  for (uint32 i=0;i<max_perm_value;i++)
    if (!d[i]) throw Exception("second argument is not a valid permutation");
  // Convert to an N-Tuple
  NTuple perm(ConvertArrayToNTuple(permutation));
  // Post-fill the N-Tuple so that the permutation covers all of the dimensions
  for (int i=permutation.length();i<NDims;i++)
    perm[i] = (i+1);
  return ArrayVector(Permute(arg[0],perm));
}

//@@Signature
//function repmat RepMatFunction jitsafe
//inputs x rows cols
//outputs y
//DOCBLOCK array_repmat

template <typename T>
static BasicArray<T> RepMat(const BasicArray<T> &dp, const NTuple &outdim, const NTuple &repcount) {
  // Copy can work by pushing or by pulling.  I have opted for
  // pushing, because we can push a column at a time, which might
  // be slightly more efficient.
  index_t colsize = dp.rows();
  index_t colcount = dp.length()/colsize;
  // copySelect stores which copy we are pushing.
  NTuple originalSize(dp.dimensions());
  NTuple copySelect(1,1);
  // anchor is used to calculate where this copy lands in the output matrix
  // sourceAddress is used to track which column we are pushing in the
  // source matrix
  index_t copyCount = repcount.count();
  BasicArray<T> x(outdim);
  for (index_t i=1;i<=copyCount;i++) {
    // Reset the source address
    NTuple sourceAddress(1,1);
    // Next, we loop over the columns of the source matrix
    for (index_t j=1;j<=colcount;j++) {
      NTuple anchor;
      // We can calculate the anchor of this copy by multiplying the source
      // address by the copySelect vector
      for (int k=0;k<NDims;k++)
	anchor[k] = (copySelect[k]-1)*originalSize[k]+sourceAddress[k];
      // Now, we map this to a point in the destination array
      index_t destanchor = outdim.map(anchor);
      // And copy the elements
      for (index_t n=1;n<=colsize;n++)
	x[destanchor+n-1] = dp[(j-1)*colsize+n];
      // Now increment the source address
      originalSize.increment(sourceAddress,0);
    }
    repcount.increment(copySelect);
  }
  return x;
}

template <typename T>
static SparseMatrix<T> RepMat(const SparseMatrix<T>& dp, const NTuple &outdim, 
			      const NTuple &repcount) {
  if (repcount.lastNotOne() > 2)
    throw Exception("repmat cannot create N-dimensional sparse arrays");
  SparseMatrix<T> retvec(outdim);
  for (int rowcopy=0;rowcopy < repcount[0];rowcopy++)
    for (int colcopy=0;colcopy < repcount[1];colcopy++) {
      ConstSparseIterator<T> iter(&dp);
      while (iter.isValid()) {
	retvec.set(NTuple(iter.row()+rowcopy*dp.rows(),
			  iter.col()+colcopy*dp.cols()),
		   iter.value());
	iter.next();
      }
    }
  return retvec;
}

template <typename T>
static Array RepMat(const Array &dp, const NTuple &outdim, const NTuple &repcount) {
  if (dp.isScalar()) {
    if (dp.allReal()) 
      return Array(Uniform(outdim,dp.constRealScalar<T>()));
    else
      return Array(Uniform(outdim,dp.constRealScalar<T>()),
		   Uniform(outdim,dp.constImagScalar<T>()));
  }
  if (dp.isSparse()) {
    if (dp.allReal())
      return Array(RepMat(dp.constRealSparse<T>(),outdim,repcount));
    else
      return Array(RepMat(dp.constRealSparse<T>(),outdim,repcount),
		   RepMat(dp.constImagSparse<T>(),outdim,repcount));
  }
  if (dp.allReal())
    return Array(RepMat(dp.constReal<T>(),outdim,repcount));
  else
    return Array(RepMat(dp.constReal<T>(),outdim,repcount),
		 RepMat(dp.constImag<T>(),outdim,repcount));
}

static Array RepMatCell(const Array &dp, const NTuple &outdim, const NTuple &repcount) {
  return Array(RepMat<Array>(dp.constReal<Array>(),outdim,repcount));
}

static Array RepMatStruct(const StructArray& dp, const NTuple &outdim, const NTuple &repcount) {
  StructArray ret(dp);
  for (int i=0;i<ret.fieldCount();i++)
    ret[i] = RepMat<Array>(ret[i],outdim,repcount);
  ret.updateDims();
  return Array(ret);
}

#define MacroRepMat(ctype,cls)					\
  case cls: return ArrayVector(RepMat<ctype>(x,outdims,repcount));

ArrayVector RepMatFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 2)
    throw Exception("repmat function requires at least two arguments");
  Array x(arg[0]);
  NTuple repcount;
  // Case 1, look for a scalar second argument
  if ((arg.size() == 2) && (arg[1].isScalar())) {
    Array t(arg[1]);
    repcount[0] = t.asInteger();
    repcount[1] = t.asInteger();
  } 
  // Case 2, look for two scalar arguments
  else if ((arg.size() == 3) && (arg[1].isScalar()) && (arg[2].isScalar())) {
    repcount[0] = arg[1].asInteger();
    repcount[1] = arg[2].asInteger();
  }
  // Case 3, look for a vector second argument
  else {
    if (arg.size() > 2) throw Exception("unrecognized form of arguments for repmat function");
    repcount = ConvertArrayToNTuple(arg[1]);
  }
  if (!repcount.isValid())
    throw Exception("negative replication counts not allowed in argument to repmat function");
  // All is peachy.  Allocate an output array of sufficient size.
  NTuple outdims;
  for (int i=0;i<NDims;i++)
    outdims[i] = x.dimensions()[i]*repcount[i];
  if (x.isEmpty()) {
    Array p(arg[0]);
    p.reshape(outdims);
    return ArrayVector(p);
  }
  switch (x.dataClass()) {
  default: throw Exception("Unhandled type for repmat");
    MacroExpandCasesNoCell(MacroRepMat);
  case CellArray:
    return ArrayVector(RepMatCell(x,outdims,repcount));
  case Struct:
    return ArrayVector(RepMatStruct(x.constStructPtr(),outdims,repcount));
  }
}

//@@Signature
//function diag DiagFunction jitsafe
//inputs x n
//outputs y
//DOCBLOCK array_diag
ArrayVector DiagFunction(int nargout, const ArrayVector& arg) {
  // First, get the diagonal order, and synthesize it if it was
  // not supplied
  int diagonalOrder;
  if (arg.size() == 0)
    throw Exception("diag requires at least one argument.\n");
  if (arg.size() == 1) 
    diagonalOrder = 0;
  else {
    if (!arg[1].isScalar()) 
      throw Exception("second argument must be a scalar.\n");
    diagonalOrder = arg[1].asInteger();
  }
  // Next, make sure the first argument is 2D
  if (!arg[0].is2D()) 
    throw Exception("First argument to 'diag' function must be 2D.\n");
  // Case 1 - if the number of columns in a is 1, then this is a diagonal
  // constructor call.
  if (arg[0].isVector())
    {
      Array a = arg[0];
      a.ensureNotScalarEncoded();
      return ArrayVector(DiagonalArray(a,diagonalOrder));
    }
  else
    return ArrayVector(GetDiagonal(arg[0],diagonalOrder));
}

//@@Signature
//sfunction cellfun CellFunFunction
//inputs varargin
//output varargout
//DOCBLOCK array_cellfun
static ArrayVector CellFunNonuniformAnon(int nargout, const ArrayVector &arg,
					  Interpreter *eval, NTuple argdims,
					  int argcount, Array fun)
{
  Array outputs(CellArray, argdims);
  BasicArray<Array> &op = outputs.real<Array>();
  for (int i=0;i<argdims.count();i++)
    {
      ArrayVector input;
      input.push_back(fun);
      for (int j=1;j<argcount;j++)
	input.push_back(ArrayFromCellArray(arg[j].get(i+1)));
      ArrayVector ret = AnonFuncFevalFunction(nargout,input,eval);
      Array g = CellArrayFromArrayVector(ret);
      op[i+1] = g;
    }
  return outputs;  
}

static ArrayVector CellFunNonuniform(int nargout, const ArrayVector &arg,
				      Interpreter *eval, NTuple argdims,
				      int argcount, FuncPtr fptr)
{
  Array outputs(CellArray, argdims);
  BasicArray<Array> &op = outputs.real<Array>();
  for (int i=0;i<argdims.count();i++)
    {
      ArrayVector input;
      for (int j=1;j<argcount;j++)
	input.push_back(ArrayFromCellArray(arg[j].get(i+1)));
      ArrayVector ret = fptr->evaluateFunc(eval,input,fptr->outputArgCount());
      Array g = CellArrayFromArrayVector(ret);
      op[i+1] = g;
    }
  return outputs;  
}

static ArrayVector CellFunUniformAnon(int nargout, const ArrayVector &arg,
				       Interpreter *eval, NTuple argdims,
				       int argcount, Array fun)
{
  ArrayVector outputs;
  for (int i=0;i<argdims.count();i++)
    {
      ArrayVector input;
      input.push_back(fun);
      for (int j=1;j<argcount;j++)
	input.push_back(ArrayFromCellArray(arg[j].get(i+1)));
      ArrayVector ret = AnonFuncFevalFunction(nargout,input,eval);
      if (ret.size() < nargout)
	throw Exception("function returned fewer outputs than expected");
      if (i==0)
	for (int j=0;j<nargout;j++)
	  {
	    if (!ret[j].isScalar()) throw Exception("function returned non-scalar result");
	    outputs.push_back(ret[j]);
	    outputs[j].resize(argdims);
	  }
      else
	for (int j=0;j<nargout;j++)
	  outputs[j].set(i+1,ret[j]);
    }
  return outputs;
}

static ArrayVector CellFunUniform(int nargout, const ArrayVector &arg,
				   Interpreter *eval, NTuple argdims,
				   int argcount, FuncPtr fptr)
{
  ArrayVector outputs;
  for (int i=0;i<argdims.count();i++)
    {
      ArrayVector input;
      for (int j=1;j<argcount;j++)
	input.push_back(ArrayFromCellArray(arg[j].get(i+1)));
      ArrayVector ret = fptr->evaluateFunc(eval,input,nargout);
      if (ret.size() < nargout)
	throw Exception("function returned fewer outputs than expected");
      if (i==0)
	for (int j=0;j<nargout;j++)
	  {
	    if (!ret[j].isScalar()) throw Exception("function returned non-scalar result");
	    outputs.push_back(ret[j]);
	    outputs[j].resize(argdims);
	  }
      else
	for (int j=0;j<nargout;j++)
	  outputs[j].set(i+1,ret[j]);
    }
  return outputs;
}

ArrayVector CellFunFunction(int nargout, const ArrayVector& arg, 
			     Interpreter*eval) {
  if (arg.size() < 2) return ArrayVector(); // Don't bother
  // Remove the key/value properties
  int argcount = arg.size();
  Array errorHandler;
  bool uniformOutput = true; // We assume this to be the case
  bool foundNVP = true;
  bool customEH = false;
  while (foundNVP && (argcount >=2))
    {
      foundNVP = false;
      if (arg[argcount-2].isString() &&
	  (arg[argcount-2].asString() == "UniformOutput"))
	{
	  uniformOutput = arg[argcount-1].asInteger();
	  argcount-=2;
	  foundNVP = true;
	}
      if (arg[argcount-2].isString() &&
	  (arg[argcount-2].asString() == "ErrorHandler"))
	{
	  errorHandler = arg[argcount-1];
	  customEH = true;
	  argcount-=2;
	  foundNVP = true;
	}
    }
  if (argcount < 2) return ArrayVector();
  NTuple argdims = arg[1].dimensions();
  for (int i=1;i<argcount;i++)
    {
      if (arg[i].dimensions() != argdims)
	throw Exception("All arguments must match dimensions");
      if (arg[i].dataClass() != CellArray)
	throw Exception("All arguments must be cell arrays");
    }
  FuncPtr eh;
  if (customEH) 
    {
      eh = FuncPtrLookup(eval,errorHandler);
      eh->updateCode(eval);
      eval->setTryCatchActive(true);
    }
  try
    {
      if (arg[0].className() == "anonfunction")
	{
	  if (nargout == 0) nargout = 1;
	  if (uniformOutput)
	    return CellFunUniformAnon(nargout,arg,eval,argdims,argcount,arg[0]);
	  return CellFunNonuniformAnon(nargout,arg,eval,argdims,argcount,arg[0]);
	}
      else
	{
	  // Map the first argument to a function ptr
	  FuncPtr fptr = FuncPtrLookup(eval,arg[0]);
	  fptr->updateCode(eval);
	  if (nargout == 0) nargout = 1;
	  if (uniformOutput)
	    return CellFunUniform(nargout,arg,eval,argdims,argcount,fptr);
	  return CellFunNonuniform(nargout,arg,eval,argdims,argcount,fptr);
	}
    }
  catch (Exception &e)
    {
      if (customEH)
	{
	  ArrayVector input;
	  return eh->evaluateFunc(eval,input,1);
	}
      else
	throw;
    }
}


//@@Signature
//sfunction arrayfun ArrayFunFunction
//inputs varargin
//output varargout
//DOCBLOCK array_arrayfun

static ArrayVector ArrayFunNonuniformAnon(int nargout, const ArrayVector &arg,
					  Interpreter *eval, NTuple argdims,
					  int argcount, Array fun)
{
  Array outputs(CellArray, argdims);
  BasicArray<Array> &op = outputs.real<Array>();
  for (int i=0;i<argdims.count();i++)
    {
      ArrayVector input;
      input.push_back(fun);
      for (int j=1;j<argcount;j++)
	input.push_back(arg[j].get(i+1));
      ArrayVector ret = AnonFuncFevalFunction(nargout,input,eval);
      Array g = CellArrayFromArrayVector(ret);
      op[i+1] = g;
    }
  return outputs;  
}

static ArrayVector ArrayFunNonuniform(int nargout, const ArrayVector &arg,
				      Interpreter *eval, NTuple argdims,
				      int argcount, FuncPtr fptr)
{
  Array outputs(CellArray, argdims);
  BasicArray<Array> &op = outputs.real<Array>();
  for (int i=0;i<argdims.count();i++)
    {
      ArrayVector input;
      for (int j=1;j<argcount;j++)
	input.push_back(arg[j].get(i+1));
      ArrayVector ret = fptr->evaluateFunc(eval,input,fptr->outputArgCount());
      Array g = CellArrayFromArrayVector(ret);
      op[i+1] = g;
    }
  return outputs;  
}

static ArrayVector ArrayFunUniformAnon(int nargout, const ArrayVector &arg,
				       Interpreter *eval, NTuple argdims,
				       int argcount, Array fun)
{
  ArrayVector outputs;
  for (int i=0;i<argdims.count();i++)
    {
      ArrayVector input;
      input.push_back(fun);
      for (int j=1;j<argcount;j++)
	input.push_back(arg[j].get(i+1));
      ArrayVector ret = AnonFuncFevalFunction(nargout,input,eval);
      if (ret.size() < nargout)
	throw Exception("function returned fewer outputs than expected");
      if (i==0)
	for (int j=0;j<nargout;j++)
	  {
	    if (!ret[j].isScalar()) throw Exception("function returned non-scalar result");
	    outputs.push_back(ret[j]);
	    outputs[j].resize(argdims);
	  }
      else
	for (int j=0;j<nargout;j++)
	  outputs[j].set(i+1,ret[j]);
    }
  return outputs;
}

static ArrayVector ArrayFunUniform(int nargout, const ArrayVector &arg,
				   Interpreter *eval, NTuple argdims,
				   int argcount, FuncPtr fptr)
{
  ArrayVector outputs;
  for (int i=0;i<argdims.count();i++)
    {
      ArrayVector input;
      for (int j=1;j<argcount;j++)
	input.push_back(arg[j].get(i+1));
      ArrayVector ret = fptr->evaluateFunc(eval,input,nargout);
      if (ret.size() < nargout)
	throw Exception("function returned fewer outputs than expected");
      if (i==0)
	for (int j=0;j<nargout;j++)
	  {
	    if (!ret[j].isScalar()) throw Exception("function returned non-scalar result");
	    outputs.push_back(ret[j]);
	    outputs[j].resize(argdims);
	  }
      else
	for (int j=0;j<nargout;j++)
	  outputs[j].set(i+1,ret[j]);
    }
  return outputs;
}

ArrayVector ArrayFunFunction(int nargout, const ArrayVector& arg, 
			     Interpreter*eval) {
  if (arg.size() < 2) return ArrayVector(); // Don't bother
  // Remove the key/value properties
  int argcount = arg.size();
  Array errorHandler;
  bool uniformOutput = true; // We assume this to be the case
  bool foundNVP = true;
  bool customEH = false;
  while (foundNVP && (argcount >=2))
    {
      foundNVP = false;
      if (arg[argcount-2].isString() &&
	  (arg[argcount-2].asString() == "UniformOutput"))
	{
	  uniformOutput = arg[argcount-1].asInteger();
	  argcount-=2;
	  foundNVP = true;
	}
      if (arg[argcount-2].isString() &&
	  (arg[argcount-2].asString() == "ErrorHandler"))
	{
	  errorHandler = arg[argcount-1];
	  customEH = true;
	  argcount-=2;
	  foundNVP = true;
	}
    }
  if (argcount < 2) return ArrayVector();
  NTuple argdims = arg[1].dimensions();
  for (int i=1;i<argcount;i++)
    if (arg[i].dimensions() != argdims)
      throw Exception("All arguments must match dimensions");
  FuncPtr eh;
  if (customEH) 
    {
      eh = FuncPtrLookup(eval,errorHandler);
      eh->updateCode(eval);
      eval->setTryCatchActive(true);
    }
  try
    {
      if (arg[0].className() == "anonfunction")
	{
	  if (nargout == 0) nargout = 1;
	  if (uniformOutput)
	    return ArrayFunUniformAnon(nargout,arg,eval,argdims,argcount,arg[0]);
	  return ArrayFunNonuniformAnon(nargout,arg,eval,argdims,argcount,arg[0]);
	}
      else
	{
	  // Map the first argument to a function ptr
	  FuncPtr fptr = FuncPtrLookup(eval,arg[0]);
	  fptr->updateCode(eval);
	  if (nargout == 0) nargout = 1;
	  if (uniformOutput)
	    return ArrayFunUniform(nargout,arg,eval,argdims,argcount,fptr);
	  return ArrayFunNonuniform(nargout,arg,eval,argdims,argcount,fptr);
	}
    }
  catch (Exception &e)
    {
      if (customEH)
	{
	  ArrayVector input;
	  return eh->evaluateFunc(eval,input,1);
	}
      else
	throw;
    }
}
