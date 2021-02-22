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
#include "HandleCommands.hpp"
#include "HandleFigure.hpp"

#include "Parser.hpp"
#include "Scanner.hpp"
#include "Token.hpp"

#include <qgl.h>
#include <QtGui>
#include <QtSvg>
#include <ctype.h>
#include <algorithm>
#include "HandleLineSeries.hpp"
#include "HandleObject.hpp"
#include "HandleText.hpp"
#include "HandleAxis.hpp"
#include "HandleImage.hpp"
#include <qapplication.h>
#include "HandleList.hpp"
#include "HandleSurface.hpp"
#include "HandlePatch.hpp"
#include "HandleWindow.hpp"
#include "HandleContour.hpp"
#include "HandleUIControl.hpp"
#include "QTRenderEngine.hpp"
#include "Interpreter.hpp"
#include <math.h>

// Subplot
// labels don't always appear properly.
// linestyle/symbol specification
// images

QWidget *save = NULL;

void SaveFocus() {
  save = qApp->focusWidget();
}
  
void RestoreFocus() {
  if (save)
    save->setFocus();
}

HandleWindow* Hfigs[MAX_FIGS];
int HcurrentFig = -1;

static bool HGInitialized = false;

void InitializeHandleGraphics() {
  if (HGInitialized) return;
  for (int i=0;i<MAX_FIGS;i++) Hfigs[i] = NULL;
  HcurrentFig = -1;
  HGInitialized = true;
}

void ShutdownHandleGraphics() {
  for (int i=0;i<MAX_FIGS;i++) {
    if  (Hfigs[i]) {
      Hfigs[i]->hide();
      delete Hfigs[i];
    }
    Hfigs[i] = NULL;
  }
  HcurrentFig = -1;
}

// Magic constant - limits the number of figures you can have...
  
HandleList<HandleObject*> objectset;

void NotifyFigureClosed(unsigned figNum) {
  //    delete Hfigs[figNum];
  Hfigs[figNum] = NULL;
  if (((int)figNum) == HcurrentFig)
    HcurrentFig = -1;
  // Check for all figures closed
  bool allClosed = true;
  for (int i=0;allClosed && i<MAX_FIGS;i++)
    allClosed = Hfigs[i] == NULL;
}

void NotifyFigureActive(unsigned figNum) {
  HcurrentFig = figNum;
}

static void NewFig(Interpreter *eval) {
  // First search for an unused fig number
  int figNum = 0;
  bool figFree = false;
  while ((figNum < MAX_FIGS) && !figFree) {
    figFree = (Hfigs[figNum] == NULL);
    if (!figFree) figNum++;
  }
  if (!figFree) {
    throw Exception("No more fig handles available!  Close some figs...");
  }
  Hfigs[figNum] = new HandleWindow(figNum, eval);
  SaveFocus();
  Hfigs[figNum]->show();
  RestoreFocus();
  HcurrentFig = figNum;
}

static HandleWindow* CurrentWindow(Interpreter *eval) {
  if (HcurrentFig == -1)
    NewFig( eval );
  return (Hfigs[HcurrentFig]);
}

static HandleFigure* CurrentFig(Interpreter *eval) {
  if (HcurrentFig == -1)
    NewFig(eval);
  return (Hfigs[HcurrentFig]->HFig());
}

static void SelectFigNoCreate(int fignum) {
  if (fignum < 0)
    throw Exception("Illegal argument to SelectFigNoCreate");
  if (Hfigs[fignum] == NULL)
    throw Exception("Non-existent figure in SelectFigNoCreate");
  SaveFocus();
  Hfigs[fignum]->show();
  Hfigs[fignum]->raise();
  RestoreFocus();
  HcurrentFig = fignum;
}


static void SelectFig(int fignum, Interpreter *eval) {
  if (fignum < 0)
    throw Exception("Illegal argument to SelectFig");
  if (Hfigs[fignum] == NULL)
    Hfigs[fignum] = new HandleWindow(fignum, eval);
  SaveFocus();
  Hfigs[fignum]->show();
  Hfigs[fignum]->raise();
  RestoreFocus();
  HcurrentFig = fignum;
}


bool AnyDirty() {
  bool retval = false;
  if (!HGInitialized) return false;
  for (int i=0;i<MAX_FIGS;i++) 
    if (Hfigs[i] && (Hfigs[i]->HFig()->isDirty()))  
      retval = true;
  return retval;
}

void RefreshFigs() {
  if (!GfxEnableFlag()) return;
  if (!HGInitialized) return;
  for (int i=0;i<MAX_FIGS;i++) {
    if (Hfigs[i] && (Hfigs[i]->HFig()->isDirty())) {
      Hfigs[i]->redraw();
    }
  }
}

static bool in_DoDrawNow = false;

static void DoDrawNow() {
  if (in_DoDrawNow) return;
  in_DoDrawNow = true;
  while (AnyDirty())
    qApp->processEvents(QEventLoop::AllEvents, 50);
  in_DoDrawNow = false;
}

//@@Signature
//gfunction drawnow DrawNowFunction
//input none
//output none
//DOCBLOCK handle_drawnow
ArrayVector DrawNowFunction(int nargout, const ArrayVector& arg) {
  GfxEnableRepaint();
  DoDrawNow();
  GfxDisableRepaint();
  return ArrayVector();
}

HandleObject* LookupHandleObject(unsigned handle) {
  return (objectset.lookupHandle(handle-HANDLE_OFFSET_OBJECT));
}


HandleFigure* LookupHandleFigureNoCreate(unsigned handle) {
  if (Hfigs[handle-HANDLE_OFFSET_FIGURE] != NULL)
    return Hfigs[handle-HANDLE_OFFSET_FIGURE]->HFig();
  else {
    SelectFigNoCreate(handle-HANDLE_OFFSET_FIGURE);
    return Hfigs[handle-HANDLE_OFFSET_FIGURE]->HFig();
  }
}

HandleFigure* LookupHandleFigure(unsigned handle, Interpreter *eval) {
  if (Hfigs[handle-HANDLE_OFFSET_FIGURE] != NULL)
    return Hfigs[handle-HANDLE_OFFSET_FIGURE]->HFig();
  else {
    SelectFig(handle-HANDLE_OFFSET_FIGURE, eval);
    return Hfigs[handle-HANDLE_OFFSET_FIGURE]->HFig();
  }
}


void ValidateHandle(unsigned handle) {
  if (handle == 0) return;
  if (handle >= HANDLE_OFFSET_OBJECT)
    LookupHandleObject(handle);
  else
    LookupHandleFigureNoCreate(handle);
}

unsigned AssignHandleObject(HandleObject* hp) {
  return (objectset.assignHandle(hp)+HANDLE_OFFSET_OBJECT);
}

void FreeHandleObject(unsigned handle) {
  objectset.deleteHandle(handle-HANDLE_OFFSET_OBJECT);
}


//@@Signature
//sgfunction figure HFigureFunction
//input number
//output handle
//DOCBLOCK handle_figure
ArrayVector HFigureFunction(int nargout,const ArrayVector& arg, Interpreter *eval) {
  if (arg.size() == 0) {
    NewFig(eval);
    return ArrayVector(Array(double(HcurrentFig+1)));
  } else {
    Array t(arg[0]);
    int fignum = t.asInteger();
    if ((fignum<=0) || (fignum>MAX_FIGS))
      throw Exception("figure number is out of range - it must be between 1 and 50");
    SelectFig(fignum-1, eval);
    return ArrayVector();
  }
}

void AddToCurrentFigChildren(unsigned handle, Interpreter *eval) {
  HandleFigure *fig = CurrentFig(eval);
  HPHandles *cp = (HPHandles*) fig->LookupProperty("children");
  QVector<unsigned> children(cp->Data());
  // Check to make sure that children does not contain our handle already
  int i=0;
  while (i<children.size()) {
    if (children[i] == handle)
      children.erase(children.begin()+i);
    else
      i++;
  }
  children.insert(children.begin(),1,handle);
  cp->Data(children);
}

//@@Signature
//sgfunction axes HAxesFunction
//input varargin
//output handle
//DOCBLOCK handle_axes
ArrayVector HAxesFunction(int nargout, const ArrayVector& arg, Interpreter *eval) {
  if (arg.size() != 1) {
    HandleObject *fp = new HandleAxis;
    unsigned int handle = AssignHandleObject(fp);
    ArrayVector t(arg);
    while (t.size() >= 2) {
      QString propname(t[0].asString());
      fp->LookupProperty(propname)->Set(t[1]);
      t.pop_front();
      t.pop_front();
    }
    // Get the current figure
    HandleFigure *fig = CurrentFig(eval);
    fp->SetPropertyHandle("parent",HcurrentFig+1);
    fig->SetPropertyHandle("currentaxes",handle);
    // Add us to the children...
    AddToCurrentFigChildren(handle, eval);
    fp->UpdateState();
    return ArrayVector(Array(double(handle)));
  } else {
    unsigned int handle = (unsigned int) arg[0].asInteger();
    HandleObject* hp = LookupHandleObject(handle);
    if (!hp->IsType("axes"))
      throw Exception("single argument to axes function must be handle for an axes"); 
    AddToCurrentFigChildren(handle, eval);
    // Get the current figure
    CurrentFig(eval)->SetPropertyHandle("currentaxes",handle);     
    CurrentFig(eval)->UpdateState();
    return ArrayVector();
  }
}

void HSetChildrenFunction(HandleObject *fp, Array children, Interpreter *eval) {
  children = children.toClass(UInt32);
  const BasicArray<uint32> &dp(children.constReal<uint32>());
  // make sure they are all valid handles
  for (index_t i=1;i<=dp.length();i++) 
    ValidateHandle(dp[i]);
  // Retrieve the current list of children
  HandleObject *gp;
  HPHandles *hp = (HPHandles*) fp->LookupProperty("children");
  QVector<unsigned> my_children(hp->Data());
  for (int i=0;i<my_children.size();i++) {
    unsigned handle = my_children[i];
    if (handle >= HANDLE_OFFSET_OBJECT) {
      gp = LookupHandleObject(handle);
      gp->Dereference();
    }
  }
  // Loop through the new list of children
  for (index_t i=1;i<=dp.length();i++) {
    unsigned handle = dp[i];
    if (handle >= HANDLE_OFFSET_OBJECT) {
      gp = LookupHandleObject(handle);
      gp->Reference();
    }
  }
  // Check for anyone with a zero reference count - it should
  // be deleted
  for (int i=0;i<my_children.size();i++) {
    unsigned handle = my_children[i];
    if (handle >= HANDLE_OFFSET_OBJECT) {
      gp = LookupHandleObject(handle);
      if (gp->RefCount() <= 0) {
	if (gp->StringPropertyLookup("type") == "uicontrol")
	  ((HandleUIControl*) gp)->Hide();
	FreeHandleObject(handle);
	delete gp;
      } 
    }
  }
  // Call the generic set function now
  hp->Set(children);
}


//@@Signature
//sgfunction sizefig SizeFigFunction
//input width height
//output none
//DOCBLOCK handle_sizefig
ArrayVector SizeFigFunction(int nargout, const ArrayVector& arg, Interpreter *eval) {
  if (arg.size() < 2)
    throw Exception("sizefig requires width and height");
  int width = arg[0].asInteger();
  int height = arg[1].asInteger();
  HandleWindow* currentWindow = CurrentWindow(eval);
  QSize main_window_size = currentWindow->size();
  QSize central_window_size = currentWindow->centralWidget()->size();
  currentWindow->resize(width + main_window_size.width() - central_window_size.width(),
			  height + main_window_size.height() - central_window_size.height());
  currentWindow->HFig()->markDirty();
  return ArrayVector();
}

//@@Signature
//sgfunction set HSetFunction
//input varargin
//output none
//DOCBLOCK handle_set

static HandleObject* LookupObject(int handle, Interpreter * eval) {
  if (handle <= 0)
    throw Exception("Illegal handle used for handle graphics operation");
  if (handle >= HANDLE_OFFSET_OBJECT)
    return LookupHandleObject(handle);
  else
    return ((HandleObject*) LookupHandleFigure(handle, eval));
}

static void RecursiveUpdateState(int handle, Interpreter * eval) {
  HandleObject *fp = LookupObject(handle, eval);
  fp->UpdateState();
  HPHandles *children = (HPHandles*) fp->LookupProperty("children");
  QVector<unsigned> handles(children->Data());
  for (int i=0;i<handles.size();i++) {
    HandleObject *fp = LookupObject(handles[i], eval);
    fp->UpdateState();
  }  
}

ArrayVector HSetFunction(int nargout, const ArrayVector& arg, Interpreter * eval) {
  if (arg.size() < 3)
    throw Exception("set doesn't handle all cases yet!");
  int handle = arg[0].asInteger();
  // Lookup the handle
  HandleObject *fp = LookupObject(handle, eval);
  int ptr = 1;
  while (arg.size() >= (ptr+2)) {
    // Use the address and property name to lookup the Get/Set handler
    QString propname = arg[ptr].asString();
    // Special case 'set' for 'children' - this can change reference counts
    // Special case 'set' for 'figsize' - this requires help from the OS
    if (propname == "children")
      HSetChildrenFunction(fp,arg[ptr+1], eval);
    else if ((handle < HANDLE_OFFSET_OBJECT) && (propname == "figsize")) {
      fp->LookupProperty(propname)->Set(arg[ptr+1]);
    } else {
      try {
	fp->LookupProperty(propname)->Set(arg[ptr+1]);
      } catch (Exception &e) {
	throw Exception(QString("Got error ") + QString(e.msg()) + 
			QString(" for property ") + propname);
      }
    }
    ptr+=2;
  }
  RecursiveUpdateState(handle, eval);
  //  fp->UpdateState();
  if (!fp->IsType("figure") && !fp->IsType("uicontrol")) {
    //FIXME
    HandleFigure *fig = fp->GetParentFigure();
    fig->UpdateState();
    //     fig->Repaint();
  }
  CurrentWindow(eval)->HFig()->markDirty();
  return ArrayVector();
}

//@@Signature
//sgfunction get HGetFunction
//input handle property
//output value
//DOCBLOCK handle_get

ArrayVector HGetFunction(int nargout, const ArrayVector& arg, Interpreter *eval) {
  if (arg.size() != 2)
    throw Exception("get doesn't handle all cases yet!");
  int handle = arg[0].asInteger();
  QString propname = arg[1].asString();
  // Lookup the handle
  HandleObject *fp = LookupObject(handle, eval);
  // Use the address and property name to lookup the Get/Set handler
  return ArrayVector(fp->LookupProperty(propname)->Get());
}

static unsigned GenericConstructor(HandleObject* fp, const ArrayVector& arg, Interpreter* eval,
				   bool autoParentGiven = true) {
  unsigned int handle = AssignHandleObject(fp);
  ArrayVector t(arg);
  while (t.size() >= 2) {
    QString propname(t[0].asString());
    if (propname == "autoparent") {
      QString pval(t[1].asString());
      autoParentGiven = (pval == "on");
    }	else {
      try {
	fp->LookupProperty(propname)->Set(t[1]);
      } catch (Exception &e) {
	throw Exception(QString("Got error ") + e.msg() + 
			QString(" for property ") + propname);
      }
    }
    t.pop_front();
    t.pop_front();
  }
  if (autoParentGiven) {
    HandleFigure *fig = CurrentFig(eval);
    unsigned current = fig->HandlePropertyLookup("currentaxes");
    if (current == 0) {
      ArrayVector arg2;
      HAxesFunction(0,arg2, eval);
      current = fig->HandlePropertyLookup("currentaxes");
    }
    HandleAxis *axis = (HandleAxis*) LookupHandleObject(current);
    HPHandles *cp = (HPHandles*) axis->LookupProperty("children");
    QVector<unsigned> children(cp->Data());
    children.push_back(handle);
    cp->Data(children);
    cp = (HPHandles*) fp->LookupProperty("parent");
    QVector<unsigned> parent;
    parent.push_back(current);
    cp->Data(parent);
    axis->UpdateState();
    fig->markDirty();
  }
  fp->UpdateState();
  return handle;
}


//@@Signature
//sgfunction hline HLineFunction
//input varargin
//output handle
//DOCBLOCK handle_hline
ArrayVector HLineFunction(int nargout, const ArrayVector& arg, Interpreter *eval) {
  return ArrayVector(Array(double(GenericConstructor(new HandleLineSeries,arg,eval))));
}

//@@Signature
//sgfunction hcontour HContourFunction
//input varargin
//output handle
//DOCBLOCK handle_hcontour
ArrayVector HContourFunction(int nargout, const ArrayVector& arg, Interpreter *eval) {
  return ArrayVector(Array(double(GenericConstructor(new HandleContour,arg, eval))));
}

//@@Signature
//sgfunction uicontrol HUIControlFunction
//input varargin
//output handle
//DOCBLOCK handle_uicontrol
ArrayVector HUIControlFunction(int nargout, const ArrayVector& arg, Interpreter *eval) {
  HandleUIControl *o = new HandleUIControl;
  o->SetEvalEngine(eval);
  o->ConstructWidget(CurrentWindow(eval));
  unsigned handleID = GenericConstructor(o,arg,eval,false);
  // Parent the control to the current figure
  AddToCurrentFigChildren(handleID, eval);
  HPHandles* cp = (HPHandles*) o->LookupProperty("parent");
  QVector<unsigned> parent;
  parent.push_back(HcurrentFig+1);
  cp->Data(parent);
  return ArrayVector(Array(double(handleID)));
}

//@@Signature
//sgfunction himage HImageFunction
//input varargin
//output handle
//DOCBLOCK handle_himage
ArrayVector HImageFunction(int nargout, const ArrayVector& arg, Interpreter *eval) {
  return ArrayVector(Array(double(GenericConstructor(new HandleImage,arg, eval))));
}

//@@Signature
//sgfunction htext HTextFunction
//input varargin
//output handle
//DOCBLOCK handle_htext
ArrayVector HTextFunction(int nargout, const ArrayVector& arg, Interpreter *eval) {
  return ArrayVector(Array(double(GenericConstructor(new HandleText,arg, eval))));
}

//@@Signature
//sgfunction surface HSurfaceFunction
//input varargin
//output handle
//DOCBLOCK handle_hsurface
ArrayVector HSurfaceFunction(int nargout, const ArrayVector& arg, Interpreter *eval) {
  return ArrayVector(Array(double(GenericConstructor(new HandleSurface,arg, eval))));
}

//@@Signature
//sgfunction hpatch HPatchFunction
//input varargin
//output handle
//DOCBLOCK handle_hpatch
ArrayVector HPatchFunction(int nargout, const ArrayVector& arg, Interpreter *eval) {
  return ArrayVector(Array(double(GenericConstructor(new HandlePatch,arg,eval))));
}


//@@Signature
//sgfunction figraise FigRaiseFunction
//input handle
//output none
//DOCBLOCK handle_figraise
ArrayVector FigRaiseFunction(int nargout, const ArrayVector& args, Interpreter * eval) {
  if (args.size() == 0)
    CurrentWindow(eval)->raise();
  else {
    int fignum = args[0].asInteger();
    if ((fignum >= 1) && (fignum < MAX_FIGS+1)) {
      if (Hfigs[fignum-1])
	Hfigs[fignum-1]->raise();
      else {
	Hfigs[fignum-1] = new HandleWindow(fignum-1, eval);
	Hfigs[fignum-1]->show();
	Hfigs[fignum-1]->raise();
      }
    }
  }
  return ArrayVector();
}

//@@Signature
//sgfunction figlower FigLowerFunction
//input handle
//output none
//DOCBLOCK handle_figlower
ArrayVector FigLowerFunction(int nargout, const ArrayVector& args, Interpreter *eval) {
  if (args.size() == 0)
    CurrentWindow(eval)->lower();
  else {
    int fignum = args[0].asInteger();
    if ((fignum >= 1) && (fignum < MAX_FIGS+1)) {
      if (Hfigs[fignum-1])
	Hfigs[fignum-1]->lower();
      else {
	Hfigs[fignum-1] = new HandleWindow(fignum-1, eval);
	Hfigs[fignum-1]->show();
	Hfigs[fignum-1]->lower();
      }
    }
  }
  return ArrayVector();
}

//@@Signature
//sgfunction gcf HGCFFunction
//input none
//output handle
//DOCBLOCK handle_gcf
ArrayVector HGCFFunction(int nargout, const ArrayVector& arg, Interpreter *eval) {
  if (HcurrentFig == -1)
    NewFig(eval);      
  return ArrayVector(Array(double(HcurrentFig+1)));
}

//@@Signature
//sgfunction gca HGCAFunction
//input none
//output handle
//DOCBLOCK handle_gca
ArrayVector HGCAFunction(int nargout, const ArrayVector& arg, Interpreter *eval) {
  // Get the current figure...
  if (HcurrentFig == -1)
    NewFig(eval);
  HandleFigure* fig = CurrentFig(eval);
  unsigned current = fig->HandlePropertyLookup("currentaxes");
  if (current == 0) {
    ArrayVector arg2;
    HAxesFunction(0,arg2, eval);
    current = fig->HandlePropertyLookup("currentaxes");
  }
  return ArrayVector(Array(double(current)));
}

//@@Signature
//gfunction pvalid HPropertyValidateFunction
//input type property
//output bool
//DOCBLOCK handle_pvalid
ArrayVector HPropertyValidateFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2)
    throw Exception("pvalid requires two arguments, an object type name and a property name");
  QString objectname = arg[0].asString();
  HandleObject *fp;
  if (objectname == "axes")
    fp = new HandleAxis;
  else if (objectname == "line")
    fp = new HandleLineSeries;
  else if (objectname == "text")
    fp = new HandleText;
  else if (objectname == "patch")
    fp = new HandlePatch;
  else
    throw Exception("Unrecognized object type name " + objectname);
  QString propname = arg[1].asString();
  bool isvalid;
  isvalid = true;
  try {
    fp->LookupProperty(propname);
  } catch (Exception& e) {
    isvalid = false;
  }
  delete fp;
  return ArrayVector(Array(bool(isvalid)));
}

bool PrintBaseFigure(HandleWindow* g, QString filename, 
		     QString type) {
  bool retval;
  HandleFigure* h = g->HFig();
  HPColor *color = (HPColor*) h->LookupProperty("color");
  double cr, cg, cb;
  cr = color->At(0); cg = color->At(1); cb = color->At(2);
  h->SetThreeVectorDefault("color",1,1,1);

  bool bRepaintFlag = GfxEnableFlag();

  GfxEnableRepaint();
  h->UpdateState();
  while (h->isDirty())
    qApp->processEvents(QEventLoop::AllEvents, 50);
  if ((type == "PDF") || (type == "PS") || (type == "EPS")){
    QPrinter prnt;
    if (type == "PDF")
      prnt.setOutputFormat(QPrinter::PdfFormat);
    else
      prnt.setOutputFormat(QPrinter::PostScriptFormat);
    prnt.setOutputFileName(filename);
    QPainter pnt(&prnt);
    QTRenderEngine gc(&pnt,0,0,g->width(),g->height());
    h->markDirty();
    h->PaintMe(gc);
    retval = true;
  } else if (type == "SVG") {
    QSvgGenerator gen;
    gen.setFileName(filename);
    gen.setSize(QSize(g->width(),g->height()));
    QPainter pnt(&gen);
    QTRenderEngine gc(&pnt,0,0,g->width(),g->height());
    h->PaintMe(gc);
    retval = true;
  } else {
    // Binary print - use grabWidget
    QPixmap pxmap(QPixmap::grabWidget(g->GetQtWidget()));
    QImage img(pxmap.toImage());
    retval = img.save(filename,type.toAscii().data());
  }
  h->SetThreeVectorDefault("color",cr,cg,cb);
  h->markDirty();
  while (h->isDirty())
    qApp->processEvents(QEventLoop::AllEvents, 50);
  if( !bRepaintFlag )
    GfxDisableRepaint();
  return retval;
}
  
void CloseHelper(int fig) {
  if (fig == -1) return;
  if (Hfigs[fig] == NULL) return;
  Hfigs[fig]->hide();
  delete Hfigs[fig];
  Hfigs[fig] = NULL;
  if (HcurrentFig == fig)
    HcurrentFig = -1;
}

//@@Signature
//gfunction close HCloseFunction
//input handle
//output none
//DOCBLOCK handle_close
ArrayVector HCloseFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() > 1)
    throw Exception("close takes at most one argument - either the string 'all' to close all figures, or a scalar integer indicating which figure is to be closed.");
  int action;
  if (arg.size() == 0) 
    action = 0;
  else {
    Array t(arg[0]);
    if (t.isString()) {
      QString allflag = t.asString();
      if (allflag == "all") 
	action = -1;
      else
	throw Exception("string argument to close function must be 'all'");
    } else {
      int handle = t.asInteger();
      if (handle < 1)
	throw Exception("Invalid figure number argument to close function");
      action = handle;
    }
  }
  if (action == 0) {
    if (HcurrentFig != -1) 
      CloseHelper(HcurrentFig);
  } else if (action == -1) {
    for (int i=0;i<MAX_FIGS;i++)
      CloseHelper(i);
    HcurrentFig = -1;
  } else {
    if ((action < MAX_FIGS) && (action >= 1))
      CloseHelper(action-1);
  }
  return ArrayVector();
}

//@@Signature
//gfunction copy HCopyFunction
//input none 
//output none
//DOCBLOCK handle_copy
ArrayVector HCopyFunction(int nargout, const ArrayVector& arg) {
  if (HcurrentFig == -1)
    return ArrayVector();
  HandleWindow *f = Hfigs[HcurrentFig];
  // use grabWidget - doesnt work for openGL yet
  QClipboard *cb = QApplication::clipboard();
  cb->setPixmap(QPixmap::grabWidget(f));
  return ArrayVector();
}
  
static QString NormalizeImageExtension(QString ext) {
  QString upperext(ext.toUpper());
  QString lowerext(ext.toLower());
  if (upperext == "JPG") return QString("JPEG");
  if ((upperext == "SVG") || (upperext == "PDF") || 
      (upperext == "PS") || (upperext == "EPS")) return upperext;
  QList<QByteArray> formats(QImageWriter::supportedImageFormats());
  for (int i=0;i<formats.count();i++) {
    if (formats.at(i).data() == upperext) return upperext;
    if (formats.at(i).data() == lowerext) return lowerext;
  }
  return QString();
}

QString FormatListAsString() {
  QString ret_text = "Supported Formats: ";
  QList<QByteArray> formats(QImageWriter::supportedImageFormats());
  for (int i=0;i<formats.count();i++)
    ret_text = ret_text + formats.at(i).data() + " ";
  return ret_text;
}

//@@Signature
//gfunction print HPrintFunction
//input varargin
//output none
//DOCBLOCK handle_print
ArrayVector HPrintFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("print function takes a single, string argument");
  if (!(arg[0].isString()))
    throw Exception("print function takes a single, string argument");
  Array t(arg[0]);
  if (HcurrentFig == -1)
    return ArrayVector();
  HandleWindow *f = Hfigs[HcurrentFig];
  QString outname(t.asString());
  int pos = outname.lastIndexOf(".");
  if (pos < 0)
    throw Exception("print function argument must contain an extension - which is used to determine the format for the output");
  QString original_extension(outname.mid(pos+1,outname.size()));
  QString modified_extension = 
    NormalizeImageExtension(original_extension);
  if (modified_extension.isEmpty())
    throw Exception(QString("unsupported output format ") + 
		    original_extension + " for print.\n" + 
		    FormatListAsString());
  if (!PrintBaseFigure(f,outname,modified_extension))
    throw Exception("Printing failed!");
  return ArrayVector();
}

//@@Signature
//gfunction htextbitmap HTextBitmapFunction
//input fontname size text
//output bitmap
//DOCBLOCK handle_htextbitmap
const int m_pad = 10;
ArrayVector HTextBitmapFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 3) throw Exception("htextbitmap requires three arguments");
  QString fontname(arg[0].asString());
  int fontsize(arg[1].asInteger());
  QString fonttext(arg[2].asString());
  QFont fnt(fontname,fontsize);
  QFontMetrics fmet(fnt);
  int ascent = fmet.ascent();
  int descent = fmet.descent();
  int height = ascent + descent + 1;
  int width = fmet.width(fonttext);
  QImage img(width,height,QImage::Format_RGB32);
  QPainter pnt(&img);
  pnt.setPen(QColor(Qt::black));
  pnt.setBrush(QColor(Qt::black));
  pnt.drawRect(0,0,width,height);
  pnt.setPen(QColor(Qt::white));
  pnt.setBrush(QColor(Qt::white));
  pnt.setFont(fnt);
  pnt.drawText(0,height-descent-1,fonttext);
  Array M(UInt8,NTuple(height,width));
  BasicArray<uint8> &val(M.real<uint8>());
  for (int i=0;i<width;i++)
    for (int j=0;j<height;j++) {
      QRgb p = img.pixel(i,j);
      val[NTuple(j+1,i+1)] = qGray(p);
    }
  return ArrayVector(M);
}

//@@Signature
//gfunction hrawplot HRawPlotFunction
//input filename commands
//output none
//DOCBLOCK handle_hrawplot
ArrayVector HRawPlotFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 2) throw Exception("hrawplot requires 4 arguments");
  QString filename(arg[0].asString());
  if (arg[1].dataClass() != CellArray) throw Exception("Expect a cell array of commands.");
  if (!filename.endsWith(".pdf")) throw Exception("filename must end with PDF");
  QPrinter prnt;
  prnt.setOutputFormat(QPrinter::PdfFormat);
  prnt.setOutputFileName(filename);
  prnt.setColorMode(QPrinter::Color);
  prnt.setPageSize(QPrinter::Ledger);
  QPainter pnt(&prnt);
  QRect rect = pnt.viewport();
  const BasicArray<Array>& dp(arg[1].constReal<Array>());
  int descent = 0;
  for (index_t i=1;i<=dp.length();i++) {
    ArrayVector cmdp(ArrayVectorFromCellArray(dp[i]));
    QString cmd = QString("%1 %2").arg(cmdp[0].asString()).arg(int(i));
    if (cmdp[0].asString().toUpper() == "LINE") {
      if (cmdp.size() != 5) throw Exception("malformed line: " + cmd);
      int x1 = cmdp[1].asInteger();
      int y1 = cmdp[2].asInteger();
      int x2 = cmdp[3].asInteger();
      int y2 = cmdp[4].asInteger();
      pnt.drawLine(x1,y1,x2,y2);
    } else if (cmdp[0].asString().toUpper() == "BOX") {
      if (cmdp.size() != 5) throw Exception("malformed line: " + cmd);
      int x1 = cmdp[1].asInteger();
      int y1 = cmdp[2].asInteger();
      int x2 = cmdp[3].asInteger();
      int y2 = cmdp[4].asInteger();
      pnt.drawRect(QRect(QPoint(x1,y1),QPoint(x2,y2)));
    } else if (cmdp[0].asString().toUpper() == "HTEXTBOX") {
      if (cmdp.size() != 6) throw Exception("malformed line: " + cmd);
      int x1 = cmdp[1].asInteger();
      int y1 = cmdp[2].asInteger();
      int x2 = cmdp[3].asInteger();
      int y2 = cmdp[4].asInteger();
      QString txt = cmdp[5].asString();
      QRect rect(QPoint(x1,y1),QPoint(x2,y2));
      pnt.fillRect(rect,pnt.brush());
      pnt.drawText(rect,Qt::AlignCenter | Qt::TextWordWrap,txt);
    } else if (cmdp[0].asString().toUpper() == "VTEXTBOX") {
      if (cmdp.size() != 6) throw Exception("malformed line: " + cmd);
      int x1 = cmdp[1].asInteger();
      int y1 = cmdp[2].asInteger();
      int x2 = cmdp[3].asInteger();
      int y2 = cmdp[4].asInteger();
      QString txt = cmdp[5].asString();
      pnt.save();
      pnt.translate(QPoint(pnt.window().width()/2,pnt.window().height()/2));
      pnt.rotate(-90);
      pnt.translate(QPoint(-pnt.window().width()/2,-pnt.window().height()/2));
      QRect rect(pnt.transform().inverted().map(QPoint(x1,y1)),
		 pnt.transform().inverted().map(QPoint(x2,y2)));
      pnt.fillRect(rect,pnt.brush());
      pnt.drawText(rect,Qt::AlignCenter,txt);
      pnt.restore();
    } else if (cmdp[0].asString().toUpper() == "FONT") {
      if (cmdp.size() != 3) throw Exception("malformed line: " + cmd);
      QString name = cmdp[1].asString();
      int size = cmdp[2].asInteger();
      QFont fnt(name,size);
      QFontMetrics metrics(fnt);
      descent = metrics.descent();
      pnt.setFont(QFont(name,size));
    } else if (cmdp[0].asString().toUpper() == "TEXT") {
      if (cmdp.size() != 4) throw Exception("malformed line: " + cmd);
      int x1 = cmdp[1].asInteger();
      int y1 = cmdp[2].asInteger();
      QString txt = cmdp[3].asString();
      pnt.drawText(x1,y1-descent,txt);
    } else if (cmdp[0].asString().toUpper() == "STYLE") {
      if (cmdp.size() != 2) throw Exception("malformed line: " + cmd);
      QString style = cmdp[1].asString();
      if (style.toUpper() == "SOLID")
	pnt.setPen(Qt::SolidLine);
      else if (style.toUpper() == "DOTTED")
	pnt.setPen(Qt::DotLine);
      else
	throw Exception("malformed line: " + cmd);
    } else if (cmdp[0].asString().toUpper() == "PAGE") {
      prnt.newPage();
    } else if (cmdp[0].asString().toUpper() == "BRUSH") {
      if (cmdp.size() != 2 && cmdp.size() != 4) throw Exception("malformed line: " + cmd);
      if (cmdp.size() == 2)
	pnt.setBrush(QBrush(QColor(cmdp[1].asString())));
      else
	pnt.setBrush(QBrush(QColor(cmdp[1].asDouble(),
				   cmdp[2].asDouble(),
				   cmdp[3].asDouble()))); 
    } else if (cmdp[0].asString().toUpper() == "PEN") {
      QPen pen(pnt.pen());
      pen.setColor(QColor(cmdp[1].asString()));
      pnt.setPen(pen);
    } else if (cmdp[0].asString().toUpper() == "SIZE") {
      if (cmdp.size() != 3) throw Exception("malformed line: " + cmd);
      int width = cmdp[1].asInteger();
      int height = cmdp[2].asInteger();
      QSize size(width,height);
      size.scale(rect.size(),Qt::KeepAspectRatio);
      pnt.setViewport(rect.x() + (rect.width()-size.width())/2,
		      rect.y() + (rect.height()-size.height())/2,
		      size.width(),size.height());
      pnt.setWindow(QRect(0,0,width,height));
      pnt.eraseRect(0,0,width,height);
    } else
      throw Exception("malformed line: " + cmd);
  }
  return ArrayVector();
}


//@@Signature
//gfunction hpoint HPointFunction
//input none
//output coords
//DOCBLOCK handle_hpoint
ArrayVector HPointFunction(int nargout, const ArrayVector& arg) {
  if (HcurrentFig == -1)
    return ArrayVector();
  HandleWindow *f = Hfigs[HcurrentFig];
  f->raise();
  f->activateWindow();
  f->setFocus(Qt::OtherFocusReason);
  int x, y;
  GfxEnableRepaint();
  f->GetClick(x,y);
  GfxDisableRepaint();
  BasicArray<double> retvec(NTuple(2,1));
  retvec.set(1,x);
  retvec.set(2,y);
  return ArrayVector(Array(retvec));
}

//@@Signature
//gfunction is2dview HIs2DViewFunction
//input handle
//output bool
//DOCBLOCK handle_is2dview
ArrayVector HIs2DViewFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0) throw Exception("is2DView expects a handle to axes");
  unsigned int handle = (unsigned int) (arg[0].asInteger());
  HandleObject* hp = LookupHandleObject(handle);
  if (!hp->IsType("axes"))
    throw Exception("single argument to axes function must be handle for an axes"); 
  HandleAxis *axis = (HandleAxis*) hp;
  return ArrayVector(Array(bool(axis->Is2DView())));
}

#if 0
class contour_point {
public:
  double x;
  double y;
  inline contour_point(double a, double b) : x(a), y(b) {};
};

inline bool operator==(const contour_point& p1, const contour_point& p2) {
  return ((p1.x == p2.x) && (p1.y == p2.y));
}

typedef QList<contour_point> cline;
typedef QList<cline> lineset;

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

ArrayVector ContourCFunction(int nargout, const ArrayVector& arg) {
  lineset allLines;
  lineset bundledLines;
  if (arg.size() < 2) throw Exception("contourc expects two arguments");
  double val = arg[1].asDouble();
  if (!arg[0].is2D())
    throw Exception("First argument to contourc must be a 2D matrix");
  Array m(arg[0].toClass(Double));
  const double *func = (const double *) m.getDataPointer();
  int outcnt = 0;
  int numy = m.rows();
  int numx = m.columns();
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
  int allcount = allLines.size();
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
  int outcount = bundledLines.size() + 2*allcount + 1;
  Array out(Array::doubleMatrixConstructor(2,outcount));
  double *output = (double *) out.getReadWriteDataPointer();
  for (int i=0;i<bundledLines.size();i++) {
    *output++ = val;
    *output++ = bundledLines[i].size();
    cline bline(bundledLines[i]);
    for (int j=0;j<bline.size();j++) {
      *output++ = bline[j].x;
      *output++ = bline[j].y;
    }
  }
  return ArrayVector() << out;
}
#endif 

void LoadHandleGraphicsFunctions(Context* context) {
  InitializeHandleGraphics();
};
