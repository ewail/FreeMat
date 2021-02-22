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
#ifndef __Vector_hpp__
#define __Vector_hpp__

#include <QSharedData>
#include <new>
#include "Exception.hpp"

inline size_t compute_initial_capacity(size_t len)
{
  return len*2;
}

template <typename T>
inline void initialize_memory(T* ptr, size_t len)
{
  if (sizeof(T) <= 8)
    memset(ptr,0,sizeof(T)*len);
}

template <typename T>
inline void copy_memory(T* dest, const T* src, size_t count)
{
  if (sizeof(T) <= 8)
    memcpy(dest,src,count*sizeof(T));
  else
    for (int i=0;i<count;i++)
      dest[i] = src[i];
}


template <typename T>
class VectorData : public QSharedData
{
  T* m_data;
  size_t m_len;
  size_t m_capacity;

  void allocate(size_t len) {
    try {
      m_data = new T[len];
      initialize_memory(m_data,len);
    } catch (const std::bad_alloc& ) {
      throw Exception(QString("Cannot allocate enough memory to store an array of size %1").arg(m_len));
    }
  }
  void copyvec(const VectorData& copy) {
    if (m_data) delete[] m_data;
    m_len = copy.m_len;
    m_capacity = copy.m_capacity;
    if (m_len) {
      allocate(m_capacity);
      copy_memory<T>(m_data,copy.m_data,m_len);
    }
  }
public:
  inline VectorData() : m_data(0), m_len(0), m_capacity(0) {}
  inline VectorData(size_t dim) {
    try {
      m_len = dim;
      m_capacity = compute_initial_capacity(m_len);
      allocate(m_capacity);
    } catch (const std::bad_alloc& ) {
      throw Exception(QString("Cannot allocate enough memory to store an array of size %1").arg(dim));
    }
  }
  inline VectorData(const VectorData& copy) : QSharedData(copy), m_data(0), m_len(0), m_capacity(0) {
    copyvec(copy);
  }
  inline ~VectorData() {
    if (m_data) delete[] m_data;
  }
  inline void resize(size_t new_len) {
    if (new_len <= m_capacity)
      m_len = new_len;
    else
      {
	try {
	  size_t new_capacity = compute_initial_capacity(new_len);
	  T* new_data = new T[new_capacity];
	  initialize_memory(new_data,new_capacity);
	  copy_memory(new_data,m_data,m_len);
	  delete[] m_data;
	  m_len = new_len;
	  m_capacity = new_capacity;
	  m_data = new_data;
	} catch (const std::bad_alloc&) {
	  throw Exception(QString("Cannot allocate enough memory to resize array to size %1").arg(new_len));
	}
      }
  }
  inline T operator[](size_t p) const {
    return m_data[p];
  }
  inline T& operator[](size_t p) {
    return m_data[p];
  }
  inline T at(size_t p) const {
    return m_data[p];
  }
  inline T* data() {
    return m_data;
  }
  inline const T* data() const {
    return m_data;
  }
  inline void clear() {
    m_len = 0;
    if (m_data) 
      delete[] m_data;
    m_data = 0;
  }
};

template <typename T>
class Vector
{
public:
  inline Vector() {d = new VectorData<T>();}
  inline Vector(size_t dim) {d = new VectorData<T>(dim);}
  inline Vector(const Vector& copy) : d(copy.d) {}
  inline T operator[](size_t p) const {
    return (*d)[p];
  }
  inline T& operator[](size_t p) {
    return (*d)[p];
  }
  inline T at(size_t p) const {
    return d->at[p];
  }
  inline T* data() {
    return d->data();
  }
  inline const T* data() const {
    return d->data();
  }
  inline void clear() {
    d->clear();
  }
  inline void resize(size_t newlen) {
    d->resize(newlen);
  }
private:
  QSharedDataPointer<VectorData<T> > d;
};

#endif
