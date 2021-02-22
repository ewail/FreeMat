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
#include "IndexArray.hpp"
#include "BasicArray.hpp"

index_t getSliceIndex(const NTuple &dimensions, const IndexArrayVector& index) {
  NTuple dim;
  dim[0] = 1;
  for (int i=1;i<index.size();i++)
    dim[i] = index[i][1];
  index_t offset = dimensions.map(dim)-1;
  return offset;
}

bool IsColonOp(const IndexArray& ndx) {
  if (!ndx.isScalar()) return false;
  return (ndx.get(1) == -1);
}

IndexArray ExpandColons(const IndexArray& ndx, index_t len) {
  if (!IsColonOp(ndx)) return ndx;
  IndexArray retvec(NTuple(len,1));
  for (int i=0;i<len;i++) 
    retvec.set(i+1,i+1);
  return retvec;
}

bool isSliceIndexCase(const NTuple &dims, const IndexArrayVector& index) {
  if (IsColonOp(index[0])) {
    bool allScalars = true;
    for (int i=1;i<index.size();i++)
      allScalars = allScalars && !IsColonOp(index[i]) && index[i].isScalar()
	&& (index[i][1] <= dims[1]);
    return allScalars;
  }
  return false;
}

BasicArray<bool> GetDeletionMap(const IndexArray& vec, index_t length) {
  BasicArray<bool> retvec(NTuple(length,1));
  for (index_t i=1;i<=vec.length();i++)
    retvec[vec[i]] = true;
  return retvec;
}

bool DimensionCovered(const IndexArray& ndx, index_t length) {
  return AllTrue(GetDeletionMap(ndx,length));
}

IndexArray ScalarToIndex(index_t val) {
  return IndexArray(val);
}

IndexArrayVector ScalarToIndex(const NTuple& val) {
  IndexArrayVector ret;
  for (int i=0;i<val.lastNotOne();i++)
    ret.push_back(IndexArray(val[i]));
  return ret;
}
