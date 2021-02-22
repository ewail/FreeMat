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
#include <QtGui>
#include <QDebug>
#include "HistoryWidget.hpp"
#include "Interpreter.hpp"

HistoryWidget::HistoryWidget(QWidget *parent) : QDockWidget("History",parent) {
  m_flist = new QTreeWidget(this);
  m_flist->header()->hide();
  m_parent = 0;
  setWidget(m_flist);
  readSettings();
  m_parent = new QTreeWidgetItem((QTreeWidget*)0,
				 QStringList("%% " + QDateTime::currentDateTime().toString()));
  m_flist->addTopLevelItem(m_parent);
  connect(m_flist,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
	  this,SLOT(doubleClicked(QTreeWidgetItem*,int)));
  setObjectName("history");
  m_popup = new QMenu;
  m_execute = new QAction("Execute selection",this);
  connect(m_execute,SIGNAL(triggered()),this,SLOT(execute()));
  m_clearall = new QAction("Clear all",this);
  connect(m_clearall,SIGNAL(triggered()),this,SLOT(clearall()));
  m_copy = new QAction("Copy selection",this);
  m_copy->setShortcut(Qt::Key_C | Qt::CTRL);
  connect(m_copy,SIGNAL(triggered()),this,SLOT(copy()));
  m_popup->addAction(m_execute);
  m_popup->addAction(m_clearall);
  m_popup->addAction(m_copy);
  m_flist->setSelectionMode(QAbstractItemView::ExtendedSelection);
  setFocusPolicy(Qt::StrongFocus);
  m_flist->scrollToBottom();
  m_last_added = m_parent;
}

void HistoryWidget::execute() {
  QList<QTreeWidgetItem *>items = m_flist->selectedItems();
  if (items.size() > 0)
    for (int i=0;i<items.size();i++)
      emit sendCommand(items[i]->text(0));
}

void HistoryWidget::clearall() {
  clear();
}

void HistoryWidget::copy() {
  QString txt;
  QList<QTreeWidgetItem *>items = m_flist->selectedItems();
  if (items.size() > 0)
    for (int i=0;i<items.size();i++)
      txt += items[i]->text(0) + "\n";
  QApplication::clipboard()->setText(txt,QClipboard::Clipboard);
}

void HistoryWidget::contextMenuEvent(QContextMenuEvent *e) {
  m_popup->exec(e->globalPos());
}

void HistoryWidget::doubleClicked(QTreeWidgetItem* item, int) {
  emit sendCommand(item->text(0));
}

void HistoryWidget::closeEvent(QCloseEvent *ce) {
  //  writeSettings();
  emit writeHistory();
  ce->accept();
}

void HistoryWidget::addCommand(QString t) {
  if (!t.isEmpty()) {
    if (m_last_added->text(0) == t) return;
    //     if (m_flist->count() >= 1000) {
    //       QTreeWidgetItem *p = m_flist->takeItem(0);
    //       delete p;
    //     }
    m_last_added = new QTreeWidgetItem(m_parent,QStringList(t));
    m_flist->scrollToItem(m_last_added);
    update();
  }
}

void HistoryWidget::readSettings() {
  QSettings settings("FreeMat", Interpreter::getVersionString());
  QStringList historyList = settings.value("interpreter/history").toStringList();
  for (int i=0;i<historyList.size();i++) {
    if (historyList[i].startsWith("%%")) {
      m_parent = new QTreeWidgetItem((QTreeWidget*)0,QStringList(historyList[i]));
      m_flist->addTopLevelItem(m_parent);
      m_flist->expandItem(m_parent);
    } else {
      new QTreeWidgetItem(m_parent,QStringList(historyList[i]));
    }
  }
}

void HistoryWidget::clear() {
  if (QMessageBox::warning(this,"Clear History","Are you sure you want to clear the history?  There is no undo operation",QMessageBox::Yes,QMessageBox::No | QMessageBox::Default) == QMessageBox::Yes) {
    m_flist->clear();
    m_parent = new QTreeWidgetItem((QTreeWidget*)0,QStringList("%% " + QDateTime::currentDateTime().toString()));
    m_flist->addTopLevelItem(m_parent);
    m_last_added = m_parent;
    emit clearHistory();
  }
}


