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
#ifndef __InterpreterThread_hpp__
#define __InterpreterThread_hpp__

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include "Interpreter.hpp"
#include "Context.hpp"

using namespace FreeMat;

class InterpreterThread : public QThread
{
  Q_OBJECT
  Context *context;
  Interpreter *eval;
  QVector<QString> cmd_buffer;
  bool guimode;
  QMutex mutex;
  QWaitCondition condition;

protected:
  void run();
public:
  void ExecuteLine(QString cmd);
  void SetGuiMode(bool t) {guimode = t;}
  Interpreter* GetInterpreter() {return eval;}
  void Setup();
signals:
  void Ready();
};

#endif
