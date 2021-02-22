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
#ifndef __NTuple_hpp__
#define __NTuple_hpp__

#include <QString>
#include <iostream>

#include "Types.hpp"

#if defined(_MSC_VER)  
double round( double x );
float roundf( float x );
#endif

class NTuple {
  index_t m_data[NDims];
public:
  inline bool is2D() const {
    for (int i=2;i<NDims;i++)
      if (m_data[i] != 1) return false;
    return true;
  }
  inline bool isValid() const {
    for (int i=0;i<NDims;i++)
      if (m_data[i] < 0) return false;
    return true;
  }
  inline void zero() {
    for (int i=0;i<NDims;i++) m_data[i] = 0;
  }
  inline index_t max() const {
    index_t v = m_data[0];
    for (int i=1;i<NDims;i++)
      if (m_data[i] > v) v = m_data[i];
    return v;
  }
  inline index_t rows() const {
    return m_data[0];
  }
  inline index_t cols() const {
    return m_data[1];
  }
  inline bool isVector() const {
    return (is2D() && ((m_data[0] == 1) || (m_data[1] == 1)));
  }
  inline bool isColumnVector() const {
    return (is2D() && (m_data[1] == 1));
  }
  inline bool isRowVector() const {
    return (is2D() && (m_data[0] == 1));
  }
  inline bool isScalar() const {
    return (count() == 1);
  }
  inline index_t stride(int dim) const {
    if ((dim<0) || (dim >= NDims))
      throw Exception("Illegal range in stride");
    index_t nextCoeff = 1;
    for (int i=1;i<=dim;i++) {
      nextCoeff *= m_data[i-1];
    }
    return nextCoeff;
  }
  inline index_t map(const NTuple& pos) const {
    index_t retval = 1;
    index_t nextCoeff = 1;
    for (int i=0;i<NDims;i++) {
      retval += nextCoeff*(round(pos[i])-1);
      nextCoeff *= m_data[i];
    }
    return retval;
  }
  inline void map(index_t vecndx, NTuple& pos) const {
    int64 vecndxi = (int64) (vecndx);
    vecndxi--;
    for (int i=0;i<NDims;i++) {
      pos[i] = (vecndxi % ((int64) m_data[i])) + 1;
      vecndxi /= ((int64) m_data[i]);
    }
  }
  inline bool validate(const NTuple& pos) const {
    for (int i=0;i<NDims;i++) 
      if ((pos.m_data[i] <= 0) || 
	  (pos.m_data[i] > m_data[i])) return false;
    return true;
  }
  inline void ripple(NTuple &pos) const {
    for (int i=0;i<(NDims-1);i++) {
      if (pos[i] > m_data[i]) {
	pos[i] = 1;
	pos[i+1]++;
      }
    }
  }
  inline void ripplePinned(NTuple &pos, int pin_dim) const {
    if ((pin_dim<0) || (pin_dim >= NDims))
      throw Exception("Illegal range in stride");
    for (int i=0;i<(NDims-1);i++) {
      int64 dim = (int64) m_data[i];
      if (i == pin_dim) dim = 1;
      if (pos[i] > dim) {
	pos[i] = 1;
	pos[i+1]++;
      }
    }
  }
  inline void increment(NTuple &pos) const {
    pos[0]++;
    ripple(pos);
  }
  inline NTuple forceOne(int pin_dim) const {
    if ((pin_dim<0) || (pin_dim >= NDims))
      throw Exception("Illegal range in forceOne");
    NTuple copy(*this);
    copy[pin_dim] = 1;
    return copy;
  }
  inline void increment(NTuple &pos, int pin_dim) const {
    if ((pin_dim<0) || (pin_dim >= NDims))
      throw Exception("Illegal range in increment");
    if (pin_dim == 0) 
      pos[1]++;
    else
      pos[0]++;
    ripplePinned(pos,pin_dim);
  }
  inline NTuple replace(int dim, index_t val) const {
    if ((dim<0) || (dim >= NDims))
      throw Exception("Illegal range in replace");
    NTuple copy(*this);
    copy[dim] = val;
    return copy;
  }
  inline NTuple(index_t rows, index_t cols) {
    m_data[0] = rows;
    m_data[1] = cols;
    for (int i=2;i<NDims;i++) m_data[i] = 1;
  }
  inline NTuple(index_t rows, index_t cols, index_t slices) {
    m_data[0] = rows;
    m_data[1] = cols;
    m_data[2] = slices;
    for (int i=3;i<NDims;i++) m_data[i] = 1;
  }
  inline NTuple() {
    m_data[0] = 0;
    m_data[1] = 0;
    for (int i=2;i<NDims;i++) m_data[i] = 1;
  }
  inline void set(int dim, index_t len) {
    if (len < 0) throw Exception("Negative dimensions are not allowed");
    if ((dim<0) || (dim >= NDims)) throw Exception("Illegal range in set");
    m_data[dim] = len;
  }
  inline index_t& get(int dim) {
    if ((dim<0) || (dim >= NDims)) throw Exception("Illegal range in get");
    return m_data[dim];
  }
  inline const index_t get(int dim) const {
    if ((dim<0) || (dim >= NDims)) throw Exception("Illegal range in get");
    return m_data[dim];
  }
  inline const index_t operator[](int dim) const {
    if ((dim<0) || (dim >= NDims)) throw Exception("Illegal range in get");
    return get(dim);
  }
  inline index_t& operator[](int dim) {
    if ((dim<0) || (dim >= NDims)) throw Exception("Illegal range in get");
    return get(dim);
  }
  inline bool operator>=(const NTuple& alt) const {
    for (int i=0;i<NDims;i++)
      if (m_data[i] < alt.m_data[i]) return false;
    return true;
  }
  inline bool operator<=(const NTuple& alt) const {
    for (int i=0;i<NDims;i++)
      if (m_data[i] > alt.m_data[i]) return false;
    return true;
  }
  inline bool operator!=(const NTuple& alt) const {
    return !(*this == alt);
  }
  inline bool operator==(const NTuple& alt) const {
    for (int i=0;i<NDims;i++)
      if (m_data[i] != alt.m_data[i]) return false;
    return true;
  }
  // Compare this NTuple to the given size.  Extension
  // is defined as:
  //  All dimensions are the same, except for the 
  //  last non-singleton dimension
  inline bool extends(const NTuple& size) const {
    int firstDifferent = NDims;
    int i = 0;
    while (i < NDims && (size.m_data[i] == m_data[i])) i++;
    firstDifferent = i;
    if (firstDifferent == NDims) return false;
    i = firstDifferent+1;
    while (i < NDims && (size.m_data[i] == 1) && (m_data[i] == 1)) i++;
    if (i == NDims) return true;
    return false;
  }
  NTuple permute(const NTuple& perm) const {
    NTuple ret;
    for (int i=0;i<NDims;i++)
      ret.m_data[i] = m_data[int(perm[i]-1)];
    return ret;
  }
  bool operator==(index_t alt) const {
    if (m_data[0] != alt) return false;
    for (int i=1;i<NDims;i++)
      if (m_data[i] != 1) return false;
    return true;
  }
  inline const index_t count() const {
    index_t ret = 1;
    for (int i=0;i<NDims;i++) ret *= m_data[i];
    return ret;
  }
  inline int lastNotOne() const {
    int last_not_one = NDims;
    while ((m_data[last_not_one-1] == 1) && 
	   (last_not_one > 2)) last_not_one--;
    return last_not_one;
  }
  inline int firstNonsingular() const {
    int non_singular = 0;
    while ((m_data[non_singular] == 1) && (non_singular < NDims))
      non_singular++;
    if (non_singular == NDims)
      return 0;
    return non_singular;
  }
  inline int lastSingular() const {
    int last_singular = NDims;
    while ((m_data[last_singular-1] == 1) &&
	   (last_singular > 0)) last_singular--;
    return last_singular;
  }
  inline QString toString() const {
    int last_not_one = lastNotOne();
    QString ret = QString("%1").arg(m_data[0]);
    for (int i=1;i<last_not_one;i++)
      ret += QString("x%1").arg(m_data[i]);
    return ret;
  }
};

inline std::ostream& operator<<(std::ostream& o, const NTuple &t) {
  o << "<";
  for (int i=0;i<NDims;i++)
    o << t[i] << " ";
  o << ">";
  return o;
}

inline NTuple max(const NTuple &a, const NTuple &b) {
  NTuple ret;
  for (int i=0;i<NDims;i++)
    ret[i] = (a[i] > b[i]) ? a[i] : b[i];
  return ret;
}

#endif
