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
#ifndef __FuncTerminal_hpp__
#define __FuncTerminal_hpp__

#include "KeyManager.hpp"

class FuncTerminal : public KeyManager {
  char m_buffer[4096];
  bool line_sent;
public:
  FuncTerminal(char *argv[], int argc, int funcMode);
  virtual ~FuncTerminal();
  virtual void Initialize();
  virtual void RestoreOriginalMode();
  virtual void OutputRawString(QString txt); 
  virtual char* getLine(QString prompt);
  virtual void ResizeEvent();
  virtual void MoveDown();
  virtual void MoveUp();
  virtual void MoveRight();
  virtual void MoveLeft();
  virtual void ClearEOL();
  virtual void ClearEOD();
  virtual void MoveBOL();
  virtual void ClearDisplay();
  virtual int getTerminalWidth();
};

#endif
