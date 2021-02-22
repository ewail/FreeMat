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
#ifndef __CommonIterator_hpp__
#define __CommonIterator_hpp__

#include "NTuple.hpp"
#include <QtGlobal>

template <typename S, typename T>
class BaseIterator {
  S *m_ptr;
  NTuple m_pos;
  int m_pinDim;
  NTuple m_dim;
  index_t m_ndx;
  index_t m_stride;
public:
  BaseIterator(S *ptr, int dim) : 
    m_ptr(ptr), m_pos(NTuple(1,1)), m_pinDim(dim),
    m_dim(ptr->dimensions()) {
    m_ndx = m_dim.map(m_pos);
    m_stride = m_dim.stride(m_pinDim);
  }
  inline void nextSlice() {
    m_dim.increment(m_pos,m_pinDim);
    m_ndx = m_dim.map(m_pos);
  }
  inline void next() {m_ndx += m_stride;}
  inline void prev() {m_ndx -= m_stride;}
  inline bool isValid() const {return (m_pos <= m_dim);}
  inline index_t size() const {return m_dim[m_pinDim];}
  inline void set(const T& val) {m_ptr->set(m_ndx,val);}
  inline const T get() const {return m_ptr->get(m_ndx);}
  inline NTuple pos() const {return m_pos;}
  ~BaseIterator() {}
};

template <typename S, typename T>
class ConstBaseIterator {
  const S *m_ptr;
  NTuple m_pos;
  int m_pinDim;
  NTuple m_dim;
  index_t m_ndx;
  index_t m_stride;
public:
  ConstBaseIterator(const S *ptr, int dim) : 
    m_ptr(ptr), m_pos(NTuple(1,1)), m_pinDim(dim),
    m_dim(ptr->dimensions()) {
    m_ndx = m_dim.map(m_pos);
    m_stride = m_dim.stride(m_pinDim);
  }
  inline void nextSlice() {
    m_dim.increment(m_pos,m_pinDim);
    m_ndx = m_dim.map(m_pos);
  }
  inline void next() {m_ndx += m_stride;}
  inline void prev() {m_ndx -= m_stride;}
  inline bool isValid() const {return (m_pos <= m_dim);}
  inline index_t size() const {return m_dim[m_pinDim];}
  inline const T get() const {return m_ptr->get(m_ndx);}
  inline NTuple pos() const {return m_pos;}
  ~ConstBaseIterator() {}
};

template <typename S, typename T>
class Transformer {
  BaseIterator<S,T> m_dest;
  ConstBaseIterator<S,T> m_source;
public:
  inline Transformer(S* dest, const S* src, int destdim = 0, int srcdim = 0)
    : m_dest(dest,destdim), m_source(src,srcdim) {}
  inline index_t size() const {return qMin(m_source.size(),m_dest.size());}
  inline bool isValid() const {return (m_dest.isValid() && m_source.isValid());}
  inline void next() {m_dest.next(); m_source.next();}
  inline void nextSlice() {m_dest.nextSlice(); m_source.nextSlice();}
  inline const T get() const {return m_source.get();}
  inline void set(const T& val) {m_dest.set(val);}
};

template <typename T>
class SpinIterator {
  const T &m_value;
public:
  SpinIterator(const T& value) : m_value(value) {}
  inline T get(index_t) const {return m_value;}
  inline T getNoBoundsCheck(index_t) const {return m_value;}
};

#endif



