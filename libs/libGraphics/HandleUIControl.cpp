/*
 * Copyright (c) 2009 Samit Basu
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
#include "HandleUIControl.hpp"
#include "HandleMultiEdit.hpp"
#include "HandleWindow.hpp"
#include "Interpreter.hpp"
#include <QtGui>

HandleUIControl::HandleUIControl() {
  ConstructProperties();
  SetupDefaults();
  widget = NULL;
}
  
HandleUIControl::~HandleUIControl() {
  if (widget) delete widget;
}

void HandleUIControl::SetEvalEngine(Interpreter *eval) {
  m_eval = eval;
}

// Need to handle initialization of "value" property
void HandleUIControl::UpdateState() {
  bool newwidget = false;
  if (HasChanged("style")) {
    if (widget) delete widget;
    widget = NULL;
    if (StringCheck("style","pushbutton")) {
      widget = new QPushButton(parentWidget->GetQtWidget());
      connect(widget,SIGNAL(clicked()),this,SLOT(clicked()));
    }
    if (StringCheck("style","radiobutton")) {
      widget = new QRadioButton(parentWidget->GetQtWidget());
      ((QRadioButton*) widget)->setAutoExclusive(false);
      connect(widget,SIGNAL(clicked()),this,SLOT(clicked()));
    }
    if (StringCheck("style","checkbox")) {
      widget = new QCheckBox(parentWidget->GetQtWidget());
      connect(widget,SIGNAL(clicked()),this,SLOT(clicked()));
    }
    if (StringCheck("style","toggle")) {
      widget = new QPushButton(parentWidget->GetQtWidget());
      ((QPushButton*) widget)->setCheckable(true);
      connect(widget,SIGNAL(clicked()),this,SLOT(clicked()));
    }
    if (StringCheck("style","slider")) {
      QVector<double> sizevec(VectorPropertyLookup("position"));
      if (sizevec[3] > sizevec[2])
	widget = new QSlider(Qt::Vertical,parentWidget->GetQtWidget());
      else
	widget = new QSlider(Qt::Horizontal,parentWidget->GetQtWidget());
      ((QSlider*)widget)->setTracking(false);
      connect(widget,SIGNAL(valueChanged(int)),this,SLOT(clicked()));
    }
    if (StringCheck("style","popupmenu")) {
      widget = new QComboBox(parentWidget->GetQtWidget());
      connect(widget,SIGNAL(activated(int)),this,SLOT(clicked()));
      ((QComboBox*) widget)->setEditable(false);
    }
    if (StringCheck("style","listbox")) {
      widget = new QListWidget(parentWidget->GetQtWidget());
      connect(widget,SIGNAL(itemSelectionChanged()),this,SLOT(clicked()));
    }
    if (StringCheck("style","text")) {
      widget = new QLabel(parentWidget->GetQtWidget());
    }
    if (StringCheck("style","edit")) {
      double min(ScalarPropertyLookup("min"));
      double max(ScalarPropertyLookup("max"));
      if ((max-min) > 1) {
	widget = new HandleMultiEdit(parentWidget->GetQtWidget());
	widget->setObjectName("multiline");
      } else {
	widget = new QLineEdit(parentWidget->GetQtWidget());
	widget->setObjectName("singleline");
      }
      connect(widget,SIGNAL(editingFinished()),this,SLOT(clicked()));
    }
    if (widget)
      widget->show();
    ClearChanged("style");
    newwidget = true;
  }
  if (!widget) {
    ClearAllChanged();
    return;
  }
  if (HasChanged("position") || newwidget) {
    QVector<double> sizevec(VectorPropertyLookup("position"));
    widget->setGeometry((int)sizevec[0],(int)sizevec[1],(int)sizevec[2],(int)sizevec[3]);
    ClearChanged("position");
  }
  if (HasChanged("string") || newwidget) {
    if (StringCheck("style","pushbutton") || 
	StringCheck("style","toggle")) {
      ((QPushButton*)widget)->setText(StringPropertyLookup("string"));
    }
    if (StringCheck("style","radiobutton")) {
      ((QRadioButton*)widget)->setText(StringPropertyLookup("string"));
    }
    if (StringCheck("style","checkbox")) {
      ((QCheckBox*)widget)->setText(StringPropertyLookup("string"));
    }
    if (StringCheck("style","popupmenu")) {
      ((QComboBox*)widget)->clear();
      QString txt(StringPropertyLookup("string"));
      StringVector data = txt.split("|");
      for (int i=0;i<data.size();i++)
	((QComboBox*)widget)->addItem(data[i]);
    }
    if (StringCheck("style","listbox")) {
      ((QListWidget*)widget)->clear();
      QString txt(StringPropertyLookup("string"));
      StringVector data = txt.split("|");
      for (int i=0;i<data.size();i++)
	((QListWidget*)widget)->addItem(data[i]);
    }
    if (StringCheck("style","text")) {
      ((QLabel*)widget)->setText(StringPropertyLookup("string"));
    }
    if (StringCheck("style","edit")) {
      if (widget->objectName() == "multiline")
	((HandleMultiEdit*)widget)->setPlainText(StringPropertyLookup("string"));
      else
	((QLineEdit*)widget)->setText(StringPropertyLookup("string"));
	  
    }
    ClearChanged("string");
  }
  if (HasChanged("tooltipstring") || newwidget) {
    widget->setToolTip(StringPropertyLookup("tooltipstring"));
    ClearChanged("tooltipstring");
  }
  if (StringCheck("style","slider") && 
      (HasChanged("min") || HasChanged("max") || 
       HasChanged("sliderstep")  || newwidget)) {
    double min(ScalarPropertyLookup("min"));
    double max(ScalarPropertyLookup("max"));
    QVector<double> steps(VectorPropertyLookup("sliderstep"));
    ((QSlider*)widget)->setMinimum(0);
    ((QSlider*)widget)->setMaximum((int)((max-min)/steps[0]));
    ((QSlider*)widget)->setSingleStep(1);
    ((QSlider*)widget)->setPageStep((int)(steps[1]));
    ((HPVector*) LookupProperty("value"))->Data(min+steps[0]*((QSlider*)widget)->value());
    ClearChanged("min");
    ClearChanged("max");
    ClearChanged("sliderstep");
  }
  if (StringCheck("style","listbox") &&
      (HasChanged("min") || HasChanged("max") || newwidget)) {
    double min(ScalarPropertyLookup("min"));
    double max(ScalarPropertyLookup("max"));
    if (max-min > 1)
      ((QListWidget*)widget)->setSelectionMode(QAbstractItemView::MultiSelection);
    else
      ((QListWidget*)widget)->setSelectionMode(QAbstractItemView::SingleSelection);
    ClearChanged("min");
    ClearChanged("max");
  }
  if (HasChanged("listboxtop") || newwidget) {
    if (StringCheck("style","listbox")) {
      int listtop((int)ScalarPropertyLookup("listboxtop"));
      ((QListWidget*)widget)->scrollToItem(((QListWidget*)widget)->item(listtop-1),
					   QAbstractItemView::PositionAtTop);
    }
    ClearChanged("listboxtop");
  }

  if (StringCheck("style","toggle") && 
      (HasChanged("value") || HasChanged("min") ||
       HasChanged("max") || newwidget)) {
    double min(ScalarPropertyLookup("min"));
    double max(ScalarPropertyLookup("max"));
    QVector<double> value(VectorPropertyLookup("value"));
    if (value.size() > 0) {
      if (value[0] == min)
	((QPushButton*) widget)->setChecked(false);
      else if (value[0] == max) 
	((QPushButton*) widget)->setChecked(true);
    }
    ClearChanged("max");
    ClearChanged("min");
    ClearChanged("value");
  }

  if (StringCheck("style","radiobutton") && 
      (HasChanged("value") || HasChanged("min") ||
       HasChanged("max") || newwidget)) {
    double min(ScalarPropertyLookup("min"));
    double max(ScalarPropertyLookup("max"));
    QVector<double> value(VectorPropertyLookup("value"));
    if (value.size() > 0) {
      if (value[0] == min)
	((QRadioButton*) widget)->setChecked(false);
      else if (value[0] == max) 
	((QRadioButton*) widget)->setChecked(true);
    }
    ClearChanged("max");
    ClearChanged("min");
    ClearChanged("value");
  }

  if (StringCheck("style","checkbox") && 
      (HasChanged("value") || HasChanged("min") ||
       HasChanged("max") || newwidget)) {
    double min(ScalarPropertyLookup("min"));
    double max(ScalarPropertyLookup("max"));
    QVector<double> value(VectorPropertyLookup("value"));
    if (value.size() > 0) {
      if (value[0] == min)
	((QCheckBox*) widget)->setChecked(false);
      else if (value[0] == max) 
	((QCheckBox*) widget)->setChecked(true);
    }
    ClearChanged("max");
    ClearChanged("min");
    ClearChanged("value");
  }

  if (HasChanged("value") || newwidget) {
    if (StringCheck("style","slider")) {
      double min(ScalarPropertyLookup("min"));
      QVector<double> steps(VectorPropertyLookup("sliderstep"));
      QVector<double> value(VectorPropertyLookup("value"));
      if (value.size() > 0)
	((QSlider*)widget)->setValue((int)((value[0]-min)/steps[0]));
    }
    if (StringCheck("style","popupmenu")) {
      QVector<double> value(VectorPropertyLookup("value"));
      ((QComboBox*)widget)->setCurrentIndex((int)(value[0]-1));
    }
    if (StringCheck("style","listbox")) {
      QVector<double> value(VectorPropertyLookup("value"));
      ((QListWidget*)widget)->clearSelection();
      for (int i=0;i<value.size();i++) 
	((QListWidget*)widget)->setItemSelected(((QListWidget*)widget)->item((int)(value[i]-1)),true);
    }
    ClearChanged("value");
  }
}

void HandleUIControl::ConstructWidget(HandleWindow *f) {
  parentWidget = f;
  //   QVector<double> sizevec(VectorPropertyLookup("position"));
  //   if (!widget) return;
  //   widget->setGeometry(sizevec[0],sizevec[1],sizevec[2],sizevec[3]);

  //   if (StringCheck("style","slider")) {
  //     double min(ScalarPropertyLookup("min"));
  //     double max(ScalarPropertyLookup("max"));
  //     QVector<double> steps(VectorPropertyLookup("sliderstep"));
  //     ((QSlider*)widget)->setMinimum(0);
  //     ((QSlider*)widget)->setMaximum((max-min)/steps[0]);
  //     ((QSlider*)widget)->setSingleStep(1);
  //     ((QSlider*)widget)->setPageStep(steps[1]);
  //     ((HPVector*) LookupProperty("value"))->Data(min+steps[0]*((QSlider*)widget)->value());
  //   }
  //   widget->show();
}

void HandleUIControl::clicked() {
  if (StringCheck("style","slider") && widget) {
    double min(ScalarPropertyLookup("min"));
    QVector<double> steps(VectorPropertyLookup("sliderstep"));
    ((HPVector*) LookupProperty("value"))->Data(min+steps[0]*((QSlider*)widget)->value());
    // No infinite loops, please
    ClearChanged("value");
  }
  if (StringCheck("style","popupmenu") && widget) {
    ((HPVector*) LookupProperty("value"))->Data(((QComboBox*)widget)->currentIndex()+1);
    ClearChanged("value");
  }
  if (StringCheck("style","listbox") && widget) {
    QList<QListWidgetItem*> selection(((QListWidget*) widget)->selectedItems());
    QVector<double> selected;
    for (int i=0;i<selection.size();i++) 
      selected.push_back(((QListWidget*) widget)->row(selection[i])+1);
    ((HPVector*) LookupProperty("value"))->Data(selected);
    ClearChanged("value");
  }
  if (StringCheck("style","toggle") && widget) {
    if (((QPushButton*) widget)->isChecked())
      ((HPVector*) LookupProperty("value"))->Data(ScalarPropertyLookup("max"));
    else
      ((HPVector*) LookupProperty("value"))->Data(ScalarPropertyLookup("min"));
  }

  if (StringCheck("style","radiobutton") && widget) {
    if (((QRadioButton*) widget)->isChecked())
      ((HPVector*) LookupProperty("value"))->Data(ScalarPropertyLookup("max"));
    else
      ((HPVector*) LookupProperty("value"))->Data(ScalarPropertyLookup("min"));
  }

  if (StringCheck("style","checkbox") && widget) {
    if (((QCheckBox*) widget)->isChecked())
      ((HPVector*) LookupProperty("value"))->Data(ScalarPropertyLookup("max"));
    else
      ((HPVector*) LookupProperty("value"))->Data(ScalarPropertyLookup("min"));
  }

  if (StringCheck("style","edit") && widget) {
    if (widget->objectName() == "multiline") {
      SetStringDefault("string",((HandleMultiEdit*)widget)->toPlainText());
    } else {
      SetStringDefault("string",((QLineEdit*)widget)->text());
    }
  }

  m_eval->ExecuteLine(StringPropertyLookup("callback") + "\n");
}

void HandleUIControl::ConstructProperties() {
  //!
  //@Module UICONTROLPROPERTIES UI Control Properties
  //@@Section HANDLE
  //@@Usage
  //Below is a summary of the properties for user interface
  //controls.
  //\begin{itemize}
  //\item @|backgroundcolor| - @|colorspec| - The background color
  // for the widget.
  //\item @|busyaction| - Not used.
  //\item @|buttondownfcn| - Not used.
  //\item @|callback| - @|string| - the callback to execute when the 
  //GUI control does its action.  Clicking a button or moving a 
  //scroller will cause the callback to be executed.  Also, pressing 
  //enter in a text box causes the callback to be executed.
  //\item @|cdata| - an @|M x N x 3| array that represents an RGB image
  //to use as the truecolor image displayed on push bottons or toggle
  //buttons.  The values must be between 0 and 1.
  //\item @|children| - Not used.
  //\item @|createfcn| - Not used.
  //\item @|deletefcn| - Not used;
  //\item @|enable| - @|{'on','inactive','off'}| - For @|on| (the
  //default) the uicontrol behaves normally.  For inactive, it is not
  //operational, but looks the same as @|on|.  For @|off|, the
  //control is grayed out.
  //\item @|extent| - a read only property that contains the extent of
  //the text for the control.
  //\item @|fontangle| - @|{'normal','italic','oblique'}| - The angle of the fonts used
  // for text labels (e.g., tick labels).
  //\item @|fontsize| - @|scalar| - The size of fonts used for text labels (tick labels).
  //\item @|fontunits| - Not used.
  //\item @|fontname| - @|string| - The name of the font to use for the widget.
  //\item @|fontweight| - @|{'normal','bold','light','demi'}| - The weight of the font used
  //\item @|foregroundcolor| - @|colorspec| - the foreground color for text.
  //  \item @|handlevisibility| - Not used.
  //  \item @|hittest| - Not used.
  //\item @|horizontalalignment| - @|{'left','center','right}| - determines
  //the justification of text.
  //\item @|interruptible| - Not used.
  //\item @|keypressfcn| - @|functionspec| - a string or function handle
  // that is called when a key is pressed and a uicontrol object has focus.
  //\item @|listboxtop| - a scalar (used only by the listbox style of
  //uicontrols) that specifies which string appears at the top of the list
  //box.
  //\item @|max| - a scalar that specifies the largest value allowed
  //for the @|value| property.  The interpretation varies depending on
  //the type of the control
  //\begin{itemize}
  //\item @|check boxes| - specifies what @|value| is set to when the
  //check box is selected.
  //\item @|edit box| - if @|max-min>1| then the text box allows for
  //multiple lines of input.  Otherwise, it is a single line only.
  //\item @|list box| - if @|max-min>1| then multiple item selections
  //are allowed.  Otherwise, only single item selections are allowed.
  //\item @|radio buttons| - specifies what @|value| is set to when the
  //radio button is selected.
  //\item @|slider| - the maximum value the slider can take.
  //\item @|toggle button| - specifies what @|value| is set to when 
  //the toggle button is selected.
  //\end{itemize}
  //\item @|min| - a scalar that specifies the smallest value for the
  //@|value| property.  The interpretation of it depends on the type
  //of the control
  //\begin{itemize}
  //\item @|check boxes| - specifies what @|value| is set to when the
  //check box is not selected.
  //\item @|edit box| - if @|max-min>1| then the text box allows for
  //multiple lines of input.  Otherwise, it is a single line only.
  //\item @|list box| - if @|max-min>1| then multiple item selections
  //are allowed.  Otherwise, only single item selections are allowed.
  //\item @|radio buttons| - specifies what @|value| is set to when the
  //radio button is not selected.
  //\item @|slider| - the minimum value the slider can take.
  //\item @|toggle button| - specifies what @|value| is set to when 
  //the toggle button is not selected.
  //\end{itemize}
  //\item @|parent| - the handle of the parent object.
  //\item @|position| - size and location of the uicontrol as a 
  //four vector @|[left, bottom, width, height]|.  If @|width>height|
  //then sliders are horizontal, otherwise the slider is oriented
  //vertically.
  //\item @|selected| - @|{'on','off'}| - not used.
  //\item @|selectionhighlight| - @|{'on','off'}| - not used.
  //\item @|sliderstep| - a two vector @|[min_step max_step]|
  //that controls the amount the slider @|value| changes when
  //you click the mouse on the control.  If you click the arrow
  //for the slider, the value changes by @|min_step|, while if
  //you click the trough, the value changes by @|max_step|.  
  //Each value must be in the range @|[0,1]|, and is a percentage
  //of the range @|max-min|.
  //\item @|string| - @|string| - the text for the control.
  //\item @|style| - @|{'pushbutton','toggle','radiobutton','checkbox',
  //'edit','text','slider','frame','listbox','popupmenu'}|.
  //\item @|tag| - @|string| - user specified label.
  //\item @|tooltipstring| - @|string| the tooltip for the control.
  //\item @|type| - @|string| - the text is set to @|'uicontrol'|.
  //\item @|uicontextmenu| - @|handle| the handle of the @|uicontextmenu|
  //that shows up when you right-click over the control.
  //\item @|units| - not used.
  //\item @|userdata| - @|array| - any data you want to associate with the
  //control.
  //\item @|value| - The meaning of this property depends on the type of the
  //control:
  //\begin{itemize}
  //\item check box - set to @|max| when checked, and @|min| when off.
  //\item list box - set to a vector of indices corresponding to selected
  //items, with @|1| corresponding to the first item in the list.
  //\item pop up menu - set to the index of the item selected (starting with 1)
  //\item radio buttons - set to @|max| when selected, and set to @|min| when
  //not selected.
  //\item sliders - set to the value of the slider
  //\item toggle buttons - set to @|max| when selected, and set to @|min| when
  //not selected.
  //\item text controls, push buttons - do not use this property.
  //\end{itemize}
  //\item @|visible| - @|{'on','off'}| - controls whether the control is 
  //visible or not
  //\end{itemize}
  //!
  AddProperty(new HPColor,"backgroundcolor");
  AddProperty(new HPString,"busyaction");
  AddProperty(new HPString,"buttondownfcn");
  AddProperty(new HPString,"callback");
  AddProperty(new HPArray,"cdata");
  AddProperty(new HPHandles,"children");
  AddProperty(new HPString,"createfcn");
  AddProperty(new HPString,"deletefcn");
  AddProperty(new HPOnOffInactive,"enable");
  AddProperty(new HPFontAngle,"fontangle");
  AddProperty(new HPString,"fontname");
  AddProperty(new HPScalar,"fontsize");
  AddProperty(new HPFontUnits,"fontunits");
  AddProperty(new HPFontWeight,"fontweight");
  AddProperty(new HPColor,"foregroundcolor");
  AddProperty(new HPOnOff,"handlevisibility");
  AddProperty(new HPOnOff,"hittest");
  AddProperty(new HPAlignHoriz,"horizontalalignment");
  AddProperty(new HPOnOff,"interruptible");
  AddProperty(new HPString,"keypressfcn");
  AddProperty(new HPScalar,"listboxtop");
  AddProperty(new HPScalar,"max");
  AddProperty(new HPScalar,"min");
  AddProperty(new HPHandles,"parent");
  AddProperty(new HPFourVector,"position");
  AddProperty(new HPOnOff,"selected");
  AddProperty(new HPOnOff,"selectionhighlight");
  AddProperty(new HPTwoVector,"sliderstep");
  AddProperty(new HPString,"string");
  AddProperty(new HPWidgetString,"style");
  AddProperty(new HPString,"tag");
  AddProperty(new HPString,"tooltipstring");
  AddProperty(new HPString,"type");
  AddProperty(new HPHandles,"uicontextmenu");
  AddProperty(new HPUnits,"units");
  AddProperty(new HPArray,"userdata");
  AddProperty(new HPVector,"value");
}

void HandleUIControl::Hide() {
  if (widget) widget->hide();
}

void HandleUIControl::SetupDefaults() {
  SetThreeVectorDefault("backgroundcolor",0.6,0.6,0.6);
  SetStringDefault("enable","on");
  SetConstrainedStringDefault("fontangle","normal");
  SetStringDefault("fontname","helvetica");
  SetScalarDefault("fontsize",10);
  SetConstrainedStringDefault("fontunits","points");
  SetConstrainedStringDefault("fontweight","normal");
  SetThreeVectorDefault("foregroundcolor",0,0,0);
  SetConstrainedStringDefault("horizontalalignment","left");
  SetScalarDefault("listboxtop",1);
  SetScalarDefault("max",1);
  SetScalarDefault("min",0);
  ((HPVector*) LookupProperty("value"))->Data(0);
  SetFourVectorDefault("position",0,0,10,10);
  SetTwoVectorDefault("sliderstep",0.01,0.1);
  SetStringDefault("type","uicontrol");
  SetStringDefault("style","pushbutton");
}
