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
#include "PathTool.hpp"
#include "Common.hpp"
#include <QtGui>
#include "Interpreter.hpp"

PathTool::PathTool() {
  QHBoxLayout *lay = new QHBoxLayout(this);
  splitter = new QSplitter(this);
  lay->addWidget(splitter);
  setWindowIcon(QPixmap(":/images/freemat_small_mod_64.png"));

  model = new QDirModel;
  model->setFilter(QDir::AllDirs);

  tree = new QTreeView(splitter);
  tree->setModel(model);
  tree->setColumnHidden(1,true);
  tree->setColumnHidden(2,true);
  tree->setColumnHidden(3,true);

  m_flist = new QListWidget(splitter);

  QWidget *buttonpane = new QWidget;
  QVBoxLayout *blayout = new QVBoxLayout(buttonpane);
  QPushButton *add = new QPushButton("Add");
  QPushButton *addsub = new QPushButton("Add With Subfolders");
  QPushButton *remove = new QPushButton("Remove");
  QPushButton *up = new QPushButton("Move Up");
  QPushButton *down = new QPushButton("Move Down");
  QPushButton *save = new QPushButton("Save");
  QPushButton *done = new QPushButton("Done");
  blayout->addWidget(add);
  blayout->addWidget(addsub);
  blayout->addWidget(remove);
  blayout->addWidget(up);
  blayout->addWidget(down);
  blayout->addWidget(save);
  blayout->addWidget(done);

  lay->addWidget(buttonpane);

  setLayout(lay);

  connect(add,SIGNAL(clicked()),this,SLOT(add()));
  connect(addsub,SIGNAL(clicked()),this,SLOT(addsub()));
  connect(remove,SIGNAL(clicked()),this,SLOT(remove()));
  connect(up,SIGNAL(clicked()),this,SLOT(up()));
  connect(down,SIGNAL(clicked()),this,SLOT(down()));
  connect(save,SIGNAL(clicked()),this,SLOT(save()));
  connect(done,SIGNAL(clicked()),this,SLOT(alldone()));
  // Populate the list based on the current path...
  QSettings settings("FreeMat", Interpreter::getVersionString());
  QStringList path = settings.value("interpreter/path").toStringList();
  for (int i=0;i<path.size();i++)
    new QListWidgetItem(path[i],m_flist);
  readSettings();
  modified = false;
}

void PathTool::readSettings() {
  QSettings settings("FreeMat", Interpreter::getVersionString());
  QPoint gpos = settings.value("pathtool/pos", QPoint(200, 200)).toPoint();
  QSize gsize = settings.value("pathtool/size", QSize(650, 400)).toSize();
  int size0 = settings.value("pathtool/splitter/left", 250).toInt();
  int size1 = settings.value("pathtool/splitter/right", 250).toInt();
  QList<int> sizes;
  sizes << size0 << size1;
  resize(gsize);
  move(gpos);
  splitter->setSizes(sizes);
}

void PathTool::writeSettings() {
  QSettings settings("FreeMat", Interpreter::getVersionString());
  settings.setValue("pathtool/pos", pos());
  settings.setValue("pathtool/size", size());
  settings.setValue("pathtool/splitter/left", splitter->sizes()[0]);
  settings.setValue("pathtool/splitter/right", splitter->sizes()[1]);
  settings.sync();
}

void PathTool::add() {
  QItemSelectionModel *select = tree->selectionModel();
  new QListWidgetItem(model->filePath(select->currentIndex()),m_flist);
  modified = true;
}

void PathTool::addsub() {
  QItemSelectionModel *select = tree->selectionModel();
  QString basePath(model->filePath(select->currentIndex()));
  QStringList dirlist(GetRecursiveDirList(basePath));
  for(int i=0;i<dirlist.size();i++) 
    new QListWidgetItem(dirlist[i],m_flist);
  modified = true;
}

void PathTool::remove() {
  QList<QListWidgetItem*> todelete(m_flist->selectedItems());
  for (int i=0;i<todelete.size();i++) {
    m_flist->takeItem(m_flist->row(todelete[i]));
    delete todelete[i];
  }
  modified = true;
}

void PathTool::up() {
  QList<QListWidgetItem*> todelete(m_flist->selectedItems());
  if (todelete.size() > 0) {
    int rownum = m_flist->row(todelete[0]);
    m_flist->takeItem(rownum);
    m_flist->insertItem(qMax(0,rownum-1),todelete[0]);
    m_flist->setCurrentItem(todelete[0]);
  }
  modified = true;
}

void PathTool::down() {
  QList<QListWidgetItem*> todelete(m_flist->selectedItems());
  if (todelete.size() > 0) {
    int rownum = m_flist->row(todelete[0]);
    m_flist->takeItem(rownum);
    m_flist->insertItem(qMin(m_flist->count(),rownum+1),todelete[0]);
    m_flist->setCurrentItem(todelete[0]);
  }
  modified = true;
}

void PathTool::save() {
  QSettings settings("FreeMat", Interpreter::getVersionString());
  QStringList pathList;
  for (int i=0;i<m_flist->count();i++) {
    QListWidgetItem *item = m_flist->item(i);
    pathList << item->text();
  }
  settings.setValue("interpreter/path",pathList);
  settings.sync();
  modified = false;
}

void PathTool::alldone() {
  if (modified) {
    int ret = QMessageBox::warning(this, tr("FreeMat"),
				   "The path list has been modified.\n"
				   "Do you want to save your changes?",
				   QMessageBox::Yes | QMessageBox::Default,
				   QMessageBox::No,
				   QMessageBox::Cancel | QMessageBox::Escape);
    if (ret == QMessageBox::Yes)
      save();
    else if (ret == QMessageBox::Cancel)
      return;
  }
  writeSettings();
  close();
}

