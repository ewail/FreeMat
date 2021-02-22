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
#ifndef __RenderEngine_hpp__
#define __RenderEngine_hpp__

#include <vector>
#include <string>
#include <qfont.h>
#include <QPainterPath>
#include "HandleProperty.hpp"


namespace ColorMode { enum ColorMode { ColorSpec, None, Flat, Interp }; };
namespace AlphaMode { enum AlphaMode { Scalar, Flat, Interp }; };
namespace EraseMode { enum EraseMode { Normal, None, Xor, Background }; };
namespace LightningMode{ enum LightingMode { None, Flat, Gouraud, Phong }; };

class ColorData{
public:
  double r;
  double g;
  double b;
  double a;
  ColorData() {};
  ColorData( double red, double green, double blue, double alpha ) : r(red), g(green), b(blue), a(alpha) {};
};


class point{
public:
    double x;
    double y;
    double z;
    point() {};
    point( double x_, double y_, double z_ ) : x(x_), y(y_), z(z_) {};
};


class Face{
public:
    QVector<point> vertices;
    QVector<ColorData> edgecolors;
    QVector<ColorData> vertexcolors;
    ColorData FaceColor;  
    ColorData EdgeColor;
    enum ColorMode::ColorMode FaceColorMode;
    enum ColorMode::ColorMode EdgeColorMode;
};

typedef QList<Face> FaceList;

class cpoint {
public:
  double x;
  double y;
  double z;
  double r;
  double g;
  double b;
  double a;
  cpoint() {};
  cpoint(double ax, double ay, double az, double ar, double ag, double ab, double aa) 
      : x(ax), y(ay), z(az), r(ar), g(ag), b(ab), a(aa) {};
};

class RenderEngine {
public:
  virtual void debug() = 0;
  enum AlignmentFlag {Min, Mean, Max};
  enum SymbolType {Plus,Circle,Star,Dot,Times,Square,Diamond,Up,Down,Right,Left,Pentagram,Hexagram,None};
  RenderEngine() {};
  virtual ~RenderEngine() {};
  virtual void clear(QVector<double>) = 0;
  virtual double width() = 0;
  virtual double height() = 0;
  virtual void toPixels(double x, double y, double z, int &a, int &b) = 0;
  virtual void toPixels(double x, double y, double z, double &a, double &b) = 0;
  virtual void toPixels(double x, double y, double z, double &a, double &b, bool &clipped) = 0;
  virtual void lookAt(double px, double py, double pz,
		      double tx, double ty, double tz,
		      double ux, double uy, double uz) = 0;
  virtual void scale(double sx, double sy, double sz) = 0;
  virtual void mapPoint(double x, double y, double z,
			double &a, double &b, double &c) = 0;
  virtual void project(double xmin, double xmax, double ymin, double ymax,
		       double zmin, double zmax) = 0;
  virtual void viewport(double x0, double y0, double width, double height) = 0;
  virtual void quad(double x1, double y1, double z1,
		    double x2, double y2, double z2,
		    double x3, double y3, double z3,
		    double x4, double y4, double z4) = 0;
  virtual void quadline(double x1, double y1, double z1,
			double x2, double y2, double z2,
			double x3, double y3, double z3,
			double x4, double y4, double z4) = 0;
  virtual void tri(double x1, double y1, double z1,
		   double x2, double y2, double z2,
		   double x3, double y3, double z3) = 0;
  virtual void triLine(double x1, double y1, double z1,
		       double x2, double y2, double z2,
		       double x3, double y3, double z3) = 0;
  virtual void color(QVector<double>) = 0;
  virtual void setLineStyle(QString style) = 0;
  void color(double r, double g, double b) {QVector<double> t; t.push_back(r); t.push_back(g); t.push_back(b); color(t);}
  virtual void lineWidth(double n) = 0;
  virtual void line(double x1, double y1, double z1,
		    double x2, double y2, double z2) = 0;
  virtual void line(double x1, double y1,
		    double x2, double y2) = 0;
  virtual void lineSeries(QVector<double> xs, 
			  QVector<double> ys,
			  QVector<double> zs) = 0;
  virtual void setupDirectDraw() = 0;
  virtual void releaseDirectDraw() = 0;
  virtual void getViewport(int viewp[4]) = 0;
  virtual void getProjectionMatrix(double proj[16]) = 0;
  virtual void getModelviewMatrix(double model[16]) = 0;
  virtual void putText(double x, double y, QString txt, 
		       QVector<double> color, 
		       AlignmentFlag xflag, AlignmentFlag yflag,
		       QFont fnt, double rotation) = 0;
  virtual void measureText(QString txt, QFont fnt, AlignmentFlag xflag, 
			   AlignmentFlag yflag,int &width, int &height,
			   int &xoffset, int &yoffset) = 0;
  virtual void depth(bool) = 0;
  virtual void rect(double x1, double y1, double x2, double y2) = 0;
  virtual void rectFill(double x1, double y1, double x2, double y2) = 0;
  virtual void circle(double x1, double y1, double radius) = 0;
  virtual void circleFill(double x1, double y1, double radius) = 0;
  virtual void drawImage(double x1, double y1, double x2, double y2,
			 QImage pic) = 0;
  virtual void drawImage(HPTwoVector* xp, HPTwoVector* yp, 
			 HPTwoVector* xlim, HPTwoVector* ylim,
			 bool xflip, bool yflip, QImage pic) = 0;
  virtual void quadStrips(QVector<QVector<cpoint> > faces, bool flatfaces,
			  QVector<QVector<cpoint> > edges, bool flatedges) = 0;
  virtual void setClipBox(QVector<double> limits) {};
  virtual void drawPatch(const FaceList& faces) = 0;

};
  
void DrawSymbol(RenderEngine& gc, RenderEngine::SymbolType symb,
		double x, double y, double z, double sze,
		QVector<double> edgecolor, 
		QVector<double> fillcolor, 
		double width);

RenderEngine::SymbolType StringToSymbol(QString);

#endif
