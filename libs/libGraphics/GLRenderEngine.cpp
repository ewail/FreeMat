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
#include "GLRenderEngine.hpp"
#include <qimage.h>
#include <qpainter.h>
#include <QtOpenGL>
#include <math.h>
#include "IEEEFP.hpp"
#ifndef __APPLE__
#include <GL/glu.h>
#else
#include <glu.h>
#endif


GLRenderEngine::GLRenderEngine(QGLWidget *widget, double x1, double y1,
			       double width, double height) {
  m_x1 = x1;
  m_y1 = y1;
  m_width = width;
  m_height = height;
  m_widget = widget;
  glEnable(GL_TEXTURE_2D);

}

QGLWidget* GLRenderEngine::widget() {
  return m_widget;
}

GLRenderEngine::~GLRenderEngine() {
}

void GLRenderEngine::clear(QVector<double> color) {
  if (color[0] != -1) {
    glClearColor(color[0], color[1], color[2], 0.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }
}

void GLRenderEngine::toPixels(double x, double y, double z, 
			      double &a, double &b, bool &clipped) {
  toPixels(x,y,z,a,b);
  clipped = ((a < viewp[0]) || (a > (viewp[0] + viewp[2])) ||
	     (b < viewp[1]) || (b > (viewp[1] + viewp[3])));
}

void GLRenderEngine::toPixels(double x, double y, double z, 
			      double &a, double &b) {
  double c1, c2, c3;
  gluProject(x,y,z,model,proj,viewp,&c1,&c2,&c3);
  a = c1;
  b = c2;
}

void GLRenderEngine::toPixels(double x, double y, double z, 
			      int &a, int &b) {
  double c1, c2;
  toPixels(x,y,z,c1,c2);
  a = (int) c1;
  b = (int) c2;
}

void GLRenderEngine::scale(double sx, double sy, double sz) {
  glMatrixMode(GL_MODELVIEW);
  glScaled(sx,sy,sz);
  glGetDoublev(GL_MODELVIEW_MATRIX,model);
}

void GLRenderEngine::lookAt(double px, double py, double pz,
			    double tx, double ty, double tz,
			    double ux, double uy, double uz) {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(px,py,pz,tx,ty,tz,ux,uy,uz);
  glGetDoublev(GL_MODELVIEW_MATRIX,model);
}

  
static void gluMultMatrixVecd(const double matrix[16], const double in[4], double out[4])
{
  int i;
    
  for (i=0; i<4; i++) {
    out[i] = 
      in[0] * matrix[0*4+i] +
      in[1] * matrix[1*4+i] +
      in[2] * matrix[2*4+i] +
      in[3] * matrix[3*4+i];
  }
}
  
void GLRenderEngine::mapPoint(double x, double y, double z,
			      double &a, double &b, double &c) {
  double out[4];
  double in[4];
  in[0] = x;
  in[1] = y;
  in[2] = z;
  in[3] = 1.0;
  gluMultMatrixVecd(model,in,out);
  a = out[0];
  b = out[1];
  c = out[2];
}

void GLRenderEngine::project(double xmin, double xmax, double ymin, 
			     double ymax, double zmin, double zmax) {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(xmin,xmax,ymin,ymax,zmin,zmax);
  glGetDoublev(GL_PROJECTION_MATRIX,proj);
}

void GLRenderEngine::viewport(double x0, double y0, 
			      double width, double height) {
  glViewport((int)x0,(int)y0,(int)width,(int)height);
  glGetIntegerv(GL_VIEWPORT,viewp);
}

void GLRenderEngine::quad(double x1, double y1, double z1,
			  double x2, double y2, double z2,
			  double x3, double y3, double z3,
			  double x4, double y4, double z4) {
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glBegin(GL_QUADS);
  glVertex3f(x1,y1,z1);
  glVertex3f(x2,y2,z2);
  glVertex3f(x3,y3,z3);
  glVertex3f(x4,y4,z4);
  glEnd();
}

void GLRenderEngine::quadline(double x1, double y1, double z1,
			      double x2, double y2, double z2,
			      double x3, double y3, double z3,
			      double x4, double y4, double z4) {
  glBegin(GL_LINE_LOOP);
  glVertex3f(x1,y1,z1);
  glVertex3f(x2,y2,z2);
  glVertex3f(x3,y3,z3);
  glVertex3f(x4,y4,z4);
  glEnd();
}

void GLRenderEngine::color(QVector<double> col) {
  glColor3f(col[0],col[1],col[2]);
}
  
void GLRenderEngine::setLineStyle(QString style) {
  if (style == "-") {
    glDisable(GL_LINE_STIPPLE);
    return;
  }
  if (style == "--") {
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1,0x00FF);
    return;
  }
  if (style == ":") {
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1,0xDDDD);
    return;
  }
  if (style == "-.") {
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1,0xF0D0);
    return;
  }
  if (style == "none") {
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1,0x0000);
    return;
  }
}

void GLRenderEngine::lineWidth(double n) {
  glLineWidth(n);
}

void GLRenderEngine::line(double x1, double y1, double z1,
			  double x2, double y2, double z2) {
  glBegin(GL_LINES);
  glVertex3f(x1,y1,z1);
  glVertex3f(x2,y2,z2);
  glEnd();
}

void GLRenderEngine::line(double x1, double y1, double x2, double y2) {
  glBegin(GL_LINES);
  glVertex2f(x1,y1);
  glVertex2f(x2,y2);
  glEnd();
}

void GLRenderEngine::lineSeries(QVector<double> xs, 
				QVector<double> ys, 
				QVector<double> zs) {
  glBegin(GL_LINE_STRIP);
  for (int i=0;i<xs.size();i++)
    glVertex3f(xs[i],ys[i],zs[i]);
  glEnd();
}

void GLRenderEngine::debug() {
  double tmodel[16];
  double tproj[16];
  GLint tviewp[4];
  glGetDoublev(GL_MODELVIEW_MATRIX,tmodel);
  glGetDoublev(GL_PROJECTION_MATRIX,tproj);
  glGetIntegerv(GL_VIEWPORT,tviewp);
  //   qDebug("GL Modelview matrix (before setupdirect)");
  //   qDebug("%f %f %f %f",tmodel[0],tmodel[4],tmodel[8],tmodel[12]);
  //   qDebug("%f %f %f %f",tmodel[1],tmodel[5],tmodel[9],tmodel[13]);
  //   qDebug("%f %f %f %f",tmodel[2],tmodel[6],tmodel[10],tmodel[14]);
  //   qDebug("%f %f %f %f",tmodel[3],tmodel[7],tmodel[11],tmodel[15]);
  //   qDebug("GL Projection matrix (before setupdirect)");
  //   qDebug("%f %f %f %f",tproj[0],tproj[4],tproj[8],tproj[12]);
  //   qDebug("%f %f %f %f",tproj[1],tproj[5],tproj[9],tproj[13]);
  //   qDebug("%f %f %f %f",tproj[2],tproj[6],tproj[10],tproj[14]);
  //   qDebug("%f %f %f %f",tproj[3],tproj[7],tproj[11],tproj[15]);
  //   qDebug("GL Viewport (before setupdirect)");
  //   qDebug("%d %d %d %d",tviewp[0],tviewp[1],tviewp[2],tviewp[3]);  
}

void GLRenderEngine::setupDirectDraw() {
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glViewport((int)m_x1,(int)m_y1,(int)m_width,(int)m_height);
  glOrtho(m_x1,m_x1+m_width,m_y1,m_y1+m_height,-1,1);
  glDisable(GL_DEPTH_TEST);
}

void GLRenderEngine::releaseDirectDraw() {
  glEnable(GL_DEPTH_TEST);
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();    
  glViewport(viewp[0],viewp[1],viewp[2],viewp[3]);
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}
  
void GLRenderEngine::getModelviewMatrix(double amodel[16]) {
  for (int i=0;i<16;i++)
    amodel[i] = model[i];
}
  
void GLRenderEngine::getProjectionMatrix(double aproj[16]) {
  for (int i=0;i<16;i++)
    aproj[i] = proj[16];
}

void GLRenderEngine::getViewport(int aviewp[4]) {
  for (int i=0;i<4;i++)
    aviewp[i] = viewp[i];
}

void GLRenderEngine::putText(double x, double y, QString txt, 
			     QVector<double> color, 
			     AlignmentFlag xflag, AlignmentFlag yflag,
			     QFont fnt, double rotation) {
  QFontMetrics fm(fnt);
  QRect sze(fm.boundingRect(txt));
  //  int x0 = sze.left();
  //  int y0 = sze.bottom();
  int width = sze.width();
  int height = sze.height();
  // We now now the width and height.  From this,
  // we can compute the radial length
  int radlength = static_cast<int>(sqrt(static_cast<float>(width*width+height*height))*2);
  // We need a bitmap surface that is 2X this size.
  QImage img(radlength,radlength,QImage::Format_RGB32);
  QPainter pnt(&img);
  pnt.setRenderHint(QPainter::TextAntialiasing);
  pnt.setRenderHint(QPainter::Antialiasing);
  pnt.setBackground(QColor(255,255,255));
  pnt.eraseRect(0,0,radlength,radlength);
  pnt.setFont(fnt);
  pnt.setPen(QColor(0,0,0));
  // We translate to the center of the bitmap
  pnt.translate(radlength/2,radlength/2);
  pnt.rotate(-rotation);
  //    pnt.drawText(-width/2,height/2,txt.c_str());
  pnt.drawText(0,0,txt);
  pnt.end();

  // The next step is to trim the bitmap from the bottom up
  bool allempty = true;
  int row_offset = 0;
  while (allempty && (row_offset < (radlength/2-1))) {
    QRgb* dbits = (QRgb*) img.scanLine(radlength-1-row_offset);
    allempty = true;
    for (int i=0;allempty && (i<radlength);i++) 
      allempty = (qRed(dbits[i]) == 255);
    row_offset++;
  }
  row_offset--;
  row_offset = qMax(row_offset,0);

  // Now we trim from the left side
  allempty = true;
  int col_offset = 0;
  while (allempty && (col_offset < (radlength/2-1))) {
    allempty = true;
    for (int i=0;allempty && (i<radlength);i++)
      allempty = (qRed(((QRgb*)img.scanLine(i))[col_offset]) == 255);
    col_offset++;
  }
  col_offset--;
  col_offset = qMax(col_offset,0);
    
  int newwidth = radlength-col_offset;
  int newheight = radlength-row_offset;
  // Copy the text into an openGL bitmap
  QImage pic = QImage(newwidth,newheight,QImage::Format_ARGB32);
  // Set the color bits to all be the same color as specified
  // in the argument list, and use the grey scale to modulate
  // the transparency
  int cred, cgreen, cblue;
  cred = (int)(color[0]*255);
  cgreen = (int)(color[1]*255);
  cblue = (int)(color[2]*255);
  for (int i=0;i<newheight;i++) {
    QRgb* ibits = (QRgb*) img.scanLine(i);
    QRgb* obits = (QRgb*) pic.scanLine(i);
    for (int j=0;j<newwidth;j++) 
      obits[j] = qRgba(cred,cgreen,cblue,255-qRed(ibits[j+col_offset]));
  }
  pic = QGLWidget::convertToGLFormat(pic);
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
  double costhet, sinthet;
  costhet = cos(rotation*M_PI/180.0);
  sinthet = sin(rotation*M_PI/180.0);
  double xpos, ypos;
  xpos = x-radlength/2.0+xdelta*costhet-ydelta*sinthet+col_offset;
  ypos = y-radlength/2.0+xdelta*sinthet+ydelta*costhet+row_offset;
  glRasterPos2d(xpos,ypos);
  glDrawPixels(newwidth,newheight,GL_RGBA,GL_UNSIGNED_BYTE,pic.bits());
}

void GLRenderEngine::depth(bool flag) {
  if (flag)
    glEnable(GL_DEPTH_TEST);
  else
    glDisable(GL_DEPTH_TEST);
}

void GLRenderEngine::measureText(QString txt, QFont fnt, AlignmentFlag xflag, 
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

void GLRenderEngine::rect(double x1, double y1, double x2, double y2) {
  glBegin(GL_LINE_LOOP);
  glVertex2f(x1,y1);
  glVertex2f(x1,y2);
  glVertex2f(x2,y2);
  glVertex2f(x2,y1);
  glEnd();
}

void GLRenderEngine::rectFill(double x1, double y1, double x2, double y2) {
  glRectf(x1,y1,x2,y2);
}

void GLRenderEngine::tri(double x1, double y1, double z1,
			 double x2, double y2, double z2,
			 double x3, double y3, double z3) {
  glBegin(GL_TRIANGLES);
  glVertex3f(x1,y1,z1);
  glVertex3f(x2,y2,z2);
  glVertex3f(x3,y3,z3);
  glEnd();
}

void GLRenderEngine::triLine(double x1, double y1, double z1,
			     double x2, double y2, double z2,
			     double x3, double y3, double z3) {
  glBegin(GL_LINE_LOOP);
  glVertex3f(x1,y1,z1);
  glVertex3f(x2,y2,z2);
  glVertex3f(x3,y3,z3);
  glEnd();
}

void GLRenderEngine::circle(double x, double y, double radius) {
}

void GLRenderEngine::circleFill(double x, double y, double radius) {
}
  
void GLRenderEngine::drawImage(double x1, double y1, double x2,
			       double y2, QImage pic) {
  int texid = m_widget->bindTexture(pic);
  glColor3f(1,1,1);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glPushAttrib( GL_LIGHTING_BIT );
  glDisable(GL_LIGHTING);
  glBegin(GL_QUADS);
  glTexCoord2d(0,0); glVertex2f(x1,y1);
  glTexCoord2d(1,0); glVertex2f(x2,y1);
  glTexCoord2d(1,1); glVertex2f(x2,y2);
  glTexCoord2d(0,1); glVertex2f(x1,y2);
  glEnd();
  glPopAttrib();
  m_widget->deleteTexture(texid);
}

void GLRenderEngine::drawImage(HPTwoVector* xp, HPTwoVector* yp, 
			       HPTwoVector* xlim, HPTwoVector* ylim,
			       bool xflip, bool yflip,
			       QImage pic)
{ 
    float x1 = xp->Data()[0];
    float y1 = yp->Data()[0];
    float x2 = xp->Data()[1];
    float y2 = yp->Data()[1];
    float data_width = x2 - x1;
    float data_height = y2 - y1;

    if( data_width <= 0 || data_height <= 0 )
	return;

    float viewport_x = xlim->Data()[0];
    float viewport_y = ylim->Data()[0];
    float viewport_width = xlim->Data()[1]-viewport_x;
    float viewport_height = ylim->Data()[1]-viewport_y;
    
    float s1, s2, t1, t2;
    float sc_w = viewport_width / (data_width*data_width);
    float sc_h = viewport_height / (data_height*data_height);

    s1 = sc_w*(viewport_x - x1);
    t1 = sc_h*(viewport_y - y1);
    s2 = sc_w*(viewport_x - x1 + viewport_width);
    t2 = sc_h*(viewport_y - y1 + viewport_height);

    int texid = m_widget->bindTexture(pic);
    glColor3f(1,1,1);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glPushAttrib( GL_LIGHTING_BIT );
    glDisable(GL_LIGHTING);
    glBegin(GL_QUADS);
    glTexCoord2d(s1,t1); glVertex2f(x1,y1);
    glTexCoord2d(s2,t1); glVertex2f(x2,y1);
    glTexCoord2d(s2,t2); glVertex2f(x2,y2);
    glTexCoord2d(s1,t2); glVertex2f(x1,y2);
    glEnd();
    glPopAttrib();
    m_widget->deleteTexture(texid);
}

void GLRenderEngine::quadStrips(QVector<QVector<cpoint> > faces, bool flatfaces,
				QVector<QVector<cpoint> > edges, bool flatedges) {
  glDisable(GL_CULL_FACE);
  glEnable(GL_POLYGON_OFFSET_FILL);
  if (flatfaces)
    glShadeModel(GL_FLAT);
  else
    glShadeModel(GL_SMOOTH);
  glPolygonOffset(2,2);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  for (int i=0;i<faces.size();i++) {
    QVector<cpoint> qlist(faces[i]);
    glBegin(GL_QUAD_STRIP);
    for (int j=0;j<qlist.size();j++) {
      glColor4f(qlist[j].r,qlist[j].g,qlist[j].b,qlist[j].a);
      glVertex3f(qlist[j].x,qlist[j].y,qlist[j].z);
    }
    glEnd();
  }
  glDisable(GL_POLYGON_OFFSET_FILL);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  if (flatedges)
    glShadeModel(GL_FLAT);
  else
    glShadeModel(GL_SMOOTH);
  for (int i=0;i<edges.size();i++) {
    QVector<cpoint> qlist(edges[i]);
    glBegin(GL_QUAD_STRIP);
    for (int j=0;j<qlist.size();j++) {
      glColor4f(qlist[j].r,qlist[j].g,qlist[j].b,qlist[j].a);
      glVertex3f(qlist[j].x,qlist[j].y,qlist[j].z);
    }
    glEnd();
  }
}

void GLRenderEngine::drawPatch(const FaceList& faces)
{
    FaceList::const_iterator it = faces.constBegin();
    
    glDisable(GL_CULL_FACE);
    
    while( it != faces.constEnd() ){
	const Face face = *it;
	QVector<point>::const_iterator vert_it = face.vertices.constBegin();
	QVector<ColorData>::const_iterator color_it = face.vertexcolors.constBegin();
	QVector<point>::const_iterator vert_it_edge = face.vertices.constBegin();
	QVector<ColorData>::const_iterator color_it_edge = face.vertexcolors.constBegin();

	QPolygonF poly;

	glBegin(GL_TRIANGLE_FAN);

	//First we draw polygon faces
	glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(2,2);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if( face.FaceColorMode == ColorMode::Interp )
	    glShadeModel(GL_SMOOTH);
	else
	    glShadeModel(GL_FLAT);

	if( face.FaceColorMode == ColorMode::ColorSpec ){
	    glColor4f( face.FaceColor.r, face.FaceColor.g,
		face.FaceColor.b, face.FaceColor.a);
	}
	if( face.FaceColorMode == ColorMode::Flat ){
	    glColor4f( face.vertexcolors[0].r, face.vertexcolors[0].g, 
		face.vertexcolors[0].b, face.vertexcolors[0].a );
	}
	while( vert_it != face.vertices.constEnd() ){
	    point v = *vert_it;
	    if( face.FaceColorMode == ColorMode::Interp ){
		ColorData c = *color_it;
		glColor4f( c.r, c.g, c.b, c.a );
		++color_it;
	    }
	    glVertex3f( v.x, v.y, v.z );
	    ++vert_it;
	}

	//Now we draw the edges, which may be different color than the faces.
	glDisable(GL_POLYGON_OFFSET_FILL);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (face.EdgeColorMode == ColorMode::Interp)
	    glShadeModel(GL_FLAT);
	else
	    glShadeModel(GL_SMOOTH);

	if( face.EdgeColorMode == ColorMode::ColorSpec ){
	    glColor4f( face.EdgeColor.r, face.EdgeColor.g,
		face.EdgeColor.b, face.EdgeColor.a);
	}
	if( face.FaceColorMode == ColorMode::Flat ){
	    glColor4f( face.edgecolors[0].r, face.edgecolors[0].g, 
		face.edgecolors[0].b, face.edgecolors[0].a );
	}
	while( vert_it_edge != face.vertices.constEnd() ){
	    point v = *vert_it_edge;
	    if( face.EdgeColorMode == ColorMode::Interp ){
		ColorData c = *color_it_edge;
		glColor4f( c.r, c.g, c.b, c.a );
		++color_it_edge;
	    }
	    glVertex3f( v.x, v.y, v.z );
	    ++vert_it_edge;
	}
	glEnd();
	++it;
    }
}

