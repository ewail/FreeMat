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
#include "FuncTerminal.hpp"
#include <qapplication.h>
#include <qeventloop.h>

FuncTerminal::FuncTerminal(char *myargv[], int myargc, int funcMode) {
  sprintf(m_buffer,"%s",myargv[funcMode+1]);
  for (int i=funcMode+2;i<myargc;i++) {
    strcat(m_buffer," ");
    strcat(m_buffer,myargv[i]);
  }
  strcat(m_buffer,"\n");
  line_sent = false;
}

void FuncTerminal::Initialize() {
}

void FuncTerminal::RestoreOriginalMode() {
}

FuncTerminal::~FuncTerminal() {
}
  
void FuncTerminal::ResizeEvent() {
}

void FuncTerminal::MoveDown() {
  printf("\n");
  fflush(stdout);
}

void FuncTerminal::MoveUp() {
}

void FuncTerminal::MoveRight() {
}

void FuncTerminal::MoveLeft() {
}

void FuncTerminal::ClearEOL() {
}

void FuncTerminal::ClearEOD() {
}

void FuncTerminal::MoveBOL() {
  putchar('\r');
  fflush(stdout);
}

void FuncTerminal::ClearDisplay() {
}

void FuncTerminal::OutputRawString(QString txt) {
  printf("%s",qPrintable(txt));
  fflush(stdout);
}
  
char* FuncTerminal::getLine(QString prompt) {
  if (!line_sent) {
    line_sent = true;
    return strdup(m_buffer);
  } else
    return strdup("quit;\n");
}

int FuncTerminal::getTerminalWidth() {
  return 80;
}
