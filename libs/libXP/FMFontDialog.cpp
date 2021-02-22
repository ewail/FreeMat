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
#include "FMFontDialog.hpp"
#include <QtGui>

FMFontDialog::FMFontDialog(const QFont &ref, QWidget *parent) : QDialog(parent) {
  ui.setupUi(this);
  connect(ui.namelist,SIGNAL(currentTextChanged(const QString&)),this,SLOT(namePicked(const QString&)));
  connect(ui.namelist,SIGNAL(currentTextChanged(const QString&)),this,SLOT(updateSample()));
  connect(ui.sizelist,SIGNAL(currentTextChanged(const QString&)),this,SLOT(sizePicked(const QString&)));
  connect(ui.sizelist,SIGNAL(currentTextChanged(const QString&)),this,SLOT(updateSample()));
  // Populate the font list with font names
  QStringList families(fdb.families());
  for (int i=0;i<families.size();i++) {
    if (fdb.isFixedPitch(families[i]))
      new QListWidgetItem(families[i],ui.namelist);
  }
  name = ref.family();
  size = ref.pointSize();
  QList<QListWidgetItem*> findset = ui.namelist->findItems(name,Qt::MatchContains);
  if (findset.size() > 0)
    ui.namelist->setCurrentItem(findset[0]);
};

QFont FMFontDialog::font() {
  return QFont(name,size);
}

void FMFontDialog::namePicked(const QString &itemtext) {
  ui.sizelist->clear();
  QList<int> pointSizes(fdb.pointSizes(itemtext));
  for (int i=0;i<pointSizes.size();i++) {
    new QListWidgetItem(QString::number(pointSizes[i]),ui.sizelist);
  }
  name = itemtext;
  QList<QListWidgetItem*> findset = ui.sizelist->findItems(QString::number(size),Qt::MatchFixedString);
  if (findset.size() > 0)
    ui.sizelist->setCurrentItem(findset[0]);
  else
    ui.sizelist->setCurrentRow(0);
}

void FMFontDialog::sizePicked(const QString &itemtext) {
  if (itemtext.toInt() > 0)
    size = itemtext.toInt();
}

void FMFontDialog::updateSample() {
  ui.sample->setFont(QFont(name,size));
}
