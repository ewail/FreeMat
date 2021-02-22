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
#include "Array.hpp"
#include "IEEEFP.hpp"
#include "Algorithms.hpp"
#include <QString>

//@@Signature
//function hex2dec Hex2DecFunction
//inputs x
//outputs y
//DOCBLOCK elementary_hex2dec
ArrayVector Hex2DecFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    throw Exception("hex2dec requires an argument");
  if (arg[0].isEmpty()) return ArrayVector(EmptyConstructor());
  if (!arg[0].isString() && (arg[0].dataClass() != Double))
    throw Exception("hex2dec argument must be a string");
  Array x(arg[0]);
  if (x.dataClass() == Double) x = x.toClass(StringArray);
  if (x.isVector()) 
    return ArrayVector(Array(double(x.asString().toLongLong(0,16))));
  else {
    StringVector sv(StringVectorFromArray(x));
    Array rp(Double,NTuple(sv.size(),1));
    BasicArray<double> &qp(rp.real<double>());
    for (int i=0;i<sv.size();i++)
      qp[index_t(i+1)] = sv[i].toInt(0,16);
    return ArrayVector(rp);
  }
  return ArrayVector();
}

//@@Signature
//function dec2hex Dec2HexFunction
//inputs x digits
//outputs y
//DOCBLOCK elementary_dec2hex
ArrayVector Dec2HexFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("dec2hex requires at least one argument");
  Array x = arg[0].asDenseArray().toClass(Int64);
  const BasicArray<int64> &xp(x.constReal<int64>());
  int n = 0;
  if (arg.size() > 1) n = arg[1].asInteger();
  if ((n < 0) || (n > 32)) 
    throw Exception("illegal number of digits requested in dec2hex function");
  StringVector ret;
  int maxlen = 0;
  for (index_t i=1;i<=xp.length();i++) {
    QString t = QString("%1").arg(xp[i],n,16,QChar('0'));
    maxlen = qMax(maxlen,t.size());
  }
  if (n == 0) n = maxlen;
  for (index_t i=1;i<=xp.length();i++) {
    QString t = QString("%1").arg(xp[i],n,16,QChar('0')).toUpper();
    ret << t;
  }
  return ArrayVector(StringArrayFromStringVector(ret,QChar(' ')));
}


//@@Signature
//function num2hex Num2HexFunction
//inputs x
//outputs y
//DOCBLOCK elementary_num2hex

template <typename T>
static inline Array Num2HexFunction(const BasicArray<T> &data) {
  if (data.isEmpty())
    return Array(StringArray,NTuple(0,16));
  StringVector st;
  for (index_t i=1;i<=data.length();i++)
    st.push_back(ToHexString(data[i]));
  return StringArrayFromStringVector(st,QChar(' '));
}

ArrayVector Num2HexFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("num2hex fucntion requires a single argument");
  if ((arg[0].dataClass() != Float) && (arg[0].dataClass() != Double))
    throw Exception("num2hex only works on single and double arrays");
  Array x = arg[0].asDenseArray();
  if (x.dataClass() == Float) 
    return ArrayVector(Num2HexFunction(x.constReal<float>()));
  else 
    return ArrayVector(Num2HexFunction(x.constReal<double>()));
}
