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

#ifndef __Print_hpp__
#define __Print_hpp__
#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include "Array.hpp"

class Interpreter;
void PrintArrayClassic(Array A, int printLimit, Interpreter *eval);
QString ArrayToPrintableString(const Array& a);
QString SummarizeArrayCellEntry(const Array &a);

typedef enum {
  format_native,
  format_short,
  format_long,
  format_short_e,
  format_long_e,
} FMFormatMode;

void SetPrintFormatMode(FMFormatMode);
FMFormatMode GetPrintFormatMode();
#endif
