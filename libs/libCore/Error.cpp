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
#include "Interpreter.hpp"

//@@Signature
//sfunction lasterr LasterrFunction
//inputs msg
//outputs msg
//DOCBLOCK flow_lasterr
ArrayVector LasterrFunction(int nargout, const ArrayVector& arg,
			    Interpreter* eval) {
  if (arg.size() == 0) 
    return ArrayVector(Array(eval->getLastErrorString()));
  eval->setLastErrorString(arg[0].asString());
  return ArrayVector();
}


//@@Signature
//sfunction errorcount ErrorCountFunction
//inputs none
//outputs count
//DOCBLOCK freemat_errorcount
ArrayVector ErrorCountFunction(int nargout, const ArrayVector& arg,
			       Interpreter* eval) {
  return ArrayVector(Array(double(eval->getErrorCount())));
}

static QString BoolToFlag(bool t)
{
  if (t) return "on";
  return "off";
}

//@@Signature
//sfunction warning WarningFunction
//inputs msg
//outputs flag
//DOCBLOCK flow_warning
ArrayVector WarningFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() == 0)
    return Array(BoolToFlag(eval->getEnableWarnings()));
  if (!(arg[0].isString()))
    throw Exception("Input to error function must be a string");
  QString txt = arg[0].asString();
  if (txt.toLower() == "on")
    {
      Array ret = Array(BoolToFlag(eval->getEnableWarnings()));
      eval->setEnableWarnings(true);
      return ret; 
   }
  else if (txt.toLower() == "off")
    {
      Array ret = Array(BoolToFlag(eval->getEnableWarnings()));
      eval->setEnableWarnings(false);
      return ret;
    }
  eval->warningMessage(txt);
  return ArrayVector();
}

//@@Signature
//function error ErrorFunction
//inputs string
//outputs none
//DOCBLOCK flow_error
ArrayVector ErrorFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    return ArrayVector();
  QString etxt(arg[0].asString());
  if (!etxt.isEmpty()) 
    throw Exception(etxt);
  return ArrayVector();
}
