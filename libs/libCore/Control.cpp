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

#include "Interpreter.hpp"
#include "Array.hpp"

//@@Signature
//sfunction jitstat JITStatFunction
//inputs none
//outputs count
ArrayVector JITStatFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  return ArrayVector(Array(double(eval->JITCounter())));
}

//@@Signature
//sfunction jitcontrol JITControlFunction
//inputs flag
//outputs flag
//DOCBLOCK freemat_jitcontrol
ArrayVector JITControlFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() < 1) {
    JITControlFlag flag = eval->JITControl();
    switch (flag)
      {
      case JITOff: return ArrayVector(Array(QString("off")));
      case JITOn: return ArrayVector(Array(QString("on")));
      case JITTrace: return ArrayVector(Array(QString("trace")));
      }
  } else {
    if (!arg[0].isString())
      throw Exception("jitcontrol function takes only a single, string argument");
    QString txt = arg[0].asString().toUpper();
    if (txt == "ON")
      eval->setJITControl(JITOn);
    else if (txt == "OFF")
      eval->setJITControl(JITOff);
    else if (txt == "TRACE")
      eval->setJITControl(JITTrace);
    else if (txt == "FLUSH")
      {
	ClearJITCache();
      }
    else
      throw Exception("jitcontrol function argument needs to be 'on/off/trace/flush'");
  }
  return ArrayVector();
}

//@@Signature
//sfunction dbauto DbAutoFunction
//inputs flag
//outputs flag
//DOCBLOCK debug_dbauto
ArrayVector DbAutoFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() < 1) {
    if (eval->AutoStop()) 
      return ArrayVector(Array(QString("on")));
    else 
      return ArrayVector(Array(QString("off")));
  } else {
    if (!arg[0].isString())
      throw Exception("dbauto function takes only a single, string argument");
    QString txt = arg[0].asString().toUpper();
    if (txt == "ON")
      eval->setAutoStop(true);
    else if (txt == "OFF")
      eval->setAutoStop(false);
    else
      throw Exception("dbauto function argument needs to be 'on/off'");
  }
  return ArrayVector();
}


