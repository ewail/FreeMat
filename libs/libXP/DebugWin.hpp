/*
 * Copyright (c) 2008-2009 Eugene Ingerman, Samit Basu
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
#ifndef __DebugWin_hpp__
#define __DebugWin_hpp__

#include <QWidget>
#include <QListWidget>
#include <QMenu>
#include <QDockWidget>

class DebugWin : public QDockWidget {
  Q_OBJECT
public:
  DebugWin(QWidget *parent);
  void clear();
protected:
  void contextMenuEvent(QContextMenuEvent *e);
private:
  QListWidget *m_list;
  QMenu *m_popup;
  QAction *m_selectall;
  QAction *m_copy;		      
protected slots:
  void addString(const QString &t);
public slots:
  void selectall();
  void copy();
};

#endif
