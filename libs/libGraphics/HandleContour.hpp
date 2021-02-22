/*
 * Copyright (c) 2002-2007 Samit Basu
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
#ifndef __HandleContour_hpp__
#define __HandleContour_hpp__

#include "HandleObject.hpp"
#include <QList>

class contour_point {
public:
  double x;
  double y;
  inline contour_point(double a, double b) : x(a), y(b) {};
};

typedef QList<contour_point> cline;
typedef QList<cline> lineset;
typedef QList<lineset> linecollection;

class HandleContour : public HandleObject {
  linecollection pset;
  QList<double> zvals;
  void SelectColor(RenderEngine& gc, double zval);
  lineset ContourCDriver(Array m, double val, Array x, Array y);
  void RebuildContourMatrix();
  Array GetCoordinateMatrix(QString name, bool isXcoord);
public:
  HandleContour();
  virtual ~HandleContour();
  virtual void ConstructProperties();
  virtual void SetupDefaults();
  virtual void UpdateState();
  virtual void PaintMe(RenderEngine& gc);    
  QVector<double> GetLimits();
};

#endif
