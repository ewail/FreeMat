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
#ifndef __HistoryWidget_hpp__
#define __HistoryWidget_hpp__

#include <QWidget>
#include <QTreeWidget>
#include <QDockWidget>
#include <QAction>

class HistoryWidget : public QDockWidget {
  Q_OBJECT
public:
  HistoryWidget(QWidget *parent);
  void clear();
private:
  QTreeWidget *m_flist;
  QTreeWidgetItem *m_parent;
  QTreeWidgetItem *m_last_added;
  QMenu *m_popup;
  QAction *m_execute, *m_clearall, *m_copy;
  void readSettings();
protected:
  void contextMenuEvent(QContextMenuEvent *e);
  void closeEvent(QCloseEvent*);
public slots:
  void addCommand(QString t);
  void doubleClicked(QTreeWidgetItem* item, int column);
  void execute();
  void clearall();
  void copy();
signals:
  void sendCommand(QString t);
  void clearHistory();
  void writeHistory();
};

#endif
