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
#ifndef __JITInfo_hpp__
#define __JITInfo_hpp__

#include <QSharedDataPointer>
#include "JITFuncBase.hpp"

class JITInfo {
public:
  typedef enum {
    UNTRIED,
    FAILED,
    SUCCEEDED
  } JITState_t;
private:
  JITState_t m_jitstate;
  JITFuncBase* m_jitfunc;
public:
  JITInfo() : m_jitstate(UNTRIED), m_jitfunc(NULL) {}
  inline JITState_t JITState() const {return m_jitstate;}
  inline void setJITState(JITState_t t) {m_jitstate = t;}
  inline JITFuncBase* JITFunction() const {return m_jitfunc;}
  inline void setJITFunction(JITFuncBase *t) {m_jitfunc = t;}
};

#endif
