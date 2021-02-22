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
#ifndef __FN_hpp__
#define __FN_hpp__

#include "Array.hpp"
#include "Interpreter.hpp"

ArrayVector FitFunFunction(int nargout, const ArrayVector& arg, Interpreter* eval);
ArrayVector ErfcFunction(int nargout, const ArrayVector& arg);
ArrayVector ErfFunction(int nargout, const ArrayVector& arg);
ArrayVector GammaFunction(int nargout, const ArrayVector& arg);
ArrayVector GammaLnFunction(int nargout, const ArrayVector& arg);
ArrayVector Interplin1Function(int nargout, const ArrayVector& arg);

#endif
