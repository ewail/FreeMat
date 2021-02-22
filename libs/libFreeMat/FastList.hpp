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
#ifndef __FastList_hpp__
#define __FastList_hpp__

#include <QVector>
#include "Exception.hpp"

template <typename T>
class FastList {
  T m_slot1;
  T m_slot2;
  QVector<T> *m_vec;
  int m_size;
public:
  inline FastList() : m_vec(0), m_size(0) {}
  inline FastList(const T& val) {
    m_vec = 0;
    m_size = 1;
    m_slot1 = val;
    m_slot2 = T();
  }
  inline FastList(const FastList &copy) : 
    m_slot1(copy.m_slot1),
    m_slot2(copy.m_slot2),
    m_size(copy.m_size) {
    if (copy.m_vec)
      m_vec = new QVector<T>(*copy.m_vec);
    else
      m_vec = 0;
  }

  inline FastList& operator=(const FastList& copy) {
    if (&copy == this) return *this;
    m_slot1 = copy.m_slot1;
    m_slot2 = copy.m_slot2;
    m_size = copy.m_size;
    if (copy.m_vec) {
      if( m_vec )
	delete m_vec;
      m_vec = new QVector<T>(*copy.m_vec);
    }
    return *this;
  }

  inline void clear() {
    if (m_size > 2) m_vec->clear();
    m_slot1 = T();
    m_slot2 = T();
    m_size = 0;
  }
  inline ~FastList() {
    if (m_vec) delete m_vec;
  }
  inline int size() const {return m_size;}
  inline void push_back(const T &el) {
    if (m_size == 0)
      m_slot1 = el;
    else if (m_size == 1)
      m_slot2 = el;
    else {
      if (!m_vec) m_vec = new QVector<T>();
      m_vec->push_back(el);
    }
    m_size++;
  }
  inline T& operator[](int i) {
    if (i == 0)
      return m_slot1;
    else if (i==1)
      return m_slot2;
    else if (m_vec)
      return m_vec->operator[](i-2);
    else
      throw Exception("Illegal operator[] access");
  }
  inline const T& operator[](int i) const {
    if (i == 0)
      return m_slot1;
    else if (i==1)
      return m_slot2;
    else if (m_vec)
      return m_vec->at(i-2);
    else
      throw Exception("Illegal operator[] access");
  }
  inline bool empty() const {
    return (m_size == 0);
  }
  inline void push_front(const T & value) {
    if (m_size == 0) {
      m_slot1 = value;
    } else if (m_size == 1) {
      m_slot2 = m_slot1;
      m_slot1 = value;
    } else {
      if (!m_vec) m_vec = new QVector<T>();
      m_vec->push_front(m_slot2);
      m_slot2 = m_slot1;
      m_slot1 = value;
    }
    m_size++;
  }
  inline const T& at(int i) const {
    if (i == 0)
      return m_slot1;
    else if (i == 1)
      return m_slot2;
    else if (m_vec)
      return m_vec->at(i-2);
    else
      throw Exception("Ilegal at() access");
  }
  inline void pop_front() {
    if (m_size == 0)
      return;
    else if (m_size == 1)
      m_slot1 = T();
    else if (m_size == 2) {
      m_slot1 = m_slot2;
      m_slot2 = T();
    } else if (m_vec) {
      m_slot1 = m_slot2;
      m_slot2 = m_vec->front();
      m_vec->pop_front();
    } else
      throw Exception("failure on pop_back in list");
    m_size--;
  }
  inline void pop_back() {
    if (m_size == 0)
      return;
    else if (m_size == 1)
      m_slot1 = T();
    else if (m_size == 2) {
      m_slot2 = T();
    } else if (m_vec) 
      m_vec->pop_back();
    else
      throw Exception("failure on pop_back in list");
    m_size--;
  }
  inline T& front() {
    if (m_size == 0)
      throw Exception("failure on front in list");
    return m_slot1;
  }
  inline const T& front() const {
    if (m_size == 0)
      throw Exception("failure on front in list");
    return m_slot1;
  }
  inline T& back() {
    if (m_size == 0)
      throw Exception("failure on back in list");
    else if (m_size == 1)
      return m_slot1;
    else if (m_size == 2)
      return m_slot2;
    else if (m_vec)
      return m_vec->back();
    else
      throw Exception("failure in back");
  }
  inline const T& back() const {
    if (m_size == 0)
      throw Exception("failure on back in list");
    else if (m_size == 1)
      return m_slot1;
    else if (m_size == 2)
      return m_slot2;
    else if (m_vec)
      return m_vec->back();
    else
      throw Exception("failure in back");
  }
  inline FastList mid(int start, int count) const {
    FastList ret;
    for (int i=start;i<start+count;i++) 
      ret.push_back(at(i));
    return ret;
  }
  inline FastList& operator<<(const T &other) {
    push_back(other);
    return *this;
  }
  inline FastList& operator+=(const FastList<T> &other) {
    for (int i=0;i<other.size();i++)
      push_back(other.at(i));
    return *this;
  }
  void printMe(std::ostream& o) const {
    for (int i=0;i<size();i++)
      o << at(i) << "\n";
  }
};

template <typename T>
std::ostream& operator<<(std::ostream& o, const FastList<T> &a) {
  a.printMe(o);
  return o;
}

#endif
