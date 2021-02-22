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
#ifndef __RefVec_hpp__
#define __RefVec_hpp__

#include <vector>
#include <iostream>
#include "Exception.hpp"
using namespace std;

template <class T>
class RefVec;

template <class T>
class RefVecNode {
private:
  vector<T> data;
  int count;
  friend class RefVec<T>;
  RefVecNode() : count(0) {}
};

template <class T>
class RefVec {
  RefVecNode<T> *p;
  inline void Release() {
    if (p) {
      p->count--;
      if (p->count == -1) {
	cout << "ERROR\n";
      }	
      if (p->count <=0) 
	delete p;
    }
  }
  inline void Allocate() {
    p = new RefVecNode<T>;
    p->count++;
  }
  inline void Duplicate() {
    RefVecNode<T> *q = new RefVecNode<T>;
    q->data = p->data;
    q->count = 1;
    Release();
    p = q;
  }
public:
  inline RefVec() {
    p = NULL;
  }
  inline RefVec(int cnt) {
    Allocate();
    p->data = std::vector<T>(cnt);
  }
  inline RefVec(const RefVec& copy) {
    p = copy.p;
    if (p) p->count++;
  }
  inline ~RefVec() {
    Release();
  }
  inline void operator=(const RefVec& copy) {
    Release();
    p = copy.p;
    if (p) p->count++;
  }
  inline bool operator==(const RefVec& copy) {
    if ((!p) && (!copy.p)) return true;
    if ((!p) || (!copy.p)) return false;
    return (p->data == copy.p->data);
  }
  inline unsigned size() const {
    if (!p) return 0;
    return p->data.size();
  }
  inline int begin() {
    return 0;
  }
  inline void push_front(const T& x) {
    if (!p) Allocate();
    if (p->count > 1) Duplicate();
    p->data.insert(p->data.begin(),x);
  }
  inline void append(const RefVec<T>& x) {
    for (unsigned p=0;p<x.size();p++)
      push_back(x[p]);
  }
  inline void operator+=(const RefVec<T>& x) {
    append(x);
  }
  inline void push_back(const T& x) {
    if (!p) Allocate();
    if (p->count > 1) Duplicate();
    p->data.push_back(x);
  }
  inline void pop_back() {
    if (p) {
      if (p->count > 1) Duplicate();
      p->data.pop_back();
    }
  }
  inline void pop_front() {
    if (p) {
      if (p->count > 1) Duplicate();
      p->data.erase(p->data.begin());
    }
  }
  inline void clear() {
    if (p) {
      if (p->count > 1) Duplicate();
      p->data.clear();
    }
  }
  inline bool empty() const {
    if (!p) return true;
    return p->data.empty();
  }
  inline T& back() {
    if (!p)
      throw Exception("Illegal internal array access!");
    if (p->count > 1) Duplicate();
    return p->data.back();  
  }
  inline T& front() {
    if (!p)
      throw Exception("Illegal internal array access!");
    if (p->count > 1) Duplicate();
    return p->data.front();  
  }
  inline T& operator[] (unsigned n) {
    if (!p)
      throw Exception("Illegal internal array access!");
    if (p->count > 1) Duplicate();
    return p->data[n];
  }
  inline T& at(unsigned n) {
    if (!p)
      throw Exception("Illegal internal array access!");
    if (p->count > 1) Duplicate();
    return p->data.at(n);
  }
  inline void erase(unsigned n) {
    if (p) {
      if (p->count > 1) Duplicate();
      p->data.erase(p->data.begin()+n);
    }
  }
  inline const T& back() const {
    if (!p)
      throw Exception("Illegal internal array access!");
    return p->data.back();  
  }
  inline const T& front() const {
    if (!p)
      throw Exception("Illegal internal array access!");
    return p->data.front();  
  }
  inline const T& operator[] (unsigned n) const {
    if (!p)
      throw Exception("Illegal internal array access!");
    return p->data[n];
  }
  inline const T& at(unsigned n) const {
    if (!p)
      throw Exception("Illegal internal array access!");
    return p->data.at(n);
  }
};

#endif
