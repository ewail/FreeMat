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
#include <qglobal.h>
#include "DumbTerminal.hpp"
#include <qapplication.h>
#include <qeventloop.h>
#include <stdio.h>
#if !defined(_MSC_VER ) 
#include <unistd.h>
#else
#include <io.h>
#endif

DumbTerminal::DumbTerminal() {
}

DumbTerminal::~DumbTerminal() {
}
  
void DumbTerminal::MoveDown() {
  printf("\n");
  fflush(stdout);
}

void DumbTerminal::MoveUp() {
}

void DumbTerminal::MoveRight() {
}

void DumbTerminal::MoveLeft() {
}

void DumbTerminal::ClearEOL() {
}

void DumbTerminal::ClearEOD() {
}

void DumbTerminal::ClearDisplay() {
}

void DumbTerminal::MoveBOL() {
  putchar('\r');
  fflush(stdout);
}

void DumbTerminal::OutputRawString(QString txt) {
  printf("%s",qPrintable(txt));
  fflush(stdout);
}

void DumbTerminal::DoRead() {
  char c;
#if !defined(_MSC_VER ) 
  while (read(STDIN_FILENO, &c, 1) == 1)
#else
  while (_read(0, &c, 1) == 1)
#endif
    emit OnChar(c);
}
