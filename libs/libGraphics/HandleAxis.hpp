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
#ifndef __HandleAxis_hpp__
#define __HandleAxis_hpp__

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "Array.hpp"
#include "SymbolTable.hpp"
#include "HandleObject.hpp"
#include "RenderEngine.hpp"
#include "Context.hpp"
#include <QPainterPath>

/**
 * This utility function returns a 
 * "reasonable" set of points that bracket
 * and divide an interval
 */
QList<double> GetTicksOuter(double amin, double amax, bool isLog, int requestedCounts);
QList<double> GetTicksInner(double amin, double amax, bool isLog, int requestedCounts);

//The HandleAxis class encapsulates a 3D axis object, and is
//manipulated through the Set/Get interface.
class HandleAxis : public HandleObject {
  double x1pos[3], x2pos[3];
  double y1pos[3], y2pos[3];
  double z1pos[3], z2pos[3];
  bool xvisible, yvisible, zvisible;
  QFont m_font;
  double model[16];
  double proj[16];
  int viewp[4];
  QVector<double> m_box;

  void UpdateAxisFont();
  void HandlePlotBoxFlags();
  void GetMaxTickMetric(RenderEngine& gc,
			StringVector labs,
			double &maxx, double &maxy);
  void DrawLabel(RenderEngine& gc,
		 double dx, double dy, 
		 double x2, double y2, 
		 QVector<double> color,
		 QString txt);
  void SetupAxis(RenderEngine& gc);
  void RecalculateTicks(RenderEngine& gc);
  int GetTickCount(RenderEngine& gc, double x1, double y1, 
		   double z1, double x2, double y2, double z2);
  void DrawXGridLine(RenderEngine& gc, double t, 
		     QVector<double> limits);
  void DrawYGridLine(RenderEngine& gc, double t, 
		     QVector<double> limits);
  void DrawZGridLine(RenderEngine& gc, double t, 
		     QVector<double> limits);
  double flipX(double t);
  double flipY(double t);
  double flipZ(double t);
  FM::SymbolTable<HandleProperty*> properties;
  void RePackFigure(RenderEngine& gc);
  void UpdateLimits(bool x, bool y, bool z, bool a, bool c);
public:
  HandleAxis();
  virtual ~HandleAxis();
  bool Is2DView(); 
  void SetupCamera(RenderEngine &gc);
  void MapCorners(RenderEngine &gc, 
		  double &xmin, double &xmax,
		  double &ymin, double &ymax,
		  double &zmin, double &zmax);
  void UpdateXYZPos(RenderEngine &gc);
  QVector<double> GetClientAreaInPixels() {return m_box;}
  virtual void ConstructProperties();
  virtual void UpdateState();
  void SetupDefaults();
  virtual void PaintMe(RenderEngine &gc);
  virtual void PaintBoundingBox(RenderEngine& gc);
  QVector<double> UnitsReinterpret(RenderEngine& gc, QVector<double> a);
  QVector<double> GetPropertyVectorAsPixels(RenderEngine& gc, QString name);
  QVector<double> GetAxisLimits(bool rawmode = false);
  void GetAxisLimitsHelper(QString axisname, QVector<double> &lims, bool rawmode);
  void SetAxisLimits(QVector<double>);
  double MapX(double x);
  double MapY(double y);
  double MapZ(double z);
  QVector<double> ReMap(QVector<double>);
  void ReMap(QVector<double>,
	     QVector<double>,
	     QVector<double>,
	     QVector<double>&,
	     QVector<double>&,
	     QVector<double>&);
  void SetupProjection(RenderEngine& gc);
  void InitialSetupAxis(RenderEngine& gc);
  void DrawBox(RenderEngine& gc);
  void DrawMinorGridLines(RenderEngine& gc);
  void DrawGridLines(RenderEngine& gc);
  void DrawAxisLines(RenderEngine& gc);
  void DrawTickMarks(RenderEngine& gc);
  void DrawTickLabels(RenderEngine& gc,
		      QVector<double> color,
		      double px1, double py1, double pz1,
		      double px2, double py2, double pz2,
		      double limmin, double limmax,
		      double unitx, double unity, double unitz,
		      QVector<double>  maptics,
		      QVector<double>  minortics,
		      StringVector labels,
		      QString labelname,
		      int ticlen, double ticdir);
  void DrawAxisLabels(RenderEngine& gc);
  void DrawChildren(RenderEngine& gc);
};


void LoadHandleGraphicsFunctions(Context* context);

#endif
