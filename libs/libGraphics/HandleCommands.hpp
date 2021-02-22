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
#ifndef __HandleCommands_hpp__
#define __HandleCommands_hpp__

#include "Context.hpp"
#include "HandleObject.hpp"
#include "HandleFigure.hpp"
#define MAX_FIGS 100
#define HANDLE_OFFSET_OBJECT 100000
#define HANDLE_OFFSET_FIGURE 1

void SaveFocus();
void RestoreFocus();
void LoadHandleGraphicsFunctions(Context* context);

HandleObject* LookupHandleObject(unsigned handle);
HandleFigure* LookupHandleFigure(unsigned handle, Interpreter *eval);
HandleFigure* LookupHandleFigureNoCreate(unsigned handle);
unsigned AssignHandleObject(HandleObject*);
unsigned AssignHandleFigure(HandleFigure*);
void FreeHandleObject(unsigned handle);
void FreeHandleFigure(unsigned handle);
void ValidateHandle(unsigned handle);
void NotifyFigureClosed(unsigned handle);
void NotifyFigureActive(unsigned handle);
  
void InitializeHandleGraphics();
void ShutdownHandleGraphics();
void CloseHelper(int fig);
void IndirectWindowClose(int handle);

ArrayVector HCopyFunction(int nargout, const ArrayVector& arg);
ArrayVector HPrintFunction(int nargout, const ArrayVector& arg);
ArrayVector DrawNowFunction(int nargout, const ArrayVector& arg);

void RefreshFigs();

#endif
