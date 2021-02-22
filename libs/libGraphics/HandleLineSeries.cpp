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
#include "HandleLineSeries.hpp"
#include "HandleList.hpp"
#include "HandleObject.hpp"
#include "HandleAxis.hpp"
#include "IEEEFP.hpp"

HandleLineSeries::HandleLineSeries() {
  ConstructProperties();
  SetupDefaults();
}

HandleLineSeries::~HandleLineSeries() {
}

// Calculate the limits - should return a vector of the
// form...
// [xmin, xmax, ymin, ymax, zmin, zmax, cmin, cmax, amin, amax]
QVector<double> HandleLineSeries::GetLimits() {
  QVector<double> xs(VectorPropertyLookup("xdata"));
  QVector<double> ys(VectorPropertyLookup("ydata"));
  QVector<double> zs(VectorPropertyLookup("zdata"));
  QVector<double> limits;
  limits.push_back(VecMin(xs));
  limits.push_back(VecMax(xs));
  limits.push_back(VecMin(ys));
  limits.push_back(VecMax(ys));
  limits.push_back(VecMin(zs));
  limits.push_back(VecMax(zs));
  limits.push_back(0);
  limits.push_back(0);
  limits.push_back(0);
  limits.push_back(0);    
  return limits;
}

void HandleLineSeries::UpdateState() {
  // Check that x, y and z data are the same size
  // Generate the x coordinates if necessary
  QVector<double> xs(VectorPropertyLookup("xdata"));
  QVector<double> ys(VectorPropertyLookup("ydata"));
  QVector<double> zs(VectorPropertyLookup("zdata"));
  if (IsAuto("xdatamode")) {
    xs.clear();
    for (int i=0;i<ys.size();i++)
      xs.push_back(i+1.0);
  }
  if (zs.size() == 0)
    for (int i=0;i<ys.size();i++)
      zs.push_back(0.0);
  HPVector *sp;
  sp = (HPVector*) LookupProperty("xdata");
  sp->Data(xs);
  sp = (HPVector*) LookupProperty("zdata");
  sp->Data(zs);
}

void HandleLineSeries::PaintMe(RenderEngine& gc) {
  if (StringCheck("visible","off"))
    return;
  // Draw the line...
  double width(ScalarPropertyLookup("linewidth"));
  gc.lineWidth(width);
  HPColor *lc = (HPColor*) LookupProperty("color");
  // remap...
  QVector<double> xs(VectorPropertyLookup("xdata"));
  QVector<double> ys(VectorPropertyLookup("ydata"));
  QVector<double> zs(VectorPropertyLookup("zdata"));
  if (!((xs.size() == ys.size()) && (ys.size() == zs.size())))
    return;
  QVector<double> mxs, mys, mzs;
  HandleAxis *parent = (HandleAxis*) GetParentAxis();
  parent->ReMap(xs,ys,zs,mxs,mys,mzs);
  if (!lc->IsNone()) {
    gc.color(lc->Data());
    gc.setLineStyle(StringPropertyLookup("linestyle"));
    // Partition it into segments of finite entries..
    int n = 0;
    while (n < mxs.size()) {
      QVector<double> local_mxs, local_mys, local_mzs;
      while ((n < mxs.size()) && IsFinite(mxs[n]) && IsFinite(mys[n]) && (IsFinite(mzs[n]))) {
	local_mxs.push_back(mxs[n]);
	local_mys.push_back(mys[n]);
	local_mzs.push_back(mzs[n]);
	n++;
      }
      gc.lineSeries(local_mxs,local_mys,local_mzs);
      while ((n < mxs.size()) && !(IsFinite(mxs[n]) && IsFinite(mys[n]) && (IsFinite(mzs[n])))) n++;
    }
  }
  // Draw the symbols
  HPColor *ec = (HPColor*) LookupProperty("markeredgecolor");
  HPColor *fc = (HPColor*) LookupProperty("markerfacecolor");
  RenderEngine::SymbolType typ = StringToSymbol(StringPropertyLookup("marker"));
  double sze = ScalarPropertyLookup("markersize")/2.0;
  // Make sure there's something to draw...
  if ((typ != RenderEngine::None) && (ec->IsNone() || fc->IsNone())) {
    // Calculate the u/v coordinates (pixels)
    QVector<double> uc;
    QVector<double> vc;
    for (int i=0;i<mxs.size();i++) {
      double u, v;
      bool clipped;
      gc.toPixels(mxs[i],mys[i],mzs[i],u,v,clipped);
      if (!clipped) {
	uc.push_back(u); vc.push_back(v);
      }
      gc.setupDirectDraw();
      for (int i=0;i<uc.size();i++) 
	DrawSymbol(gc,typ,uc[i],vc[i],0,sze,ec->Data(),fc->Data(),width);
      gc.releaseDirectDraw();
    }
  }
}

void HandleLineSeries::SetupDefaults() {
  SetThreeVectorDefault("color",0,0,0);
  SetConstrainedStringDefault("linestyle","-");
  SetScalarDefault("linewidth",1.0);
  SetConstrainedStringDefault("marker","none");
  SetThreeVectorDefault("markeredgecolor",0,0,0);
  SetThreeVectorDefault("markerfacecolor",-1,-1,-1);
  SetScalarDefault("markersize",6);
  SetStringDefault("type","line");
  SetConstrainedStringDefault("visible","on");
  SetConstrainedStringDefault("xdatamode","manual");
}
  
void HandleLineSeries::ConstructProperties() {
  //!
  //@Module LINEPROPERTIES Line Series Object Properties
  //@@Section HANDLE
  //@@Usage
  //Below is a summary of the properties for a line series.
  //\begin{itemize}
  //  \item @|color| - @|colorspec| - The color that is used to 
  // draw the line.
  //  \item @|children| - Not used.
  //  \item @|displayname| - The name of this line series as it
  // appears in a legend.
  //  \item @|linestyle| - @|{'-','--',':','-.','none'}| - The style of the line.
  //  \item @|linewidth| - @|scalar| - The width of the line.
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
  //  \item @|parent| - @|handle| - The axis that contains this object.
  //  \item @|tag| - @|string| - A string that can be used to tag the object.
  //  \item @|type| - @|string| - Returns the string @|'line'|.
  //  \item @|visible| - @|{'on','off'}| - Controls visibility of the the line.
  //  \item @|xdata| - @|vector| - Vector of x coordinates of points on the line.  Must be
  // the same size as the @|ydata| and @|zdata| vectors.
  //  \item @|ydata| - @|vector| - Vector of y coordinates of points on the line.  Must be
  // the same size as the @|xdata| and @|zdata| vectors.
  //  \item @|zdata| - @|vector| - Vector of z coordinates of points on the line.  Must be
  // the same size as the @|xdata| and @|ydata| vectors.
  //  \item @|xdatamode| - @|{'auto','manual'}| - When set to @|'auto'| FreeMat will autogenerate
  // the x coordinates for the points on the line.  These values will be @|1,..,N| where
  // @|N| is the number of points in the line.
  //  \item @|userdata| - @|array| - Available to store any variable you
  // want in the handle object.
  //\end{itemize}
  //!
  AddProperty(new HPColor,"color");
  AddProperty(new HPHandles,"children");
  AddProperty(new HPString,"displayname");
  AddProperty(new HPLineStyle,"linestyle");
  AddProperty(new HPScalar,"linewidth");
  AddProperty(new HPSymbol,"marker");
  AddProperty(new HPColor,"markeredgecolor");
  AddProperty(new HPColor,"markerfacecolor");
  AddProperty(new HPScalar,"markersize");
  AddProperty(new HPHandles,"parent");
  AddProperty(new HPString,"tag");
  AddProperty(new HPString,"type");
  AddProperty(new HPOnOff,"visible");
  AddProperty(new HPVector,"xdata");
  AddProperty(new HPAutoManual,"xdatamode");
  AddProperty(new HPArray,"userdata");
  AddProperty(new HPVector,"ydata");
  AddProperty(new HPVector,"zdata");
}
