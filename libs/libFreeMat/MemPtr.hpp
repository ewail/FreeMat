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

#ifndef __MemPtr_hpp__
#define __MemPtr_hpp__

template <class T> class MemBlock {
  T* ptr;
public:
  MemBlock(size_t count) {ptr = new T[count]; memset(ptr,0,sizeof(T)*count);}
  ~MemBlock() {delete [] ptr;}
  T* Pointer() {return ptr;}
  T* operator&() {return ptr;}
  T operator[](size_t n) {return ptr[n];}
};

#define MacroBlockAlloc(ctype,csize,tmpname,name)	\
  MemBlock<ctype> tmpname(csize); ctype *name = &tmpname;

#endif
