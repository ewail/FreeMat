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
#include <QtGui>
#include "DataTable.hpp"

DataTable::DataTable(const QStringList &column_names) :
  QAbstractTableModel(), m_colnames(column_names) {
  m_icons["dir"] = qApp->style()->standardIcon(QStyle::SP_DirIcon);
  m_icons["file"] = qApp->style()->standardIcon(QStyle::SP_FileIcon);
}

int DataTable::rowCount(const QModelIndex&) const {
  return m_data.size();
}

int DataTable::columnCount(const QModelIndex&) const {
  return m_colnames.size();
}

void DataTable::loadData(const QList<QVariant> & data) {
  m_data = data;
  emit layoutChanged();
}

QVariant DataTable::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role != Qt::DisplayRole) return QVariant();
  if (orientation != Qt::Horizontal) return QVariant();
  if (section < m_colnames.size())
    return m_colnames[section];
  return QVariant();
}

QVariant DataTable::data(const QModelIndex &index, int role) const {
  if ((role == Qt::DecorationRole) && (index.column() == 1)) {
    int row = index.row();
    QList<QVariant> md(m_data[row].toList());
    if (md.size() > 0) 
      return QVariant(m_icons[md[0].toString()]);
    else
      return QVariant();
  }
  if (role != Qt::DisplayRole)
    return QVariant();
  int row = index.row();
  int col = index.column();
  if ((row >= 0) && (row < m_data.size())) {
    QList<QVariant> md(m_data[row].toList());
    if ((col >= 0) && (col < md.size()))
      return md[col];
    return QVariant();
  }
  return QVariant();
}

template <typename T>
QList<QVariant> SubsortByFirstColumn(const QMultiMap<T,QVariant> &map) {
  // Get the list of all keys
  QList<T> keys = map.uniqueKeys();
  QList<QVariant> result;
  // Loop over the keys..
  for (int i=0;i<keys.size();i++) {
    // Get a list of the values for this key
    QList<QVariant> valset = map.values(keys[i]);
    // Resort this list by the entry of the first column
    QMultiMap<QString,QVariant> submap;
    for (int i=0;i<valset.size();i++)
      submap.insert(valset[i].toList()[1].toString().toUpper(),valset[i]);
    result += submap.values();
  }
  return result;
}

void DataTable::sort(int column, Qt::SortOrder order) {
  if (m_data.size() == 0) return;
  // Determine the type of the data for this column
  QList<QVariant> row(m_data[0].toList());
  if (column >= row.size()) return;
  switch(row[column].type()) {
  case QVariant::Int:
  case QVariant::Double:
    {
      QMultiMap<double,QVariant> map;
      for (int i=0;i<m_data.size();i++) 
	map.insert(m_data[i].toList()[column].toDouble(),m_data[i]);
      // We want to subsort by the contents of column 1 (which must be 
      // a string
      m_data = SubsortByFirstColumn(map);
      break;
    }
  case QVariant::String:
    {
      QMultiMap<QString,QVariant> map;
      for (int i=0;i<m_data.size();i++)
	map.insert(m_data[i].toList()[column].toString(),m_data[i]);
      m_data = SubsortByFirstColumn(map);
      break;
    }
  case QVariant::DateTime:
    {
      QMultiMap<QDateTime,QVariant> map;
      for (int i=0;i<m_data.size();i++)
	map.insert(m_data[i].toList()[column].toDateTime(),m_data[i]);
      m_data = SubsortByFirstColumn(map);
      break;
    }
  default:
    return;
  }
  if (order == Qt::DescendingOrder) {
    QList<QVariant> tmp;
    for (int i=0;i<m_data.size();i++)
      tmp.push_back(m_data[m_data.size()-1-i]);
    m_data = tmp;
  }
  emit layoutChanged();
}

QVariant mkStats(bool flag, QString nm, int minv, int maxv) {
  QList<QVariant> ret;
  if (flag)
    ret.push_back(QVariant(qApp->style()->standardPixmap(QStyle::SP_FileIcon)));
  else
    ret.push_back(QVariant(qApp->style()->standardPixmap(QStyle::SP_DirIcon)));
  ret.push_back(QVariant(nm));
  ret.push_back(QVariant(minv));
  ret.push_back(QVariant(maxv));
  return QVariant(ret);
}

void DataView::loadSettings(QSettings *set, QString tagname, QVector<int> default_cols) {
  int n = model()->columnCount();
  QList<QVariant> hidden(set->value(tagname+"/hidden").toList());
  QList<QVariant> widths(set->value(tagname+"/widths").toList());
  if ((hidden.size() >= n) || (widths.size() >= n)) {
    for (int i=0;i<n;i++)
      setColumnHidden(i,hidden[i].toBool());
    for (int i=0;i<n;i++)
      setColumnWidth(i,widths[i].toInt());
  } else {
    for (int i=0;i<n;i++)
      setColumnHidden(i,true);
    for (int i=0;i<default_cols.size();i++)
      setColumnHidden(default_cols[i],false);
  }
  ((DataHeaderView*)header())->initializeFieldChooser();
}

void DataView::saveSettings(QSettings *set, QString tagname) {
  int n = model()->columnCount();
  QList<QVariant> hidden;
  for (int i=0;i<n;i++)
    hidden.push_back(QVariant(isColumnHidden(i)));
  set->setValue(tagname + "/hidden",hidden);
  QList<QVariant> widths;
  for (int i=0;i<n;i++)
    widths.push_back(QVariant(columnWidth(i)));
  set->setValue(tagname + "/widths",widths);
}


void DataHeaderView::setColumnHidden(int col, bool stat) {
  if(col <= 1) return;
  m_fields[col-2]->setChecked(!stat);
}

void DataHeaderView::initializeFieldChooser() {
  int col = model()->columnCount();
  m_fields = new QAction*[col-2]; // two columns do not show up -- name & dirflag
  m_popup = new QMenu;
  for (int i=2;i<col;i++) {
    m_fields[i-2] = new QAction(model()->headerData(i,Qt::Horizontal).toString(),this);
    m_fields[i-2]->setCheckable(true);
    m_fields[i-2]->setChecked(!isSectionHidden(i));
    connect(m_fields[i-2],SIGNAL(toggled(bool)),this,SLOT(updateChecked(bool)));
    m_popup->addAction(m_fields[i-2]);
  }
}

void DataHeaderView::contextMenuEvent(QContextMenuEvent *e) {
  m_popup->exec(e->globalPos());
}

void DataHeaderView::updateChecked(bool) {
  for (int i=2;i<model()->columnCount();i++) {
    bool vis = m_fields[i-2]->isChecked();
    m_dview->setColumnHidden(i,!vis);
    if (((QTreeView*)m_dview)->columnWidth(i) < 50)
      m_dview->setColumnWidth(i,50);
  }
}

void DataView::setModel(QAbstractItemModel *m) {
  QTreeView::setModel(m);
  m_header = new DataHeaderView(Qt::Horizontal,this);
  m_header->setMovable(true);
  m_header->setStretchLastSection(true);
  m_header->setDefaultAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  m_header->setModel(m);
  m_header->initializeFieldChooser();
  setHeader(m_header);
  setSortingEnabled(true);
  setItemsExpandable(false);
  setRootIsDecorated(false);
}
