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
#ifndef __HandleUIControl_hpp__
#define __HandleUIControl_hpp__

#include "HandleObject.hpp"
#include <QObject>

class HandleWindow;
class Interpreter;

class HandleUIControl : public QObject, public HandleObject {
  Q_OBJECT
  QWidget *widget;
  Interpreter *m_eval;
  HandleWindow *parentWidget;
public:
  HandleUIControl();
  virtual ~HandleUIControl();
  virtual void ConstructProperties();
  virtual void SetupDefaults();
  virtual void UpdateState();
  virtual void PaintMe(RenderEngine&) {};
  void Hide();
  void ConstructWidget(HandleWindow *f);
  void SetEvalEngine(Interpreter *eval);
private slots:
void clicked();
};
  

#endif
