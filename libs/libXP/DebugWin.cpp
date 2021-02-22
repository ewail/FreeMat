/*
 * Copyright (c) 2008, 2009 Eugene Ingerman, Samit Basu
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
#include "DebugWin.hpp"
#include "DebugStream.hpp"

DebugStream dbout;

DebugWin::DebugWin(QWidget *parent) : QDockWidget("Debug",parent) {
  m_list = new QListWidget(this);
  m_list->setFocusPolicy(Qt::StrongFocus);
  m_list->setWordWrap(true);
  m_list->setSelectionMode(QAbstractItemView::ContiguousSelection);
  m_popup = new QMenu;
  m_selectall = new QAction("Select all",this);
  connect(m_selectall,SIGNAL(triggered()),this,SLOT(selectall()));
  m_copy = new QAction("Copy selection",this);
  m_copy->setShortcut(Qt::Key_C | Qt::CTRL);
  connect(m_copy,SIGNAL(triggered()),this,SLOT(copy()));
  m_popup->addAction(m_selectall);
  m_popup->addAction(m_copy);
  setWidget(m_list);
  setObjectName("Debug");
  setFocusPolicy(Qt::StrongFocus);
  dbout.setWin(this);
}

void DebugWin::addString(const QString &t) {
  QStringList list = t.split(QRegExp("\\n"));
  for (int i=0;i<list.size();i++)
    if (list[i].size() > 0) {
      m_list->addItem(list[i].replace('\r',""));
    }
  m_list->scrollToBottom();
  while (m_list->count() > 10000)
    delete m_list->takeItem(0);
}

void DebugWin::selectall() {
  m_list->selectAll();
}

void DebugWin::copy() {
  QString txt;
  QList<QListWidgetItem *>items = m_list->selectedItems();
  if (items.size() > 0)
    for (int i=0;i<items.size();i++)
      txt += items[i]->text() + "\n";
  QApplication::clipboard()->setText(txt,QClipboard::Clipboard);    
}

void DebugWin::contextMenuEvent(QContextMenuEvent *e) {
  m_popup->exec(e->globalPos());
}
