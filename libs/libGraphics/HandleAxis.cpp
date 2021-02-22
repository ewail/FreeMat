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
#include "HandleAxis.hpp"
#include "HandleList.hpp"
#include "HandleFigure.hpp"
#include "HandleText.hpp"
#include <qapplication.h>
#include <math.h>
#include <qpainter.h>
#include "QTRenderEngine.hpp"
#include "HandleCommands.hpp"
#include <qgl.h>
#include <QList>
#include "IEEEFP.hpp"

QList<double> GetTicksOuter(double amin, double amax, bool isLog, int requestedCounts) {
  double arange = amax - amin;
  double astep = pow(10.0,floor(log10(arange)));
  double nsteps = arange/astep;
  double aval;
  QList<double> retvec;
  if (requestedCounts >= 10) {
    if (nsteps <= 1)
      astep /= 10.0;
    else if (nsteps <= 2)
      astep /= 5.0;
    else if (nsteps <= 5)
      astep /= 2.0;
  } else if (requestedCounts >= 5) {
    if (nsteps > 5) {
      astep *= 2.0;
      nsteps = arange/astep;
    }
    if (nsteps <= 1)
      astep /= 5.0;
  } else if (requestedCounts >= 2) {
    if (nsteps > 2) {
      astep *= 5.0;
      nsteps = arange/astep;
    }
    if (nsteps <= 1)
      astep /= 2.0;
  }
  if (isLog)
    astep = ceil(astep);
  if ((amin < 0) && (amax > 0)) {
    aval = 0;
    while (aval < amax) {
      retvec.push_back(aval);
      aval += astep;
    }
    retvec.push_back(aval);
    aval = -astep;
    while (aval > amin) {
      retvec.push_front(aval);
      aval -= astep;
    }
    retvec.push_front(aval);
  } else if ((amin == 0) && (amax > 0)) {
    aval = 0;
    while (aval < amax) {
      retvec.push_back(aval);
      aval += astep;
    }
    retvec.push_back(aval);
  } else if ((amin < 0) && (amax == 0)) {
    aval = 0;
    while (aval > amin) {
      retvec.push_front(aval);
      aval -= astep;
    }
    retvec.push_front(aval);
  } else {
    aval = floor(amin/astep)*astep;
    while (aval < amax) {
      retvec.push_back(aval);
      aval += astep;
    }
    retvec.push_back(aval);
  }
  return retvec;
}

QList<double> GetTicksInner(double amin, double amax, bool isLog, int requestedCounts) {
  double arange = amax - amin;
  double astep = pow(10.0,floor(log10(arange)));
  double nsteps = arange/astep;
  double aval;
  QList<double> retvec;
  if (requestedCounts >= 10) {
    if (nsteps <= 1)
      astep /= 10.0;
    else if (nsteps <= 2)
      astep /= 5.0;
    else if (nsteps <= 5)
      astep /= 2.0;
  } else if (requestedCounts >= 5) {
    if (nsteps > 5) {
      astep *= 2.0;
      nsteps = arange/astep;
    }
    if (nsteps <= 1)
      astep /= 5.0;
  } else if (requestedCounts >= 2) {
    if (nsteps > 2) {
      astep *= 5.0;
      nsteps = arange/astep;
    }
    if (nsteps <= 1)
      astep /= 2.0;
  }
  if (isLog) astep = ceil(astep);
  if ((amin < 0) && (amax > 0)) {
    aval = 0;
    while (aval <= amax) {
      retvec.push_back(aval);
      aval += astep;
    }
    aval = -astep;
    while (aval >= amin) {
      retvec.push_front(aval);
      aval -= astep;
    }
  } else if ((amin == 0) && (amax > 0)) {
    aval = 0;
    while (aval <= amax) {
      retvec.push_back(aval);
      aval += astep;
    }
  } else if ((amin < 0) && (amax == 0)) {
    aval = 0;
    while (aval >= amin) {
      retvec.push_front(aval);
      aval -= astep;
    }
  } else {
    aval = ceil(amin/astep)*astep;
    while (aval <= amax) {
      retvec.push_back(aval);
      aval += astep;
    }
  }
  return retvec;
}


// Property list & status
//    activepositionproperty
//    alim
//    alimmode
//    ambientlightcolor
//    box
//    cameraposition - done
//    camerapositionmode  - done
//    cameratarget - done
//    cameratargetmode - done
//    cameraupvector - done
//    cameraviewangle 
//    cameraviewanglemode
//    children
//    clim
//    climmode
//    clipping
//    color - done (does 'none' work?)
//    colororder - done
//    dataaspectratio
//    dataaspectratiomode
//    fontangle - done
//    fontname - done
//    fontsize - done
//    fontunits - not implemented
//    fontweight - done
//    gridlinestyle - done
//    handlevisibility
//    hittest
//    interruptible
//    layer
//    //    linestyleorder
//    linewidth
//    minorgridlinestyle - done
//    nextplot
//    outerposition - done - need linkage to position
//    parent - done
//    plotboxaspectratio
//    plotboxaspectratiomode
//    position - done
//    projection
//    selected
//    selectionhighlight
//    tag - done
//    tickdir - done - need labels to not follow tick direction
//    tickdirmode - done
//    ticklength - need 2d support
//    tightinset
//    title
//    type
//    units
//    //    userdata
//    visible
//    xaxislocation - done
//    yaxislocation - done
//    xcolor - done
//    ycolor - done
//    zcolor - done
//    xdir - done
//    ydir - done
//    zdir - done
//    xgrid - done
//    ygrid - done
//    zgrid - done
//    xlabel - done
//    ylabel - done
//    zlabel - done
//    xlim - done
//    ylim - done
//    zlim - done
//    xlimmode - done
//    ylimmode - done
//    zlimmode - done
//    xminorgrid - done
//    yminorgrid - done
//    zminorgrid - done
//    xscale - done
//    yscale - done
//    zscale - done
//    xtick - done
//    ytick - done
//    ztick - done
//    xticklabel - done
//    yticklabel - done
//    zticklabel - done
//    xtickmode - done
//    ytickmode - done
//    ztickmode - done
//    xticklabelmode - done
//    yticklabelmode - done
//    zticklabelmode - done
 

// Need to build the transformation matrix...
// Given a position rectangle and a camera matrix,
//   1.  Map the 8 corners of the data space into the camera plane
//   1b. Use the data scaling factors in DataAspectRatio when
//       mapping the coordinates.
//   2.  Calculate scale factor & offsets appropriately to 
//       stretch the data to fit the position rectangle
//   Create a special 4x4 matrix that is the composite of 
//   the data scale, camera and xy scale matrices
//  
//   This is the final (view) matrix.
//
// Check out gluLookAt
//
// Actually, not...  it is more complicated than this.
// The view matrix maps _rotated_ x,y,z coordinates
// to the screen.  We want to specify the clipping
// volume in _unrotated_ coordinates.  So, what needs
// to happen is that we must transform the 8 corners
// of the clipped volume using the current modelview
// matrix, use them to establish new clipping planes
// in x, y, z, and then map the clipped volume to viewer
// space.

// Probably a better way to do this...

class pt3d {
public:
  double x;
  double y;
  double z;
  pt3d() : x(0), y(0), z(0) {}
  pt3d(double a, double b, double c) : x(a), y(b), z(c) {}
  pt3d operator-(const pt3d& t) {return pt3d(x-t.x,y-t.y,z-t.z);}
};

static QString TrimPrint(double val, bool scientificNotation) {
  char buffer[1000];
  char *p;
  if (!scientificNotation) {
    sprintf(buffer,"%f",val);
    p = buffer + strlen(buffer) - 1;
    while (*p == '0') {
      *p = 0;
      p--;
    }
    if ((*p == '.') || (*p == ',')) {
      *p = 0;
      //	*(p+1) = '0';
      //	*(p+2) = 0;
    }
    return QString(buffer);
  } else {
    sprintf(buffer,"%e",val);
    QString label(buffer);
    unsigned int ePtr;
    ePtr = label.size() - 1;
    while ((label[ePtr] != 'e') && (label[ePtr] != 'E'))
      ePtr--;
    ePtr--;
    while (label[ePtr] == '0') {
      label.remove(ePtr,1);
      ePtr--;
    }
    if ((label[ePtr] == '.') || (label[ePtr] == ','))
      label.insert(ePtr+1, '0');
    ePtr = label.size() - 1;
    while ((label[ePtr] != 'e') && (label[ePtr] != 'E'))
      ePtr--;
    ePtr+=2;
    while ((label[ePtr] == '0') && ePtr < (unsigned int)(label.size())) {
      label.remove(ePtr,1);
    }
    if (ePtr == (unsigned int)(label.size()))
      label.append("0");
    return label;
  }
}

const int MAX_TICK_COUNT = 1000;

double tlog(double x) {
  if (x>0) 
    return log10(x);
  else
    return -10;
}

// Construct an axis 
void FormatAxisManual(double t1, double t2, int tickcount,
		      bool isLogarithmic,
		      double& tStart, double &tStop,
		      QVector<double> &tickLocations,
		      StringVector &tlabels) {
  int tCount;
  tickLocations.clear();
  tlabels.clear();
  bool exponentialForm;
  exponentialForm = false;
  QList<double> tick_locations(GetTicksInner(t1,t2,isLogarithmic,tickcount));
  if (tick_locations.empty()) return;
  tStart = tick_locations.front();
  tStop = tick_locations.back();
  tCount = tick_locations.size();
  for (int i=0;i<tCount;i++) {
    double tloc = tick_locations[i];
    if (!isLogarithmic)
      tickLocations.push_back(tloc);
    else
      tickLocations.push_back(pow(10.0,tloc));
    if (tloc != 0.0)
      exponentialForm |= (fabs(log10(fabs(tloc))) >= 4.0);
  }
  for (int i=0;i<tCount;i++) {
    double tloc = tick_locations[i];
    if (!isLogarithmic)
      tlabels.push_back(TrimPrint(tloc,exponentialForm));
    else
      tlabels.push_back(TrimPrint(pow(10.0,tloc),true));
  }
}
  
void FormatAxisAuto(double tMin, double tMax, int tickcount,
		    bool isLogarithmic,
		    double& tStart, double &tStop,
		    QVector<double> &tickLocations,
		    StringVector &tlabels) {
  int tCount;
  tickLocations.clear();
  tlabels.clear();
  bool exponentialForm;
  exponentialForm = false;
  //  const double *dp = (const double *) trange.getDataPointer();
  QList<double> tick_locations(GetTicksOuter(tMin,tMax,isLogarithmic,tickcount));
  if (tick_locations.empty()) return;
  tStart = tick_locations.front();
  tStop = tick_locations.back();
  tCount = tick_locations.size();
  for (int i=0;i<tCount;i++) {
    double tloc = tick_locations[i];
    if (!isLogarithmic)
      tickLocations.push_back(tloc);
    else
      tickLocations.push_back(pow(10.0,tloc));
    if (tloc != 0.0)
      exponentialForm |= (fabs(log10(fabs(tloc))) >= 4.0);
  }
  for (int i=0;i<tCount;i++) {
    double tloc = tick_locations[i];
    if (!isLogarithmic)
      tlabels.push_back(TrimPrint(tloc,exponentialForm));
    else
      tlabels.push_back(TrimPrint(pow(10.0,tloc),true));
  }
}

void HandleAxis::GetMaxTickMetric(RenderEngine &gc,
				  StringVector labs,
				  double &maxx, double &maxy) {
  maxx = 0;
  maxy = 0;
  for (int i=0;i<labs.size();i++) {
    int width, height, xoffset, yoffset;
    gc.measureText(labs[i],m_font,
		   RenderEngine::Min,RenderEngine::Min,
		   width, height, xoffset, yoffset);
    maxx = qMax(maxx,(double)width);
    maxy = qMax(maxy,(double)height);
  }      
}
  
void HandleAxis::ConstructProperties() {
  // These are all the properties of the axis
  //!
  //@Module AXISPROPERTIES Axis Object Properties
  //@@Section HANDLE
  //@@Usage
  //Below is a summary of the properties for the axis. 
  //\begin{itemize}
  //  \item @|activepositionproperty| - @|four vector| - Not used.
  //  \item @|alim| - @|two vector| - Controls the mapping of 
  // transparency.  The vector @|[a_1,a_2]|@ defines the scale for transparency.
  // Plots then map @|a_1| to a completely opaque value, and @|a_2| to a 
  // completely transparent value.  This mapping is applied to the alpha
  // data of the plot data.
  //  \item @|alimmode| - @|{'auto','manual'}| - For @|auto| mode, we 
  // map the alpha ranges of all objects in the plot to a full scale.
  // For @|manual| mode, we use the @|alim| vector.
  //  \item @|ambientlightcolor| - @|colorspec| - Not used.
  //  \item @|box| - @|On/Off| - Not used.
  //  \item @|cameraposition| - @|three vector| - Set the position for the
  // camera in axis space.
  //  \item @|camerapositionmode| - @|{'auto','manual'}| - For @|manual|
  // mode, the camera position is picked up from the @|cameraposition| vector.
  // For @|auto| mode, the camera position is set to be centered on the 
  // @|x| and @|y| axis limits, and beyond the @|z| maximum limit.
  //  \item @|cameratarget| - @|three vector| - Defines the point in axis
  // space that the camera is targetted at.
  //  \item @|cameratargetmode| - @|{'auto','manual'}| - For @|manual|
  // mode the camera target is picked up from the @|cameratarget| vector.  For
  // @|auto| mode, the camera target is chosen to be the center of the 
  // three axes.
  //  \item @|cameraupvector| - @|three vector| - Defines the upwards vector
  // for the camera (what is ultimately mapped to the vertical axis of the
  // plot or screen).  This vector must not be parallel to the vector that
  // is defined by the optical axis (i.e., the one connecting the target to the
  // camera position).
  //  \item @|cameraupvectormode| - @|{'auto','manual'}| - For @|manual|
  // mode, the camera up vector is picked up from the @|cameraupvector|.  The
  // @|auto| mode chooses the up vector to point along the positive @|y| axis.
  //  \item @|cameraviewangle| - @|scalar| - Not used.
  //  \item @|cameraviewanglemode| - @|{'auto','manual'}| - Not used.
  //  \item @|children| - @|vector of handles| - A vector containing handles to
  // children of the current axis.  Be careful as to how you manipulate this
  // vector.  FreeMat uses a reference counting mechanism for graphics objects,
  // so if you remove a handle from the @|children| property of an axis, and
  // you have not added it to the @|children| property of another object, it
  // will be deleted.
  //  \item @|clim| - @|two vector| - The color range vector.  This vector
  // contains two values that dictate how children of this axis get mapped
  // to the colormap.  Values between the two endpoints of this vector are mapped
  // to the extremes of the colormap.
  //  \item @|climmode| - @|{'auto','manual'}| - For @|auto| mode, the color limits
  // are chosen to span the colordata for all of the children objects.  For @|manual|
  // mode, the color mapping is based on @|clim|.
  //  \item @|clipping| - @|{'on','off'}| - Not used.
  //  \item @|color| - @|colorspec| - The color used to draw the background box
  // for the axes.  Defaults to white.
  //  \item @|colororder| - @|color vector| - A vector of color specs (in 
  // RGB) that are cycled between when drawing line plots into this axis.
  // The default is order blue,green,red,cyan,magenta,yellow,black.
  //  \item @|datalimits| - @|six vector| - A vector that contains the x, y and z
  // limits of the data for children of the current axis.  Changes to this
  // property are ignored - it is calculated by FreeMat based on the datasets.
  //  \item @|dataaspectratio| - @|three vector| - A vector that describes the
  // aspect ratio of the data.  You can think of this as the relative scaling of 
  // units for each axis.  For example, if one unit along the x axis is twice
  // as long as one unit along the y axis, you would specify a data aspect 
  // ratio of @|[2,1,1]|.
  //  \item @|dataaspectratiomode| - @|{'auto','manual'}| - When the data aspect 
  // ratio is set to @|manual|, the data is scaled by the data aspect ratio before
  // being plotted.  When the data aspect ratio mode is @|auto| a complex set of
  // rules are applied to determine how the data should be scaled.  If @|dataaspectratio|
  // mode is @|auto| and @|plotboxaspectratio| is @|auto|, then the default data aspect
  // ratio is set to @|[1,1,1]| and the default plot box aspect ratio is chosen proportional
  // to @|[xrange,yrange,zrange]|, where @|xrange| is the span of data along the @|x|
  // axis, and similarly for @|yrange| and @|zrange|.  If @|plotboxaspectratio| is set to
  // @|[px,py,pz]|, then the @|dataaspectratio| is set to @|[xrange/px,yrange/py,zrange/pz]|.
  // If one of the axes has been specified manually, then the data will be scaled to fit
  // the axes as well as possible.
  //  \item @|fontangle| - @|{'normal','italic','oblique'}| - The angle of the fonts used
  // for text labels (e.g., tick labels).
  //  \item @|fontsize| - @|scalar| - The size of fonts used for text labels (tick labels).
  //  \item @|fontunits| - Not used.
  //  \item @|fontweight| - @|{'normal','bold','light','demi'}| - The weight of the font used
  // for tick labels.
  //  \item @|gridlinestyle| - @|{'-','--',':','-.','none'}| - The line style to use for 
  // drawing the grid lines.  Defaults to @|':'|.
  //  \item @|handlevisibility| - Not used.
  //  \item @|hittest| - Not used.
  //  \item @|interruptible| - Not used.
  //  \item @|layer| - Not used.
  //  \item @|linestyleorder| - @|linestyle vector| - A vector of linestyles that are cycled
  // through when plotted line series.
  //  \item @|linewidth| - @|scalar| - The width of line used to draw grid lines, axis lines, 
  // and other lines.
  //  \item @|minorgridlinestyle| - @|{'-','--',':','-.','none'}| - The line style used for
  // drawing grid lines through minor ticks.
  //  \item @|nextplot| - @|{'add','replace','replacechildren'}| - Controls how the next plot
  // interacts with the axis.  If it is set to @|'add'| the next plot will be added to the
  // current axis.  If it is set to @|'replace'| the new plot replaces all of the previous
  // children.
  //  \item @|outerposition| - @|four vector| - Specifies the coordinates of the outermost
  // box that contains the axis relative to the containing figure.  This vector is in normalized
  // coordinates and corresponds to the @|x, y, width, height| coordinates of the box.
  //  \item @|parent| - @|handle| - The handle for the containing object (a figure).
  //  \item @|plotboxaspectratio| - @|three vector| - Controls the aspect ratio of the plot
  // box.  See the entry under @|dataaspectratio| for details on how FreeMat uses this
  // vector in combination with the axis limits and the @|plotboxaspectratio| to determine
  // how to scale the data. 
  //  \item @|plotboxaspectratiomode| - @|{'auto','manual'}| - The plot box aspect ratio mode
  // interacts with the @|dataaspectratiomode| and the axis limits.
  //  \item @|position| - @|fourvector| - The normalized coordinates of the plot box space.
  // Should be inside the rectable defined by @|outerposition|.
  //  \item @|positionmode| - @|{'auto','manual'}| - the position mode is normally @|'auto'|
  // which means that FreeMat computes the position vector to fit the plot inside the @|outerposition|
  // vector.  If you set the @|position| vector manually (using a @|set| command), this @|mode|
  // flag becomes @|'manual'| and remains that way until reset to @|'auto'.
  //  \item @|projection| - Not used.
  //  \item @|selected| - Not used.
  //  \item @|selectionhighlight| - Not used.
  //  \item @|tag| - A string that can be set to tag the axes with a name.
  //  \item @|textheight| - @|scalar| - This value is set by FreeMat to the height of the
  // current font in pixels.  
  //  \item @|tickdir| - @|{'in','out'}| - The direction of ticks.  Defaults to @|'in'| for 2D
  // plots, and @|'out'| for 3D plots if @|tickdirmode| is @|auto|.
  //  \item @|tickdirmode| - @|{'auto','manual'}| - When set to @|'auto'| the @|tickdir| 
  // defaults to @|'in'| for 2D plots, and @|'out'| for 3D plots.
  //  \item @|ticklength| - @|two vector| - The first element is the length of the tick in 
  // 2D plots, and the second is the length of the tick in the 3D plots.  The lengths are 
  // described as fractions of the longer dimension (width or height).
  //  \item @|tightinset| - Not used.
  //  \item @|title| - @|handle| - The handle of the label used to represent the title of
  // the plot.
  //  \item @|type| - @|string| - Takes the value of @|'axes'| for objects of the axes type.
  //  \item @|units| - Not used.
  //  \item @|userdata| - @|array| - An arbitrary array you can set to anything you want.
  //  \item @|visible| - @|{'on','off'}| - If set to @|'on'| the axes are drawn as normal.
  // If set to @|'off'|, only the children of the axes are drawn. The plot box, axis lines,
  // and tick labels are not drawn.
  //  \item @|xaxislocation| - @|{'top','bottom'}| - Controls placement of the x axis.
  //  \item @|yaxislocation| - @|{'left','right'}| - Controls placement of the y axis.
  //  \item @|xcolor| - @|colorspec| - The color of the x elements including the the x axis
  // line, ticks, grid lines and tick labels
  //  \item @|ycolor| - @|colorspec| - The color of the y elements including the the y axis
  // line, ticks, grid lines and tick labels.
  //  \item @|zcolor| - @|colorspec| - The color of the z elements including the the z axis
  // line, ticks, grid lines and tick labels.
  //  \item @|xdir| - @|{'normal','reverse'}| - For @|normal|, axes are drawn as you
  // would expect (e.g, in default 2D mode, the x axis has values increasing from left
  // to right.  For @|reverse|, the x axis has values increasing from right to left.
  //  \item @|ydir| - @|{'normal','reverse'}| - For @|normal|, axes are drawn as you
  // would expect (e.g, in default 2D mode, the y axis has values increasing from bottom
  // to top.  For @|reverse|, the y axis has values increasing from top to bottom.
  //  \item @|zdir| - @|{'normal','reverse'}| - For @|normal|, axes are drawn as you
  // would expect. In default 3D mode, the z axis has values increasing in some direction
  // (usually up).  For @|reverse| the z axis increases in the opposite direction.
  //  \item @|xgrid| - @|{'on','off'}| - Set to @|on| to draw grid lines from ticks on
  // the x axis.
  //  \item @|ygrid| - @|{'on','off'}| - Set to @|on| to draw grid lines from ticks on
  // the y axis.
  //  \item @|zgrid| - @|{'on','off'}| - Set to @|on| to draw grid lines from ticks on
  // the z axis.
  //  \item @|xlabel| - @|handle| - The handle of the text label attached to the x axis.
  // The position of that label and the rotation angle is computed automatically by
  // FreeMat.
  //  \item @|ylabel| - @|handle| - The handle of the text label attached to the y axis.
  // The position of that label and the rotation angle is computed automatically by
  // FreeMat.
  //  \item @|zlabel| - @|handle| - The handle of the text label attached to the z axis.
  // The position of that label and the rotation angle is computed automatically by
  // FreeMat.
  //  \item @|xlim| - @|two vector| - Contains the limits of the data along the x axis.
  // These are set automatically for @|xlimmode|.  When manually set it allows you to
  // zoom into the data.  The first element of this vector should be the smallest x value
  // you want mapped to the axis, and the second element should be the largest.
  //  \item @|ylim| - @|two vector| - Contains the limits of the data along the y axis.
  // These are set automatically for @|ylimmode|.  When manually set it allows you to
  // zoom into the data.  The first element of this vector should be the smallest y value
  // you want mapped to the axis, and the second element should be the largest.
  //  \item @|zlim| - @|two vector| - Contains the limits of the data along the z axis.
  // These are set automatically for @|zlimmode|.  When manually set it allows you to
  // zoom into the data.  The first element of this vector should be the smallest z value
  // you want mapped to the axis, and the second element should be the largest.
  //  \item @|xlimmode| - @|{'auto','manual'}| - Determines if @|xlim| is determined
  // automatically or if it is determined manually.  When determined automatically, it
  // is chosen to span the data range (at least).
  //  \item @|ylimmode| - @|{'auto','manual'}| - Determines if @|ylim| is determined
  // automatically or if it is determined manually.  When determined automatically, it
  // is chosen to span the data range (at least).
  //  \item @|zlimmode| - @|{'auto','manual'}| - Determines if @|zlim| is determined
  // automatically or if it is determined manually.  When determined automatically, it
  // is chosen to span the data range (at least).
  //  \item @|xminorgrid| - @|{'on','off'}| - Set to @|on| to draw grid lines from minor ticks on
  // the x axis.
  //  \item @|yminorgrid| - @|{'on','off'}| - Set to @|on| to draw grid lines from minor ticks on
  // the y axis.
  //  \item @|zminorgrid| - @|{'on','off'}| - Set to @|on| to draw grid lines from minor ticks on
  // the z axis.
  //  \item @|xscale| - @|{'linear','log'}| - Determines if the data on the x axis is linear or
  // logarithmically scaled.
  //  \item @|yscale| - @|{'linear','log'}| - Determines if the data on the y axis is linear or
  // logarithmically scaled.
  //  \item @|zscale| - @|{'linear','log'}| - Determines if the data on the z axis is linear or
  // logarithmically scaled.
  //  \item @|xtick| - @|vector| - A vector of x coordinates where ticks are placed on the 
  // x axis.  Setting this vector allows you complete control over the placement of ticks on 
  // the axis.
  //  \item @|ytick| - @|vector| - A vector of y coordinates where ticks are placed on the 
  // y axis.  Setting this vector allows you complete control over the placement of ticks on 
  // the axis.
  //  \item @|ztick| - @|vector| - A vector of z coordinates where ticks are placed on the 
  // z axis.  Setting this vector allows you complete control over the placement of ticks on 
  // the axis.
  //  \item @|xticklabel| - @|string vector| - A string vector, of the form @|'string|string|string'|
  // that contains labels to assign to the labels on the axis.  If this vector is shorter than
  // @|xtick|, then FreeMat will cycle through the elements of this vector to fill out the labels.
  //  \item @|yticklabel| - @|string vector| - A string vector, of the form @|'string|string|string'|
  // that contains labels to assign to the labels on the axis.  If this vector is shorter than
  // @|ytick|, then FreeMat will cycle through the elements of this vector to fill out the labels.
  //  \item @|zticklabel| - @|string vector| - A string vector, of the form @|'string|string|string'|
  // that contains labels to assign to the labels on the axis.  If this vector is shorter than
  // @|ztick|, then FreeMat will cycle through the elements of this vector to fill out the labels.
  //  \item @|xtickmode| - @|{'auto','manual'}| - Set to @|'auto'| if you want FreeMat to calculate
  // the tick locations.  Setting @|'xtick'| will cause this property to switch to @|'manual'|.
  //  \item @|ytickmode| - @|{'auto','manual'}| - Set to @|'auto'| if you want FreeMat to calculate
  // the tick locations.  Setting @|'ytick'| will cause this property to switch to @|'manual'|.
  //  \item @|ztickmode| - @|{'auto','manual'}| - Set to @|'auto'| if you want FreeMat to calculate
  // the tick locations.  Setting @|'ztick'| will cause this property to switch to @|'manual'|.
  //  \item @|xticklabelmode| - @|{'auto','manual'}| - Set to @|'auto'| if you want FreeMat to
  // set the tick labels.  This will be based on the vector @|xtick|.
  //  \item @|yticklabelmode| - @|{'auto','manual'}| - Set to @|'auto'| if you want FreeMat to
  // set the tick labels.  This will be based on the vector @|ytick|.
  //  \item @|zticklabelmode| - @|{'auto','manual'}| - Set to @|'auto'| if you want FreeMat to
  // set the tick labels.  This will be based on the vector @|ztick|.
  //\end{itemize}
  //!
  AddProperty(new HPPosition, "activepositionproperty");
  AddProperty(new HPTwoVector,"alim");
  AddProperty(new HPAutoManual,"alimmode");
  AddProperty(new HPColor,"ambientlightcolor");
  AddProperty(new HPOnOff,"box");
  AddProperty(new HPThreeVector,"cameraposition");
  AddProperty(new HPAutoManual,"camerapositionmode");
  AddProperty(new HPThreeVector,"cameratarget");
  AddProperty(new HPAutoManual,"cameratargetmode");
  AddProperty(new HPThreeVector,"cameraupvector");
  AddProperty(new HPAutoManual,"cameraupvectormode");
  AddProperty(new HPScalar,"cameraviewangle");
  AddProperty(new HPAutoManual,"cameraviewanglemode");
  AddProperty(new HPHandles,"children");
  AddProperty(new HPTwoVector,"clim");
  AddProperty(new HPAutoManual,"climmode");
  AddProperty(new HPOnOff,"clipping");
  AddProperty(new HPColor,"color");
  AddProperty(new HPColorVector,"colororder");
  AddProperty(new HPSixVector,"datalimits");
  AddProperty(new HPThreeVector,"dataaspectratio");
  AddProperty(new HPAutoManual,"dataaspectratiomode");
  AddProperty(new HPFontAngle,"fontangle");
  AddProperty(new HPString,"fontname");
  AddProperty(new HPScalar,"fontsize");
  AddProperty(new HPFontUnits,"fontunits");
  AddProperty(new HPFontWeight,"fontweight");
  AddProperty(new HPLineStyle,"gridlinestyle");
  AddProperty(new HPOnOff,"handlevisibility");
  AddProperty(new HPOnOff,"hittest");
  AddProperty(new HPOnOff,"interruptible");
  AddProperty(new HPTopBottom,"layer");
  AddProperty(new HPLineStyleOrder,"linestyleorder");
  AddProperty(new HPScalar,"linewidth");
  AddProperty(new HPLineStyle,"minorgridlinestyle");
  AddProperty(new HPNextPlotMode,"nextplot");
  AddProperty(new HPFourVector,"outerposition");
  AddProperty(new HPHandles,"parent");
  AddProperty(new HPThreeVector,"plotboxaspectratio");
  AddProperty(new HPAutoManual,"plotboxaspectratiomode");
  AddProperty(new HPFourVector,"position");
  AddProperty(new HPAutoManual,"positionmode");
  AddProperty(new HPProjectionMode,"projection");
  AddProperty(new HPOnOff,"selected");
  AddProperty(new HPOnOff,"selectionhighlight");
  AddProperty(new HPString,"tag");
  AddProperty(new HPScalar,"textheight");
  AddProperty(new HPInOut,"tickdir");
  AddProperty(new HPAutoManual,"tickdirmode");
  AddProperty(new HPTwoVector,"ticklength");
  AddProperty(new HPFourVector,"tightinset");
  AddProperty(new HPHandles,"title");
  AddProperty(new HPString,"type");
  AddProperty(new HPUnits,"units");
  AddProperty(new HPArray,"userdata");
  AddProperty(new HPOnOff,"visible");
  AddProperty(new HPTopBottom,"xaxislocation");
  AddProperty(new HPLeftRight,"yaxislocation");
  AddProperty(new HPColor,"xcolor");
  AddProperty(new HPColor,"ycolor");
  AddProperty(new HPColor,"zcolor");
  AddProperty(new HPNormalReverse,"xdir");
  AddProperty(new HPNormalReverse,"ydir");
  AddProperty(new HPNormalReverse,"zdir");
  AddProperty(new HPOnOff,"xgrid");
  AddProperty(new HPOnOff,"ygrid");
  AddProperty(new HPOnOff,"zgrid");
  AddProperty(new HPHandles,"xlabel");
  AddProperty(new HPHandles,"ylabel");
  AddProperty(new HPHandles,"zlabel");
  AddProperty(new HPTwoVector,"xlim");
  AddProperty(new HPTwoVector,"ylim");
  AddProperty(new HPTwoVector,"zlim");
  AddProperty(new HPTwoVector,"xlim_raw");
  AddProperty(new HPTwoVector,"ylim_raw");
  AddProperty(new HPTwoVector,"zlim_raw");
  AddProperty(new HPAutoManual,"xlimmode");
  AddProperty(new HPAutoManual,"ylimmode");
  AddProperty(new HPAutoManual,"zlimmode");
  AddProperty(new HPOnOff,"xminorgrid");
  AddProperty(new HPOnOff,"yminorgrid");
  AddProperty(new HPOnOff,"zminorgrid");
  AddProperty(new HPLinearLog,"xscale");
  AddProperty(new HPLinearLog,"yscale");
  AddProperty(new HPLinearLog,"zscale");
  AddProperty(new HPVector,"xtick");
  AddProperty(new HPVector,"ytick");
  AddProperty(new HPVector,"ztick");
  AddProperty(new HPStringSet,"xticklabel");
  AddProperty(new HPStringSet,"yticklabel");
  AddProperty(new HPStringSet,"zticklabel");
  AddProperty(new HPAutoManual,"xtickmode");
  AddProperty(new HPAutoManual,"ytickmode");
  AddProperty(new HPAutoManual,"ztickmode");
  AddProperty(new HPAutoManual,"xticklabelmode");
  AddProperty(new HPAutoManual,"yticklabelmode");
  AddProperty(new HPAutoManual,"zticklabelmode");
}

HandleAxis::HandleAxis() {
  ConstructProperties();
  SetupDefaults();
}

HandleAxis::~HandleAxis() {
}

void HandleAxis::SetupDefaults() {
  SetConstrainedStringDefault("activepositionproperty","outerposition");
  SetConstrainedStringDefault("alimmode","auto");
  SetConstrainedStringDefault("box","off");
  SetThreeVectorDefault("cameraposition",0,0,1);
  SetConstrainedStringDefault("camerapositionmode","auto");
  SetThreeVectorDefault("cameratarget",0,0,0);
  SetConstrainedStringDefault("cameratargetmode","auto");
  SetThreeVectorDefault("cameraupvector",0,1,0);
  SetConstrainedStringDefault("cameraupvectormode","auto");
  SetConstrainedStringDefault("cameraviewanglemode","auto");
  SetConstrainedStringDefault("climmode","auto");
  SetConstrainedStringDefault("clipping","on");
  SetThreeVectorDefault("color",1,1,1);
  // Set up the default color order
  QVector<double> colors;
  colors.push_back(0.0); colors.push_back(0.0); colors.push_back(1.0); 
  colors.push_back(0.0); colors.push_back(0.5); colors.push_back(0.0); 
  colors.push_back(1.0); colors.push_back(0.0); colors.push_back(0.0);
  colors.push_back(0.0); colors.push_back(.75); colors.push_back(.75);
  colors.push_back(.75); colors.push_back(0.0); colors.push_back(.75);
  colors.push_back(.75); colors.push_back(.75); colors.push_back(0.0);
  colors.push_back(.25); colors.push_back(.25); colors.push_back(.25);
  HPVector *hp = (HPVector*) LookupProperty("colororder");
  hp->Data(colors);
  SetThreeVectorDefault("dataaspectratio",1,1,1);
  SetConstrainedStringDefault("dataaspectratiomode","auto");
  //    SetConstrainedStringDefault("drawmode","normal");
  SetConstrainedStringDefault("fontangle","normal");
  SetStringDefault("fontname","helvetica");
  SetScalarDefault("fontsize",10);
  SetConstrainedStringDefault("fontunits","points");
  SetConstrainedStringDefault("fontweight","normal");
  SetConstrainedStringDefault("gridlinestyle",":");
  SetConstrainedStringDefault("handlevisibility","on");
  SetConstrainedStringDefault("hittest","on");
  SetConstrainedStringDefault("interruptible","on");
  SetConstrainedStringDefault("layer","bottom");
  SetScalarDefault("linewidth",1.0);
  SetConstrainedStringSetDefault("linestyleorder","-|--|:|-.");
  SetConstrainedStringDefault("minorgridlinestyle",":");
  SetFourVectorDefault("outerposition",0,0,1,1);
  SetConstrainedStringDefault("nextplot","replace");
  SetThreeVectorDefault("plotboxaspectratio",1,1,1);
  SetConstrainedStringDefault("plotboxaspectratiomode","auto");
  SetFourVectorDefault("position",0.13,0.11,0.775,0.815); // x0, y0, delx, dely
  SetConstrainedStringDefault("positionmode","auto");
  SetConstrainedStringDefault("projection","orthographic");
  SetConstrainedStringDefault("selected","off");
  SetConstrainedStringDefault("selectionhighlight","on");
  SetConstrainedStringDefault("tickdir","in");
  SetConstrainedStringDefault("tickdirmode","auto");
  SetTwoVectorDefault("ticklength",0.01,0.025);
  SetStringDefault("type","axes");
  SetConstrainedStringDefault("units","normalized");
  SetConstrainedStringDefault("visible","on");
  SetConstrainedStringDefault("xaxislocation","bottom");
  SetConstrainedStringDefault("yaxislocation","left");
  SetThreeVectorDefault("xcolor",0,0,0);
  SetThreeVectorDefault("ycolor",0,0,0);
  SetThreeVectorDefault("zcolor",0,0,0);
  SetConstrainedStringDefault("xdir","normal");
  SetConstrainedStringDefault("ydir","normal");
  SetConstrainedStringDefault("zdir","normal");
  SetConstrainedStringDefault("xgrid","off");
  SetConstrainedStringDefault("ygrid","off");
  SetConstrainedStringDefault("zgrid","off");
  SetTwoVectorDefault("xlim",0,1);
  SetTwoVectorDefault("ylim",0,1);
  SetTwoVectorDefault("zlim",0,1);
  SetConstrainedStringDefault("xlimmode","auto");
  SetConstrainedStringDefault("ylimmode","auto");
  SetConstrainedStringDefault("zlimmode","auto");
  SetConstrainedStringDefault("xminorgrid","off");
  SetConstrainedStringDefault("yminorgrid","off");
  SetConstrainedStringDefault("zminorgrid","off");
  SetConstrainedStringDefault("xscale","linear");
  SetConstrainedStringDefault("yscale","linear");
  SetConstrainedStringDefault("zscale","linear");
  SetConstrainedStringDefault("xtickmode","auto");
  SetConstrainedStringDefault("ytickmode","auto");
  SetConstrainedStringDefault("ztickmode","auto");
  SetConstrainedStringDefault("xticklabelmode","auto");
  SetConstrainedStringDefault("yticklabelmode","auto");
  SetConstrainedStringDefault("zticklabelmode","auto");
  UpdateAxisFont();
}

void HandleAxis::SetAxisLimits(QVector<double> lims) {
  //     qDebug("Set Limits %f %f %f %f %f %f",
  // 	   lims[0],lims[1],lims[2],lims[3],lims[4],lims[5]);
  HPLinearLog *sp;
  sp = (HPLinearLog*) LookupProperty("xscale");
  if (sp->Is("linear")) 
    SetTwoVectorDefault("xlim",lims[0],lims[1]);
  else 
    SetTwoVectorDefault("xlim",pow(10.0,lims[0]),pow(10.0,lims[1]));
  sp = (HPLinearLog*) LookupProperty("yscale");
  if (sp->Is("linear")) 
    SetTwoVectorDefault("ylim",lims[2],lims[3]);
  else 
    SetTwoVectorDefault("ylim",pow(10.0,lims[2]),pow(10.0,lims[3]));
  sp = (HPLinearLog*) LookupProperty("zscale");
  if (sp->Is("linear")) 
    SetTwoVectorDefault("zlim",lims[4],lims[5]);
  else 
    SetTwoVectorDefault("zlim",pow(10.0,lims[4]),pow(10.0,lims[5]));
}

void HandleAxis::GetAxisLimitsHelper(QString axisname, QVector<double> &lims, bool rawmode)
{
  HPTwoVector *hp;
  HPLinearLog *sp;
  double minval, maxval;
  if (rawmode && IsAuto(axisname + "limmode"))
    hp = (HPTwoVector*) LookupProperty(axisname + "lim_raw");
  else
    hp = (HPTwoVector*) LookupProperty(axisname + "lim");
  minval = hp->Data()[0];
  maxval = hp->Data()[1];
  if (!IsFinite(minval) || !IsFinite(maxval))
    {
      hp = (HPTwoVector*) LookupProperty(axisname + "lim_raw");
      if (!IsFinite(minval)) minval = hp->Data()[0];
      if (!IsFinite(maxval)) maxval = hp->Data()[1];
    }
  sp = (HPLinearLog*) LookupProperty(axisname + "scale");
  if (sp->Is("linear")) {
    lims.push_back(minval);
    lims.push_back(maxval);
  } else {
    lims.push_back(tlog(minval));
    lims.push_back(tlog(maxval));
  }
}

QVector<double> HandleAxis::GetAxisLimits(bool rawmode) {
  QVector<double> lims;
  GetAxisLimitsHelper("x",lims,rawmode);
  GetAxisLimitsHelper("y",lims,rawmode);
  GetAxisLimitsHelper("z",lims,rawmode);
  return lims;
}

QVector<double> HandleAxis::UnitsReinterpret(RenderEngine &gc, QVector<double> a) {
  unsigned width = gc.width();
  unsigned height = gc.height();
  HPUnits *hp = (HPUnits*) LookupProperty("units");
  if (hp->Is("normalized")) {
    for (int i=0;i<a.size();i+=2) {
      a[i] *= width;
      a[i+1] *= height;
    }
    return a;
  } else if (hp->Is("pixels")) {
    return a;
  } else {
    throw Exception("Units of " + hp->Data() + " not yet implemented - please file a Request For Enhancement (RFE) on the FreeMat web site");
  }
}
  
QVector<double> HandleAxis::GetPropertyVectorAsPixels(RenderEngine& gc, QString name) {
  HPFourVector *hp = (HPFourVector*) LookupProperty(name);
  return (UnitsReinterpret(gc,hp->Data()));
}

static void MinMaxVector(double *vals, int len, double &vmin, double &vmax) {
  vmin = vmax = vals[0];
  for (int i=0;i<len;i++) {
    vmin = (vals[i] < vmin) ? vals[i] : vmin;
    vmax = (vals[i] > vmax) ? vals[i] : vmax;
  }
}

QVector<double> HandleAxis::ReMap(QVector<double> t) {
  QVector<double> s;
  for (int i=0;i<t.size();i+=3) {
    s.push_back(MapX(t[i]));
    s.push_back(MapY(t[i+1]));
    s.push_back(MapZ(t[i+2]));
  }
  return s;
}

void HandleAxis::ReMap(QVector<double> xs, QVector<double> ys,
		       QVector<double> zs, QVector<double> &ax,
		       QVector<double> &ay, QVector<double> &az) {
  for (int i=0;i<xs.size();i++) {
    ax.push_back(MapX(xs[i]));
    ay.push_back(MapY(ys[i]));
    az.push_back(MapZ(zs[i]));
  }    
}
  
// x in [a,b]
// a->b
// b->a
// y = a-x+b = (a+b) - x
double HandleAxis::MapX(double x) {
  HPNormalReverse *hp;    
  hp = (HPNormalReverse*) LookupProperty("xdir");
  HPTwoVector *xlim;
  xlim = (HPTwoVector*) LookupProperty("xlim");
  QVector<double> lims(xlim->Data());
  HPLinearLog *sp;
  sp = (HPLinearLog*) LookupProperty("xscale");
  if (sp->Is("log"))
    x = tlog(x);
  double xmin(lims[0]);
  double xmax(lims[1]);
  if (hp->Is("reverse")) 
    return(xmin+xmax-x);
  else
    return(x);
}

double HandleAxis::MapY(double y) {
  HPNormalReverse *hp;    
  hp = (HPNormalReverse*) LookupProperty("ydir");
  HPTwoVector *ylim;
  ylim = (HPTwoVector*) LookupProperty("ylim");
  QVector<double> lims(ylim->Data());
  HPLinearLog *sp;
  sp = (HPLinearLog*) LookupProperty("yscale");
  if (sp->Is("log"))
    y = tlog(y);
  double ymin(lims[0]);
  double ymax(lims[1]);
  if (hp->Is("reverse")) 
    return(ymin+ymax-y);
  else
    return(y);
}

double HandleAxis::MapZ(double z) {
  HPNormalReverse *hp;    
  hp = (HPNormalReverse*) LookupProperty("zdir");
  HPTwoVector *zlim;
  zlim = (HPTwoVector*) LookupProperty("zlim");
  QVector<double> lims(zlim->Data());
  HPLinearLog *sp;
  sp = (HPLinearLog*) LookupProperty("zscale");
  if (sp->Is("log"))
    z = tlog(z);
  double zmin(lims[0]);
  double zmax(lims[1]);
  if (hp->Is("reverse")) 
    return(zmin+zmax-z);
  else
    return(z);
}

void rescale(double& amin, double &amax, double &ascale) {
  double amean = (amin+amax)/2.0;
  amin = amean-ascale*(amean-amin);
  amax = amean+ascale*(amax-amean);
}

void rerange(double& amin, double& amax, double arange) {
  double amean = (amin+amax)/2.0;
  amin = amean-arange/2.0;
  amax = amean+arange/2.0;
}

void HandleAxis::SetupProjection(RenderEngine &gc) {
  SetupCamera(gc);
  // Scale using the data aspect ratio
  QVector<double> dar(VectorPropertyLookup("dataaspectratio"));
  gc.scale(1.0/dar[0],1.0/dar[1],1.0/dar[2]);
  // Get the axis limits
  double xmin, xmax, ymin, ymax, zmin, zmax;
  MapCorners(gc,xmin,xmax,ymin,ymax,zmin,zmax);
  QVector<double> position(GetPropertyVectorAsPixels(gc,"position"));
  if (StringCheck("plotboxaspectratiomode","manual") ||
      StringCheck("dataaspectratiomode","manual")) {
    // Now we have to deal with the scale-to-fit issue.  If we
    // have scale-to-fit disabled, we get a single scale factor
    // to zoom
    double xratio = (xmax-xmin)/position[2];
    double yratio = (ymax-ymin)/position[3];
    double maxratio = qMax(xratio,yratio);
    rerange(xmin,xmax,maxratio*position[2]);
    rerange(ymin,ymax,maxratio*position[3]);
  }
  //    qDebug("Limits %f %f %f %f %f %f",
  //	   xmin,xmax,ymin,ymax,zmin,zmax);
  gc.project(xmin,xmax,ymin,ymax,-zmax,-zmin);
  gc.viewport(position[0],position[1],position[2],position[3]);

  gc.getModelviewMatrix(model);
  gc.getProjectionMatrix(proj);
  gc.getViewport(viewp);
  //     for (int i=0;i<4;i++)
  //       qDebug("%f %f %f %f",
  // 	     model[i*4],model[i*4+4],model[i*4+8],model[i*4+12]);
}

void HandleAxis::DrawBox(RenderEngine &gc) {
  // Q: Is this outerposition that's supposed to be colored?
  // Get the limits
  HPColor *hp = (HPColor*) LookupProperty("color");
  if (hp->IsNone()) return;
  QVector<double> limits(GetAxisLimits());
  gc.color(hp->Data());
  gc.depth(false);
  gc.quad( limits[0], limits[2], limits[4],
	   limits[1], limits[2], limits[4],
	   limits[1], limits[3], limits[4],
	   limits[0], limits[3], limits[4]);
  gc.quad( limits[0], limits[2], limits[5],
	   limits[0], limits[3], limits[5],
	   limits[1], limits[3], limits[5],
	   limits[1], limits[2], limits[5]);
  gc.quad( limits[0], limits[2], limits[4],
	   limits[0], limits[3], limits[4],
	   limits[0], limits[3], limits[5],
	   limits[0], limits[2], limits[5]);
  gc.quad( limits[1], limits[2], limits[4],
	   limits[1], limits[2], limits[5],
	   limits[1], limits[3], limits[5],
	   limits[1], limits[3], limits[4]);
  gc.quad( limits[0], limits[2], limits[4],
	   limits[0], limits[2], limits[5],
	   limits[1], limits[2], limits[5],
	   limits[1], limits[2], limits[4]);
  gc.quad( limits[0], limits[3], limits[4],
	   limits[1], limits[3], limits[4],
	   limits[1], limits[3], limits[5],
	   limits[0], limits[3], limits[5]);
  
  gc.depth(true);
}


void HandleAxis::DrawGridLines(RenderEngine &gc) {
  QVector<double> limits(GetAxisLimits());
  gc.depth(false);
  // The normals of interest are 
  // [0,0,1],[0,0,-1],
  // [0,1,0],[0,-1,0],
  // [1,0,0],[-1,0,0]
  // We will multiply the transformation matrix 
  // by a directional vector.  Then we test the
  // sign of the z component.  This sequence of
  // operations is equivalent to simply taking the
  // 2, 6, 10 elements of m, and drawing the corresponding
  // Select the set of grids to draw based on these elements
  // Draw the grid
  gc.lineWidth(ScalarPropertyLookup("linewidth"));
  gc.setLineStyle(((HPLineStyle*) LookupProperty("gridlinestyle"))->Data());
  HPVector *hp;
  hp = (HPVector*) LookupProperty("xtick");
  QVector<double> xticks(hp->Data());
  hp = (HPVector*) LookupProperty("ytick");
  QVector<double> yticks(hp->Data());
  hp = (HPVector*) LookupProperty("ztick");
  QVector<double> zticks(hp->Data());
  HPColor *xc = (HPColor*) LookupProperty("xcolor");
  HPColor *yc = (HPColor*) LookupProperty("ycolor");
  HPColor *zc = (HPColor*) LookupProperty("zcolor");
  if (xvisible && ((HPOnOff*) LookupProperty("xgrid"))->AsBool()) {
    gc.color(xc->Data());
    for (int i=0;i<xticks.size();i++) {
      double t = MapX(xticks[i]);
      DrawXGridLine(gc,t,limits);
    }
  }
  if (yvisible && ((HPOnOff*) LookupProperty("ygrid"))->AsBool()) {
    gc.color(yc->Data());
    for (int i=0;i<yticks.size();i++) {
      double t = MapY(yticks[i]);
      DrawYGridLine(gc,t,limits);
    }
  }
  if (zvisible && ((HPOnOff*) LookupProperty("zgrid"))->AsBool()) {
    gc.color(zc->Data());
    for (int i=0;i<zticks.size();i++) {
      double t = MapZ(zticks[i]);
      DrawZGridLine(gc,t,limits);
    }
  }
  gc.depth(true);
}

void HandleAxis::DrawXGridLine(RenderEngine &gc, double t, 
			       QVector<double> limits) {
  double m[16];
  gc.getModelviewMatrix(m);
  if (m[10] > 0) {
    gc.line(t,limits[2],limits[4],
	    t,limits[3],limits[4]);
  } else if (m[10] < 0) {
    gc.line(t,limits[2],limits[5],
	    t,limits[3],limits[5]);
  }
  if (m[6] > 0) {
    gc.line(t,limits[2],limits[4],
	    t,limits[2],limits[5]);
  } else if (m[6] < 0) {
    gc.line(t,limits[3],limits[4],
	    t,limits[3],limits[5]);
  }
}
  
void HandleAxis::DrawYGridLine(RenderEngine &gc, double t,
			       QVector<double> limits) {
  double m[16];
  gc.getModelviewMatrix(m);
  if (m[10] > 0) {
    gc.line(limits[0],t,limits[4],
	    limits[1],t,limits[4]);
  } else if (m[10] < 0) {
    gc.line(limits[0],t,limits[5],
	    limits[1],t,limits[5]);
  }
  if (m[2] > 0) {
    gc.line(limits[0],t,limits[4],
	    limits[0],t,limits[5]);
  } else if (m[2] < 0) {
    gc.line(limits[1],t,limits[4],
	    limits[1],t,limits[5]);
  }
}

void HandleAxis::DrawZGridLine(RenderEngine &gc, double t,
			       QVector<double> limits) {
  double m[16];
  gc.getModelviewMatrix(m);
  if (m[6] > 0) {
    gc.line(limits[0],limits[2],t,
	    limits[1],limits[2],t);
  } else if (m[6] < 0) {
    gc.line(limits[0],limits[3],t,
	    limits[1],limits[3],t);
  }
  if (m[2] > 0) {
    gc.line(limits[0],limits[2],t,
	    limits[0],limits[3],t);
  } else if (m[2] < 0) {
    gc.line(limits[1],limits[2],t,
	    limits[1],limits[3],t);
  }
}				 

void HandleAxis::DrawMinorGridLines(RenderEngine &gc) {
  QVector<double> limits(GetAxisLimits());
  gc.setLineStyle(((HPLineStyle*) LookupProperty("minorgridlinestyle"))->Data());
  gc.depth(false);
  HPVector *hp;
  hp = (HPVector*) LookupProperty("xtick");
  QVector<double> xticks(hp->Data());
  hp = (HPVector*) LookupProperty("ytick");
  QVector<double> yticks(hp->Data());
  hp = (HPVector*) LookupProperty("ztick");
  QVector<double> zticks(hp->Data());
  HPColor *xc = (HPColor*) LookupProperty("xcolor");
  HPColor *yc = (HPColor*) LookupProperty("ycolor");
  HPColor *zc = (HPColor*) LookupProperty("zcolor");
  HPLinearLog *sp;
  if (((HPOnOff*) LookupProperty("xminorgrid"))->AsBool()) {
    gc.color(xc->Data());
    sp = (HPLinearLog*) LookupProperty("xscale");
    if (sp->Is("linear")) {
      for (int i=0;i<xticks.size()-1;i++) {
	double t = MapX((xticks[i]+xticks[i+1])/2);
	DrawXGridLine(gc,t,limits);
      }
    } else {
      for (int i=0;i<xticks.size()-1;i++) {
	// Ticks should be in integer divisions
	double t1 = xticks[i];
	double t2 = xticks[i+1];
	if (t2 > t1) {
	  int n = 2;
	  while ((t1*n)<t2) {
	    double t = MapX(n*t1);
	    n++;
	    DrawXGridLine(gc,t,limits);
	  }
	}
      }
    }
  }
  if (((HPOnOff*) LookupProperty("yminorgrid"))->AsBool()) {
    gc.color(yc->Data());
    sp = (HPLinearLog*) LookupProperty("yscale");
    if (sp->Is("linear")) {
      for (int i=0;i<yticks.size()-1;i++) {
	double t = MapY((yticks[i]+yticks[i+1])/2);
	DrawYGridLine(gc,t,limits);
      }
    } else {
      for (int i=0;i<yticks.size()-1;i++) {
	// Ticks should be in integer divisions
	double t1 = yticks[i];
	double t2 = yticks[i+1];
	if (t2 > t1) {
	  int n = 2;
	  while ((t1*n)<t2) {
	    double t = MapY(n*t1);
	    n++;
	    DrawYGridLine(gc,t,limits);
	  }
	}
      }
    }
  }
  if (((HPOnOff*) LookupProperty("zminorgrid"))->AsBool()) {
    gc.color(zc->Data());
    sp = (HPLinearLog*) LookupProperty("zscale");
    if (sp->Is("linear")) {
      for (int i=0;i<zticks.size()-1;i++) {
	double t = MapZ((zticks[i]+zticks[i+1])/2);
	DrawZGridLine(gc,t,limits);
      }
    } else {
      for (int i=0;i<zticks.size()-1;i++) {
	// Ticks should be in integer divisions
	double t1 = zticks[i];
	double t2 = zticks[i+1];
	if (t2 > t1) {
	  int n = 2;
	  while ((t1*n)<t2) {
	    double t = MapZ(n*t1);
	    n++;
	    DrawZGridLine(gc,t,limits);
	  }
	}
      }
    }
  }
  gc.depth(true);
}
  
double HandleAxis::flipX(double t) {
  QVector<double> limits(GetAxisLimits());
  if (t == limits[0])
    return limits[1];
  return limits[0];
}

double HandleAxis::flipY(double t) {
  QVector<double> limits(GetAxisLimits());
  if (t == limits[2])
    return limits[3];
  return limits[2];
}

double HandleAxis::flipZ(double t) {
  QVector<double> limits(GetAxisLimits());
  if (t == limits[4])
    return limits[5];
  return limits[4];
}

void HandleAxis::SetupAxis(RenderEngine &gc) {
  double model[16];
  gc.getModelviewMatrix(model);
  UpdateXYZPos(gc);
}

bool HandleAxis::Is2DView() {
  return (!(xvisible && yvisible && zvisible));
}

void HandleAxis::SetupCamera(RenderEngine &gc) {
  HPThreeVector *tv1, *tv2, *tv3;
  tv1 = (HPThreeVector*) LookupProperty("cameraposition");
  tv2 = (HPThreeVector*) LookupProperty("cameratarget");
  tv3 = (HPThreeVector*) LookupProperty("cameraupvector");
  gc.lookAt(tv1->Data()[0],tv1->Data()[1],tv1->Data()[2],
	    tv2->Data()[0],tv2->Data()[1],tv2->Data()[2],
	    tv3->Data()[0],tv3->Data()[1],tv3->Data()[2]);
}

void HandleAxis::MapCorners(RenderEngine &gc, 
			    double &xmin, double &xmax,
			    double &ymin, double &ymax,
			    double &zmin, double &zmax) {
  // Get the axis limits
  QVector<double> limits(GetAxisLimits());
  // Map the 8 corners of the clipping cube to rotated space
  double xvals[8];
  double yvals[8];
  double zvals[8];
  gc.mapPoint(limits[0],limits[2],limits[4],xvals[0],yvals[0],zvals[0]);
  gc.mapPoint(limits[0],limits[2],limits[5],xvals[1],yvals[1],zvals[1]);
  gc.mapPoint(limits[0],limits[3],limits[4],xvals[2],yvals[2],zvals[2]);
  gc.mapPoint(limits[0],limits[3],limits[5],xvals[3],yvals[3],zvals[3]);
  gc.mapPoint(limits[1],limits[2],limits[4],xvals[4],yvals[4],zvals[4]);
  gc.mapPoint(limits[1],limits[2],limits[5],xvals[5],yvals[5],zvals[5]);
  gc.mapPoint(limits[1],limits[3],limits[4],xvals[6],yvals[6],zvals[6]);
  gc.mapPoint(limits[1],limits[3],limits[5],xvals[7],yvals[7],zvals[7]);
  // Get the min and max x, y and z coordinates
  MinMaxVector(xvals,8,xmin,xmax);
  MinMaxVector(yvals,8,ymin,ymax);
  MinMaxVector(zvals,8,zmin,zmax);
  if (zmin == zmax) {
    zmin = zmax-1;
    zmax = zmax+1;
  }  
}

void HandleAxis::UpdateXYZPos(RenderEngine &gc) {
  QVector<double> limits(GetAxisLimits());
  // Query the axisproperties to set the z-position of the
  // x and y axis
  if (((HPTopBottom*)LookupProperty("xaxislocation"))->Is("bottom")) {
    x1pos[2] = limits[4];
  } else
    x1pos[2] = limits[5];
  if (((HPLeftRight*)LookupProperty("yaxislocation"))->Is("left")) {
    y1pos[2] = limits[4];
  } else
    y1pos[2] = limits[5];

  if ((model[10] > 0) && (model[6] > 0)) {
    if (x1pos[2] == limits[4])
      x1pos[1] = limits[3];
    else
      x1pos[1] = limits[2];
  } else if ((model[10] > 0) && (model[6] <= 0)) {
    if (x1pos[2] == limits[4])
      x1pos[1] = limits[2];
    else
      x1pos[1] = limits[3];
  } else if ((model[10] <= 0) && (model[6] > 0)) {
    if (x1pos[2] == limits[4])
      x1pos[1] = limits[2];
    else
      x1pos[1] = limits[3];
  } else if ((model[10] <= 0) && (model[6] <= 0)) {
    if (x1pos[2] == limits[4])
      x1pos[1] = limits[3];
    else
      x1pos[1] = limits[2];
  } 

  // There are two possibilities for where the opposite x axis is
  //   - one option is to use the opposite axis in the y direction
  //   - the other option is to use the opposite position in the z direction
  //   - we have to decide which one to use.  What we can do is take
  //   - the longer axis
  double px0, py0, px1, py1, px2, py2;
  gc.toPixels(limits[0],x1pos[1],x1pos[2],px0,py0);
  gc.toPixels(limits[0],flipY(x1pos[1]),x1pos[2],px1,py1);
  gc.toPixels(limits[0],x1pos[1],flipZ(x1pos[2]),px2,py2);
  double len1, len2;
  len1 = ((px1-px0)*(px1-px0) + (py1-py0)*(py1-py0));
  len2 = ((px2-px0)*(px2-px0) + (py2-py0)*(py2-py0));
  if ((len1 > len2) && (len1 > 0)) {
    x2pos[1] = flipY(x1pos[1]);
    x2pos[2] = x1pos[2];
  } else {
    x2pos[1] = x1pos[1];
    x2pos[2] = flipZ(x1pos[2]);
  }

  //     if (x1pos[1] == limits[3])
  //       x2pos[1] = limits[2];
  //     else if (x1pos[1] == limits[2])
  //       x2pos[1] = limits[3];

  if ((model[10] > 0) && (model[2] > 0)) {
    if (y1pos[2] == limits[4])
      y1pos[0] = limits[1];
    else
      y1pos[0] = limits[0];
  } else if ((model[10] <= 0) && (model[2] > 0)) {
    if (y1pos[2] == limits[4])
      y1pos[0] = limits[0];
    else
      y1pos[0] = limits[1];
  } else if ((model[10] > 0) && (model[2] <= 0)) {
    if (y1pos[2] == limits[4])
      y1pos[0] = limits[0];
    else
      y1pos[0] = limits[1];
  } else if ((model[10] <= 0) && (model[2] <= 0)) {
    if (y1pos[2] == limits[4])
      y1pos[0] = limits[1];
    else
      y1pos[0] = limits[0];
  } 
  gc.toPixels(y1pos[0],limits[2],y1pos[2],px0,py0);
  gc.toPixels(flipX(y1pos[0]),limits[2],y1pos[2],px1,py1);
  gc.toPixels(y1pos[0],limits[2],flipZ(y1pos[2]),px2,py2);
  len1 = ((px1-px0)*(px1-px0) + (py1-py0)*(py1-py0));
  len2 = ((px2-px0)*(px2-px0) + (py2-py0)*(py2-py0));
  if ((len1 > len2) && (len1 > 0)) {
    y2pos[0] = y1pos[0];
    y2pos[2] = flipZ(y1pos[2]);
  } else {
    y2pos[0] = flipX(y1pos[0]);
    y2pos[2] = y1pos[2];
  }

  //     if (y1pos[0] == limits[1])
  //       y2pos[0] = limits[0];
  //     else if (y1pos[0] == limits[0])
  //       y2pos[0] = limits[1];
    
  if (model[6] > 0)
    z1pos[0] = limits[1];
  else if (model[6] <= 0)
    z1pos[0] = limits[0];
  if (model[2] > 0)
    z1pos[1] = limits[2];
  else if (model[2] <= 0)
    z1pos[1] = limits[3];

  //sgn - x - y
  //111 - H - H
  //110 - L - H
  //101 - H - L
  //100 - L - L
  //011 - L - L
  //010 - H - L
  //001 - L - H
  //000 - H - H
  //
  // so, x=H if (!10 ^ 2), and y = H if (!10 ^ 6)
  if ((model[10] > 0) && (model[6] > 0) && (model[2] > 0)) {
    z2pos[0] = limits[1];
    z2pos[1] = limits[3];
  } else if ((model[10] > 0) && (model[6] > 0) && (model[2] < 0)) {
    z2pos[0] = limits[0];
    z2pos[1] = limits[3];
  } else if ((model[10] > 0) && (model[6] < 0) && (model[2] > 0)) {
    z2pos[0] = limits[1];
    z2pos[1] = limits[2];
  } else if ((model[10] > 0) && (model[6] < 0) && (model[2] < 0)) {
    z2pos[0] = limits[0];
    z2pos[1] = limits[2];
  } else if ((model[10] < 0) && (model[6] > 0) && (model[2] > 0)) {
    z2pos[0] = limits[0];
    z2pos[1] = limits[2];
  } else if ((model[10] < 0) && (model[6] > 0) && (model[2] < 0)) {
    z2pos[0] = limits[1];
    z2pos[1] = limits[2];
  } else if ((model[10] < 0) && (model[6] < 0) && (model[2] > 0)) {
    z2pos[0] = limits[0];
    z2pos[1] = limits[3];
  } else if ((model[10] < 0) && (model[6] < 0) && (model[2] < 0)) {
    z2pos[0] = limits[1];
    z2pos[1] = limits[3];
  }

  // Check for ordinal views
  // Z axis isn't visible
  if ((model[2] == 0) && (model[6] == 0)) {
    x2pos[1] = flipY(x1pos[1]);
    x2pos[2] = x1pos[2];
    y2pos[0] = flipX(y1pos[0]);
    y2pos[2] = y2pos[2];
  }
  // X axis isn't visible
  if ((model[6] == 0) && (model[10] == 0)) {
    y2pos[0] = y1pos[0];
    y2pos[2] = flipZ(y1pos[2]);
    z2pos[0] = z1pos[0];
    z2pos[1] = flipY(z1pos[1]);
  }
  // Y axis isn't visible
  if ((model[2] == 0) && (model[10] == 0)) {
    x2pos[1] = x1pos[1];
    x2pos[2] = flipZ(x1pos[2]);
    z2pos[0] = flipX(z1pos[0]);
    z2pos[1] = z1pos[1];
  }

  double x1, y1, x2, y2;
  gc.toPixels(limits[0],x1pos[1],x1pos[2],x1,y1);
  gc.toPixels(limits[1],x1pos[1],x1pos[2],x2,y2);
  xvisible = (fabs(x1-x2) > 2) || (fabs(y1-y2) > 2);
  gc.toPixels(y1pos[0],limits[2],y1pos[2],x1,y1);
  gc.toPixels(y1pos[0],limits[3],y1pos[2],x2,y2);
  yvisible = (fabs(x1-x2) > 2) || (fabs(y1-y2) > 2);
  gc.toPixels(z1pos[0],z1pos[1],limits[4],x1,y1);
  gc.toPixels(z1pos[0],z1pos[1],limits[5],x2,y2);
  zvisible = (fabs(x1-x2) > 2) || (fabs(y1-y2) > 2);    
}

void HandleAxis::InitialSetupAxis(RenderEngine &gc) {
  SetupCamera(gc);
  // Scale using the data aspect ratio
  QVector<double> dar(VectorPropertyLookup("dataaspectratio"));
  gc.scale(1.0/dar[0],1.0/dar[1],1.0/dar[2]);
  // Get the axis limits
  double xmin, xmax, ymin, ymax, zmin, zmax;
  MapCorners(gc,xmin,xmax,ymin,ymax,zmin,zmax);
  QVector<double> position(GetPropertyVectorAsPixels(gc,"outerposition"));
  if (StringCheck("plotboxaspectratiomode","manual") ||
      StringCheck("dataaspectratiomode","manual")) {
    // Now we have to deal with the scale-to-fit issue.  If we
    // have scale-to-fit disabled, we get a single scale factor
    // to zoom
    double xratio = (xmax-xmin)/position[2];
    double yratio = (ymax-ymin)/position[3];
    double maxratio = qMax(xratio,yratio);
    rerange(xmin,xmax,maxratio*position[2]);
    rerange(ymin,ymax,maxratio*position[3]);
  }
  //    qDebug("Limits %f %f %f %f %f %f",
  //	   xmin,xmax,ymin,ymax,zmin,zmax);
  gc.project(xmin,xmax,ymin,ymax,-zmax,-zmin);
  gc.viewport(position[0],position[1],position[2],position[3]);

  gc.getModelviewMatrix(model);
  gc.getProjectionMatrix(proj);
  gc.getViewport(viewp);
  
  UpdateXYZPos(gc);
}

void HandleAxis::DrawAxisLines(RenderEngine &gc) { 
  QVector<double> limits(GetAxisLimits());
  HPColor *xc = (HPColor*) LookupProperty("xcolor");
  HPColor *yc = (HPColor*) LookupProperty("ycolor");
  HPColor *zc = (HPColor*) LookupProperty("zcolor");
  gc.setLineStyle("-");
  gc.lineWidth(ScalarPropertyLookup("linewidth"));
  if (xvisible) {
    gc.color(xc->Data());
    double px0, py0, px1, py1;
    gc.toPixels(limits[0],x1pos[1],x1pos[2],px0,py0);
    gc.toPixels(limits[1],x1pos[1],x1pos[2],px1,py1);
    gc.setupDirectDraw();
    gc.line(px0,py0,px1,py1);
    gc.releaseDirectDraw();
    if (Is2DView()) {
      gc.toPixels(limits[0],x2pos[1],x2pos[2],px0,py0);
      gc.toPixels(limits[1],x2pos[1],x2pos[2],px1,py1);
      gc.setupDirectDraw();
      gc.line(px0,py0,px1,py1);
      gc.releaseDirectDraw();
    }
  }
  if (yvisible) {
    gc.color(yc->Data());
    double px0, py0, px1, py1;
    gc.toPixels(y1pos[0],limits[2],y1pos[2],px0,py0);
    gc.toPixels(y1pos[0],limits[3],y1pos[2],px1,py1);
    gc.setupDirectDraw();
    gc.line(px0,py0,px1,py1);
    gc.releaseDirectDraw();
    if (Is2DView()) {
      gc.toPixels(y2pos[0],limits[2],y2pos[2],px0,py0);
      gc.toPixels(y2pos[0],limits[3],y2pos[2],px1,py1);
      gc.setupDirectDraw();
      gc.line(px0,py0,px1,py1);
      gc.releaseDirectDraw();
    }
  } 
  if (zvisible) {
    gc.color(zc->Data());
    double px0, py0, px1, py1;
    gc.toPixels(z1pos[0],z1pos[1],limits[4],px0,py0);
    gc.toPixels(z1pos[0],z1pos[1],limits[5],px1,py1);
    gc.setupDirectDraw();
    gc.line(px0,py0,px1,py1);
    gc.releaseDirectDraw();
    if (Is2DView()) {
      gc.toPixels(z2pos[0],z2pos[1],limits[4],px0,py0);
      gc.toPixels(z2pos[0],z2pos[1],limits[5],px1,py1);
      gc.setupDirectDraw();
      gc.line(px0,py0,px1,py1);
      gc.releaseDirectDraw();
    }
  }
}

// Assemble a font for the axis
void HandleAxis::UpdateAxisFont() {
  QFont::Style fstyle = QFont::StyleNormal;
  QFont::Weight fweight = QFont::Normal;
  HPString *fontname = (HPString*) LookupProperty("fontname");
  HPFontAngle *fontangle = (HPFontAngle*) LookupProperty("fontangle");
  HPFontWeight *fontweight = (HPFontWeight*) LookupProperty("fontweight");
  HPScalar *fontsize = (HPScalar*) LookupProperty("fontsize");
  if (fontangle->Is("normal"))
    fstyle = QFont::StyleNormal;
  if (fontangle->Is("italic"))
    fstyle = QFont::StyleItalic;
  if (fontangle->Is("oblique"))
    fstyle = QFont::StyleOblique;
  if (fontweight->Is("normal"))
    fweight = QFont::Normal;
  if (fontweight->Is("bold"))
    fweight = QFont::Bold;
  if (fontweight->Is("light"))
    fweight = QFont::Light;
  if (fontweight->Is("demi"))
    fweight = QFont::DemiBold;
  // Lookup the font
  QFont fnt(fontname->Data(),(int)(fontsize->Data()[0]));
  fnt.setStyle(fstyle);
  fnt.setWeight(fweight);
  m_font = fnt;
  QFontMetrics fm(m_font);
  QRect sze(fm.boundingRect("|"));
  SetScalarDefault("textheight",sze.height());
}

int HandleAxis::GetTickCount(RenderEngine &gc,
			     double x1, double y1, double z1,
			     double x2, double y2, double z2) {
  double u1, v1, u2, v2;
  gc.toPixels(x1,y1,z1,u1,v1);
  gc.toPixels(x2,y2,z2,u2,v2);
  double axlen;
  axlen = sqrt((u2-u1)*(u2-u1) + (v2-v1)*(v2-v1));
  int numtics = (int)(qMax(2.0,axlen/25.0));
  return numtics;
}

void HandleAxis::RecalculateTicks(RenderEngine& gc) {
  // We have to calculate the tick sets for each axis...
  QVector<double> limits(GetAxisLimits(true));
  QVector<double> xticks;
  StringVector xlabels;
  QVector<double> yticks;
  StringVector ylabels;
  QVector<double> zticks;
  StringVector zlabels;
  int xcnt, ycnt, zcnt;
  // Cyclic behaviour on xlim and ylim and zlim!
  xcnt = GetTickCount(gc,limits[0],x1pos[1],x1pos[2],
 		      limits[1],x1pos[1],x1pos[2]);
  ycnt = GetTickCount(gc,y1pos[0],limits[2],y1pos[2],
 		      y1pos[0],limits[3],y1pos[2]);
  zcnt = GetTickCount(gc,z1pos[0],z1pos[1],limits[4],
 		      z1pos[0],z1pos[1],limits[5]);
  //  xcnt = ycnt = zcnt = numtics;
  double xStart, xStop;
  double yStart, yStop;
  double zStart, zStop;
  HPTwoVector *tp;
  HPLinearLog *lp;
  lp = (HPLinearLog*)LookupProperty("xscale");
  if (IsAuto("xlimmode")) {
    FormatAxisAuto(limits[0],limits[1],xcnt,
		   lp->Is("log"),xStart,xStop,xticks,xlabels);
    tp = (HPTwoVector*) LookupProperty("xlim");
    QVector<double> lims; 
    if (lp->Is("linear")) {
      lims.push_back(xStart);
      lims.push_back(xStop);
    } else {
      lims.push_back(pow(10.0,xStart));
      lims.push_back(pow(10.0,xStop));
    }
    tp->Data(lims);
  } else
    FormatAxisManual(limits[0],limits[1],xcnt,
		     lp->Is("log"),xStart,xStop,xticks,xlabels);

  lp = (HPLinearLog*)LookupProperty("yscale");
  if (IsAuto("ylimmode")) {
    FormatAxisAuto(limits[2],limits[3],ycnt,
		   lp->Is("log"),yStart,yStop,yticks,ylabels);
    tp = (HPTwoVector*) LookupProperty("ylim");
    QVector<double> lims; 
    if (lp->Is("linear")) {
      lims.push_back(yStart);
      lims.push_back(yStop);
    } else {
      lims.push_back(pow(10.0,yStart));
      lims.push_back(pow(10.0,yStop));
    }
    tp->Data(lims);
  } else
    FormatAxisManual(limits[2],limits[3],ycnt,
		     lp->Is("log"),yStart,yStop,yticks,ylabels);

  lp = (HPLinearLog*)LookupProperty("zscale");
  if (IsAuto("zlimmode")) {
    FormatAxisAuto(limits[4],limits[5],zcnt,
		   lp->Is("log"),zStart,zStop,zticks,zlabels);
    tp = (HPTwoVector*) LookupProperty("zlim");
    QVector<double> lims; 
    if (lp->Is("linear")) {
      lims.push_back(zStart);
      lims.push_back(zStop);
    } else {
      lims.push_back(pow(10.0,zStart));
      lims.push_back(pow(10.0,zStop));
    }
    tp->Data(lims);
  } else
    FormatAxisManual(limits[4],limits[5],zcnt,
		     lp->Is("log"),zStart,zStop,zticks,zlabels);
  // Update the limits...
    
  HPVector *hp;
  HPStringSet *qp;
  if (IsAuto("xtickmode")) {
    hp = (HPVector*) LookupProperty("xtick");
    hp->Data(xticks);
  }
  if (IsAuto("xticklabelmode")) {
    qp = (HPStringSet*) LookupProperty("xticklabel");
    qp->Data(xlabels);
  }
  if (IsAuto("ytickmode")) {
    hp = (HPVector*) LookupProperty("ytick");
    hp->Data(yticks);
  }
  if (IsAuto("yticklabelmode")) {
    qp = (HPStringSet*) LookupProperty("yticklabel");
    qp->Data(ylabels);
  }
  if (IsAuto("ztickmode")) {
    hp = (HPVector*) LookupProperty("ztick");
    hp->Data(zticks);
  }
  if (IsAuto("zticklabelmode")) {
    qp = (HPStringSet*) LookupProperty("zticklabel");
    qp->Data(zlabels);
  }
}

void HandleAxis::RePackFigure(RenderEngine &gc) {
  if (!IsAuto("positionmode")) return;
  int titleHeight = 0;
  int xlabelHeight = 0;
  int ylabelHeight = 0;
  int zlabelHeight = 0;
  int maxLabelHeight = 0;
  int tickHeight = 0;
  HPHandles *lbl;
  int maxTickWidth = 0;
  int maxTickHeight = 0;
  QFontMetrics fm(m_font);
  {
    lbl = (HPHandles*) LookupProperty("xlabel");
    if (!lbl->Data().empty()) {
      HandleText *fp = (HandleText*) LookupHandleObject(lbl->Data()[0]);
      xlabelHeight = fp->GetTextHeightInPixels();
    }
    HPStringSet *hp = (HPStringSet*) LookupProperty("xticklabel");
    StringVector xlabels(hp->Data());
    for (int i=0;i<xlabels.size();i++) {
      QRect sze(fm.boundingRect(xlabels[i]));
      maxTickWidth = qMax(maxTickWidth,sze.width());
      maxTickHeight = qMax(maxTickHeight,sze.height());
    }
  }
  {
    lbl = (HPHandles*) LookupProperty("ylabel");
    if (!lbl->Data().empty()) {
      HandleText *fp = (HandleText*) LookupHandleObject(lbl->Data()[0]);
      ylabelHeight = fp->GetTextHeightInPixels();
    }
    HPStringSet *hp = (HPStringSet*) LookupProperty("yticklabel");
    StringVector ylabels(hp->Data());
    for (int i=0;i<ylabels.size();i++) {
      QRect sze(fm.boundingRect(ylabels[i]));
      maxTickWidth = qMax(maxTickWidth,sze.width());
      maxTickHeight = qMax(maxTickHeight,sze.height());
    }
  }
  {
    lbl = (HPHandles*) LookupProperty("zlabel");
    if (!lbl->Data().empty()) {
      HandleText *fp = (HandleText*) LookupHandleObject(lbl->Data()[0]);
      zlabelHeight = fp->GetTextHeightInPixels();
    }
    HPStringSet *hp = (HPStringSet*) LookupProperty("zticklabel");
    StringVector zlabels(hp->Data());
    for (int i=0;i<zlabels.size();i++) {
      QRect sze(fm.boundingRect(zlabels[i]));
      maxTickWidth = qMax(maxTickWidth,sze.width());
      maxTickHeight = qMax(maxTickHeight,sze.height());
    }
  }
  lbl = (HPHandles*) LookupProperty("title");
  if (!lbl->Data().empty()) {
    HandleText *fp = (HandleText*) LookupHandleObject(lbl->Data()[0]);
    titleHeight = fp->GetTextHeightInPixels();
  }
  QRect sze(fm.boundingRect("|"));
  tickHeight =  sze.height();

  // Take the maximum of the title, and label sizes to compute
  // the padding...
  maxLabelHeight = qMax(titleHeight,xlabelHeight);
  maxLabelHeight = qMax(maxLabelHeight,ylabelHeight);
  maxLabelHeight = qMax(maxLabelHeight,zlabelHeight);
  // Get the outer position vector...
  QVector<double> outerpos(GetPropertyVectorAsPixels(gc,"outerposition"));

  // Special case - no labels at all --> super tight packing
  unsigned width = gc.width();
  unsigned height = gc.height();
  if ((maxTickWidth == 0) && (maxTickHeight == 0) && (maxLabelHeight == 0)) {
    HPFourVector *hp = (HPFourVector*) LookupProperty("position");
    hp->Value(outerpos[0]/width,outerpos[1]/height,
	      outerpos[2]/width,outerpos[3]/height);
    return;
  }
  // To avoid recursion - we take a linear fit approach
  // First, we compute a candidate position vector from the outerposition
  // rectangle
  double posx0,posy0,poswidth,posheight;
  posx0 = outerpos[2]*0.1+outerpos[0];
  posy0 = outerpos[3]*0.1+outerpos[1];
  poswidth = outerpos[2]*0.8;
  posheight = outerpos[3]*.8;

  //     poswidth = qMin(0.9*outerpos[2]-2*maxTickWidth,0.775*outerpos[2]);
  //     posheight = qMin(0.815*outerpos[2],0.9*outerpos[3]-2*maxTickHeight);
  // Pad the label height
  maxLabelHeight = (int)(maxLabelHeight*1.2 + tickHeight);
  // Check posx0 against maxLabelHeight..
  if (posx0 < maxLabelHeight)
    posx0 = maxLabelHeight;
  // Check posy0 against maxLabelHeight...
  if (posy0 < maxLabelHeight)
    posy0 = maxLabelHeight;
  // Check the width against maxLabelHeight...
  if ((outerpos[2] - poswidth) < 2*maxLabelHeight) {
    poswidth = outerpos[2] - 2*maxLabelHeight;
  }
  if ((outerpos[3] - posheight) < 2*maxLabelHeight) {
    posheight = outerpos[3] - 2*maxLabelHeight;
  }

  HPFourVector *hp = (HPFourVector*) LookupProperty("position");
  //  hp->Value(posx0,posy0,poswidth,posheight);
  hp->Value(posx0/width,posy0/height,poswidth/width,posheight/height);
  hp->ClearModified();
}

void HandleAxis::UpdateLimits(bool x, bool y, bool z, bool a, bool c) {
  if (!x && !y && !z && !a && !c) return;
  // Get our set of children
  QVector<double> limits;
  bool first = true;
  HPHandles *children = (HPHandles*) LookupProperty("children");
  QVector<unsigned> handles(children->Data());
  for (int i=0;i<handles.size();i++) {
    HandleObject *fp = LookupHandleObject(handles[i]);
    QVector<double> child_limits(fp->GetLimits());
    if (!child_limits.empty()) {
      if (first) {
	limits = child_limits;
	first = false;
      } else {
	for (int i=0;i<qMin(limits.size(),child_limits.size());i+=2) {
	  limits[i] = qMin(limits[i],child_limits[i]);
	  limits[i+1] = qMax(limits[i+1],child_limits[i+1]);
	}
      }
    }
  }
  if (first) return;
  if (limits[1] == limits[0]) {
    limits[0] = limits[0] - 0.5;
    limits[1] = limits[0] + 1;
  }
  if (limits[3] == limits[2]) {
    limits[2] = limits[2] - 0.5;
    limits[3] = limits[2] + 1;
  }
  if (limits[5] == limits[4]) {
    limits[4] = limits[4] - 0.5;
    limits[5] = limits[4] + 1;
  }
  HPSixVector *hp = (HPSixVector*) LookupProperty("datalimits");
  hp->Value(limits[0],limits[1],limits[2],limits[3],limits[4],limits[5]);
  if (x) SetTwoVectorDefault("xlim",limits[0],limits[1]);
  if (y) SetTwoVectorDefault("ylim",limits[2],limits[3]);
  if (z) SetTwoVectorDefault("zlim",limits[4],limits[5]);
  if (x) SetTwoVectorDefault("xlim_raw",limits[0],limits[1]);
  if (y) SetTwoVectorDefault("ylim_raw",limits[2],limits[3]);
  if (z) SetTwoVectorDefault("zlim_raw",limits[4],limits[5]);
  if (c) SetTwoVectorDefault("clim",limits[6],limits[7]);
  if (a) SetTwoVectorDefault("alim",limits[8],limits[9]);
}

void HandleAxis::HandlePlotBoxFlags() {
  bool xflag, yflag, zflag, aflag, cflag;
  xflag = IsAuto("xlimmode");
  yflag = IsAuto("ylimmode");
  zflag = IsAuto("zlimmode");
  aflag = IsAuto("alimmode");
  cflag = IsAuto("climmode");

  // Check for the various cases
  bool axesauto = xflag && yflag && zflag;
  bool darauto = IsAuto("dataaspectratiomode");
  bool pbaauto = IsAuto("plotboxaspectratiomode");
  bool onemanual = (!xflag && yflag && zflag) || (xflag && !yflag && zflag) || 
    (xflag && yflag && !zflag);
  // 	   axesauto,darauto,pbaauto,onemanual);
  QVector<double> limits(GetAxisLimits());
  double xrange = limits[1] - limits[0];
  double yrange = limits[3] - limits[2];
  double zrange = limits[5] - limits[4];
  double minrange = qMin(xrange,qMin(yrange,zrange));
  double maxrange = qMax(xrange,qMax(yrange,zrange));
  QVector<double> pba(VectorPropertyLookup("plotboxaspectratio"));
  double xratio = pba[0];
  double yratio = pba[1];
  double zratio = pba[2];
  double minratio = qMin(xratio,qMin(yratio,zratio));
  xratio/=minratio;
  yratio/=minratio;
  zratio/=minratio;
  QVector<double> dar(VectorPropertyLookup("dataaspectratio"));
  double xscale = dar[0];
  double yscale = dar[1];
  double zscale = dar[2];
  double minscale = qMin(xscale,qMin(yscale,zscale));
  xscale /= minscale;
  yscale /= minscale;
  zscale /= minscale;
    
  if (darauto && pbaauto) {
    SetThreeVectorDefault("dataaspectratio",1,1,1);
    SetThreeVectorDefault("plotboxaspectratio",xrange/minrange,yrange/minrange,zrange/minrange);
  } else if (darauto && !pbaauto) {
    // Use calculated limits, set data aspect ratio to achieve the desired plot box ratio
    SetThreeVectorDefault("plotboxaspectratio",xratio,yratio,zratio);
    // xratio = xrange/xscale, so if xscale is changable, and xrange is fixed,
    SetThreeVectorDefault("dataaspectratio",xrange/xratio,yrange/yratio,zrange/zratio);
  } else if (!darauto && pbaauto) {
    SetThreeVectorDefault("dataaspectratio",xscale,yscale,zscale);
  } else {
    if (axesauto) {
      // Plot box aspect ratio has been specified, but the dataaspect ratio is
      // fixed...  Collect the various quantities...
      // PBA = xratio = xrange/xscale --> xrange = xratio/xscale
      rerange(limits[0],limits[1],xratio/xscale*maxrange);
      rerange(limits[2],limits[3],yratio/yscale*maxrange);
      rerange(limits[4],limits[5],zratio/zscale*maxrange);
      SetAxisLimits(limits);
    } else if (onemanual) {
      if (!xflag) {
	rerange(limits[2],limits[3],yratio/yscale*xrange);
	rerange(limits[4],limits[5],zratio/zscale*xrange);
	SetAxisLimits(limits);
      } else if (!yflag) {
	rerange(limits[0],limits[1],xratio/xscale*yrange);
	rerange(limits[4],limits[5],zratio/zscale*yrange);
	SetAxisLimits(limits);
      } else if (!zflag) {
	rerange(limits[0],limits[1],xratio/xscale*zrange);
	rerange(limits[2],limits[3],yratio/yscale*zrange);
	SetAxisLimits(limits);
      }
    } else {
      // Ignore plotboxaspectratio...
    }
  }
}
  
void HandleAxis::UpdateState() {
  StringVector tset;
  if (HasChanged("xlim")) ToManual("xlimmode");
  if (HasChanged("ylim")) ToManual("ylimmode");
  if (HasChanged("zlim")) ToManual("zlimmode");
  if (HasChanged("alim")) ToManual("alimmode");
  if (HasChanged("clim")) ToManual("climmode");
  if (HasChanged("xtick")) ToManual("xtickmode");
  if (HasChanged("ytick")) ToManual("ytickmode");
  if (HasChanged("ztick")) ToManual("ztickmode");
  if (HasChanged("xticklabel")) ToManual("xticklabelmode");
  if (HasChanged("yticklabel")) ToManual("yticklabelmode");
  if (HasChanged("zticklabel")) ToManual("zticklabelmode");
  if (HasChanged("position")) ToManual("positionmode");
  tset.push_back("fontangle");  tset.push_back("fontname");
  tset.push_back("fontsize");   tset.push_back("fontunits");
  tset.push_back("fontweight"); tset.push_back("xticklabel");
  tset.push_back("yticklabel"); tset.push_back("zticklabel");
  tset.push_back("xcolor");     tset.push_back("ycolor"); 
  tset.push_back("zcolor"); 
  if (HasChanged(tset)) {
    UpdateAxisFont();
    ClearChanged(tset);
  }
  // Limits
  bool xflag, yflag, zflag, aflag, cflag;
  xflag = IsAuto("xlimmode");
  yflag = IsAuto("ylimmode");
  zflag = IsAuto("zlimmode");
  aflag = IsAuto("alimmode");
  cflag = IsAuto("climmode");
  UpdateLimits(xflag,yflag,zflag,aflag,cflag);

  if (HasChanged("dataaspectratio")) ToManual("dataaspectratiomode");
  if (HasChanged("plotboxaspectratio")) ToManual("plotboxaspectratiomode");

  HandlePlotBoxFlags();

  // Camera properties...
  if (HasChanged("cameratarget")) 
    ToManual("cameratargetmode");
  if (IsAuto("cameratargetmode")) {
    // Default to 2D
    HPThreeVector *tv = (HPThreeVector*) LookupProperty("cameratarget");
    QVector<double> limits(GetAxisLimits());
    tv->Value((limits[0]+limits[1])/2.0,
	      (limits[2]+limits[3])/2.0,
	      (limits[4]+limits[5])/2.0);
  }
  if (HasChanged("cameraposition"))
    ToManual("camerapositionmode");
  if (IsAuto("camerapositionmode")) {
    // Default to 2D
    HPThreeVector *tv = (HPThreeVector*) LookupProperty("cameraposition");
    QVector<double> limits(GetAxisLimits());
    tv->Value((limits[0]+limits[1])/2.0,
	      (limits[2]+limits[3])/2.0,
	      limits[5]+1);
  }
  if (HasChanged("cameraupvector"))
    ToManual("cameraupvectormode");
  if (IsAuto("cameraupvectormode")) {
    // Default to 2D
    HPThreeVector *tv = (HPThreeVector*) LookupProperty("cameraupvector");
    tv->Value(0,1,0);
  }
  HPHandles *children = (HPHandles*) LookupProperty("children");
  QVector<unsigned> handles(children->Data());
  for (int i=0;i<handles.size();i++) {
    HandleObject *fp = LookupHandleObject(handles[i]);
    fp->UpdateState();
  }  
  ClearAllChanged();
}

// The orientation of the label depends on the angle of the
// tick
void HandleAxis::DrawLabel(RenderEngine& gc,
			   double dx, double dy, 
			   double x2, double y2, 
			   QVector<double> color,
			   QString txt) {
  double angle = atan2(dy,dx)*180.0/M_PI;
  RenderEngine::AlignmentFlag xalign = RenderEngine::Min;
  RenderEngine::AlignmentFlag yalign = RenderEngine::Min;
  if (fabs(angle) < 10) {
    xalign = RenderEngine::Min;
    yalign = RenderEngine::Mean;
  } else if (fabs(angle) > 170) {
    xalign = RenderEngine::Max;
    yalign = RenderEngine::Mean;
  } else if ((angle >= 10) && (angle < 80)) {
    xalign = RenderEngine::Min;
    yalign = RenderEngine::Min;
  } else if ((angle >= 80) && (angle < 100)) {
    xalign = RenderEngine::Mean;
    yalign = RenderEngine::Min;
  } else if ((angle >= 100) && (angle < 170)) {
    xalign = RenderEngine::Max;
    yalign = RenderEngine::Min;
  } else if ((angle <= -10) && (angle > -80)) {
    xalign = RenderEngine::Min;
    yalign = RenderEngine::Max;
  } else if ((angle <= -80) && (angle > -100)) {
    xalign = RenderEngine::Mean;
    yalign = RenderEngine::Max;
  } else if ((angle <= -100) && (angle > -170)) {
    xalign = RenderEngine::Max;
    yalign = RenderEngine::Max;
  }
  gc.setupDirectDraw();    
  gc.putText(x2,y2,txt,color,xalign,yalign,m_font,0);
  gc.releaseDirectDraw();
}

//
// Look at the z axis... if T*[0;0;1;0] y component is positive,
// put x and y axis at the bottom.  otherwise, put them at the top.
//
void HandleAxis::DrawTickMarks(RenderEngine &gc) {
  // The trick here is to determine where to attach the 
  // three axes - there should be two possibilities for
  // each axis.  We start with four possibilities for
  // each axis.  Now each axis sits on the boundary of
  // two facets.  If exactly one of the two facets is
  // visible, then the axis line is visible.
  HPVector *hp;
  hp = (HPVector*) LookupProperty("xtick");
  QVector<double> xticks(hp->Data());
  hp = (HPVector*) LookupProperty("ytick");
  QVector<double> yticks(hp->Data());
  hp = (HPVector*) LookupProperty("ztick");
  QVector<double> zticks(hp->Data());
  gc.lineWidth(ScalarPropertyLookup("linewidth"));
  HPColor *xc = (HPColor*) LookupProperty("xcolor");
  HPColor *yc = (HPColor*) LookupProperty("ycolor");
  HPColor *zc = (HPColor*) LookupProperty("zcolor");
  // Compute the longest 
  QVector<double> position(GetPropertyVectorAsPixels(gc,"position"));
  int maxlen = (int)((position[2] > position[3]) ? position[2] : position[3]);
  HPTwoVector *kp = (HPTwoVector*) LookupProperty("ticklength");
  QVector<double> ticklen(kp->Data());
  int ticlen;
  if (Is2DView())
    ticlen = (int) (maxlen*ticklen[0]);
  else
    ticlen = (int) (maxlen*ticklen[1]);
  float ticdir;
  if (IsAuto("tickdirmode")) {
    if (Is2DView())
      ticdir = 1;
    else
      ticdir = -1;
  } else {
    if (((HPInOut*) LookupProperty("tickdir"))->Is("in")) 
      ticdir = 1;
    else
      ticdir = -1;
  }
  HPStringSet *qp;
  qp = (HPStringSet*) LookupProperty("xticklabel");
  StringVector xlabeltxt(qp->Data());
  qp = (HPStringSet*) LookupProperty("yticklabel");
  StringVector ylabeltxt(qp->Data());
  qp = (HPStringSet*) LookupProperty("zticklabel");
  StringVector zlabeltxt(qp->Data());
  // Draw the ticks
  QVector<double> limits(GetAxisLimits());
  // Next step - calculate the tick directions...
  // We have to draw the tics in flat space
  //
  // To keep the label from touching the tick labels, we need
  // that the tick label box (x0,y0) --> (x0+maxx,y0+maxy)
  // and we need to advance by (x0+n*dx,y0+n*dy) so that
  // n = max(maxx/dx,maxy/dy)
  //
  gc.setLineStyle("-");
  QVector<double> outerpos(GetPropertyVectorAsPixels(gc,"outerposition"));
  if (xvisible) {
    QVector<double> mapticks;
    for (int i=0;i<xticks.size();i++)
      mapticks.push_back(MapX(xticks[i]));
    QVector<double> minorticks;
    HPLinearLog *sp;
    sp = (HPLinearLog*) LookupProperty("xscale");
    if (sp->Is("log")) {
      for (int i=0;i<xticks.size()-1;i++) {
	double t1 = xticks[i];
	double t2 = xticks[i+1];
	int n = 2;
	while (((t1*n) < t2) && (n < MAX_TICK_COUNT)) {
	  minorticks.push_back(MapX(n*t1));
	  n++;
	}
      }
    }
    DrawTickLabels(gc,xc->Data(),
		   0,x1pos[1],x1pos[2],
		   0,x2pos[1],x2pos[2],
		   limits[0],limits[1],
		   1,0,0,
		   mapticks,minorticks,xlabeltxt,
		   "xlabel",ticlen,ticdir);
  }
  if (yvisible) {
    QVector<double> mapticks;
    for (int i=0;i<yticks.size();i++)
      mapticks.push_back(MapY(yticks[i]));
    QVector<double> minorticks;
    HPLinearLog *sp;
    sp = (HPLinearLog*) LookupProperty("yscale");
    if (sp->Is("log")) {
      for (int i=0;i<yticks.size()-1;i++) {
	double t1 = yticks[i];
	double t2 = yticks[i+1];
	int n = 2;
	while (((t1*n) < t2) && (n < MAX_TICK_COUNT)) {
	  minorticks.push_back(MapY(n*t1));
	  n++;
	}
      }
    }
    DrawTickLabels(gc,yc->Data(),
		   y1pos[0],0,y1pos[2],
		   y2pos[0],0,y2pos[2],
		   limits[2],limits[3],
		   0,1,0,
		   mapticks,minorticks,ylabeltxt,
		   "ylabel",ticlen,ticdir);
  }
  if (zvisible) {
    QVector<double> mapticks;
    for (int i=0;i<zticks.size();i++)
      mapticks.push_back(MapZ(zticks[i]));
    QVector<double> minorticks;
    HPLinearLog *sp;
    sp = (HPLinearLog*) LookupProperty("zscale");
    if (sp->Is("log")) {
      for (int i=0;i<zticks.size()-1;i++) {
	double t1 = zticks[i];
	double t2 = zticks[i+1];
	int n = 2;
	while (((t1*n) < t2) && (n < MAX_TICK_COUNT)) {
	  minorticks.push_back(MapZ(n*t1));
	  n++;
	}
      }
    }
    DrawTickLabels(gc,zc->Data(),
		   z1pos[0],z1pos[1],0,
		   z2pos[0],z2pos[1],0,
		   limits[4],limits[5],
		   0,0,1,
		   mapticks,minorticks,zlabeltxt,
		   "zlabel",ticlen,ticdir);
  }
}

void HandleAxis::DrawTickLabels(RenderEngine& gc,
				QVector<double> color,
				double px1, double py1, double pz1,
				double px2, double py2, double pz2,
				double limmin, double limmax,
				double unitx, double unity, double unitz,
				QVector<double>  maptics,
				QVector<double>  minortics,
				StringVector labels,
				QString labelname,
				int ticlen, double ticdir) 
{
  gc.color(color);
  // Calculate the tick direction vector
  double dx1, dy1, dx2, dy2;
  gc.debug();
  gc.toPixels(limmin*unitx+px1,
	      limmin*unity+py1,
	      limmin*unitz+pz1,dx1,dy1);
  gc.toPixels(limmin*unitx+px2,
	      limmin*unity+py2,
	      limmin*unitz+pz2,dx2,dy2);
  double delx, dely;
  delx = dx2-dx1; dely = dy2-dy1;
  // normalize the tick length
  double norm = sqrt(delx*delx + dely*dely);
  if( norm > 0 ){
      delx /= norm; dely /= norm;
      // Draw the minor ticks
      for (int i=0;i<minortics.size();i++) {
	double t = minortics[i];
	// Map the coords ourselves
	double x1, y1, x2, y2;
	gc.toPixels(t*unitx+px1,
		    t*unity+py1,
		    t*unitz+pz1,x1,y1);
	x2 = delx*ticlen*ticdir*0.6 + x1;
	y2 = dely*ticlen*ticdir*0.6 + y1;
	gc.setupDirectDraw();
	gc.line(x1,y1,x2,y2);
	gc.releaseDirectDraw();
      }
      for (int i=0;i<maptics.size();i++) {
	double t = maptics[i];
	// Map the coords ourselves
	double x1, y1, x2, y2;
	gc.toPixels(t*unitx+px1,
		    t*unity+py1,
		    t*unitz+pz1,x1,y1);
	x2 = delx*ticlen*ticdir + x1;
	y2 = dely*ticlen*ticdir + y1;
	gc.setupDirectDraw();
	gc.line(x1,y1,x2,y2);
	gc.releaseDirectDraw();
	double x3, y3;
	if (ticdir > 0) {
	  x3 = -delx*0.015*norm + x1;
	  y3 = -dely*0.015*norm + y1;
	} else {
	  x3 = -delx*0.015*norm + x2;
	  y3 = -dely*0.015*norm + y2;
	}
	if (!labels.empty()) {
	  DrawLabel(gc,-delx,-dely,x3,y3,color,
		    labels[i % labels.size()]);
	}
	// For a 2D view, draw the opposite tick marks too
	if (Is2DView()) {
	  gc.toPixels(t*unitx+px2,
		      t*unity+py2,
		      t*unitz+pz2,x1,y1);
	  x2 = -delx*ticlen*ticdir + x1;
	  y2 = -dely*ticlen*ticdir + y1;
	  gc.setupDirectDraw();
	  gc.line(x1,y1,x2,y2);
	  gc.releaseDirectDraw();
	}
      }
      // Get the maximum tick metric
      double maxx, maxy;
      GetMaxTickMetric(gc,labels,maxx,maxy);
      // Draw the x label
      // Calculate the center of the x axis...
      double x1, x2, x3, y1, y2, y3;
      double meanval;
      meanval = (limmin+limmax)/2.0;
      gc.toPixels(meanval*unitx+px1,
		  meanval*unity+py1,
		  meanval*unitz+pz1,x1,y1);
      // Calculate the tick offset
      x2 = delx*ticlen*ticdir + x1;
      y2 = dely*ticlen*ticdir + y1;
      // Offset by the top of the label
      if (ticdir > 0) {
	x3 = -delx*0.015*norm + x1;
	y3 = -dely*0.015*norm + y1;
      } else {
	x3 = -delx*0.015*norm + x2;
	y3 = -dely*0.015*norm + y2;
      }
      double lx, ly;
      if (delx != 0)
	lx = fabs(maxx/delx);
      else
	lx = 1e10;
      if (dely != 0)
	ly = fabs(maxy/dely);
      else
	ly = 1e10;
      double lmax;
      lmax = qMin(lx,ly);
      // Set the position of the label
      HPHandles *lbl = (HPHandles*) LookupProperty(labelname);
      if (!lbl->Data().empty()) {
	HandleText *fp = (HandleText*) LookupHandleObject(lbl->Data()[0]);
	// To calculate the angle, we have to look at the axis
	// itself.  The direction of the axis is determined by
	// the projection of [1,0,0] onto the screen plane
	double axx1, axy1, axx2,axy2;
	gc.toPixels(0,0,0,axx1,axy1);
	gc.toPixels(unitx,unity,unitz,axx2,axy2);
	double angle = atan2(axy2-axy1,axx2-axx1)*180.0/M_PI;
	if (angle < -90) angle += 180;
	if (angle > 90) angle -= 180;
	HPScalar *sp = (HPScalar*) fp->LookupProperty("rotation");
	// The angle we want is no the rotation angle of the axis, but
	// the angle of the origin to label position.  We get this
	// taking the mean limit along the unit direction, and the
	// average of the two projected axes.
	double origx, origy;
	gc.toPixels(meanval*unitx+(px1+px2)/2.0,
		    meanval*unity+(py1+py2)/2.0,
		    meanval*unitz+(pz1+pz2)/2.0,origx,origy);
	double meanx, meany;
	gc.toPixels(meanval*unitx+px1,
		    meanval*unity+py1,
		    meanval*unitz+pz1,meanx,meany); 
         
	//       int pixpad = 1.5*fp->GetTextHeightInPixels();
	//       // Offset by the labelsize
	//       if (lmax == lx)
	// 	lmax += fabs(pixpad/delx);
	//       else
	// 	lmax += fabs(pixpad/dely);
	double xl1, yl1;
	xl1 = x3 - lmax*delx;
	yl1 = y3 - lmax*dely;
	double angle2 = atan2(y3-origy,x3-origx)*180.0/M_PI;
	if ((angle == 90) && (angle2 > -90)) {
	  angle = -90;
	}
	if (angle2 == 180)
	  angle2 = -180;
	if (angle2 < 0)
	  if (fabs(angle) != 90)
	    ((HPAlignVert *) fp->LookupProperty("verticalalignment"))->Value("top");
	  else
	    ((HPAlignVert *) fp->LookupProperty("verticalalignment"))->Value("bottom");
	else
	  ((HPAlignVert *) fp->LookupProperty("verticalalignment"))->Value("bottom");
	if ((angle == -90) && (angle2 == -180))
	  angle = 90;
	sp->Value(angle);
	// Move another couple of percent along the radial line
	xl1 += (x3-origx)*0.04;
	yl1 += (y3-origy)*0.04;
	HPThreeVector *gp = (HPThreeVector*) fp->LookupProperty("position");
	// We now have the position of the label in absolute (pixel)
	// coordinates.  Need to translate this to normalized coordinates
	// relative to outerposition.
	QVector<double> outerpos(GetPropertyVectorAsPixels(gc,"outerposition"));
	double xnorm, ynorm;
	xnorm = (xl1-outerpos[0])/outerpos[2];
	ynorm = (yl1-outerpos[1])/outerpos[3];
	gp->Value(xnorm,ynorm,0.0);
      }
    }
}

void HandleAxis::DrawAxisLabels(RenderEngine& gc) {
  // Set up the "annotation axis"
  gc.lookAt(0,0,1,0.0,0.0,0,0,1,0);
  gc.project(0,1,0,1,-1,1);
  QVector<double> outerpos(GetPropertyVectorAsPixels(gc,"outerposition"));
  gc.viewport(outerpos[0],outerpos[1],outerpos[2],outerpos[3]);
  HPHandles *lbl;
  QString xdir(StringPropertyLookup("xdir"));
  QString ydir(StringPropertyLookup("ydir"));
  SetStringDefault("xdir","normal");
  SetStringDefault("ydir","normal");
  QString xscale(StringPropertyLookup("xscale"));
  QString yscale(StringPropertyLookup("yscale"));
  SetStringDefault("xscale","linear");
  SetStringDefault("yscale","linear");
  if (xvisible) {
    lbl = (HPHandles*) LookupProperty("xlabel");
    if (!lbl->Data().empty()) {
      HandleObject *fp = LookupHandleObject(lbl->Data()[0]);
      fp->PaintMe(gc);
    }
  }
  if (yvisible) {
    lbl = (HPHandles*) LookupProperty("ylabel");
    if (!lbl->Data().empty()) {
      HandleObject *fp = LookupHandleObject(lbl->Data()[0]);
      fp->PaintMe(gc);
    }      
  }
  if (zvisible) {
    lbl = (HPHandles*) LookupProperty("zlabel");
    if (!lbl->Data().empty()) {
      HandleObject *fp = LookupHandleObject(lbl->Data()[0]);
      fp->PaintMe(gc);
    }      
  }
  lbl = (HPHandles*) LookupProperty("title");
  if (!lbl->Data().empty()) {
    HandleObject *fp = LookupHandleObject(lbl->Data()[0]);
    fp->PaintMe(gc);
  }      
  SetupProjection(gc);
  SetStringDefault("xdir",xdir);
  SetStringDefault("ydir",ydir);
  SetStringDefault("xscale",xscale);
  SetStringDefault("yscale",yscale);
}

void HandleAxis::DrawChildren(RenderEngine& gc) {
  HPHandles *children = (HPHandles*) LookupProperty("children");
  QVector<unsigned> handles(children->Data());
  for (int i=0;i<handles.size();i++) {
    HandleObject *fp = LookupHandleObject(handles[i]);
    fp->PaintMe(gc);
  }
}

void HandleAxis::PaintMe(RenderEngine& gc) {
  if (GetParentFigure() == NULL) return;
  InitialSetupAxis(gc);
  RecalculateTicks(gc);
  RePackFigure(gc);
  SetupProjection(gc);
  SetupAxis(gc);
  if (StringCheck("visible","on")) {
    DrawBox(gc);
    DrawGridLines(gc);
    DrawMinorGridLines(gc);
  }
  QVector<double> limits(GetAxisLimits());
  gc.setClipBox(limits);
  DrawChildren(gc);
  if (StringCheck("visible","on")) {
    DrawAxisLines(gc);
    DrawTickMarks(gc);
    DrawAxisLabels(gc);
  }
  m_box = GetPropertyVectorAsPixels(gc,"position");
}

void HandleAxis::PaintBoundingBox(RenderEngine& gc) {
  if (GetParentFigure() == NULL) return;
  SetupProjection(gc);
  SetupAxis(gc);
  DrawBox(gc);
}
