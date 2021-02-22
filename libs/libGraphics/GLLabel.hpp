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
#ifndef __GLLabel_hpp__
#define __GLLabel_hpp__

// A low-level interface to describe a text label in OpenGL-ish.
#include <qfont.h>
#include <qgl.h>
#include <qimage.h>

namespace FreeMat {

  class GLLabel {
    GLubyte *bits;
    int width; 
    int height;
    int x0;
    int y0;
    GLubyte red, green, blue;
    std::string text;
    QImage pic;
  public:
    std::string Text();
    enum AlignmentFlag {Min, Mean, Max};
    GLLabel(QFont fnt, std::string txt, GLubyte r, GLubyte g, GLubyte b);
    GLLabel();
    ~GLLabel();
    void DrawMe(QGLWidget *widget, int x, int y, AlignmentFlag xflag, AlignmentFlag yflag);
    int twidth();
    int theight();
    int xoffset(AlignmentFlag);
    int yoffset(AlignmentFlag);
  };

}

#endif
