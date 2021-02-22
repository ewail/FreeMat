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
#ifndef __HandleFigure_hpp__
#define __HandleFigure_hpp__

#include "HandleObject.hpp"

class HandleWindow;

//Figure
//   contains one or more axes
//   to redraw the figure, we proxy our draws to the axes
//   Axes
//   contains one or more children
//     to redraw the axes, we clear it with bkcolor
//     then draw it
//     set the transformation & clipping
//     then draw the children
class HandleFigure : public HandleObject {
  HandleWindow *m_win;
  void LoadDefaultColorMap();
public:
  HandleFigure(HandleWindow *win);
  HandleWindow* ParentWindow() {return m_win;}
  void PaintMe(RenderEngine& gc);
  virtual void ConstructProperties();
  void SetupDefaults();
  void Resize(int width,int height);
};

#endif

