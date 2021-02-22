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
#include "HandleImage.hpp"
#include "HandleAxis.hpp"
#include "HandleFigure.hpp"
#include <QMatrix>

HandleImage::HandleImage() {
  ConstructProperties();
  SetupDefaults();
}

HandleImage::~HandleImage() {
}
  
QVector<double> HandleImage::GetLimits() {
  HPTwoVector *xp = (HPTwoVector *) LookupProperty("xdata");
  HPTwoVector *yp = (HPTwoVector *) LookupProperty("ydata");
  QVector<double> limits;
  limits.push_back(xp->Data()[0]);
  limits.push_back(xp->Data()[1]);
  limits.push_back(yp->Data()[0]);
  limits.push_back(yp->Data()[1]);
  limits.push_back(0);
  limits.push_back(0);
  // The clim limit is just the min and max values of cdata
  Array cdata(ArrayPropertyLookup("cdata"));
  if (!cdata.isEmpty()) {
    cdata = cdata.toClass(Double);
    limits.push_back(ArrayMin(cdata));
    limits.push_back(ArrayMax(cdata));
  } else {
    limits.push_back(0);
    limits.push_back(1);
  }
  QVector<double> alphadata(VectorPropertyLookup("alphadata"));
  limits.push_back(VecMin(alphadata));
  limits.push_back(VecMax(alphadata));
  return limits;
}


void HandleImage::ConstructProperties() {
  //!
  //@Module IMAGEPROPERTIES Image Object Properties
  //@@Section HANDLE
  //@@Usage
  //Below is a summary of the properties for the axis.
  //\begin{itemize}
  //  \item @|alphadata| - @|vector| - This is a vector that
  // should contain as many elements as the image data itself @|cdata|,
  // or a single scalar.  For a single scalar, all values of the image
  // take on the same transparency.  Otherwise, the transparency of
  // each pixel is determined by the corresponding value from the @|alphadata|
  // vector.
  //  \item @|alphadatamapping| - @|{'scaled','direct','none'}| - For @|none|
  // mode (the default), no transparency is applied to the data.  For @|direct|
  // mode, the vector @|alphadata| contains values between @[0,M-1]| where
  // @|M| is the length of the alpha map stored in the figure.  For @|scaled|
  // mode, the @|alim| vector for the figure is used to linearly rescale the 
  // alpha data prior to lookup in the alpha map. 
  //  \item @|cdata| - @|array| - This is either a @|M x N| array or an 
  //  @|M x N x 3| array.  If the data is @|M x N| the image is a scalar
  // image (indexed mode), where the color associated with each image pixel
  // is computed using the colormap and the @|cdatamapping| mode.  If the
  // data is @|M x N x 3| the image is assumed to be in RGB mode, and the
  // colorpanes are taken directly from @|cdata| (the colormap is ignored).
  // Note that in this case, the data values must be between @[0,1]| for each
  // color channel and each pixel.
  //  \item @|cdatamapping| - @|{'scaled','direct'}| - For @|scaled| (the
  // default), the pixel values are scaled using the @|clim| vector for the
  // figure prior to looking up in the colormap.  For @|direct| mode, the
  // pixel values must be in the range @|[0,N-1| where @|N| is the number of
  // colors in the colormap if the data is integer type.  For floating point
  // data types, values must be in the range @|[1,N]|.
  //  \item @|children| - Not used.
  //  \item @|parent| - @|handle| - The axis containing the image.
  //  \item @|tag| - @|string| - You can set this to any string you want.
  //  \item @|type| - @|string| - Set to the string @|'image'|.
  //  \item @|xdata| - @|two vector| - contains the x coordinates of the
  // first and last column (respectively).  Defaults to @|[1,C]| where
  // @|C| is the number of columns in the image.
  //  \item @|ydata| - @|two vector| - contains the y coordinates of the
  // first and last row (respectively).  Defaults to @|[1,R]| where
  // @|R| is the number of rows in the image.
  //  \item @|userdata| - @|array| - Available to store any variable you
  // want in the handle object.
  //  \item @|visible| - @|{'on','off'}| - Controls whether the image is
  // visible or not.
  //\end{itemize}
  //!
  AddProperty(new HPVector, "alphadata");
  AddProperty(new HPArray, "cdata");
  AddProperty(new HPMappingMode, "alphadatamapping");
  AddProperty(new HPDataMappingMode, "cdatamapping");
  AddProperty(new HPHandles,"children");
  AddProperty(new HPHandles,"parent");
  AddProperty(new HPString,"tag");
  AddProperty(new HPString,"type");
  AddProperty(new HPTwoVector,"xdata");
  AddProperty(new HPTwoVector,"ydata");
  AddProperty(new HPArray,"userdata");
  AddProperty(new HPOnOff,"visible");
}

void HandleImage::SetupDefaults() {
  HPVector *hp = (HPVector*) LookupProperty("alphadata");
  QVector<double> gp;
  gp.push_back(1.0);
  hp->Data(gp);
  SetConstrainedStringDefault("alphadatamapping","none");
  SetConstrainedStringDefault("cdatamapping","scaled");
  // Eugene pointed out that this should be "direct" to match
  // MATLAB's behavior, but for images with range [1...N],
  // where N is the number of entries in the colormap, "scaled"
  // and "direct" should yield the same result.
  //  SetConstrainedStringDefault("cdatamapping","direct");
  SetStringDefault("type","image");
  SetTwoVectorDefault("xdata",0,1);
  SetTwoVectorDefault("ydata",0,1);
  SetConstrainedStringDefault("visible","on");
}


// Expand the current image using
// colormap
// cdatamapping
// clim
//
//  If cdatamapping == direct, outputRGB = colormap[(int)(dp[i]-1)]
//  If cdatamapping == scaled, outputRGB = colormap[rescale(dp[i])]
//    where rescale(x) = (x-min(clim))/(max(clim)-min(clim))*(colormap_count-1)
//
double* HandleImage::RGBExpandImage(const double *dp, 
				    int rows, int cols,
				    bool floatData) {
  // Allocate an output array of the right size
  double *ret = new double[rows*cols*3];
  // Retrieve the colormap
  QVector<double> cmap(((HandleObject*)GetParentFigure())->VectorPropertyLookup("colormap"));
  HandleAxis* ap(GetParentAxis());
  QVector<double> clim(((HandleObject*)ap)->VectorPropertyLookup("clim"));
  double clim_min(qMin(clim[0],clim[1]));
  double clim_max(qMax(clim[0],clim[1]));
  if (clim_min == clim_max) return ret;
  // Calculate the colormap length
  int cmaplen(cmap.size()/3);
  if (cmaplen < 1) return ret;
  if (StringCheck("cdatamapping","direct")) { 
    for (int i=0;i<rows*cols;i++) {
      int ndx;
      if (floatData)
	ndx = (int) dp[i] - 1;
      else
	ndx = (int) dp[i];
      ndx = qMin(cmaplen-1,qMax(0,ndx));
      ret[i] = cmap[3*ndx];
      ret[i+rows*cols] = cmap[3*ndx+1];
      ret[i+2*rows*cols] = cmap[3*ndx+2];
    }
  } else {
    for (int i=0;i<rows*cols;i++) {
      int ndx;
      ndx = (int) ((dp[i]-clim_min)/(clim_max-clim_min)*(cmaplen-1));
      ndx = qMin(cmaplen-1,qMax(0,ndx));
      ret[i] = cmap[3*ndx];
      ret[i+rows*cols] = cmap[3*ndx+1];
      ret[i+2*rows*cols] = cmap[3*ndx+2];
    }      
  }
  return ret;
}
				    

void HandleImage::PrepImageRGBNoAlphaMap(const double *dp,
					 int rows, int cols,
					 QVector<double> &alpha,
					 bool cdata_is_integer) {
  img = QImage(cols,rows,QImage::Format_ARGB32);
  for (int i=0;i<rows;i++) {
    QRgb *ibits = (QRgb*) img.scanLine(i);
    for (int j=0;j<cols;j++)
      if (!cdata_is_integer) {
	ibits[j] = qRgba((int)(255*dp[(i+j*rows)]),
			 (int)(255*dp[(i+j*rows)+rows*cols]),
			 (int)(255*dp[(i+j*rows)+2*rows*cols]),
			 (int)(255*alpha[i+j*rows]));
      } else {
	ibits[j] = qRgba((int)(dp[(i+j*rows)]),
			 (int)(dp[(i+j*rows)+rows*cols]),
			 (int)(dp[(i+j*rows)+2*rows*cols]),
			 (int)(255*alpha[i+j*rows]));
      }
  }
}

QVector<double> HandleImage::GetAlphaMap(int rows, int cols) {
  HPVector *hp = (HPVector*) LookupProperty("alphadata");
  QVector<double> alphain(hp->Data());
  QVector<double> alphaout;
  // Retrieve the alphamap
  QVector<double> amap(((HandleObject*)GetParentFigure())->VectorPropertyLookup("alphamap"));
  int amaplen(amap.size());
  HandleAxis* ap(GetParentAxis());
  QVector<double> alim(((HandleObject*)ap)->VectorPropertyLookup("alim"));
  double alim_min(qMin(alim[0],alim[1]));
  double alim_max(qMax(alim[0],alim[1]));
  int increment;
  if (alphain.size() == 0) {
    for (int i=0;i<rows*cols;i++)
      alphaout.push_back(1);
    return alphaout;
  } else if (alphain.size() != (rows*cols)) {
    increment = 0;
  } else
    increment = 1;
  if (StringCheck("alphadatamapping","none")) {
    for (int i=0;i<rows*cols;i++)
      alphaout.push_back(qMin(1.0,qMax(0.0,alphain[i*increment])));
  } else if (StringCheck("alphadatamapping","direct")) {
    for (int i=0;i<rows*cols;i++) {
      int ndx = (int) alphain[i*increment] -1;
      ndx = qMin(amaplen-1,qMax(0,ndx));
      alphaout.push_back(amap[ndx]);
    }
  } else {
    for (int i=0;i<rows*cols;i++) {
      int ndx = (int) alphain[i*increment] -1;
      ndx = (int) ((alphain[i*increment]-alim_min)/(alim_max-alim_min)*(amaplen-1));
      ndx = qMin(amaplen-1,qMax(0,ndx));
      alphaout.push_back(amap[ndx]);
    }
  }
  return alphaout;
}

void HandleImage::UpdateCAlphaData() {
  // Calculate the QImage
  Array cdata(ArrayPropertyLookup("cdata"));
  if (cdata.isEmpty()) return;
  bool cdata_is_integer = IsInteger(cdata);
  cdata = cdata.toClass(Double);
  // Retrieve alpha map
  QVector<double> alphas(GetAlphaMap(int(cdata.rows()),
				     int(cdata.cols())));
  // Check for the indexed or non-indexed case
  if ((cdata.dimensions().lastNotOne() == 3) &&
      (cdata.dimensions()[2] == 3)) {
    PrepImageRGBNoAlphaMap(cdata.constReal<double>().constData(),
			   int(cdata.rows()),
			   int(cdata.cols()),
			   alphas,cdata_is_integer);
  } else {
    double *dp = RGBExpandImage(cdata.constReal<double>().constData(),
				int(cdata.rows()),
				int(cdata.cols()),
				!cdata_is_integer);
    PrepImageRGBNoAlphaMap(dp,
			   int(cdata.rows()),
			   int(cdata.cols()),
			   alphas,false);
    delete[] dp;
  }
}

void HandleImage::UpdateState() {
  HandleAxis *ax = GetParentAxis();
  HandleFigure *fig = GetParentFigure();
  if (HasChanged("cdata") || ax->HasChanged("clim") || 
      fig->HasChanged("colormap") || HasChanged("cdatamapping")) {
    UpdateCAlphaData();
    fig->markDirty();
  }
  Array cdata(ArrayPropertyLookup("cdata"));
  HPTwoVector *xp = (HPTwoVector *) LookupProperty("xdata");
  if (xp->Data().empty()) {
    if (cdata.cols() > 1)
      SetTwoVectorDefault("xdata",0,cdata.cols());
    else
      SetTwoVectorDefault("xdata",0,1);
  }
  HPTwoVector *yp = (HPTwoVector *) LookupProperty("ydata");
  if (yp->Data().empty()) {
    if (cdata.rows() > 1)
      SetTwoVectorDefault("ydata",0,cdata.rows());
    else
      SetTwoVectorDefault("ydata",0,1);
  }
  ClearAllChanged();
}

void HandleImage::PaintMe(RenderEngine& gc) {
  if (StringCheck("visible","off"))
    return;
  HPTwoVector *xp = (HPTwoVector *) LookupProperty("xdata");
  HPTwoVector *yp = (HPTwoVector *) LookupProperty("ydata");
  HandleAxis *ax = GetParentAxis();
  HPTwoVector *xlim = (HPTwoVector *) ax->LookupProperty("xlim");
  HPTwoVector *ylim = (HPTwoVector *) ax->LookupProperty("ylim");
  bool xflip = (ax->StringCheck("xdir","reverse"));
  // Reverse the yflip bit - so that images naturally have the first row at the top
  bool yflip = (ax->StringCheck("ydir","reverse"));

  gc.drawImage(xp, yp, xlim, ylim, xflip, yflip, img);
}
