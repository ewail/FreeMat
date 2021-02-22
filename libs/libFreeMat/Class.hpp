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
#ifndef __Class_hpp__
#define __Class_hpp__

#include "Array.hpp"
#include "Interpreter.hpp"
#include "Types.hpp"


Array ClassTrinaryOperator(Array a, Array b, Array c, QString funcname, Interpreter* eval);

Array ClassBinaryOperator(Array a, Array b, QString funcname,
			  Interpreter* eval);

Array ClassUnaryOperator(Array a, QString funcname,
			 Interpreter* eval);

Array ClassMatrixConstructor(ArrayMatrix m, Interpreter* eval);

ArrayVector ClassFunction(int nargout, const ArrayVector& arg);

ArrayVector ClassRHSExpression(Array a, const Tree & t, Interpreter* eval);

void ClassAssignExpression(ArrayReference r, const Tree & t, const Array& value, Interpreter* eval);
  
bool ClassResolveFunction(Interpreter* eval, Array &args, QString funcName, FuncPtr& val);
  
QString ClassMangleName(QString className, QString funcName);

void LoadClassFunction(Context* context);

/**
 * Returns true if a userclass with the given name is defined
 */
bool isUserClassDefined(QString classname);
/**
 * Lookup a user class.
 */
UserClassTemplate lookupUserClass(QString classname);
/**
 * Register a new user class.
 */
void registerUserClass(QString classname, UserClassTemplate cdata);
/**
 * Clear the registered class table
 */
void clearUserClasses();

/**
 * Define the hierarchy for a class
 */
void defineHierarchyForClass(QString classname, StringVector parents);

bool isParentClass(QString parent, QString child);

#endif
