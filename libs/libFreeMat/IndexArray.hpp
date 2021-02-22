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
#ifndef __IndexArray_hpp__
#define __IndexArray_hpp__

#include "BasicArray.hpp"

BasicArray<bool> GetDeletionMap(const IndexArray& vec, index_t length);

bool IsColonOp(const IndexArray& ndx);

IndexArray ExpandColons(const IndexArray& ndx, index_t len);

bool isSliceIndexCase(const NTuple &dims, const IndexArrayVector& index);

bool DimensionCovered(const IndexArray& ndx, index_t length);

index_t getSliceIndex(const NTuple &dimensions, const IndexArrayVector& index);

IndexArray ScalarToIndex(index_t val);

IndexArrayVector ScalarToIndex(const NTuple& val);

#endif
