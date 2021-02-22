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
#include "HandleFigure.hpp"
#include "HandleList.hpp"
#include "HandleCommands.hpp"
#include "HandleWindow.hpp"
#include <math.h>

HandleFigure::HandleFigure(HandleWindow *win) {
  ConstructProperties();
  SetupDefaults();
  m_win = win;
}

// Missing features -- setsize?

void HandleFigure::Resize(int width, int height) {
  SetTwoVectorDefault("figsize",width,height);
}

void HSVRAMP(double h, double &r, double &g, double &b) {
  int i;
  double f, p, q, t;
  h *= 6;                        // sector 0 to 5
  i = (int) floor( h );
  f = h - i;                    // fractional part of h
  p = 0;
  q = 1 - f ;
  t = f ;
  switch( i ) {
  case 0:
    r = 1;      g = t;      b = p;
    break;
  case 1:
    r = q;      g = 1;      b = p;
    break;
  case 2:
    r = p;      g = 1;      b = t;
    break;
  case 3:
    r = p;      g = q;      b = 1;
    break;
  case 4:
    r = t;      g = p;      b = 1;
    break;
  default:                // case 5:
    r = 1;      g = p;      b = q;
    break;
  }
}


void HandleFigure::LoadDefaultColorMap() {
  QVector<double> cmap;
  for (int i=0;i<64;i++) {
    double h = i/(64.0);
    double r, g, b;
    HSVRAMP(h,r,g,b);
    cmap.push_back(r);
    cmap.push_back(g);
    cmap.push_back(b);
  }
  HPColorVector *hcv = (HPColorVector*) LookupProperty("colormap");
  hcv->Data(cmap);
  cmap.clear();
  cmap.push_back(1.0);
  HPVector *hv = (HPVector*) LookupProperty("alphamap");
  hv->Data(cmap);
}
  
void HandleFigure::ConstructProperties() {
  //!
  //@Module FIGUREPROPERTIES Figure Object Properties
  //@@Section HANDLE
  //@@Usage
  //Below is a summary of the properties for the axis.
  //\begin{itemize}
  //  \item @|alphamap| - @|vector| - Contains the alpha (transparency) map for the
  // figure.  If this is set to a scalar, then all values are mapped to the same 
  // transparency.  It defaults to @|1|, which is all values being fully opaque.  
  // If you set this to a vector, the values of graphics objects will be mapped to 
  // different transparency values, based on the setting of their @|alphadatamapping|
  // property.
  //  \item @|color| - @|colorspec| - The background color of the figure (defaults to a
  // gray @|[0.6,0.6,0.6]|).  During printing, this color is set to white, and then
  // is restored.
  //  \item @|colormap| - @|color vector| - an @|N x 3| matrix of RGB values that
  // specifies the colormap for the figure.  Defaults to an @|HSV| map.  
  //  \item @|children| - @|handle vector| - the handles for objects that are children
  // of this figure.  These should be axis objects.
  //  \item @|currentaxes| - @|handle| - the handle for the current axes.  Also returned
  // by @|gca|.
  //  \item @|doublebuffer| - Not used.
  //  \item @|parent| -  Not used.
  //  \item @|position| - Not used.
  //  \item @|type| - @|string| - returns the string @|'figure'|.
  //  \item @|userdata| - @|array| - arbitrary array you can use to store data associated
  // with the figure.
  //  \item @|nextplot| - @|{'add','replace','replacechildren'}| - If set to @|'add'| then
  // additional axes are added to the list of children for the current figure.  If set to 
  // @|'replace'|, then a new axis replaces all of the existing children.
  //  \item @|figsize| - @|two vector| - the size of the figure window in pixels (width x height).
  //  \item @|renderer| - @|{'painters','opengl'}| - When set to @|'painters'| drawing is based
  // on the Qt drawing methods (which can handle flat shading of surfaces with transparency).
  // If you set the renderer to @|'opengl'| then OpenGL is used for rendering.  Support for 
  // OpenGL is currently in the alpha stage, and FreeMat does not enable it automatically.
  // You can set the renderer mode to @|'opengl'| manually to experiment.  Also, OpenGL 
  // figures cannot be printed yet.
  //\end{itemize}
  //!
  AddProperty(new HPVector,"alphamap");
  AddProperty(new HPColor,"color");
  AddProperty(new HPColorVector,"colormap");
  AddProperty(new HPHandles,"children");
  AddProperty(new HPHandles,"currentaxes");
  AddProperty(new HPOnOff,"doublebuffer");
  AddProperty(new HPHandles,"parent");
  AddProperty(new HPFourVector,"position");
  AddProperty(new HPString,"type");
  AddProperty(new HPArray,"userdata");
  AddProperty(new HPNextPlotMode,"nextplot");
  AddProperty(new HPTwoVector,"figsize");
  AddProperty(new HPRenderMode,"renderer");
}

void HandleFigure::SetupDefaults() {
  SetStringDefault("renderer","painters");
  SetStringDefault("type","figure");
  SetThreeVectorDefault("color",0.7,0.7,0.7);
  SetStringDefault("nextplot","replace");
  SetTwoVectorDefault("figsize",500,300);
  LoadDefaultColorMap();
}

void HandleFigure::PaintMe(RenderEngine& gc) {
  ClearAllChanged();
  try {
    HPColor *color = (HPColor*) LookupProperty("color");
    if (color->Data()[0] >= 0) {
      gc.clear(color->Data());
    }
    HPHandles *children = (HPHandles*) LookupProperty("children");
    QVector<unsigned> handles(children->Data());
    for (int i=0;i<handles.size();i++) {
      HandleObject *fp = LookupHandleObject(handles[i]);
      fp->PaintMe(gc);
    }
  } catch (Exception& e) {
    dbout << "Warning: Graphics subsystem reports: " << e.msg() << "\n";
  }
  markClean();
}

