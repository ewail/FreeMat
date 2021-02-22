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
#ifndef __GetSet_hpp__
#define __GetSet_hpp__

#include "IndexArray.hpp"

template <typename T>
void SetSlice(T& arg, const IndexArrayVector& index, const T& data) {
  index_t offset = getSliceIndex(arg.dimensions(),index);
  for (index_t i=1;i<=arg.dimensions()[0];i++)
    arg.set(offset+i,data.get(i));
}

template <typename T, typename S>
void SetSlice(T& arg, const IndexArrayVector& index, const S& data) {
  index_t offset = getSliceIndex(arg.dimensions(),index);
  for (index_t i=1;i<=arg.dimensions()[0];i++)
    arg.set(offset+i,data);  
}

template <typename T>
void Set(T& arg, const IndexArray& index, const T& data) {
  if (index.isEmpty()) return;
  if (data.isEmpty()) {
    arg.del(index);
    return;
  }
  if (IsColonOp(index)) {
    NTuple myDims;
    if (!arg.isEmpty())
      myDims = arg.dimensions();
    else
      myDims = data.dimensions();
    if (myDims.count() != data.length()) 
      throw Exception("assignment A(:) = B requires A and B to be the same size");
    T retvec(data);
    retvec.reshape(myDims);
    arg = retvec;
    return;
  }
  index_t max_ndx = MaxValue(index);
  if (max_ndx > arg.length()) arg.resize(max_ndx);
  if (index.length() != data.length()) 
    throw Exception("Assignment A(I) = B requires I and B to be the same size");
  for (index_t i=1;i<=index.length();i++)
    arg.set(index.get(i),data.get(i));
}


template <typename T>
void Set(T& arg, const IndexArrayVector& index, const T& data) {
  if (index.empty()) return;
  if (data.isEmpty()) {
    arg.del(index);
    return;
  }
  if (!arg.isEmpty() && isSliceIndexCase(arg.dimensions(),index)) {
    SetSlice(arg,index,data);
    return;
  }
  IndexArrayVector ndx;
  NTuple secdims;
  NTuple maxsze;
  if (arg.isEmpty()) {
    // Special case when A(NDX) = B, and A is empty
    if (data.isVector()) {
      bool firstcolon = true;
      for (int i=0;i<index.size();i++)
	if (IsColonOp(index[i])) {
	  if (firstcolon) {
	    ndx.push_back(ExpandColons(index[i],data.length()));
	    firstcolon = false;
	  } else
	    ndx.push_back(ScalarToIndex(1));
	} else {
	  ndx.push_back(index[i]);
	}
    } else {
      // Correction - 
      // In the assignment of the form:
      //  g(2,:,4,:) = fs;
      // we fill in the colons with the first and second dimensional sizes of fs
      int colonDim = 0;
      for (int i=0;i<index.size();i++) {
	if (IsColonOp(index[i]))
	  ndx.push_back(ExpandColons(index[i],data.dimensions()[colonDim++]));
	else
	  ndx.push_back(index[i]);
      }
    }
    for (int i=0;i<ndx.size();i++) {
      secdims[i] = ndx[i].length();
      maxsze[i] = MaxValue(ndx[i]);
    }
  } else {
    for (int i=0;i<index.size();i++) {
      ndx.push_back(ExpandColons(index[i],arg.dimensions()[i]));
      secdims[i] = ndx[i].length();
      maxsze[i] = MaxValue(ndx[i]);
    }
  }
  
  if (secdims.count() != data.length())
    throw Exception("mismatch in size for assignment A(I1,I2,...) = B");
  if (!(maxsze <= arg.dimensions()))
    arg.resize(max(arg.dimensions(),maxsze));
  NTuple pos(1,1);
  index_t j=1;
  while (pos <= secdims) {
    NTuple qp;
    for (int i=0;i<index.size();i++)
      qp[i] = ndx[i].get(pos[i]);
    arg.set(qp,data.get(j++));
    secdims.increment(pos);
  }
}

template <typename T, typename S>
void Set(T& arg, const IndexArrayVector& index, const S& data) {
  if (index.empty()) return;
  if (!arg.isEmpty() && isSliceIndexCase(arg.dimensions(),index)) {
    SetSlice(arg,index,data);
    return;
  }
  IndexArrayVector ndx;
  NTuple secdims;
  NTuple maxsze;
  for (int i=0;i<index.size();i++) {
    if (arg.isEmpty())
      ndx.push_back(ExpandColons(index[i],1));
    else
      ndx.push_back(ExpandColons(index[i],arg.dimensions()[i]));
    secdims[i] = ndx[i].length();
    maxsze[i] = MaxValue(ndx[i]);
  }
  if (!(maxsze <= arg.dimensions()))
    arg.resize(max(arg.dimensions(),maxsze));
  NTuple pos(1,1);
  while (pos <= secdims) {
    NTuple qp;
    for (int i=0;i<index.size();i++)
      qp[i] = ndx[i].get(pos[i]);
    arg.set(qp,data);
    secdims.increment(pos);
  }
}

template <typename T, typename S>
void Set(T& arg, const IndexArray& index, const S& data) {
  if (index.isEmpty()) return;
  if (IsColonOp(index)) {
    if (arg.isEmpty())
      arg.set(1,data);
    else
      for (index_t i=1;i<=arg.length();i++) 
	arg.set(i,data);
    return;
  } 
  index_t max_ndx = MaxValue(index);
  if (max_ndx > arg.length()) arg.resize(max_ndx);
  for (index_t i=1;i<=index.length();i++)
    arg.set(index.get(i),data);  
}

template <typename T>
const T Get(const T& arg, const IndexArray& index) {
  if (index.isEmpty()) return T(index.dimensions());
  if (IsColonOp(index)) {
    T retvec(arg);
    retvec.reshape(NTuple(arg.length(),1));
    return retvec;
  }
  if (arg.isEmpty())
    throw Exception("Cannot index into empty variable,");
  NTuple retdims;
  if (arg.isColumnVector() && index.isRowVector())
    retdims = NTuple(index.length(),1);
  else
    retdims = index.dimensions();
  T retvec(retdims);
  for (index_t i=1;i<=retvec.length();i++)
    retvec.set(i,arg.get(index.get(i)));
  return retvec;
}

template <typename T>
const T Get(const T& arg, const IndexArrayVector& index) {
  if (index.empty()) return T(NTuple(0,0));
  if (isSliceIndexCase(arg.dimensions(),index)) return arg.slice(index);
  IndexArrayVector ndx;
  NTuple outdims;
  for (int i=0;i<index.size();i++) {
    ndx.push_back(ExpandColons(index[i],arg.dimensions()[i]));
    outdims[i] = ndx[i].length();
  }
  T retval(outdims);
  NTuple pos(1,1);
  while (pos <= outdims) {
    NTuple qp;
    for (int i=0;i<index.size();i++) 
      qp[i] = ndx[i].get(pos[i]);
    retval.set(pos,arg.get(qp));
    outdims.increment(pos);
  }
  return retval;
}

#endif
