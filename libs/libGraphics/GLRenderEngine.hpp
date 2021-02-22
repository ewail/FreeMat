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
#ifndef __GLRenderEngine_hpp__
#define __GLRenderEngine_hpp__

#include "RenderEngine.hpp"
#include <QtOpenGL>

class GLRenderEngine : public RenderEngine {
  double model[16];
  double proj[16];
  GLint viewp[4];
  double m_x1, m_y1, m_width, m_height;
  QGLWidget *m_widget;
public:
  void debug();
  GLRenderEngine(QGLWidget *widget, double x1, double y1, 
		 double width, double height);
  ~GLRenderEngine();
  QGLWidget* widget();
  virtual double width() {return m_width;} 
  virtual double height() {return m_height;} 
  virtual void clear(QVector<double>);
  virtual void toPixels(double x, double y, double z, int &a, int &b);
  virtual void toPixels(double x, double y, double z, double &a, double &b);
  virtual void toPixels(double x, double y, double z, double &a, double &b, bool &clipped);
  virtual void lookAt(double px, double py, double pz,
		      double tx, double ty, double tz,
		      double ux, double uy, double uz);
  virtual void scale(double sx, double sy, double sz);
  virtual void mapPoint(double x, double y, double z,
			double &a, double &b, double &c);
  virtual void project(double xmin, double xmax, double ymin, double ymax,
		       double zmin, double zmax);
  virtual void viewport(double x0, double y0, double width, double height);
  virtual void quad(double x1, double y1, double z1,
		    double x2, double y2, double z2,
		    double x3, double y3, double z3,
		    double x4, double y4, double z4);
  virtual void quadline(double x1, double y1, double z1,
			double x2, double y2, double z2,
			double x3, double y3, double z3,
			double x4, double y4, double z4);
  virtual void tri(double x1, double y1, double z1,
		   double x2, double y2, double z2,
		   double x3, double y3, double z3);
  virtual void triLine(double x1, double y1, double z1,
		       double x2, double y2, double z2,
		       double x3, double y3, double z3);
  virtual void color(QVector<double>);
  virtual void setLineStyle(QString style);
  virtual void lineWidth(double n);
  virtual void line(double x1, double y1, double z1,
		    double x2, double y2, double z2);
  virtual void line(double x1, double y1,
		    double x2, double y2);
  virtual void lineSeries(QVector<double> xs, 
			  QVector<double> ys,
			  QVector<double> zs);
  virtual void setupDirectDraw();
  virtual void releaseDirectDraw();
  virtual void getModelviewMatrix(double amodel[16]);
  virtual void getProjectionMatrix(double aproj[16]);
  virtual void getViewport(int aviewp[4]);
  virtual void putText(double x, double y, QString txt, 
		       QVector<double> color, 
		       AlignmentFlag xflag, AlignmentFlag yflag,
		       QFont fnt, double rotation);
  virtual void measureText(QString txt, QFont fnt, AlignmentFlag xflag, 
			   AlignmentFlag yflag,int &width, int &height,
			   int &xoffset, int &yoffset);
  virtual void depth(bool);
  virtual void rect(double x1, double y1, double x2, double y2);
  virtual void rectFill(double x1, double y1, double x2, double y2);
  virtual void circle(double x1, double y1, double radius);
  virtual void circleFill(double x1, double y1, double radius); 
  virtual void drawImage(double x1, double y1, double x2, double y2,
			 QImage pic);
  virtual void drawImage(HPTwoVector* xp, HPTwoVector* yp, 
			 HPTwoVector* xlim, HPTwoVector* ylim,
			 bool xflip, bool yflip,
			 QImage pic);
  virtual void quadStrips(QVector<QVector<cpoint> > faces, bool flatfaces,
			  QVector<QVector<cpoint> > edges, bool flatedges);
  virtual void drawPatch(const FaceList& faces);
};

#endif
