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
#include "VariablesTool.hpp"
#include "Scope.hpp"
#include <QtGui>
#include "Array.hpp"
#include "Print.hpp"
#include "Interpreter.hpp"

VariablesTool::VariablesTool(QWidget *parent) : 
  QDockWidget("Variables",parent) {
  //  import = new QAction(QIcon(":/images/open.png"),"&Import Data",this);
  //  connect(import,SIGNAL(triggered()),this,SLOT(import()));
  //  save = new QAction(QIcon(":/images/save.png"),"&Save Data", this);
  //  connect(save,SIGNAL(triggered()),this,SLOT(save()));
  //  clear = new QAction("&Delete Variable",this);
  //  connect(clear,SIGNAL(clear()),this,SLOT(clear()));
  //  QWidget *widget = new QWidget;
  //  QToolBar *toolbar = new QToolBar("vartoolbar");
  //  toolbar->addAction(import);
  //  toolbar->addAction(save);
  //  toolbar->addSeparator();
  //  toolbar->addWidget(new QLabel("Stack:"));
  //  toolbar->addWidget(cb);
  //  QVBoxLayout *vlayout = new QVBoxLayout;
  //  vlayout->setSpacing(1);
  //  vlayout->setContentsMargins(1,1,1,1);
  //  QWidget *top_widget = new QWidget;
  //  QHBoxLayout *hlayout = new QHBoxLayout;
  //  QPushButton *pb = new QPushButton;
  //  pb->setText("**");
  //  hlayout->addWidget(pb);
  //  hlayout->addWidget(cb);
  //  top_widget->setLayout(hlayout);
  //  vlayout->addWidget(toolbar);
  //  vlayout->addWidget(top_widget);
  view = new DataView(this);
  model = new DataTable(QStringList() << "Flag" << "Name" << "Class" << "Value" << 
			"Size" << "Bytes" << "Min" << "Max" << 
			"Range" << "Mean" << "Std" << "Var");
  view->setModel(model);
  //  vlayout->addWidget(view);
  //  widget->setLayout(vlayout);
  QSettings settings("FreeMat", Interpreter::getVersionString());
  view->loadSettings(&settings,"variablestool/browser",QVector<int>() 
		     << 1 << 2 << 3 << 6 << 7 << 9);
  setWidget(view);
  setObjectName("variables");
}

void VariablesTool::updateVariableView(QVariant vars) {
  model->loadData(vars.toList());
}
