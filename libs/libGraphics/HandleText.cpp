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
#include <QtGui>
#include "HandleText.hpp"
#include "HandleAxis.hpp"
#include "HandleProperty.hpp"
#include "HandleList.hpp"
#include <math.h>
#include "IEEEFP.hpp"

HandleText::HandleText() {
  ConstructProperties();
  SetupDefaults();
}

HandleText::~HandleText() {
}

void HandleText::UpdateState() {
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
  fnt = QFont(fontname->Data(),(int)(fontsize->Data()[0]));
  fnt.setStyle(fstyle);
  fnt.setWeight(fweight);
  HPString *txt = (HPString*) LookupProperty("string");
  text = txt->Data();
  QFontMetrics fm(fnt);
  QRect sze(fm.boundingRect(text));
  SetFourVectorDefault("boundingbox",sze.left(),sze.bottom(),
		       sze.width(),sze.height());
}

int HandleText::GetTextHeightInPixels() {
  QFontMetrics fm(fnt);
  QRect sze(fm.boundingRect("|"));
  return sze.height();
}

void HandleText::PaintMe(RenderEngine& gc) {
  UpdateState();
  if (StringCheck("visible","off"))
    return;
  // Get handleaxis parent
  HandleAxis* axis = GetParentAxis();
  if (!axis) return;
  // Map position -> pixel location
  int x, y;
  QVector<double> pos(VectorPropertyLookup("position"));
  // remap it
  if (pos.size() == 2) pos << 0;
  QVector<double> mapped(axis->ReMap(pos));
  gc.toPixels(mapped[0],mapped[1],mapped[2],x,y);
  gc.setupDirectDraw();
  // Retrieve the margin...
  double margin(ScalarPropertyLookup("margin"));
  margin = margin + 1;
  RenderEngine::AlignmentFlag xalign, yalign;
  HPAlignVert *hv = (HPAlignVert*) LookupProperty("verticalalignment");
  if (hv->Is("top"))
    yalign = RenderEngine::Max;
  else if (hv->Is("middle"))
    yalign = RenderEngine::Mean;
  else
    yalign = RenderEngine::Min;
  HPAlignHoriz *hh = (HPAlignHoriz*) LookupProperty("horizontalalignment");
  if (hh->Is("left"))
    xalign = RenderEngine::Min;
  else if (hh->Is("center"))
    xalign = RenderEngine::Mean;
  else
    xalign = RenderEngine::Max;
  // Get the width and height of the label
  int textwidth;
  int textheight;
  // Get the corner offsets for the label
  int textxoffset;
  int textyoffset;
  gc.measureText(text,fnt,xalign,yalign,textwidth,textheight,textxoffset,textyoffset);
  // Construct the coordinates of the text rectangle
  int x1, y1;
  double costheta, sintheta;
  double rotation = ScalarPropertyLookup("rotation");
  costheta = cos(-rotation*M_PI/180.0);
  sintheta = sin(-rotation*M_PI/180.0);
  x1 = (int)(x + (textxoffset-margin)*costheta + (textyoffset-margin)*sintheta);
  y1 = (int)(y - (textxoffset-margin)*sintheta + (textyoffset-margin)*costheta);
  double hdelx, hdely, vdelx, vdely;
  hdelx = (textwidth+2*margin)*costheta;
  hdely = -(textwidth+2*margin)*sintheta;
  vdelx = (textheight+2*margin)*sintheta;
  vdely = (textheight+2*margin)*costheta;
  // fill background rectangle
  HPColor *bc = (HPColor*) LookupProperty("backgroundcolor");
  if (!bc->IsNone()) {
    gc.color(bc->Data());
    gc.quad(x1,y1,0,
	    x1+hdelx,y1+hdely,0,
	    x1+hdelx+vdelx,y1+hdely+vdely,0,
	    x1+vdelx,y1+vdely,0);
  }
  // draw bounding rectangle
  HPColor *ec = (HPColor*) LookupProperty("edgecolor");
  if (!ec->IsNone()) {
    gc.color(ec->Data());
    gc.setLineStyle(((HPLineStyle*) LookupProperty("linestyle"))->Data());
    gc.lineWidth(ScalarPropertyLookup("linewidth"));
    // draw bounding rectangle
    gc.quadline(x1,y1,0,
		x1+hdelx,y1+hdely,0,
		x1+hdelx+vdelx,y1+hdely+vdely,0,
		x1+vdelx,y1+vdely,0);
  }
  // draw the text
  HPColor *tc = (HPColor*) LookupProperty("color");
  gc.putText(x,y,text,tc->Data(),xalign,yalign,fnt,rotation);
  gc.releaseDirectDraw();
}

void HandleText::SetupDefaults() {
  SetConstrainedStringDefault("horizontalalignment","left");
  SetThreeVectorDefault("position",0,0,0);
  SetScalarDefault("rotation",0);
  SetConstrainedStringDefault("units","data");
  SetConstrainedStringDefault("verticalalignment","middle");
  SetThreeVectorDefault("backgroundcolor",-1,-1,-1);
  SetThreeVectorDefault("edgecolor",-1,-1,-1);
  SetScalarDefault("linewidth",0.5);
  SetConstrainedStringDefault("linestyle","-");
  SetScalarDefault("margin",0);
  SetConstrainedStringDefault("fontangle","normal");
  SetStringDefault("fontname","helvetica");
  SetScalarDefault("fontsize",10);
  SetConstrainedStringDefault("fontunits","points");
  SetConstrainedStringDefault("fontweight","normal");
  SetConstrainedStringDefault("visible","on");
  SetThreeVectorDefault("color",0,0,0);
  SetStringDefault("type","text");
}

void HandleText::ConstructProperties() {
  //!
  //@Module TEXTPROPERTIES Text Object Properties
  //@@Section HANDLE
  //@@Usage
  //Below is a summary of the properties for a text object.
  //\begin{itemize}
  //  \item @|boundingbox| - @|four vector| - The size of the bounding
  // box containing the text (in pixels).  May contain negative values
  // if the text is slanted.
  //  \item @|children| - Not used.
  //  \item @|string| - @|string| - The text contained in the label.
  //  \item @|extent| - Not used.
  //  \item @|horizontalalignment| - @|{'left','center','right'}| - Controls the 
  // alignment of the text relative to the specified position point.
  //  \item @|position| - @|three vector| - The position of the label in axis
  // coordinates.
  //  \item @|rotation| - @|scalar| - The rotation angle (in degrees) of the label.
  //  \item @|units| - Not used.
  //  \item @|verticalalignment| - @|{'top','bottom','middle'}| - Controls the
  // alignment fo the text relative to the specified position point in the
  // vertical position.
  //  \item @|backgroundcolor| - @|colorspec| - The color used to fill in the background
  // rectangle for the label.  Normally this is @|none|.
  //  \item @|edgecolor| - @|colorspec| - The color used to draw the bounding rectangle
  // for the label.  Normally this is @|none|.
  //  \item @|linewidth| - @|scalar| - The width of the line used to draw the border.
  //  \item @|linestyle| - @|{'-','--',':','-.','none'}| - The style of the line used
  // to draw the border.
  //  \item @|margin| - @|scalar| - The amount of spacing to place around the text as
  // padding when drawing the rectangle.
  //  \item @|fontangle| - @|{'normal','italic','oblique'}| - The angle of the fonts used
  // for the labels.
  //  \item @|fontsize| - @|scalar| - The size of fonts used for the text.
  //  \item @|fontunits| - Not used.
  //  \item @|fontweight| - @|{'normal','bold','light','demi'}| - The weight of the font used
  // for the label
  //  \item @|visible| - @|{'on','off'}| - Controls visibility of the the line.
  //  \item @|color| - @|colorspec| - The color of the text of the label.
  //  \item @|children| - Not used.
  //  \item @|parent| - The handle of the axis that owns this label.
  //  \item @|tag| - @|string| - A string that can be used to tag the object.
  //  \item @|type| - @|string| - Returns the string @|'text'|.
  //  \item @|userdata| - @|array| - Available to store any variable you
  // want in the handle object.
  //\end{itemize}
  //!
  AddProperty(new HPFourVector,"boundingbox");
  AddProperty(new HPHandles,"children");
  AddProperty(new HPString,"string");
  AddProperty(new HPFourVector,"extent");
  AddProperty(new HPAlignHoriz,"horizontalalignment");
  AddProperty(new HPThreeVector,"position");
  AddProperty(new HPScalar,"rotation");
  AddProperty(new HPUnits,"units");
  AddProperty(new HPAlignVert,"verticalalignment");
  AddProperty(new HPColor,"backgroundcolor");
  AddProperty(new HPColor,"edgecolor");
  AddProperty(new HPScalar,"linewidth");
  AddProperty(new HPLineStyle,"linestyle");
  AddProperty(new HPScalar,"margin");
  AddProperty(new HPFontAngle,"fontangle");
  AddProperty(new HPString,"fontname");
  AddProperty(new HPScalar,"fontsize");
  AddProperty(new HPFontUnits,"fontunits");
  AddProperty(new HPFontWeight,"fontweight");
  AddProperty(new HPOnOff,"visible");
  AddProperty(new HPColor,"color"); 
  AddProperty(new HPHandles,"children");
  AddProperty(new HPHandles,"parent");
  AddProperty(new HPString,"tag");
  AddProperty(new HPString,"type");
  AddProperty(new HPArray,"userdata");
}
