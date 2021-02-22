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
#ifndef __DataTable_hpp__
#define __DataTable_hpp__

#include <QAbstractTableModel>
#include <QTreeView>
#include <QMenu>
#include <QAction>
#include <QHeaderView>
#include <QSettings>
#include <QMap>
#include <QIcon>

class DataTable : public QAbstractTableModel {
  Q_OBJECT
public:
  DataTable(const QStringList &column_names);
  virtual ~DataTable() {}
  void loadData(const QList<QVariant> &);
  int rowCount(const QModelIndex&) const;
  int columnCount(const QModelIndex&) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  void sort(int column, Qt::SortOrder order);
private:
  int m_columns;
  QList<QVariant> m_data;
  QStringList m_colnames;
  QMap<QString,QIcon> m_icons;
};

class DataHeaderView;

class DataView : public QTreeView {
  Q_OBJECT
public:
  DataView(QWidget* parent) : QTreeView(parent) {}
  virtual ~DataView() {}
  void setModel(QAbstractItemModel *m);
  void loadSettings(QSettings *set, QString tagname, QVector<int> default_cols);
  void saveSettings(QSettings *set, QString tagname);
private:
  QMenu *m_popup;
  QAction **m_fields;
  DataHeaderView *m_header;
}; 

class DataHeaderView : public QHeaderView {
  Q_OBJECT
public:
  DataHeaderView(Qt::Orientation o, DataView *parent) : QHeaderView(o,parent), m_dview(parent) {}
  virtual ~DataHeaderView() {}
  void initializeFieldChooser();
  void contextMenuEvent(QContextMenuEvent *e);
  void setColumnHidden(int col, bool stat);
private:
  QMenu *m_popup;
  QAction **m_fields;
  DataView *m_dview;
private slots:
  void updateChecked(bool);
};

#endif
