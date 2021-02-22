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
#include "StackTool.hpp"
#include <QtGui>
#include "DebugStream.hpp"

StackTool::StackTool(QWidget *parent) :
  QWidget(parent) {
  QHBoxLayout *layout = new QHBoxLayout;
  cb = new QComboBox;
  cb->setSizeAdjustPolicy(QComboBox::AdjustToContents);
  layout->addWidget(new QLabel("Stack:"));
  layout->addWidget(cb);
  setLayout(layout);
  connect(cb,SIGNAL(activated(int)),this,SLOT(cbActivated(int)));
  m_activeDepth = 0;
}

void StackTool::cbActivated(int x) {
  emit updateStackDepth(x-m_activeDepth);
  m_activeDepth = x;  
}

void StackTool::updateStackView(QStringList p) {
  if (m_text == p)  return;
  cb->clear();
  m_activeDepth = 0;
  for (int i=0;i<p.size();i++) {
    QString entry = p[i];
    if (entry.startsWith(QChar('*'))) {
      cb->addItem(entry.remove(0,1));
      m_activeDepth = i;
    } else
      cb->addItem(entry);
  }
  cb->setCurrentIndex(m_activeDepth);
  m_text = p;
}
