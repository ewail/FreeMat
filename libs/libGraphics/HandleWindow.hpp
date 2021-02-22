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
#ifndef __HandleWindow_hpp__
#define __HandleWindow_hpp__

#include "Interpreter.hpp"
#include <QWidget>
#include "HandleFigure.hpp"
#include <QStackedWidget>
#include <QTabWidget>
#include <QObject>
#include <QGLWidget>
#include <QEventLoop>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QMainWindow>
#include <QActionGroup>
#include <QRubberBand>
#include <QtGui>

const int normal_mode = 1;
const int point_mode = 2;
const int zoom_mode = 3;
const int pan_mode = 4;
const int click_mode = 5;
const int rotate_mode = 6;
const int cam_rotate_mode = 7;
const int point_sample_mode = 8;

class HandleWindow : public QMainWindow {
  Q_OBJECT

private:
  Interpreter *m_eval;

protected:
  bool initialized;
  unsigned handle;
  bool glActive;
  //QGLWidget *glchild;
  QWidget *child;
  HandleFigure *hfig;
  //QStackedWidget *layout;
  QToolBar *toolBar;
  QAction *zoomAct, *panAct, *rotateAct, *camRotateAct, *pointSampleAct;
  QAction *saveAct, *copyAct, *closeAct;
  QActionGroup *toolGroup;
  QMenu *fileMenu, *editMenu;
  //  QTabWidget *layout;
  QEventLoop m_loop;
  int click_x, click_y;
  int mode;
  QRubberBand *band;
  QPoint origin;
  bool pan_active;
  double pan_xrange, pan_xmean;
  double pan_yrange, pan_ymean;
  bool zoom_active;
  bool rotate_active;
  QVector<double> rotate_up, rotate_target, rotate_camera, rotate_forward, rotate_right;
  double rotate_source_cam_dist;

  int remapX(int x);
  int remapY(int y);
public:
  QWidget *GetQtWidget() {return child;}
  HandleWindow(unsigned ahandle, Interpreter *eval);
  ~HandleWindow() {delete hfig;}
  unsigned Handle();
  HandleFigure *HFig();
  void closeEvent(QCloseEvent* e);
  bool event(QEvent* e);
  void GetClick(int &x, int &y);
  void mousePressEvent(QMouseEvent* e);
  void mouseMoveEvent(QMouseEvent* e);
  void mouseReleaseEvent(QMouseEvent* e);
  void createActions();
  void createMenus();
  void createToolBars();
  void redraw();
		       

public slots:
  void zoom(bool);
  void pan(bool);
  void rotate(bool);
  void camRotate(bool);
  void pointSample(bool);
  void save();
  void copy();
};


class BaseFigureQt : public QWidget {
  Q_OBJECT
  HandleFigure *hfig;
  QPixmap backStore;
public:
  BaseFigureQt(QWidget *parent, HandleFigure *fig);
  void paintEvent(QPaintEvent *e);
  void resizeEvent(QResizeEvent *e);
  QSize sizeHint() const;
  //  QSizePolicy sizePolicy() const;

};

class BaseFigureGL : public QGLWidget {
  Q_OBJECT
  HandleFigure *hfig;
public:
  BaseFigureGL(QWidget *parent, HandleFigure *fig);
  virtual void initializeGL();
  virtual void paintGL();
  virtual void resizeGL(int width, int height);
  QSize sizeHint() const;
  // Support dragging...
  //   void mousePressEvent(QMouseEvent* e);
  //   void mouseMoveEvent(QMouseEvent* e);
  //   void mouseReleaseEvent(QMouseEvent* e);
  //  virtual void Show() {QWidget::show();};
};


void GfxEnableRepaint();
void GfxDisableRepaint();
bool GfxEnableFlag();

#endif
