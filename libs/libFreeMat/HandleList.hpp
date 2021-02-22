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

#ifndef __HandleList_hpp__
#define __HandleList_hpp__
#include <map>
#include <QMutex>
#include <QMutexLocker>
#include "Exception.hpp"
#include <QFile>

template<class T>
class HandleList {
  typedef T value_type;
  std::map<unsigned int, T, std::less<unsigned int> > handles;
  unsigned int max_handle;
  QMutex mutex;
public:
  HandleList() : max_handle(0) {
  }
  ~HandleList() {
  }

  unsigned int maxHandle() {
    return max_handle;
  }

  unsigned int assignHandle(T val) {
    QMutexLocker locker(&mutex);
    unsigned nxt = 0;
    bool freeHandleFound = false;
    while ((nxt < max_handle) && !freeHandleFound) {
      freeHandleFound = (handles.count(nxt) == 0);
      if (!freeHandleFound) nxt++;
    }
    handles[nxt] = val;
    if (nxt >= max_handle) max_handle++;
    return nxt+1;
  }

  T lookupHandle(unsigned int handle) {
    QMutexLocker locker(&mutex);
    if (handles.count(handle-1) == 0)
      throw Exception("Invalid handle!");
    return handles[handle-1];
  }

  void deleteHandle(unsigned int handle) {
    QMutexLocker locker(&mutex);
    if (handle == max_handle) 
      max_handle--;
    handles.erase(handle-1);
  }
};

class FilePtr {
public:
  QFile *fp;
  bool swapflag;
};

#endif
