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
#include "HandleContour.hpp"
#include "HandleList.hpp"
#include "HandleObject.hpp"
#include "HandleAxis.hpp"
#include "IEEEFP.hpp"
#include <math.h>

HandleContour::HandleContour() {
  ConstructProperties();
  SetupDefaults();
}

HandleContour::~HandleContour() {
}

// Calculate the limits - should return a vector of the
// form...
// [xmin, xmax, ymin, ymax, zmin, zmax, cmin, cmax, amin, amax]
QVector<double> HandleContour::GetLimits() {
//   QVector<double> xs(VectorPropertyLookup("xdata"));
//   QVector<double> ys(VectorPropertyLookup("ydata"));
//  QVector<double> zs(VectorPropertyLookup("zdata"));
  UpdateState();
  QVector<double> limits;
  bool x_initialized = false;
  bool y_initialized = false;
  bool z_initialized = false;
  double xmin;
  double xmax;
  double ymin;
  double ymax;
  double zmin;
  double zmax;
  double cmin;
  double cmax;
  for (int i=0;i<pset.size();i++) {
    // For each level.
    lineset cset(pset[i]);
    for (int j=0;j<cset.size();j++) {
      cline aline(cset[j]);
      for (int k=0;k<aline.size();k++) {
	if (!x_initialized && IsFinite(aline[k].x)) {
	  xmin = xmax = aline[k].x;
	  x_initialized = true;
	}
	if (!y_initialized && IsFinite(aline[k].y)) {
	  ymin = ymax = aline[k].y;
	  y_initialized = true;
	}
	if (!z_initialized) {
	  zmin = zmax = zvals[0];
	  z_initialized = true;
	} 
	if (x_initialized && IsFinite(aline[k].x)) {
	  xmin = qMin(xmin,aline[k].x);
	  xmax = qMax(xmax,aline[k].x);
	}
	if (y_initialized && IsFinite(aline[k].y)) {
	  ymin = qMin(ymin,aline[k].y);
	  ymax = qMax(ymax,aline[k].y);
	}
	if (z_initialized) {
	  zmin = qMin(zmin,zvals[i]);
	  zmax = qMax(zmax,zvals[i]);
	}
      }
    }
  }
  if (!x_initialized) {
    xmin = -1;
    xmax = 1;
  }
  if (!y_initialized) {
    ymin = -1;
    ymax = 1;
  }
  if (!z_initialized) {
    zmin = 0;
    zmax = 0;
  }
  cmin = zmin;
  cmax = zmax;
  if (StringCheck("floating","off"))
    zmin = zmax = 0;
  limits.push_back(xmin);
  limits.push_back(xmax);
  limits.push_back(ymin);
  limits.push_back(ymax);
  limits.push_back(zmin);
  limits.push_back(zmax);
  limits.push_back(cmin);
  limits.push_back(cmax);
  limits.push_back(0);
  limits.push_back(0); 
  return limits;
}

inline bool operator==(const contour_point& p1, const contour_point& p2) {
  return ((p1.x == p2.x) && (p1.y == p2.y));
}

inline cline Reverse(const cline& src) {
  cline ret;
  for (int i=src.size()-1;i>=0;i--)
    ret << src.at(i);
  return ret;
}

inline bool Connected(const cline& current, const cline& test) {
  return ((current.front() == test.front()) || 
	  (current.front() == test.back()) ||
	  (current.back() == test.front()) ||
	  (current.back() == test.back()));
}

inline void Join(cline& current, const cline& toadd) {
  if (current.front() == toadd.front())
    current = Reverse(toadd) + current;
  else if (current.front() == toadd.back())
    current = toadd + current;
  else if (current.back() == toadd.front())
    current += toadd;
  else if (current.back() == toadd.back())
    current += Reverse(toadd);
}

#define FOLD(x) MAP((x),row-1)
#define FNEW(x) MAP((x),row)
#define MAP(x,y) func[(y)+(x)*numy]
#define AINTER(a,b) ((val-(a))/((b)-(a)))
#define ALEFT(i,j) (((j)-1)+AINTER(FOLD((i)-1),FNEW((i)-1)))
#define TOP(i) (((i)-1)+AINTER(FNEW((i)-1),FNEW((i))))
#define BOT(i) (((i)-1)+AINTER(FOLD((i)-1),FOLD((i))))
#define RIGHT(i,j) (((j)-1)+AINTER(FOLD((i)),FNEW((i))))
#define DRAW(a,b,c,d) {allLines << (cline() << contour_point((double)a,(double)b) << contour_point((double)c,(double)d));}

lineset HandleContour::ContourCDriver(Array m, double val, Array x, Array y) {
  lineset allLines;
  lineset bundledLines;
  m = m.toClass(Double);
  const double *func = m.constReal<double>().constData();
  int numy = int(m.rows());
  int numx = int(m.columns());
  for (int row=1;row<numy;row++)
    for (int col=1;col<numx;col++) {
      int l = 0;
      if (FOLD(col) >= val) l  = l + 1;
      if (FOLD(col-1) >= val) l = l + 2;
      if (FNEW(col) >= val) l = l + 4;
      if (FNEW(col-1) >= val) l = l + 8;
      switch (l) {
      case 1:
      case 14:
	DRAW(BOT(col),row-1,col,RIGHT(col,row));
	break;
      case 2:
      case 13:
	DRAW(col-1,ALEFT(col,row),BOT(col),row-1);
	break;
      case 3:
      case 12:
	DRAW(col-1,ALEFT(col,row),col,RIGHT(col,row));
	break;
      case 4:
      case 11:
	DRAW(TOP(col),row,col,RIGHT(col,row));
	break;
      case 5:
      case 10:
	DRAW(BOT(col),row-1,TOP(col),row);
	break;
      case 6:
      case 9:
	{
	  double x0 = AINTER(FOLD(col-1),FOLD(col));
	  double x1 = AINTER(FNEW(col-1),FNEW(col));
	  double y0 = AINTER(FOLD(col-1),FNEW(col-1));
	  double y1 = AINTER(FOLD(col),FNEW(col));
	  double y = (x0*(y1-y0)+y0)/(1.0-(x1-x0)*(y1-y0));
	  double x = y*(x1-x0) + x0;
	  double fx1 = MAP(col-1,row-1)+x*(MAP(col,row-1)-MAP(col-1,row-1));
	  double fx2 = MAP(col-1,row)+x*(MAP(col,row)-MAP(col-1,row));
	  double f = fx1 + y*(fx2-fx1);
	  if (f==val) {
	    DRAW(BOT(col),row-1,TOP(col),row);
	    DRAW(col-1,ALEFT(col,row),col,RIGHT(col,row));
	  } else if (((f > val) && (FNEW(col) > val)) || 
		     ((f < val) && (FNEW(col) < val))) {
	    DRAW(col-1,ALEFT(col,row),TOP(col),row);
	    DRAW(BOT(col),row-1,col,RIGHT(col,row));
	  } else {
	    DRAW(col-1,ALEFT(col,row),BOT(col),row-1);
	    DRAW(TOP(col),row,col,RIGHT(col,row));
	  }
	}
	break;
      case 7:
      case 8:
	DRAW(col-1,ALEFT(col,row),TOP(col),row);
	break;
      }
    }
  // Now we link the line segments into longer lines.
  while (!allLines.empty()) {
    // Start a new line segment
    cline current(allLines.takeAt(0));
    bool lineGrown = true;
    while (lineGrown) {
      lineGrown = false;
      int i = 0;
      while (i<allLines.size()) {
	if (Connected(current,allLines.at(i))) {
	  Join(current,allLines.takeAt(i));
	  lineGrown = true;
	} else
	  i++;
      }
    }
    bundledLines << current;
  }
  // Final step is to transform the lines into the
  // given coordinates
  const double *xp = (const double *) (x.constReal<double>().constData());
  const double *yp = (const double *) (y.constReal<double>().constData());
#define X(a,b) xp[(b)+(a)*numy]
#define Y(a,b) yp[(b)+(a)*numy]
  for (int i=0;i<bundledLines.size();i++)
    for (int j=0;j<bundledLines[i].size();j++) {
      double ndx_x = bundledLines[i][j].x;
      double ndx_y = bundledLines[i][j].y;
      // Compute a new x and y using these coordinates as
      // bilinear interpolators into xp and yp
      int indx_x = qMax(qMin((int) ndx_x, numx-2),0);
      int indx_y = qMax(qMin((int) ndx_y, numy-2),0);
      double eps_x = ndx_x - indx_x;
      double eps_y = ndx_y - indx_y;
      double xp_out_1 = X(indx_x,indx_y) + 
	eps_x*(X(indx_x+1,indx_y) - X(indx_x,indx_y));
      double xp_out_2 = X(indx_x,indx_y+1) + 
	eps_x*(X(indx_x+1,indx_y+1) - X(indx_x,indx_y+1));
      double xp_out = xp_out_1 + eps_y*(xp_out_2-xp_out_1);
      double yp_out_1 = Y(indx_x,indx_y) + 
	eps_x*(Y(indx_x+1,indx_y) - Y(indx_x,indx_y));
      double yp_out_2 = Y(indx_x,indx_y+1) + 
	eps_x*(Y(indx_x+1,indx_y+1) - Y(indx_x,indx_y+1));
      double yp_out = yp_out_1 + eps_y*(yp_out_2-yp_out_1);
      bundledLines[i][j].x = xp_out;
      bundledLines[i][j].y = yp_out;
    }
  return bundledLines;
}

void HandleContour::RebuildContourMatrix() {
  // Count how many total points there are
  int pointcount = 0;
  int linecount = 0;
  for (int i=0;i<pset.size();i++) {
    for (int j=0;j<pset[i].size();j++) {
      linecount++;
      pointcount += pset[i][j].size();
    }
  }
  // Create the contour matrix
  int outcount = pointcount+linecount;
  Array out(Double,NTuple(2,outcount));
  double *output = out.real<double>().data();
  for (int i=0;i<pset.size();i++) {
    for (int j=0;j<pset[i].size();j++) {
      *output++ = zvals[i];
      *output++ = pset[i][j].size();
      cline bline(pset[i][j]);
      for (int k=0;k<bline.size();k++) {
	*output++ = bline[k].x;
	*output++ = bline[k].y;
      }
    }
  }
  HPArray *hp = (HPArray*) LookupProperty("contourmatrix");
  hp->Data(out);
}

Array HandleContour::GetCoordinateMatrix(QString name, bool isXcoord) {
  // Get the elevation data from the object
  Array zdata(ArrayPropertyLookup("zdata"));
  index_t zrows = zdata.rows();
  index_t zcols = zdata.columns();
  if (StringCheck(name+"mode","manual")) {
    // not auto mode...
    Array cdata(ArrayPropertyLookup(name));
    if (cdata.isVector() && 
	((isXcoord && (cdata.length() == zcols)) ||
	 (!isXcoord && (cdata.length() == zrows)))) {
      cdata = cdata.toClass(Double);
      const double *qp = cdata.constReal<double>().constData();
      Array mat(Double,NTuple(zrows,zcols));
      double *dp = mat.real<double>().data();
      for (int i=0;i<zcols;i++)
	for (int j=0;j<zrows;j++) {
	  if (isXcoord)
	    *dp = qp[i];
	  else
	    *dp = qp[j];
	  dp++;
	}
      return mat;
    } else if (cdata.is2D() && (cdata.rows() == zrows) &&
	       (cdata.columns() == zcols)) {
      return cdata;
    } 
  }
  // In auto mode, or the given data is bogus...
  Array mat(Double,NTuple(zrows,zcols));
  double *dp = mat.real<double>().data();
  for (int i=0;i<zcols;i++)
    for (int j=0;j<zrows;j++) {
      if (isXcoord)
	*dp = i+1;
      else
	*dp = j+1;
      dp++;
    }
  return mat;
}

void HandleContour::UpdateState() {
  if (HasChanged("levellist")) ToManual("levellistmode");
  if (HasChanged("xdata")) ToManual("xdatamode");
  if (HasChanged("ydata")) ToManual("ydatamode");
  Array zdata(ArrayPropertyLookup("zdata"));
  double zmin = ArrayMin(zdata);
  double zmax = ArrayMax(zdata);
  Array xdata(GetCoordinateMatrix("xdata",true));
  Array ydata(GetCoordinateMatrix("ydata",false));
  QList<double> levels;
  if (StringCheck("levellistmode","auto")) {
    levels = GetTicksInner(zmin,zmax,false,10);
    if (levels.size() > 0)  {
      if (levels.front() == zmin) levels.pop_front();
      if (levels.back() == zmax) levels.pop_back();
    }
    QVector<double> ulevels;
    for (int i=0;i<levels.size();i++)
      ulevels.push_back(levels[i]);
    HPVector *hp = (HPVector*) LookupProperty("levellist");
    hp->Data(ulevels);
    hp->ClearModified();
  } else {
    QVector<double> ulevels(VectorPropertyLookup("levellist"));
    for (int i=0;i<ulevels.size();i++)
      levels.push_back(ulevels[i]);
  }
  pset.clear();
  zvals.clear();
  for (int i=0;i<levels.size();i++) { 
    pset << ContourCDriver(zdata,levels[i],xdata,ydata);
    zvals << levels[i];
  }
  RebuildContourMatrix();
}

void HandleContour::SelectColor(RenderEngine& gc, double zval) {
  // Need to select a color for the contour
  // Retrieve the colormap
  QVector<double> cmap(((HandleObject*)GetParentFigure())->VectorPropertyLookup("colormap"));
  HandleAxis* ap(GetParentAxis());
  QVector<double> clim(((HandleObject*)ap)->VectorPropertyLookup("clim"));
  double clim_min(qMin(clim[0],clim[1]));
  double clim_max(qMax(clim[0],clim[1]));
  // Calculate the colormap length
  int cmaplen(cmap.size()/3);
  int ndx = (int) ((zval-clim_min)/(clim_max-clim_min)*(cmaplen-1));
  ndx = qMin(cmaplen-1,qMax(0,ndx));
  QVector<double> color;
  color.push_back(cmap[3*ndx]);
  color.push_back(cmap[3*ndx+1]);
  color.push_back(cmap[3*ndx+2]);
  gc.color(color);
}

void HandleContour::PaintMe(RenderEngine& gc) {
  if (StringCheck("visible","off"))
    return;
  // Draw the line...
  double width(ScalarPropertyLookup("linewidth"));
  HPAutoColor *lc = (HPAutoColor*) LookupProperty("linecolor");
  bool floatflag = StringCheck("floating","on");
  if (!StringCheck("linecolor","none")) {
    gc.setLineStyle(StringPropertyLookup("linestyle"));
    gc.lineWidth(width);
    HandleAxis *parent = (HandleAxis*) GetParentAxis();
    for (int i=0;i<pset.size();i++) {
      // For each level.
      lineset cset(pset[i]);
      if (StringCheck("linecolor","auto"))
	SelectColor(gc,zvals[i]);
      else 
	gc.color(lc->ColorSpec());
      for (int j=0;j<cset.size();j++) {
	cline aline(cset[j]);
	QVector<double> xs;
	QVector<double> ys;
	QVector<double> zs;
	for (int k=0;k<aline.size();k++) {
	  xs.push_back(aline[k].x);
	  ys.push_back(aline[k].y);
	  if (floatflag)
	    zs.push_back(zvals[i]);
	  else
	    zs.push_back(0);
	}
	QVector<double> mxs, mys, mzs;
	parent->ReMap(xs,ys,zs,mxs,mys,mzs);
	gc.lineSeries(mxs,mys,mzs);
      }
    }
  }
}

void HandleContour::SetupDefaults() {
  SetConstrainedStringDefault("linecolor","auto");
  SetConstrainedStringDefault("levellistmode","auto");
  SetConstrainedStringDefault("linestyle","-");
  SetScalarDefault("linewidth",1.0);
  SetConstrainedStringDefault("floating","off");
  SetStringDefault("type","contour");
  SetConstrainedStringDefault("visible","on");
  SetConstrainedStringDefault("xdatamode","auto");
  SetConstrainedStringDefault("ydatamode","auto");
}
  
void HandleContour::ConstructProperties() {
  //!
  //@Module COUNTOUR Contour Object Properties
  //@@Section HANDLE
  //@@Usage
  //Below is a summary of the properties for a line series.
  //\begin{itemize}
  //  \item @|contourmatrix| - @|array| - the matrix containing contour data
  // for the plot.  This is a @|2 x N| matrix containing x and y coordinates
  // for points on the contours.  In addition, each contour line starts with
  // a column containing the number of points and the contour value.
  //  \item @|displayname| - @|string| - The name of this line series as it
  //    appears in a legend.
  //  \item @|floating| - @|{'on','off'}| - set to on to have floating (3D) contours
  //  \item @|levellist| - @|vector| - a vector of Z-values for the contours
  //  \item @|levellistmode| - @|{'auto','manual'}| - set to auto for 
  //    automatic selection  of Z-values of the contours.
  //  \item @|linecolor| - color of the contour lines.
  //  \item @|linestyle| - @|{'-','--',':','-.','none'}| - the line style to draw the contour in.
  //  \item @|linewidth| - @|scalar| - the width of the lines
  //  \item @|parent| - @|handle| - The axis that contains this object
  //  \item @|tag| - @|string| - A string that can be used to tag the object.
  //  \item @|type| - @|string| - Returns the string @|'contour'|.
  //  \item @|userdata| - @|array| - Available to store any variable you
  // want in the handle object.
  //  \item @|visible| - @|{'on','off'}| - Controls visibility of the the line.
  //  \item @|xdata| - @|matrix| - Contains the x coordinates of the 
  // surface on which the zdata is defined.  This can either be a monotonic
  // vector of the same number of columns as @|zdata|, or a 2D matrix
  // that is the same size as @|zdata|.
  //  \item @|xdatamode| - @|{'auto','manual'}| - When set to @|'auto'| 
  //FreeMat will autogenerate the x coordinates for the contours.  
  //These values will be @|1,..,N| where @|N| is the number of columns
  //of @|zdata|.
  //  \item @|ydata| - @|matrix| - Contains the y coordinates of the
  // surface on which the zdata is defined.  This can either be a monotonic
  // vector of the same number of rows as @|zdata| or a 2D matrix that is
  // the same size as @|zdata|.
  //  \item @|ydatamode| - @|{'auto','manual'}| - When set to @|'auto'| 
  //FreeMat will autogenerate the y coordinates for the contour data.
  //  \item @|zdata| - @|matrix| - The matrix of z values that are to
  // be contoured.
  //\end{itemize}
  //@@Tests
  //@{ test_contour1.m
  //function y = test_contour1
  //  contour(zeros(10));
  //  y = true;
  //@}
  //!
  AddProperty(new HPArray,"contourmatrix");      //done
  AddProperty(new HPHandles,"children");         //done
  AddProperty(new HPString,"displayname");       //done
  AddProperty(new HPOnOff,"floating");           //done
  AddProperty(new HPVector,"levellist");         //done
  AddProperty(new HPAutoManual,"levellistmode"); //done
  AddProperty(new HPAutoColor,"linecolor");      //done
  AddProperty(new HPLineStyle,"linestyle");      //done
  AddProperty(new HPScalar,"linewidth");         //done
  AddProperty(new HPHandles,"parent");           //done
  AddProperty(new HPString,"tag");               //done
  AddProperty(new HPString,"type");              //done
  AddProperty(new HPArray,"userdata");           //done
  AddProperty(new HPOnOff,"visible");            //done
  AddProperty(new HPArray,"xdata");              //done
  AddProperty(new HPAutoManual,"xdatamode");     //done
  AddProperty(new HPArray,"ydata");              //done
  AddProperty(new HPAutoManual,"ydatamode");     //done
  AddProperty(new HPArray,"zdata");              //done
}
