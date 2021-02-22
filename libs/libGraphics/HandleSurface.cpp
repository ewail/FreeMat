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
#include "HandleSurface.hpp"
#include "HandleAxis.hpp"
#include "HandleFigure.hpp"
#include <qgl.h>

HandleSurface::HandleSurface() {
  ConstructProperties();
  SetupDefaults();
}

HandleSurface::~HandleSurface() {
}

QVector<double> HandleSurface::GetLimits() {
  if (HasChanged("xdata"))
    ToManual("xdatamode");
  if (HasChanged("ydata"))
    ToManual("ydatamode");
  if (HasChanged("cdata")) 
    ToManual("cdatamode");
  QVector<double> limits;
  Array zdata(ArrayPropertyLookup("zdata"));
  if (IsAuto("xdatamode")) {
    limits.push_back(1);
    limits.push_back(zdata.columns());
  } else {
    Array xdata(ArrayPropertyLookup("xdata"));
    limits.push_back(ArrayMin(xdata));
    limits.push_back(ArrayMax(xdata));
  }
  if (IsAuto("ydatamode")) {
    limits.push_back(1);
    limits.push_back(zdata.rows());
  } else {
    Array ydata(ArrayPropertyLookup("ydata"));
    limits.push_back(ArrayMin(ydata));
    limits.push_back(ArrayMax(ydata));
  }
  limits.push_back(ArrayMin(zdata));
  limits.push_back(ArrayMax(zdata));
  if (IsAuto("cdatamode")) {
    limits.push_back(ArrayMin(zdata));
    limits.push_back(ArrayMax(zdata));
  } else {
    Array cdata(ArrayPropertyLookup("cdata"));
    limits.push_back(ArrayMin(cdata));
    limits.push_back(ArrayMax(cdata));
  }
  QVector<double> alphadata(VectorPropertyLookup("alphadata"));
  limits.push_back(VecMin(alphadata));
  limits.push_back(VecMax(alphadata));
  return limits;
}

void HandleSurface::ConstructProperties() {
  //!
  //@Module SURFACEPROPERTIES Surface Object Properties
  //@@Section HANDLE
  //@@Usage
  //Below is a summary of the properties for the axis.
  //\begin{itemize}
  //  \item @|alphadata| - @|vector| - This is a vector that
  // should contain as many elements as the surface data itself @|cdata|,
  // or a single scalar.  For a single scalar, all values of the surface
  // take on the same transparency.  Otherwise, the transparency of
  // each pixel is determined by the corresponding value from the @|alphadata|
  // vector.
  //  \item @|alphadatamapping| - @|{'scaled','direct','none'}| - For @|none|
  // mode (the default), no transparency is applied to the data.  For @|direct|
  // mode, the vector @|alphadata| contains values between @[0,M-1]| where
  // @|M| is the length of the alpha map stored in the figure.  For @|scaled|
  // mode, the @|alim| vector for the figure is used to linearly rescale the 
  // alpha data prior to lookup in the alpha map. 
  //  \item @|ambientstrength| - Not used.
  //  \item @|backfacelighting| - Not used.
  //  \item @|cdata| - @|array| - This is either a @|M x N| array or an 
  //  @|M x N x 3| array.  If the data is @|M x N| the surface is a scalar
  // surface (indexed mode), where the color associated with each surface pixel
  // is computed using the colormap and the @|cdatamapping| mode.  If the
  // data is @|M x N x 3| the surface is assumed to be in RGB mode, and the
  // colorpanes are taken directly from @|cdata| (the colormap is ignored).
  // Note that in this case, the data values must be between @[0,1]| for each
  // color channel and each point on the surface.
  //  \item @|cdatamapping| - @|{'scaled','direct'}| - For @|scaled| (the
  // default), the pixel values are scaled using the @|clim| vector for the
  // figure prior to looking up in the colormap.  For @|direct| mode, the
  // pixel values must be in the range @|[0,N-1| where @|N| is the number of
  // colors in the colormap.
  //  \item @|children| - Not used.
  //  \item @|diffusestrength| - Not used.
  //  \item @|edgealpha| - @|{'flat','interp','scalar'}| - Controls how the
  // transparency is mapped for the edges of the surface.
  //  \item @|edgecolor| - @|{'flat','interp','none',colorspec}| - Specifies
  // how the edges are colored.  For @|'flat'| the edges are flat colored,
  // meaning that the line segments that make up the edges are not shaded.
  // The color for the line is determined by the first edge point it is connected
  // to.
  //  \item @|edgelighting| - Not used.
  //  \item @|facealpha| - @|{'flat','interp','texturemap',scalar}| - Controls
  // how the transparency of the faces of the surface are controlled.  For
  // flat shading, the faces are constant transparency.  For interp mode, the faces
  // are smoothly transparently mapped.  If set to a scalar, all faces have the
  // same transparency.
  //  \item @|facecolor| - @|{'none','flat','interp',colorspec}| - Controls
  // how the faces are colored.  For @|'none'| the faces are uncolored, and
  // the surface appears as a mesh without hidden lines removed.  For @|'flat'|
  // the surface faces have a constant color.  For @|'interp'| smooth shading
  // is applied to the surface.  And if a colorspec is provided, then the
  // faces all have the same color.
  //  \item @|facelighting| - Not used.
  //  \item @|linestyle| - @|{'-','--',':','-.','none'}| - The style of the line used
  // to draw the edges.
  //  \item @|linewidth| - @|scalar| - The width of the line used to draw the edges.
  //  \item @|marker| - @|{'+','o','*','.','x','square','s','diamond','d','^','v','>','<'}| - 
  // The marker for data points on the line.  Some of these are redundant, as @|'square'| 
  // @|'s'| are synonyms, and @|'diamond'| and @|'d'| are also synonyms.
  //  \item @|markeredgecolor| - @|colorspec| - The color used to draw the marker.  For some
  // of the markers (circle, square, etc.) there are two colors used to draw the marker.
  // This property controls the edge color (which for unfilled markers) is the primary
  // color of the marker.
  //  \item @|markerfacecolor| - @|colorspec| - The color used to fill the marker.  For some
  // of the markers (circle, square, etc.) there are two colors used to fill the marker.
  //  \item @|markersize| - @|scalar| - Control the size of the marker.  Defaults to 6, which
  // is effectively the radius (in pixels) of the markers.
  //  \item @|meshstyle| - @|{'both','rows','cols}| - This property controls how the mesh is
  // drawn for the surface.  For @|rows| and @|cols| modes, only one set of edges is drawn.
  //  \item @|normalmode| - Not used.
  //  \item @|parent| - @|handle| - The axis containing the surface.
  //  \item @|specularcolorreflectance| - Not used.
  //  \item @|specularexponent| - Not used.
  //  \item @|specularstrength| - Not used.
  //  \item @|tag| - @|string| - You can set this to any string you want.
  //  \item @|type| - @|string| - Set to the string @|'surface'|.
  //  \item @|userdata| - @|array| - Available to store any variable you
  // want in the handle object.
  //  \item @|vertexnormals| - Not used.
  //  \item @|xdata| - @|array| - Must be a numeric array of size @|M x N| which contains
  // the x location of each point in the defined surface. Must be the same size as @|ydata|
  // and @|zdata|.  Alternately, you can specify an array of size @|1 x N| in which case
  // FreeMat replicates the vector to fill out an @|M x N| matrix.
  //  \item @|xdatamode| - @|{'auto','manual'}| - When set to @|auto| then FreeMat will
  // automatically generate the x coordinates.
  //  \item @|ydata| - @|array| - Must be a numeric array of size @|M x N| which contains
  // the y location of each point in the defined surface. Must be the same size as @|xdata|
  // and @|zdata|.   Alternately, you can specify an array of size @|M x 1| in which case
  // FreeMat replicates the vector to fill out an @|M x N| matrix.
  //  \item @|ydatamode| - @|{'auto','manual'}| - When set to @|auto| then FreeMat will
  // automatically generate the y coordinates.
  //  \item @|zdata| - @|array| - Must be a numeric array of size @|M x N| which contains
  // the y location of each point in the defined surface. Must be the same size as @|xdata|
  // and @|ydata|.
  //  \item @|visible| - @|{'on','off'}| - Controls whether the surface is
  // visible or not.
  //\end{itemize}
  //!
  AddProperty(new HPVector, "alphadata");
  AddProperty(new HPMappingMode, "alphadatamapping");
  AddProperty(new HPScalar,"ambientstrength");
  AddProperty(new HPBackFaceLighting,"backfacelighting");
  AddProperty(new HPArray, "cdata");
  AddProperty(new HPDataMappingMode, "cdatamapping");
  AddProperty(new HPAutoManual, "cdatamode");
  AddProperty(new HPHandles,"children");
  AddProperty(new HPScalar,"diffusestrength");
  AddProperty(new HPEdgeAlpha,"edgealpha");
  AddProperty(new HPColorInterp,"edgecolor");
  AddProperty(new HPLightingMode,"edgelighting");
  AddProperty(new HPFaceAlpha,"facealpha");
  AddProperty(new HPColorInterp,"facecolor");
  AddProperty(new HPLightingMode,"facelighting");
  AddProperty(new HPLineStyle,"linestyle");
  AddProperty(new HPScalar,"linewidth");
  AddProperty(new HPSymbol,"marker");
  AddProperty(new HPAutoFlatColor,"markeredgecolor");
  AddProperty(new HPAutoFlatColor,"markerfacecolor");
  AddProperty(new HPScalar,"markersize");
  AddProperty(new HPRowColumns,"meshstyle");
  AddProperty(new HPAutoManual,"normalmode");
  AddProperty(new HPHandles,"parent");
  AddProperty(new HPScalar,"specularcolorreflectance");
  AddProperty(new HPScalar,"specularexponent");
  AddProperty(new HPScalar,"specularstrength");
  AddProperty(new HPString,"tag");
  AddProperty(new HPString,"type");
  AddProperty(new HPArray,"userdata");
  AddProperty(new HPArray,"vertexnormals");
  AddProperty(new HPArray,"xdata");
  AddProperty(new HPAutoManual,"xdatamode");
  AddProperty(new HPArray,"ydata");
  AddProperty(new HPAutoManual,"ydatamode");
  AddProperty(new HPArray,"zdata");
  AddProperty(new HPOnOff,"visible");
}

void HandleSurface::SetupDefaults() {
  HPVector *hp = (HPVector*) LookupProperty("alphadata");
  QVector<double> gp;
  gp.push_back(1.0);
  hp->Data(gp);
  SetConstrainedStringDefault("alphadatamapping","none");
  SetScalarDefault("ambientstrength",0.55);
  SetConstrainedStringDefault("backfacelighting","unlit");
  SetConstrainedStringDefault("cdatamapping","scaled");
  SetConstrainedStringDefault("cdatamode","auto");
  SetScalarDefault("diffusestrength",0.6);
  SetScalarDefault("specularcolorreflectance",0.4);
  SetScalarDefault("specularexponent",0.1);
  SetScalarDefault("specularstrength",0.5);
  SetStringDefault("type","image");
  SetConstrainedStringDefault("visible","on");
  SetConstrainedStringScalarDefault("edgealpha","scalar",1);
  SetConstrainedStringColorDefault("edgecolor","colorspec",0,0,0);
  SetConstrainedStringDefault("edgelighting","none");
  SetConstrainedStringScalarDefault("facealpha","scalar",1);
  SetConstrainedStringDefault("facecolor","flat");
  SetConstrainedStringDefault("facelighting","none");
  SetConstrainedStringDefault("linestyle","-");
  SetScalarDefault("linewidth",0.5);
  SetConstrainedStringDefault("marker","none");
  SetConstrainedStringColorDefault("markeredgecolor","auto",0,0,0);
  SetConstrainedStringColorDefault("markerfacecolor","none",0,0,0);
  SetScalarDefault("markersize",6);
  SetConstrainedStringDefault("meshstyle","both");
  SetConstrainedStringDefault("normalmode","auto");
  SetStringDefault("type","surface");
  SetStringDefault("xdatamode","auto");
  SetStringDefault("ydatamode","auto");
}

void HandleSurface::DoAutoXMode() {
  Array zdata(ArrayPropertyLookup("zdata"));
  Array xdata(Double,NTuple(zdata.rows(),zdata.columns()));
  double *dp = xdata.real<double>().data();
  int cols = int(zdata.columns());
  int rows = int(zdata.rows());
  for (int j=0;j<cols;j++)
    for (int i=0;i<rows;i++)
      dp[i+j*rows] = j+1;
  HPArray *hp = (HPArray*) LookupProperty("xdata");
  hp->Data(xdata);
}

void HandleSurface::DoAutoYMode() {
  Array zdata(ArrayPropertyLookup("zdata"));
  Array ydata(Double,NTuple(zdata.rows(),zdata.columns()));
  double *dp = ydata.real<double>().data();
  int cols = int(zdata.columns());
  int rows = int(zdata.rows());
  for (int j=0;j<cols;j++)
    for (int i=0;i<rows;i++)
      dp[i+j*rows] = i+1;
  HPArray *hp = (HPArray*) LookupProperty("ydata");
  hp->Data(ydata);
}

void HandleSurface::DoAutoCMode() {
  Array zdata(ArrayPropertyLookup("zdata"));
  HPArray *hp = (HPArray*) LookupProperty("cdata");
  hp->Data(zdata);
}

void HandleSurface::UpdateState() {
  if (HasChanged("xdata"))
    ToManual("xdatamode");
  if (HasChanged("ydata"))
    ToManual("ydatamode");
  if (HasChanged("cdata")) 
    ToManual("cdatamode");
  if (IsAuto("xdatamode")) 
    DoAutoXMode();
  if (IsAuto("ydatamode"))
    DoAutoYMode();
  if (IsAuto("cdatamode"))
    DoAutoCMode();
  HandleAxis *ax = GetParentAxis();
  HandleFigure *fig = GetParentFigure();
  if (HasChanged("cdata") || ax->HasChanged("clim") || 
      fig->HasChanged("colormap") || HasChanged("cdatamapping")
      || HasChanged("zdata"))
    {
      UpdateCAlphaData();
      fig->markDirty();
    }
  if (HasChanged("xdata") || HasChanged("ydata") || HasChanged("zdata")
      || HasChanged("facecolor") || HasChanged("facealpha") || HasChanged("cdata")
      || ax->HasChanged("clim") || fig->HasChanged("colormap") || 
      HasChanged("cdatamapping") ||  HasChanged("edgecolor") ||
      HasChanged("edgealpha")) { 
    m_surfquads = BuildQuadsNoTexMap((HPConstrainedStringColor*) 
				     LookupProperty("facecolor"),
				     (HPConstrainedStringScalar*)
				     LookupProperty("facealpha"));
    m_edgequads = BuildQuadsNoTexMap((HPConstrainedStringColor*) 
				     LookupProperty("edgecolor"),
				     (HPConstrainedStringScalar*)
				     LookupProperty("edgealpha"));
    fig->markDirty();
  }
  ClearAllChanged();
}

/*
  0
  q1
  1
  q2
  2
*/

Array HandleSurface::GetCoordinateMatrix(QString name, bool isXcoord) {
  // Get the elevation data from the object
  Array zdata(ArrayPropertyLookup("zdata"));
  int zrows = int(zdata.rows());
  int zcols = int(zdata.columns());
  if (StringCheck(name+"mode","manual")) {
    // not auto mode...
    Array cdata(ArrayPropertyLookup(name));
    if (cdata.isVector() && 
	((isXcoord && (cdata.length() == int(zcols))) ||
	 (!isXcoord && (cdata.length() == int(zrows))))) {
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


QVector<QVector<cpoint> > HandleSurface::BuildQuadsNoTexMap(HPConstrainedStringColor* cp,
								    HPConstrainedStringScalar* ap) {
  // Get the x,y,z & color data points
  QVector<QVector<cpoint> > retval;
  Array xdata(GetCoordinateMatrix("xdata",true));
  xdata = xdata.toClass(Double);
  Array ydata(GetCoordinateMatrix("ydata",false));
  ydata = ydata.toClass(Double);
  Array zdata(ArrayPropertyLookup("zdata"));
  zdata = zdata.toClass(Double);
  if ((xdata.length() != zdata.length()) ||
      (xdata.length() != ydata.length())) return retval;
  if (zdata.isEmpty()) return retval;
  double *xdp = xdata.real<double>().data();
  double *ydp = ydata.real<double>().data();
  double *zdp = zdata.real<double>().data();
  int rows = int(zdata.rows());   int cols = int(zdata.columns());
  if (cp->Is("interp") && ((img.height() < rows) || (img.width() < cols))) return retval;
  if (ap->Is("interp") && ((img.height() < rows) || (img.width() < cols))) return retval;
  if (cp->Is("flat") && ((img.height() < rows-1) || (img.width() < cols-1))) return retval;
  if (ap->Is("flat") && ((img.height() < rows-1) || (img.width() < cols-1))) return retval;
  if (cp->Is("none")) return retval;
  QRgb *dummyline = NULL;
  if (cp->Is("colorspec") || ap->Is("scalar")) {
    dummyline = new QRgb[cols];
    double r = 0;
    double g = 0;
    double b = 0;
    double alphaval = 1.0;
    if (ap->Is("scalar"))
      alphaval = ap->Scalar();
    if (cp->Is("colorspec")) {
      QVector<double> p(cp->ColorSpec());
      if (p[0] == -1) return retval;
      r = p[0]; g = p[1]; b = p[2];
    }
    for (int i=0;i<cols;i++)
      dummyline[i] = qRgba((int)(255*r),
			   (int)(255*g),
			   (int)(255*b),
			   (int)(255*alphaval));
  }
  for (int i=0;i<rows-1;i++) {
    QRgb *cbits1 = NULL;
    QRgb *cbits2 = NULL;
    QRgb *abits1 = NULL;
    QRgb *abits2 = NULL;
    int col_lim = 0, alp_lim = 0;
    if (cp->Is("interp")) {
      cbits1 = (QRgb*) img.scanLine(i);
      cbits2 = (QRgb*) img.scanLine(i+1);
      col_lim = cols-1;
    } else if (cp->Is("flat")) {
      cbits1 = (QRgb*) img.scanLine(i);
      cbits2 = (QRgb*) img.scanLine(i);
      col_lim = cols-2;
    } else if (cp->Is("colorspec")) {
      cbits1 = (QRgb*) dummyline;
      cbits2 = (QRgb*) dummyline;
      col_lim = cols-1;
    }
    if (ap->Is("interp")) {
      abits1 = (QRgb*) img.scanLine(i);
      abits2 = (QRgb*) img.scanLine(i+1);
      alp_lim = cols-1;
    } else if (ap->Is("flat")) {
      abits1 = (QRgb*) img.scanLine(i);
      abits2 = (QRgb*) img.scanLine(i);
      alp_lim = cols-2;
    } else if (ap->Is("scalar")) {
      abits1 = (QRgb*) dummyline;
      abits2 = (QRgb*) dummyline;
      alp_lim = cols-1;
    }
    QVector<cpoint> linequads;
    for (int j=0;j<cols;j++) {
      int ccol = qMin(j,col_lim);
      int acol = qMin(j,alp_lim);
      linequads.push_back(cpoint(xdp[i+j*rows],ydp[i+j*rows],zdp[i+j*rows],
				 qRed(cbits1[ccol])/255.0,qGreen(cbits1[ccol])/255.0,
				 qBlue(cbits1[ccol])/255.0,qAlpha(abits1[acol])/255.0));
      linequads.push_back(cpoint(xdp[i+1+j*rows],ydp[i+1+j*rows],
				 zdp[i+1+j*rows],qRed(cbits2[ccol])/255.0,
				 qGreen(cbits2[ccol])/255.0,qBlue(cbits2[ccol])/255.0,
				 qAlpha(abits2[acol])/255.0));
    }
    retval.push_back(linequads);
  }
  // 0
  // 1
  // 2

  if (cp->Is("colorspec") || ap->Is("scalar")) 
    delete[] dummyline;
  return retval;
}

void HandleSurface::PaintMe(RenderEngine& gc) {
  if (StringCheck("visible","off"))
    return;

  // There are many render styles...
  // edgealpha, edgecolor, facealpha, facecolor
  // facecolor
  // Texture mapping not supported yet
  if (StringCheck("facecolor","texturemap")) return;
  if (StringCheck("facealpha","texturemap")) return;
  // A quadstrip is defined by its 
  gc.quadStrips(m_surfquads,StringCheck("facecolor","flat"),
		m_edgequads,StringCheck("edgecolor","flat"));
}

