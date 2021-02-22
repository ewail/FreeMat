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
#include "HandleWindow.hpp"
#include "HandleAxis.hpp"
#include "HandleCommands.hpp"
#include <qgl.h>
#include "GLRenderEngine.hpp"
#include "QTRenderEngine.hpp"
#include "HandleCommands.hpp"
#include <QtGui>
#include <math.h>
#include "IEEEFP.hpp"
#include "DebugStream.hpp"


QSize BaseFigureQt::sizeHint() const {
  HPTwoVector *htv = (HPTwoVector*) hfig->LookupProperty("figsize");
  return QSize((int)(htv->Data()[0]),(int)(htv->Data()[1]));
  //  return QSize(10000,10000);
}

void BaseFigureQt::resizeEvent(QResizeEvent *e) {
  QWidget::resizeEvent(e);
  hfig->Resize(qMax(8,e->size().width()),qMax(8,e->size().height()));
}

static bool enableRepaint = false;

void GfxEnableRepaint() {
  enableRepaint = true;
}

void GfxDisableRepaint() {
  enableRepaint = false;
}

bool GfxEnableFlag() {
  return enableRepaint;
}

void BaseFigureQt::paintEvent(QPaintEvent *e) {
  if (enableRepaint) {
    QPainter pnt(this);
    QTRenderEngine gc(&pnt,0,0,width(),height());
    hfig->PaintMe(gc);
  }
}

BaseFigureQt::BaseFigureQt(QWidget *parent, HandleFigure *fig) : 
  QWidget(parent) {
  hfig = fig;
  setMouseTracking(true);
  //  hfig->resizeGL(width(),height());
}

BaseFigureGL::BaseFigureGL(QWidget *parent, HandleFigure *fig) : 
  QGLWidget(parent) {
  hfig = fig;
  setMouseTracking(true);
  //hfig->resizeGL(width(),height());
}
  
void BaseFigureGL::initializeGL() {
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glEnable(GL_TEXTURE_2D);
}
  
void BaseFigureGL::paintGL() {
  GLRenderEngine gc(this,0,0,width(),height());
  hfig->PaintMe(gc);
}

void BaseFigureGL::resizeGL(int width, int height) {
  //    qDebug("GLsize");
  //  hfig->resizeGL(width,height);
  hfig->Resize(qMax(8,width),qMax(8,height));
}

QSize BaseFigureGL::sizeHint() const {
    HPTwoVector *htv = (HPTwoVector*) hfig->LookupProperty("figsize");
    return QSize((int)(htv->Data()[0]),(int)(htv->Data()[1]));
  }

#if 0
void BaseFigureGL::mousePressEvent(QMouseEvent* e) {
  if (e->button() & Qt::LeftButton)
    elevazim = true;
  else
    elevazim = false;
  beginx = e->x();
  beginy = e->y();
}

void BaseFigureGL::mouseMoveEvent(QMouseEvent* e) {
  if (elevazim) {
    elev -= (e->y() - beginy);
    azim += (e->x() - beginx);
    elev = (elev + 360) % 360;
    azim = (azim + 360) % 360;
  } else {
    arot += (e->y() - beginy);
    arot = (arot + 360) % 360;
  }
  beginx = e->x();
  beginy = e->y();
  //    updateGL();
}

void BaseFigureGL::mouseReleaseEvent(QMouseEvent* e) {
}
#endif

void HandleWindow::closeEvent(QCloseEvent* e) {
  NotifyFigureClosed(handle);
}

void HandleWindow::redraw() {
  child->update();
  //  child->updateGL();
}
  
bool HandleWindow::event(QEvent* e) {
  if (e->type() == QEvent::WindowActivate) {
    NotifyFigureActive(handle);
  }

  return QWidget::event(e);
}

HandleWindow::HandleWindow(unsigned ahandle, Interpreter *eval) : QMainWindow(),m_eval(eval) {
  initialized = false;
  setWindowIcon(QPixmap(":/images/freemat_figure_small_mod_64.png"));
  handle = ahandle;
  hfig = new HandleFigure(this);
  char buffer[1000];
  sprintf(buffer,"Figure %d",ahandle+1);
  setWindowTitle(buffer);
  child = new BaseFigureQt(this,hfig);
  //child = new BaseFigureGL(this,hfig);
  band = NULL;
  zoom_active = false;
  setMinimumSize(50,50);
  createActions();
  createMenus();
  createToolBars();
  setCentralWidget(child);
  setMouseTracking(true);
  resize(600,400);
  initialized = true;
  mode = normal_mode;
}

// Useful tools:
//
// Zoom in - fix for images
// Zoom out - fix for images
// Pan - fix for images
// Rotate - fix for imagesqw
// Annotate
// Save
// Copy
// Examine (values)
//

void HandleWindow::zoom(bool active) {
  if( mode == click_mode ){
    zoomAct->setChecked(false);
    return;
  }
  panAct->setChecked(false);
  rotateAct->setChecked(false);
  camRotateAct->setChecked(false);
  pointSampleAct->setChecked(false);
  if (active)
    mode = zoom_mode;
  else
    mode = normal_mode;
}

void HandleWindow::pan(bool active) {
  if( mode == click_mode ){
    panAct->setChecked(false);
    return;
  }
  zoomAct->setChecked(false);
  rotateAct->setChecked(false);
  camRotateAct->setChecked(false);
  pointSampleAct->setChecked(false);
  if (active){
    mode = pan_mode;
    setCursor( Qt::OpenHandCursor );
  }
  else{
    mode = normal_mode;
    setCursor( Qt::ArrowCursor );
  }
}

void HandleWindow::rotate(bool active) {
  if( mode == click_mode ){
    rotateAct->setChecked(false);
    return;
  }
  zoomAct->setChecked(false);
  panAct->setChecked(false);
  camRotateAct->setChecked(false);
  pointSampleAct->setChecked(false);
  if (active){
    mode = rotate_mode;
    setCursor( Qt::OpenHandCursor );
  }
  else{
    mode = normal_mode;
    setCursor( Qt::ArrowCursor );
  }
}

void HandleWindow::camRotate(bool active) {
  if( mode == click_mode ){
    camRotateAct->setChecked(false);
    return;
  }
  zoomAct->setChecked(false);
  panAct->setChecked(false);
  rotateAct->setChecked(false);
  pointSampleAct->setChecked(false);
  if (active){
    mode = cam_rotate_mode;
    setCursor( Qt::OpenHandCursor );
  }
  else{
    mode = normal_mode;
    setCursor( Qt::ArrowCursor );
  }
}

void HandleWindow::pointSample(bool active) {
  if( mode == click_mode ){
    pointSampleAct->setChecked(false);
    return;
  }
  zoomAct->setChecked(false);
  panAct->setChecked(false);
  rotateAct->setChecked(false);
  camRotateAct->setChecked(false);
  if (active){
    //QApplication::setOverrideCursor(Qt::CrossCursor);
    //mode = point_sample_mode;
	m_eval->ExecuteLine(QString("datacursormode('on');") + "\n");
  }
  else{
    //QApplication::restoreOverrideCursor();
    //mode = normal_mode;
  }
  pointSampleAct->setChecked(false);
}

void HandleWindow::save() {
  QString Filters, SelectedFilter;
  
  /* Find all supported bitmap formats */
  QList<QByteArray> formats(QImageWriter::supportedImageFormats());
  for (int i=0;i<formats.count();i++) {
    Filters = Filters + QString(formats.at(i).data()).toUpper() + " (*." + QString(formats.at(i).data()) + ");;";
  }
  
  /* Add vector formats */
  Filters = Filters + "PDF (*.pdf);;PS (*.ps);;EPS (*.eps);;SVG (*.svg);;All files (*)";
   
  QString fn = QFileDialog::getSaveFileName (0, "Save Figure As", QString(), Filters, &SelectedFilter);
  if (fn.isEmpty()) return;

  /* Add an extension to file name if not provided*/
  if (!fn.contains('.')) { 
    int pos = SelectedFilter.indexOf('*');
    fn = fn + SelectedFilter.mid(pos+1,SelectedFilter.size()-pos-2);
  }

  try {
    HPrintFunction(0,ArrayVector(Array(fn)));
  } catch(Exception &e) {
    QMessageBox::critical(0,"Critical Error in Save",e.msg());
  }
}

void HandleWindow::copy() {
  try {
    HCopyFunction(0,ArrayVector());
  } catch(Exception &e) {
    QMessageBox::critical(0,"Critical Error in Copy",e.msg());
  }
}

void HandleWindow::createActions() {
  zoomAct = new QAction(QIcon(":/images/zoomin.png"),"&Zoom",this);
  zoomAct->setCheckable(true);
  zoomAct->setToolTip("Left click to zoom in by 2, left click and drag to zoom a region, right click to zoom out by 2");
  connect(zoomAct,SIGNAL(triggered(bool)),this,SLOT(zoom(bool)));
  panAct = new QAction(QIcon(":/images/pan.png"),"&Pan",this);
  connect(panAct,SIGNAL(triggered(bool)),this,SLOT(pan(bool)));
  panAct->setCheckable(true);
  rotateAct = new QAction(QIcon(":/images/rotate.png"),"&Rotate",this);
  connect(rotateAct,SIGNAL(triggered(bool)),this,SLOT(rotate(bool)));
  rotateAct->setCheckable(true);
  camRotateAct = new QAction(QIcon(":/images/cam_rotate.png"),"&Camera Rotate",this);
  connect(camRotateAct,SIGNAL(triggered(bool)),this,SLOT(camRotate(bool)));
  camRotateAct->setCheckable(true);

  pointSampleAct = new QAction( QIcon(":/images/cursor-cross.png"),"Sam&ple", this);
  connect(pointSampleAct,SIGNAL(triggered(bool)),this,SLOT(pointSample(bool)));
  pointSampleAct->setCheckable(true);

  saveAct = new QAction(QIcon(":/images/save.png"),"&Save",this);
  connect(saveAct,SIGNAL(triggered()),this,SLOT(save()));
  copyAct = new QAction(QIcon(":/images/copy.png"),"&Copy",this);
  connect(copyAct,SIGNAL(triggered()),this,SLOT(copy()));
  closeAct = new QAction(QIcon(":/images/quit.png"),"&Close",this);
  connect(closeAct,SIGNAL(triggered()),this,SLOT(close()));
}

void HandleWindow::createMenus() {
  fileMenu = menuBar()->addMenu("&File");
  fileMenu->addAction(saveAct);
  fileMenu->addAction(closeAct);
  editMenu = menuBar()->addMenu("&Tools");
  editMenu->addAction(copyAct);
  editMenu->addAction(zoomAct);
  editMenu->addAction(panAct);
  editMenu->addAction(rotateAct);
  editMenu->addAction(camRotateAct);
  editMenu->addAction(pointSampleAct);
}

void HandleWindow::createToolBars() {
  toolBar = addToolBar("Tools");
  toolBar->setObjectName("ToolsToolBar");
  toolBar->addAction(saveAct);
  toolBar->addAction(closeAct);
  toolBar->addAction(copyAct);
  toolBar->addAction(zoomAct);
  toolBar->addAction(panAct);
  toolBar->addAction(rotateAct);
  toolBar->addAction(camRotateAct);
  toolBar->addAction(pointSampleAct);
}

unsigned HandleWindow::Handle() {
  return handle;
}

HandleFigure* HandleWindow::HFig() {
  return hfig;
}

void HandleWindow::GetClick(int &x, int &y) {
  // Set the cross cursor
  QApplication::setOverrideCursor(Qt::CrossCursor);
  // Run the event loop
  int save_mode = mode;
  mode = click_mode;
  hfig->markDirty();
  m_loop.exec();
  x = click_x;
  y = click_y;
  QApplication::restoreOverrideCursor();
  mode = save_mode;
}


HandleAxis* GetContainingAxis(HandleFigure *fig, int x, int y) {
  HPHandles *cp = (HPHandles*) fig->LookupProperty("children");
  QVector<unsigned> children(cp->Data());
  for (int i=0;i<children.size();i++) {
    HandleObject* hp = LookupHandleObject(children[i]);
    if (hp->IsType("axes")) {
      // Get the axis extents
      QVector<double> position(((HandleAxis*) hp)->GetClientAreaInPixels());
      if (position.size() != 4) return NULL;
      if ((x >= position[0]) && (x < (position[0]+position[2])) &&
      	  (y >= position[1]) && (y < (position[1]+position[3])))
      	return (HandleAxis*)hp;
    }
  }
  return NULL;
}

void HandleWindow::mousePressEvent(QMouseEvent* e) {
  try {
    if (mode == click_mode) {
      click_x = e->x() - centralWidget()->geometry().x();
      click_y = e->y() - centralWidget()->geometry().y();
      m_loop.exit();
    }
    if ((mode == zoom_mode) && (e->button() == Qt::LeftButton))  {
      origin = e->pos();
      if (!band)
	band = new QRubberBand(QRubberBand::Rectangle, this);
      band->setGeometry(QRect(origin,QSize()));
      band->show();
      zoom_active = true;
    } else
      zoom_active = false;
    if (mode == pan_mode) {
      origin = e->pos();
      QRect plot_region(child->geometry());
      HandleAxis *h = GetContainingAxis(hfig,remapX(e->x()),remapY(e->y()));
      if (h) {
	HPTwoVector *hp = (HPTwoVector*) h->LookupProperty("xlim");
	pan_xrange = (hp->Data()[1] - hp->Data()[0]);
	pan_xmean = (hp->Data()[1] + hp->Data()[0])/2;
	hp = (HPTwoVector*) h->LookupProperty("ylim");
	pan_yrange = (hp->Data()[1] - hp->Data()[0]);
	pan_ymean = (hp->Data()[1] + hp->Data()[0])/2;
	pan_active = true;
    } else {
	pan_active = false;
      }
    }
    if ((mode == rotate_mode) || (mode == cam_rotate_mode)) {
      origin = e->pos();
      QRect plot_region(child->geometry());
      HandleAxis *h = GetContainingAxis(hfig,remapX(e->x()),remapY(e->y()));
      if (h) {
	rotate_active = true;
	rotate_camera = ((HPThreeVector*) h->LookupProperty("cameraposition"))->Data();
	rotate_up = ((HPThreeVector*) h->LookupProperty("cameraupvector"))->Data();
	rotate_target = ((HPThreeVector*) h->LookupProperty("cameratarget"))->Data();
	rotate_forward = rotate_target;
	rotate_forward[0] -= rotate_camera[0];
	rotate_forward[1] -= rotate_camera[1];
	rotate_forward[2] -= rotate_camera[2];
	rotate_source_cam_dist = sqrt(rotate_forward[0]*rotate_forward[0] + 
				      rotate_forward[1]*rotate_forward[1] + 
				      rotate_forward[2]*rotate_forward[2]);
	rotate_forward[0] /= rotate_source_cam_dist;
	rotate_forward[1] /= rotate_source_cam_dist;
	rotate_forward[2] /= rotate_source_cam_dist;
	rotate_right = rotate_up;
	rotate_right[0] = (rotate_forward[1] * rotate_up[2]) - (rotate_forward[2] * rotate_up[1]);
	rotate_right[1] = (rotate_forward[2] * rotate_up[0]) - (rotate_forward[0] * rotate_up[2]);
	rotate_right[2] = (rotate_forward[0] * rotate_up[1]) - (rotate_forward[1] * rotate_up[0]);
      }
    }
    if ((mode == zoom_mode) && (e->button() == Qt::RightButton))  {
	origin = e->pos();
	QRect plot_region(child->geometry());
	HandleAxis *h = GetContainingAxis(hfig,remapX(e->x()),remapY(e->y()));
	if( h ){
	    dbout << remapX(e->x()) << ":" << remapY(e->y()) << "\n";
	}

    }

  } catch (Exception &e) {
  }
}

void HandleWindow::mouseMoveEvent(QMouseEvent* e) {
    e->accept();
  try {
    if ((mode == zoom_mode) && zoom_active)
      band->setGeometry(QRect(origin, e->pos()).normalized());
    if ((mode == pan_mode) && pan_active) {
      QPoint dest(e->pos());
      QRect plot_region(child->geometry());
      HandleAxis *h = GetContainingAxis(hfig,remapX(origin.x()),remapY(origin.y()));
      if (h) {
	QVector<double> position(h->GetClientAreaInPixels());
	double delx, dely;
	if (h->StringCheck("xdir","reverse"))
	  delx = (e->x() - origin.x())/position[2];
	else
	  delx = -(e->x() - origin.x())/position[2];
	if (h->StringCheck("ydir","reverse"))
	  dely = -(e->y() - origin.y())/position[3];
	else
	  dely = (e->y() - origin.y())/position[3];
	h->SetTwoVectorDefault("xlim",pan_xmean+pan_xrange*delx-pan_xrange/2,
			       pan_xmean+pan_xrange*delx+pan_xrange/2);
	h->SetConstrainedStringDefault("xlimmode","manual");
	h->SetTwoVectorDefault("ylim",pan_ymean+pan_yrange*dely-pan_yrange/2,
			       pan_ymean+pan_yrange*dely+pan_yrange/2);
	h->SetConstrainedStringDefault("ylimmode","manual");
	//	hfig->Repaint();
	hfig->markDirty();
      }
      e->accept();
    }
    if ((mode == rotate_mode) && rotate_active) {
      QPoint dest(e->pos());
      QRect plot_region(child->geometry());
      HandleAxis *h = GetContainingAxis(hfig,remapX(origin.x()),remapY(origin.y()));
      if (h) {
	double az = (e->x() - origin.x())/180.0*M_PI;
	double el = (e->y() - origin.y())/180.0*M_PI;
	// The delx means we rotate the camera 
	QVector<double> camera_position(rotate_target);
	camera_position[0] += rotate_source_cam_dist*(cos(el)*sin(az)*rotate_right[0] + 
						      -cos(el)*cos(az)*rotate_forward[0] + 
						      sin(el)*rotate_up[0]);
	camera_position[1] += rotate_source_cam_dist*(cos(el)*sin(az)*rotate_right[1] + 
						      -cos(el)*cos(az)*rotate_forward[1] + 
						      sin(el)*rotate_up[1]);
	camera_position[2] += rotate_source_cam_dist*(cos(el)*sin(az)*rotate_right[2] + 
						      -cos(el)*cos(az)*rotate_forward[2] + 
						      sin(el)*rotate_up[2]);
	QVector<double> camera_up(rotate_target);
	camera_up[0] = (cos(el+M_PI/2.0)*sin(az)*rotate_right[0] + 
			-cos(el+M_PI/2.0)*cos(az)*rotate_forward[0] + 
			sin(el+M_PI/2.0)*rotate_up[0]);
	camera_up[1] = (cos(el+M_PI/2.0)*sin(az)*rotate_right[1] + 
			-cos(el+M_PI/2.0)*cos(az)*rotate_forward[1] + 
			sin(el+M_PI/2.0)*rotate_up[1]);
	camera_up[2] = (cos(el+M_PI/2.0)*sin(az)*rotate_right[2] + 
			-cos(el+M_PI/2.0)*cos(az)*rotate_forward[2] + 
			sin(el+M_PI/2.0)*rotate_up[2]);
	h->SetThreeVectorDefault("cameraposition",camera_position[0],
				 camera_position[1],camera_position[2]);
	h->SetConstrainedStringDefault("camerapositionmode","manual");
	h->SetThreeVectorDefault("cameraupvector",camera_up[0],
				 camera_up[1],camera_up[2]);
	h->SetConstrainedStringDefault("cameraupvectormode","manual");
	//	h->UpdateState();
	//	hfig->Repaint();
	hfig->markDirty();
	//	UpdateState();
      }
    }
    if ((mode == cam_rotate_mode) && rotate_active) {
      QPoint dest(e->pos());
      QRect plot_region(child->geometry());
      HandleAxis *h = GetContainingAxis(hfig,remapX(origin.x()),remapY(origin.y()));
      if (h) {
	double el = (e->y() - origin.y())/180.0*M_PI;
	QVector<double> camera_up(rotate_target);
	camera_up[0] = cos(el)*rotate_up[0] - sin(el)*rotate_right[0];
	camera_up[1] = cos(el)*rotate_up[1] - sin(el)*rotate_right[1];
	camera_up[2] = cos(el)*rotate_up[2] - sin(el)*rotate_right[2];
	h->SetThreeVectorDefault("cameraupvector",camera_up[0],
				 camera_up[1],camera_up[2]);
	h->SetConstrainedStringDefault("cameraupvectormode","manual");
	//	h->UpdateState();
	//	hfig->Repaint();
	//	UpdateState();
	hfig->markDirty();
      }
    }
  } catch (Exception &e) {
  }
}

int HandleWindow::remapX(int x) {
  QRect plot_region(child->geometry());
  return x - plot_region.x();
}

int HandleWindow::remapY(int y) {
  QRect plot_region(child->geometry());
  return (plot_region.height()-y+plot_region.y());
}

void HandleWindow::mouseReleaseEvent(QMouseEvent * e) {
  try {
   // if (mode == point_sample_mode ) {
	  //int click_x, click_y;
	  //click_x = e->x();
	  //click_y = e->y();
	  //HandleAxis *h = GetContainingAxis(hfig,remapX(click_x),remapY(click_y));
	  //if( h ){
	  //    int remap_x = remapX(click_x);
	  //    int remap_y = remapY( click_y);
	  //    dbout << "raw: ( " << click_x << ", " << click_y << " )\n";
	  //    dbout << "remap: ( " << remap_x << ", " << remap_y << " )\n";
	  //    
	  //}
   // }
    if (mode == pan_mode){
      pan_active = false;
    }
    if (mode == rotate_mode){
      rotate_active = false;
    }
    if (mode == cam_rotate_mode){
      rotate_active = false;
    }
    
    if (mode == zoom_mode) {
      if (zoom_active) {
	band->hide();
	QRect rect(band->geometry().normalized());
	if ((rect.width()*rect.height()) < 20) {
	  // Treat as a click
	  int click_x, click_y;
	  click_x = e->x();
	  click_y = e->y();
	  HandleAxis *h = GetContainingAxis(hfig,remapX(click_x),remapY(click_y));
	  if (h) {
	    HPTwoVector *hp = (HPTwoVector*) h->LookupProperty("xlim");
	    double range = (hp->Data()[1] - hp->Data()[0])/2;
	    double mean = (hp->Data()[1] + hp->Data()[0])/2;
	    h->SetTwoVectorDefault("xlim",mean-range/2,mean+range/2);
	    h->SetConstrainedStringDefault("xlimmode","manual");
	    hp = (HPTwoVector*) h->LookupProperty("ylim");
	    range = (hp->Data()[1] - hp->Data()[0])/2;
	    mean = (hp->Data()[1] + hp->Data()[0])/2;
	    h->SetTwoVectorDefault("ylim",mean-range/2,mean+range/2);
	    h->SetConstrainedStringDefault("ylimmode","manual");
	    //	    h->UpdateState();
	    //	    hfig->Repaint();
	  }
	} else {       
	  QRect plot_region(child->geometry());
	  HandleAxis *h = GetContainingAxis(hfig,remapX(rect.x()),remapY(rect.y()));
	  if (h) {

 	    QVector<double> position(h->GetClientAreaInPixels());
	    // 	    QVector<double> pba(h->VectorPropertyLookup("plotboxaspectratio"));
	    // 	    QVector<double> dar(h->VectorPropertyLookup("dataaspectratio"));
 	    double x = position[0], y = position[1];
 	    double width = position[2], height = position[3];
	    
	    // 	    double xyratio = 1;
	    // 	    if( h->IsAuto("plotboxaspectratiomode") ) 
	    // 		xyratio *= pba[0]/pba[1];
	    // 	    if( h->IsAuto("dataaspectratiomode") )
	    // 		xyratio *= dar[0]/dar[1];
	    // 	    if( h->IsAuto("plotboxaspectratiomode") || h->IsAuto("dataaspectratiomode") ){
	    // 		if( width > height*xyratio ){
	    // 		    width = height*xyratio;
	    // 		    x += (position[2]-width)/2;
	    // 		}
	    // 		else{
	    // 		    height = width / xyratio;
	    // 		    y += (position[3]-height)/2;
	    // 		}
	    // 	    }
	    
	    double xminfrac = (remapX(rect.x()) - x)/width;
	    double xmaxfrac = (remapX(rect.x()+rect.width()) - x)/width;
	    double yminfrac = (remapY(rect.y()+rect.height()) - y)/height;
	    double ymaxfrac = (remapY(rect.y()) - y)/height;
	    xminfrac = qMax(0.,qMin(1.,xminfrac));
	    xmaxfrac = qMax(0.,qMin(1.,xmaxfrac));
	    yminfrac = qMax(0.,qMin(1.,yminfrac));
	    ymaxfrac = qMax(0.,qMin(1.,ymaxfrac));
	    if (h->StringCheck("ydir","reverse")) {
	      double y1 = 1-yminfrac;
	      double y2 = 1-ymaxfrac;
	      yminfrac = y2;
	      ymaxfrac = y1;
	    }
	    if (h->StringCheck("xdir","reverse")) {
	      double x1 = 1-xminfrac;
	      double x2 = 1-xmaxfrac;
	      xminfrac = x2;
	      xmaxfrac = x1;
	    }
	    HPTwoVector *hp = (HPTwoVector*) h->LookupProperty("xlim");
	    double range = (hp->Data()[1] - hp->Data()[0]);
	    double mean = (hp->Data()[1] + hp->Data()[0])/2;
	    h->SetTwoVectorDefault("xlim",mean-range/2+xminfrac*range,mean-range/2+xmaxfrac*range);
	    h->SetConstrainedStringDefault("xlimmode","manual");
	    hp = (HPTwoVector*) h->LookupProperty("ylim");
	    range = (hp->Data()[1] - hp->Data()[0]);
	    mean = (hp->Data()[1] + hp->Data()[0])/2;
	    h->SetTwoVectorDefault("ylim",mean-range/2+yminfrac*range,mean-range/2+ymaxfrac*range);
	    h->SetConstrainedStringDefault("ylimmode","manual");
	    //	    h->UpdateState();
	    //	    hfig->Repaint();
	  }
	}
      } else {
	// Treat as a click
	int click_x, click_y;
	click_x = e->x();
	click_y = e->y();
	QRect plot_region(child->geometry());
	HandleAxis *h = GetContainingAxis(hfig,remapX(click_x),remapY(click_y));
	if (h) {
	  HPTwoVector *hp = (HPTwoVector*) h->LookupProperty("xlim");
	  double range = (hp->Data()[1] - hp->Data()[0])*2;
	  double mean = (hp->Data()[1] + hp->Data()[0])/2;
	  h->SetTwoVectorDefault("xlim",mean-range/2,mean+range/2);
	  h->SetConstrainedStringDefault("xlimmode","manual");
	  hp = (HPTwoVector*) h->LookupProperty("ylim");
	  range = (hp->Data()[1] - hp->Data()[0])*2;
	  mean = (hp->Data()[1] + hp->Data()[0])/2;
	  h->SetTwoVectorDefault("ylim",mean-range/2,mean+range/2);
	  h->SetConstrainedStringDefault("ylimmode","manual");
	  //	  h->UpdateState();
	  //	  hfig->Repaint();
	}
      }
    }
  } catch (Exception &e) {
  }
  hfig->markDirty();
}

// void HandleWindow::UpdateState() {
//   if (!initialized) return;
//   if (dirty) return;
//   dirty = true;
//   //   HPTwoVector *htv = (HPTwoVector*) hfig->LookupProperty("figsize");
//   //   child->resize((int)(htv->Data()[0]),(int)(htv->Data()[1]));
//   //   child->updateGeometry();
//   //   adjustSize();
//   //   if (hfig->StringCheck("renderer","opengl") && (QGLFormat::hasOpenGL())) {
//   //       if ( QString("QGLWidget").compare(child->metaObject()->className() ) != 0 ) {
//   // 	  BaseFigureGL* newchild = new BaseFigureGL(this,hfig);
//   // 	  //layout->setCurrentWidget(child);
//   // 	  setCentralWidget(newchild); //deletes old child
//   // 	  child = newchild;
//   //           //child->show();
//   //           child->updateGeometry();
//   //           //repaint();
//   //           //UpdateState();
//   //        }
//   //      } else if (hfig->StringCheck("renderer","painters")) {
//   //        if (QString("QWidget").compare(child->metaObject()->className()) != 0 ) {
//   // 	   BaseFigureQt* newchild = new BaseFigureQt(this,hfig);
//   // 	   //layout->setCurrentWidget(child);
//   // 	   setCentralWidget(newchild); //deletes old child
//   // 	   child = newchild;
//   //            //child->show();
//   //            child->updateGeometry();
//   // 	   //UpdateState();
//   //        }
//   //   }
//   //  update();
// }
