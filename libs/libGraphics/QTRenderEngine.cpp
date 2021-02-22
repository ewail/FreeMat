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
#include "QTRenderEngine.hpp"
#include <math.h>
#include <algorithm>
#include "IEEEFP.hpp"

bool operator<(const quad3d& a, const quad3d& b) {
  return (a.meanz < b.meanz);
}

void cross(double ux, double uy, double uz, 
	   double vx, double vy, double vz,
	   double &sx, double &sy, double &sz) {
  sx = uy*vz-uz*vy;
  sy = uz*vx-ux*vz;
  sz = ux*vy-uy*vx;
}

QTRenderEngine::QTRenderEngine(QPainter *painter, double x1, double y1,
			       double width, double height) {
  pnt = painter;
  pnt->setRenderHint(QPainter::TextAntialiasing);
  m_x1 = x1; m_y1 = y1; m_width = width; m_height = height;
  inDirect = false;
  pnt->setPen(QColor(0,0,0));
  pnt->setBrush(QColor(0,0,0));
  debugFlag = false;
  QRect rect = pnt->viewport();
  QSize size(width,height);
  size.scale(rect.size(),Qt::KeepAspectRatio);
  pnt->setViewport(rect.x() + (rect.width()-size.width())/2,
		  rect.y() + (rect.height()-size.height())/2,
		  size.width(),size.height());
  pnt->setWindow(QRect(0,0,width,height));
}

QTRenderEngine::~QTRenderEngine() {
}

void QTRenderEngine::clear(QVector<double> col) {
  pnt->save();
  pnt->setPen(QColor((int)(col[0]*255),(int)(col[1]*255),(int)(col[2]*255)));
  pnt->setBrush(QColor((int)(col[0]*255),(int)(col[1]*255),(int)(col[2]*255)));
  pnt->drawRect((int)m_x1,(int)m_y1,(int)m_width,(int)m_height);
  pnt->restore();
}

QPointF QTRenderEngine::Map(double x, double y, double z) {
  double a, b;
  toPixels(x,y,z,a,b);
  b = m_height-1-b;
  return QPointF(rint(a),rint(b));
}

QVector<QPointF> QTRenderEngine::Map(QVector<double> xs, 
				     QVector<double> ys, 
				     QVector<double> zs) {
  QVector<QPointF> retval;
  for (int i=0;i<xs.size();i++)
    retval.push_back(Map(xs[i],ys[i],zs[i]));
  return retval;
}

QVector<quad3d> QTRenderEngine::MapQuadsFacesOnly(QVector<QVector<cpoint> > &faces) {
  QVector<quad3d> retval;
  for (int i=0;i<faces.size();i++) {
    QVector<cpoint> qlist(faces[i]);
    for (int j=2;j<qlist.size();j+=2){ 
      quad3d qx;
      bool anyclipped = false;
      double zmean = 0;
      for (int k=0;k<4;k++) {
	cpoint cpt(qlist[j-2+k]);
	double a, b, c;
	bool clipped;
	toPixels(cpt.x,cpt.y,cpt.z,a,b,c,clipped);
	anyclipped = anyclipped | clipped;
	qx.pts[k].x = a;
	qx.pts[k].y = m_height-1-b;
	qx.pts[k].z = c;
	zmean += c;
	qx.r = cpt.r;
	qx.g = cpt.g;
	qx.b = cpt.b;
	qx.a = cpt.a;
	qx.er = cpt.r;
	qx.eg = cpt.g;
	qx.eb = cpt.b;
	qx.ea = 0;
      }
      qx.meanz = -zmean/4.0;
      if (!anyclipped)
	retval.push_back(qx);
    }
  }
  return retval;
}

QVector<quad3d> QTRenderEngine::MapQuadsEdgesOnly(QVector<QVector<cpoint> > &edges) {
  QVector<quad3d> retval;
  for (int i=0;i<edges.size();i++) {
    QVector<cpoint> elist(edges[i]);
    for (int j=2;j<elist.size();j+=2){ 
      quad3d qx;
      bool anyclipped = false;
      double zmean = 0;
      for (int k=0;k<4;k++) {
	cpoint cpt(elist[j-2+k]);
	double a, b, c;
	bool clipped;
	toPixels(cpt.x,cpt.y,cpt.z,a,b,c,clipped);
	anyclipped = anyclipped | clipped;
	qx.pts[k].x = a;
	qx.pts[k].y = m_height-1-b;
	qx.pts[k].z = c;
	zmean += c;
	qx.r = cpt.r;
	qx.g = cpt.g;
	qx.b = cpt.b;
	qx.a = 0;
	qx.er = cpt.r;
	qx.eg = cpt.g;
	qx.eb = cpt.b;
	qx.ea = cpt.a;
      }
      qx.meanz = -zmean/4.0;
      if (!anyclipped)
	retval.push_back(qx);
    }
  }
  return retval;
}


QVector<quad3d> QTRenderEngine::MapQuads(QVector<QVector<cpoint> > &faces,
					 QVector<QVector<cpoint> > &edges) {
  QVector<quad3d> retval;
  if (edges.size() == 0) return MapQuadsFacesOnly(faces);
  if (faces.size() == 0) return MapQuadsEdgesOnly(edges);
  for (int i=0;i<faces.size();i++) {
    QVector<cpoint> qlist(faces[i]);
    QVector<cpoint> elist(edges[i]);
    for (int j=2;j<qlist.size();j+=2){ 
      quad3d qx;
      bool anyclipped = false;
      double zmean = 0;
      for (int k=0;k<4;k++) {
	cpoint cpt(qlist[j-2+k]);
	cpoint ept(elist[j-2+k]);
	double a, b, c;
	bool clipped;
	toPixels(cpt.x,cpt.y,cpt.z,a,b,c,clipped);
	anyclipped = anyclipped | clipped;
	qx.pts[k].x = a;
	qx.pts[k].y = m_height-1-b;
	qx.pts[k].z = c;
	zmean += c;
	qx.r = cpt.r;
	qx.g = cpt.g;
	qx.b = cpt.b;
	qx.a = cpt.a;
	qx.er = ept.r;
	qx.eg = ept.g;
	qx.eb = ept.b;
	qx.ea = ept.a;
      }
      qx.meanz = -zmean/4.0;
      if (!anyclipped)
	retval.push_back(qx);
    }
  }
  return retval;
}
  
void QTRenderEngine::toPixels(double x, double y, double z, int &a, int &b) {
  double aval, bval;
  bool clipped;
  toPixels(x,y,z,aval,bval,clipped);
  a = (int) aval;
  b = (int) bval;
}

void QTRenderEngine::toPixels(double x, double y, double z, double &a, double &b) {
  bool clipped;
  toPixels(x,y,z,a,b,clipped);
}

void QTRenderEngine::toPixels(double x, double y, double z, double &a, double &b, bool &clipped) {
  double xprime, yprime, zprime, wprime;
  double xclip, yclip, zclip, wclip;
  wprime = 1;
  mapPoint(x,y,z,xprime,yprime,zprime);
  xclip = proj[0]*xprime + proj[4]*yprime + proj[8]*zprime + proj[12];
  yclip = proj[1]*xprime + proj[5]*yprime + proj[9]*zprime + proj[13];
  zclip = proj[2]*xprime + proj[6]*yprime + proj[10]*zprime + proj[14];
  wclip = proj[3]*xprime + proj[7]*yprime + proj[11]*zprime + proj[15];
  xclip /= wclip;
  yclip /= wclip;
  zclip /= wclip;
  clipped = ((xclip < -1) | (xclip > 1) | (yclip < -1) | (yclip > 1) | (zclip < -1) | (zclip > 1));
  a = viewp[0] + (1+xclip)/2.0*viewp[2];
  b = viewp[1] + (1+yclip)/2.0*viewp[3];
}

void QTRenderEngine::toPixels(double x, double y, double z, double &a, double &b, double &c, bool &clipped) {
  double xprime, yprime, zprime, wprime;
  double xclip, yclip, zclip, wclip;
  wprime = 1;
  mapPoint(x,y,z,xprime,yprime,zprime);
  xclip = proj[0]*xprime + proj[4]*yprime + proj[8]*zprime + proj[12];
  yclip = proj[1]*xprime + proj[5]*yprime + proj[9]*zprime + proj[13];
  zclip = proj[2]*xprime + proj[6]*yprime + proj[10]*zprime + proj[14];
  wclip = proj[3]*xprime + proj[7]*yprime + proj[11]*zprime + proj[15];
  xclip /= wclip;
  yclip /= wclip;
  zclip /= wclip;
  clipped = ((xclip < -1) | (xclip > 1) | (yclip < -1) | (yclip > 1) | (zclip < -1) | (zclip > 1));
  a = viewp[0] + (1+xclip)/2.0*viewp[2];
  b = viewp[1] + (1+yclip)/2.0*viewp[3];
  c = zclip;
}

void QTRenderEngine::lookAt(double eyex, double eyey, double eyez,
			    double centerx, double centery, double centerz,
			    double upx, double upy, double upz) {
  double fx, fy, fz;
  fx = centerx - eyex;
  fy = centery - eyey;
  fz = centerz - eyez;
  double fnorm = sqrt(fx*fx+fy*fy+fz*fz);
  fx /= fnorm;
  fy /= fnorm;
  fz /= fnorm;
  double upnorm = sqrt(upx*upx+upy*upy+upz*upz);
  upx /= upnorm;
  upy /= upnorm;
  upz /= upnorm;
  double sx, sy, sz;
  cross(fx,fy,fz,upx,upy,upz,sx,sy,sz);
  double ux, uy, uz;
  cross(sx,sy,sz,fx,fy,fz,ux,uy,uz);
  model[0] = sx; model[4] = sy; model[8] = sz; model[12] = -eyex;
  model[1] = ux; model[5] = uy; model[9] = uz; model[13] = -eyey;
  model[2] =-fx; model[6] =-fy; model[10]=-fz; model[14]= -eyez;
  model[3] = 0;  model[7] = 0;  model[11] = 0; model[15] = 1;
}

void QTRenderEngine::scale(double sx, double sy, double sz) {
  model[0] *= sx; model[4] *= sy; model[8] *= sz; 
  model[1] *= sx; model[5] *= sy; model[9] *= sz; 
  model[2] *= sx; model[6] *= sy; model[10] *= sz;
  model[3] *= sx; model[7] *= sy; model[11] *= sz;
}

void QTRenderEngine::mapPoint(double x, double y, double z,
			      double &a, double &b, double &c) {
  a = model[0]*x + model[4]*y + model[8]*z + model[12];
  b = model[1]*x + model[5]*y + model[9]*z + model[13];
  c = model[2]*x + model[6]*y + model[10]*z + model[14];
}
  
void QTRenderEngine::project(double left, double right, 
			     double bottom, double top,
			     double near, double far) {
  double tx, ty, tz;
  tx = -(right+left)/(right-left);
  ty = -(top+bottom)/(top-bottom);
  tz = -(far+near)/(far-near);
  for (int i=0;i<16;i++)
    proj[i] = 0;
  proj[0] = 2/(right-left);
  proj[5] = 2/(top-bottom);
  proj[10] = -2/(far-near);
  proj[12] = tx;
  proj[13] = ty;
  proj[14] = tz;
  //    proj[15] = -1;
  proj[15] = 1; // To match GL
//   if (left != 0)
//     qDebug("Project %f %f %f %f %f %f",left,right,bottom,top,near,far);
}
  
void QTRenderEngine::viewport(double x0, double y0, double width, double height) {
  viewp[0] = (int)x0; viewp[1] = (int)y0; viewp[2] = (int)width; viewp[3] = (int)height;
  pnt->setClipRect((int)x0,(int)(m_height-(y0+height)),(int)width+1,(int)height+1);

}

void QTRenderEngine::quad(double x1, double y1, double z1,
			  double x2, double y2, double z2,
			  double x3, double y3, double z3,
			  double x4, double y4, double z4) {
  QPolygonF poly;
  poly.push_back(Map(x1,y1,z1));
  poly.push_back(Map(x2,y2,z2));
  poly.push_back(Map(x3,y3,z3));
  poly.push_back(Map(x4,y4,z4));
  pnt->drawPolygon(poly);
}

void QTRenderEngine::quadline(double x1, double y1, double z1,
			      double x2, double y2, double z2,
			      double x3, double y3, double z3,
			      double x4, double y4, double z4) {
  QPolygonF poly;
  poly.push_back(Map(x1,y1,z1));
  poly.push_back(Map(x2,y2,z2));
  poly.push_back(Map(x3,y3,z3));
  poly.push_back(Map(x4,y4,z4));
  poly.push_back(Map(x1,y1,z1));
  pnt->drawPolyline(poly);
}

void QTRenderEngine::tri(double x1, double y1, double z1,
			 double x2, double y2, double z2,
			 double x3, double y3, double z3) {
  QPolygonF poly;
  poly.push_back(Map(x1,y1,z1));
  poly.push_back(Map(x2,y2,z2));
  poly.push_back(Map(x3,y3,z3));
  pnt->drawPolygon(poly);
}

void QTRenderEngine::triLine(double x1, double y1, double z1,
			     double x2, double y2, double z2,
			     double x3, double y3, double z3) {
  QPolygonF poly;
  poly.push_back(Map(x1,y1,z1));
  poly.push_back(Map(x2,y2,z2));
  poly.push_back(Map(x3,y3,z3));
  pnt->drawPolyline(poly);
}

void QTRenderEngine::color(QVector<double> col) {
  QPen pen(pnt->pen());
  QBrush brush(pnt->brush());
  pen.setColor(QColor((int)(col[0]*255),(int)(col[1]*255),(int)(col[2]*255)));
  brush.setColor(QColor((int)(col[0]*255),(int)(col[1]*255),(int)(col[2]*255)));
  pnt->setPen(pen);
  pnt->setBrush(brush);
}

void QTRenderEngine::setLineStyle(QString style) {
  QPen pen(pnt->pen());
  if (style == "-") 
    pen.setStyle(Qt::SolidLine);
  else if (style == "--") 
    pen.setStyle(Qt::DashLine);
  else if (style == ":") 
    pen.setStyle(Qt::DotLine);
  else if (style == "-.")
    pen.setStyle(Qt::DashDotLine);
  else if (style == "none")
    pen.setStyle(Qt::NoPen);
  pnt->setPen(pen);
}

void QTRenderEngine::lineWidth(double n) {
  QPen pen(pnt->pen());
  pen.setWidthF(n);
  pnt->setPen(pen);
}

void QTRenderEngine::line(double x1, double y1, double z1,
			  double x2, double y2, double z2) {
  pnt->drawLine(Map(x1,y1,z1),Map(x2,y2,z2));
}

void QTRenderEngine::line(double x1, double y1,
			  double x2, double y2) {
  pnt->drawLine(Map(x1,y1,0),Map(x2,y2,0));    
}

void QTRenderEngine::lineSeries(QVector<double> xs, 
				QVector<double> ys,
				QVector<double> zs) {
  if (xs.size() < 2) return;
  pnt->drawPolyline(Map(xs,ys,zs));
}
  
void QTRenderEngine::debug() {
  debugFlag = !debugFlag;
//   qDebug("Projection diagonal: %f %f %f %f",
// 	 proj[0],proj[5],proj[10],proj[15]);
  return;
//   qDebug("QT Modelview matrix (before setupdirect)");
//   qDebug("%f %f %f %f",model[0],model[4],model[8],model[12]);
//   qDebug("%f %f %f %f",model[1],model[5],model[9],model[13]);
//   qDebug("%f %f %f %f",model[2],model[6],model[10],model[14]);
//   qDebug("%f %f %f %f",model[3],model[7],model[11],model[15]);
//   qDebug("QT Projection matrix (before setupdirect)");
//   qDebug("%f %f %f %f",proj[0],proj[4],proj[8],proj[12]);
//   qDebug("%f %f %f %f",proj[1],proj[5],proj[9],proj[13]);
//   qDebug("%f %f %f %f",proj[2],proj[6],proj[10],proj[14]);
//   qDebug("%f %f %f %f",proj[3],proj[7],proj[11],proj[15]);
//   qDebug("QT Viewport (before setupdirect)");
//   qDebug("%d %d %d %d",viewp[0],viewp[1],viewp[2],viewp[3]);  
}

void QTRenderEngine::setupDirectDraw() {
//   if (inDirect)
//     qDebug("DirectDraw is not reentrant!!!");
  // save the relevant matrices
  pnt->save();
  for (int i=0;i<16;i++) {
    save_model[i] = model[i];
    model[i] = 0;
    save_proj[i] = proj[i];
  }
  for (int i=0;i<4;i++)
    save_viewp[i] = viewp[i];
  model[0] = 1;
  model[5] = 1;
  model[10] = 1;
  model[15] = 1;
  viewport(m_x1,m_y1,m_width,m_height);
  project(m_x1,m_x1+m_width,m_y1,m_y1+m_height,-1,1);
  inDirect = true;
}

void QTRenderEngine::releaseDirectDraw() {
//   if (!inDirect)
//     qDebug("releaseDirectDraw called unmatched!!!");
  pnt->restore();
  for (int i=0;i<16;i++) {
    model[i] = save_model[i];
    proj[i] = save_proj[i];
  }
  for (int i=0;i<4;i++)
    viewp[i] = save_viewp[i];
  inDirect = false;
}

void QTRenderEngine::getProjectionMatrix(double aproj[16]) {
  for (int i=0;i<16;i++)
    aproj[i] = proj[i];
}

void QTRenderEngine::getModelviewMatrix(double amodel[16]) {
  for (int i=0;i<16;i++)
    amodel[i] = model[i];
}

void QTRenderEngine::getViewport(int aviewp[4]) {
  for (int i=0;i<4;i++)
    aviewp[i] = viewp[i];    
}

void QTRenderEngine::putText(double x, double y, QString txt, 
			     QVector<double> color, 
			     AlignmentFlag xflag, AlignmentFlag yflag,
			     QFont fnt, double rotation) {
  QFontMetrics fm(fnt);
  QRect sze(fm.boundingRect(txt));
  int width = sze.width();
  int height = sze.height();
  // Adjust the raster position based on the alignment offsets
  double xdelta, ydelta;
  xdelta = 0;
  ydelta = 0;
  if (xflag == Mean)
    xdelta = -width/2.0;
  if (xflag == Max)
    xdelta = -width;
  if (yflag == Mean)
    ydelta = -height/2.0;
  if (yflag == Max)
    ydelta = -height;
  // I don't understand this...
  ydelta += fm.descent();
  double costhet, sinthet;
  costhet = cos(rotation*M_PI/180.0);
  sinthet = sin(rotation*M_PI/180.0);
  double xpos, ypos;
  xpos = x+xdelta*costhet-ydelta*sinthet;
  ypos = y+xdelta*sinthet+ydelta*costhet;
  QPointF pos(Map(xpos,ypos,0));
  QPen pen(pnt->pen());
  pnt->setPen(QColor((int)(color[0]*255),(int)(color[1]*255),(int)(color[2]*255)));
  pnt->setFont(fnt);
  pnt->save();
  pnt->translate(pos);
  pnt->rotate(-rotation);
  pnt->drawText(0,0,txt);
  pnt->restore();
  pnt->setPen(pen);
}

void QTRenderEngine::measureText(QString txt, QFont fnt, AlignmentFlag xflag, 
				 AlignmentFlag yflag,int &width, int &height,
				 int &xoffset, int &yoffset) {
  QFontMetrics fm(fnt);
  QRect sze(fm.boundingRect(txt));
  width = sze.width();
  height = sze.height();
  yoffset = -height;
  xoffset = 0;
  if (xflag == Mean)
    xoffset -= width/2;
  else if (xflag == Max)
    xoffset -= width;
  if (yflag == Mean)
    yoffset += height/2;
  else if (yflag == Min)
    yoffset += height;
}

void QTRenderEngine::depth(bool) {
}

void QTRenderEngine::rect(double x1, double y1, double x2, double y2) {
  quadline(x1,y1,0,
	   x2,y1,0,
	   x2,y2,0,
	   x1,y2,0);
}
  
void QTRenderEngine::rectFill(double x1, double y1, double x2, double y2) {
  quad(x1,y1,0,
       x2,y1,0,
       x2,y2,0,
       x1,y2,0);
}

void QTRenderEngine::circle(double x1, double y1, double radius) {
  QPointF uleft(Map(x1-radius,y1+radius,0));
  QPointF lright(Map(x1+radius,y1-radius,0));
  QRectF rect;
  rect.setBottomRight(lright);
  rect.setTopLeft(uleft);
  QBrush brsh(pnt->brush());
  pnt->setBrush(Qt::NoBrush);
  pnt->drawEllipse(rect);
  pnt->setBrush(brsh);
}

void QTRenderEngine::circleFill(double x1, double y1, double radius) {
  QPointF uleft(Map(x1-radius,y1+radius,0));
  QPointF lright(Map(x1+radius,y1-radius,0));
  QRectF rect;
  rect.setBottomRight(lright);
  rect.setTopLeft(uleft);
  QPen pen(pnt->pen());
  pnt->setPen(Qt::NoPen);
  pnt->drawEllipse(rect);
  pnt->setPen(pen);
}

void QTRenderEngine::drawImage(double x1, double y1, double x2, double y2,
			       QImage pic) {
  QPointF pt(Map(x1,y1,0));
  pt.setY(pt.y()-pic.height());
  pnt->drawImage(pt,pic);
}

// Consider a nominal case
//
//   xdata = [0,1]
//   ydata = [0,1]
//
//   xlim = [-.5,1.2]
//   ylim = [.1,.6]
//
//   Compute the intersection of data and lim to get the source
//   rectangle (in normalized coordinates)
//
//   xvis = [.5,1];
//   yvis = [.1,.6];
//
//   Map back to physical pixels
//
//   start_col = (xvis(0)-xdata(0))*cols/(xdata[1]-xdata[0]);
//   width_col = (xvis(1)-xvis(0))*cols/(xdata[1]-xdata[0]);
//   start_row = (yvis(0)-ydata(0))*cols/(ydata[1]-ydata[0]);
//   width_row = (yvis(1)-yvis(0))*cols/(ydata[1]-ydata[0]);
//
//   Determine where these points map on the screen
//   QPointF topleft = MAP(xvis(0),yvis(0),0); 
//   QPointF bottomright = MAP(xvis(1),yvis(1),0); 
//
//

void QTRenderEngine::drawImage(HPTwoVector* xp, HPTwoVector* yp, 
			       HPTwoVector* xlim, HPTwoVector* ylim,
			       bool xflip, bool yflip,
			       QImage pic)
{ 
  // Unpack
  float data_x1 = xp->Data()[0];
  float data_y1 = yp->Data()[0];
  float data_x2 = xp->Data()[1];
  float data_y2 = yp->Data()[1];

  //  qDebug() << "data = " << data_x1 << " " << data_y1 << " " << data_x2 << " " << data_y2;

  float lim_x1 = xlim->Data()[0];
  float lim_y1 = ylim->Data()[0];
  float lim_x2 = xlim->Data()[1];
  float lim_y2 = ylim->Data()[1];

  //  qDebug() << "lib = " << lim_x1 << " " << lim_y1 << " " << lim_x2 << " " << lim_y2;

  // Create the intersection of lim and data
  float vis_x1 = qMax(qMin(lim_x1,data_x2),data_x1);
  float vis_x2 = qMax(qMin(lim_x2,data_x2),data_x1);
  float vis_y1 = qMax(qMin(lim_y1,data_y2),data_y1);
  float vis_y2 = qMax(qMin(lim_y2,data_y2),data_y1);

  //  qDebug() << "vis = " << vis_x1 << " " << vis_y1 << " " << vis_x2 << " " << vis_y2;
  
  // Map these to physical coordinates
  float img_x1 = (vis_x1-data_x1)*(pic.width()-1)/(data_x2-data_x1);
  float img_x2 = (vis_x2-data_x1)*(pic.width()-1)/(data_x2-data_x1);
  float img_y1 = (vis_y1-data_y1)*(pic.height()-1)/(data_y2-data_y1);
  float img_y2 = (vis_y2-data_y1)*(pic.height()-1)/(data_y2-data_y1);

  //  qDebug() << "img = " << img_x1 << " " << img_y1 << " " << img_x2 << " " << img_y2;

  if (yflip) {
    vis_y1 = lim_y1+lim_y2-vis_y1;
    vis_y2 = lim_y1+lim_y2-vis_y2;
  }
  
  if (!yflip) {
    float img_y1_t = pic.height() - img_y2;
    img_y2 = pic.height() - img_y1;
    img_y1 = img_y1_t;
  }

  if (xflip) {
    vis_x1 = lim_x1+lim_x2-vis_x1;
    vis_x2 = lim_x1+lim_x2-vis_x2;
  }

  if (xflip) {
    float img_x1_t = pic.width() - img_x2;
    img_x2 = pic.width() - img_x1;
    img_x1 = img_x1_t;
  }

  //  qDebug() << "vis = " << vis_x1 << " " << vis_y1 << " " << vis_x2 << " " << vis_y2;
  //  qDebug() << "img = " << img_x1 << " " << img_y1 << " " << img_x2 << " " << img_y2;
  
  img_x2++;
  img_y2++;

  QPointF topLeft(Map(qMin(vis_x1,vis_x2),qMax(vis_y1,vis_y2),0));
  QPointF botRight(Map(qMax(vis_x1,vis_x2),qMin(vis_y2,vis_y1),0));
  QRectF target(topLeft,botRight);
  QRectF source(QPointF(img_x1,img_y1),QPointF(img_x2,img_y2));

  //  qDebug() << "topleft = " << topLeft;
  //  qDebug() << "botRight = " << botRight;

  float xscale = xflip ? -1 : 1;
  float yscale = (!yflip) ? -1 : 1;
    
  pnt->drawImage( target, pic.transformed(QTransform().scale(xscale,yscale)), 
		  source );
}


QPainterPath QTRenderEngine::quadToPoly(double x1, double y1, double z1,
					double x2, double y2, double z2,
					double x3, double y3, double z3,
					double x4, double y4, double z4) {
  QPainterPath path;
  path.moveTo(Map(x1,y1,z1));
  path.lineTo(Map(x2,y2,z2));
  path.lineTo(Map(x3,y3,z3));
  path.lineTo(Map(x4,y4,z4));
  path.closeSubpath();
  return path;
}

void QTRenderEngine::setClipBox(QVector<double> limits) {
  pnt->setClipPath(quadToPoly( limits[0], limits[2], limits[4],
			       limits[1], limits[2], limits[4],
			       limits[1], limits[3], limits[4],
			       limits[0], limits[3], limits[4]), 
		   Qt::ReplaceClip);
  pnt->setClipPath(quadToPoly( limits[0], limits[2], limits[5],
			       limits[0], limits[3], limits[5],
			       limits[1], limits[3], limits[5],
			       limits[1], limits[2], limits[5]),
		   Qt::UniteClip);
  pnt->setClipPath(quadToPoly( limits[0], limits[2], limits[4],
			       limits[0], limits[3], limits[4],
			       limits[0], limits[3], limits[5],
			       limits[0], limits[2], limits[5]),
		   Qt::UniteClip);
  pnt->setClipPath(quadToPoly( limits[1], limits[2], limits[4],
			       limits[1], limits[2], limits[5],
			       limits[1], limits[3], limits[5],
			       limits[1], limits[3], limits[4]),
		   Qt::UniteClip);
  pnt->setClipPath(quadToPoly( limits[0], limits[2], limits[4],
			       limits[0], limits[2], limits[5],
			       limits[1], limits[2], limits[5],
			       limits[1], limits[2], limits[4]),
		   Qt::UniteClip);
  pnt->setClipPath(quadToPoly( limits[0], limits[3], limits[4],
			       limits[1], limits[3], limits[4],
			       limits[1], limits[3], limits[5],
			       limits[0], limits[3], limits[5]),
		   Qt::UniteClip);
  //  pnt->setClipPath(path);
}

void QTRenderEngine::quadStrips(QVector<QVector<cpoint> > faces, bool flatfaces,
				QVector<QVector<cpoint> > edges, bool flatedges) {
  QVector<quad3d> mapqds(MapQuads(faces,edges));
  qSort(mapqds.begin(),mapqds.end());
  for (int i=0;i<mapqds.size();i++) {
    QPolygonF poly;
    poly.push_back(QPointF(mapqds[i].pts[0].x,mapqds[i].pts[0].y));
    poly.push_back(QPointF(mapqds[i].pts[1].x,mapqds[i].pts[1].y));
    poly.push_back(QPointF(mapqds[i].pts[3].x,mapqds[i].pts[3].y));
    poly.push_back(QPointF(mapqds[i].pts[2].x,mapqds[i].pts[2].y));
    
    pnt->setBrush(QColor((int)(mapqds[i].r*255),
			 (int)(mapqds[i].g*255),
			 (int)(mapqds[i].b*255),
			 (int)(mapqds[i].a*255)));
    pnt->setPen(QColor((int)(mapqds[i].er*255),(int)(mapqds[i].eg*255),
		       (int)(mapqds[i].eb*255),(int)(mapqds[i].ea*255)));
    pnt->drawConvexPolygon(poly);
  }
}

void QTRenderEngine::drawPatch(const FaceList& faces)
{
    FaceList::const_iterator it = faces.constBegin();

    while( it != faces.constEnd() ){
	const Face face = *it;
	QVector<point>::const_iterator vert_it = face.vertices.constBegin();

	QPolygonF poly;

	if( face.FaceColorMode == ColorMode::ColorSpec ){
	    pnt->setBrush( QColor( (int)(face.FaceColor.r*255), (int)(face.FaceColor.g*255), 
		(int)(face.FaceColor.b*255), (int)(face.FaceColor.a*255) ) );
	}
	if( face.FaceColorMode == ColorMode::Flat || face.FaceColorMode == ColorMode::Interp ){
	    pnt->setBrush( QColor( (int)(face.vertexcolors[0].r*255), (int)(face.vertexcolors[0].g*255), 
		(int)(face.vertexcolors[0].b*255), (int)(face.vertexcolors[0].a*255) ) );
	}
	if( face.EdgeColorMode == ColorMode::ColorSpec ){
	    pnt->setPen( QColor( (int)(face.EdgeColor.r*255), (int)(face.EdgeColor.g*255), 
		(int)(face.EdgeColor.b*255), (int)(face.EdgeColor.a*255) ) );
	}
	if( face.EdgeColorMode == ColorMode::Flat || face.EdgeColorMode == ColorMode::Interp ){
	    pnt->setPen( QColor( (int)(face.edgecolors[0].r*255), (int)(face.edgecolors[0].g*255), 
		(int)(face.edgecolors[0].b*255), (int)(face.edgecolors[0].a*255) ) );
	}
	    
	while( vert_it != face.vertices.constEnd() ){
	    point v = *vert_it;
	    poly.push_back( Map( v.x, v.y, v.z ));
	    ++vert_it;
	}
	pnt->drawPolygon( poly );
	++it;
    }
}

//};
