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
#include "FileTool.hpp"
#include "Interpreter.hpp"
#include <QtGui>

FileTool::FileTool(QWidget *parent) : 
  QDockWidget("File Browser",parent) {
  view = new DataView(this);
  model = new DataTable(QStringList() << "Flag" << "Filename" << 
			"Size" << "Date Modified" << "Type");
  view->setModel(model);
  setWidget(view);
  QSettings settings("FreeMat", Interpreter::getVersionString());  
  view->loadSettings(&settings,"filetool/browser",QVector<int>() << 1 << 2 << 3 << 4);
  connect(view,SIGNAL(doubleClicked(const QModelIndex&)), 
   	  this,SLOT(doubleClicked(const QModelIndex&)));
  setObjectName("filetool");
  //  setMinimumSize(50,50);
}

void FileTool::updateDirView(QVariant files) {
  model->loadData(files.toList());
}

void FileTool::doubleClicked(const QModelIndex& index) {
  QModelIndex p = index.sibling(index.row(),0);
  QString rowType = model->data(p).toString();
  p = index.sibling(index.row(),1);
  QString rowName = model->data(p).toString();
  if (rowType == "dir") {
    if (rowName == " .. (Parent Folder)")
      emit sendCommand("cd ..");
    else
      emit sendCommand("cd '" + rowName + "'\n");
  } else {
    QFileInfo fileInfo(rowName);
    if (fileInfo.suffix().toLower() == "dat" ||
	fileInfo.suffix().toLower() == "mat")
      emit sendCommand("load '" + rowName + "'\n");
    else
      emit sendCommand("edit '" + rowName + "'\n");
  }
}
