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
#include <QtCore>
#include "InterpreterThread.hpp"
#include "Context.hpp"
#include "Class.hpp"
#include "LoadCore.hpp"
#include "LoadFN.hpp"
#include "HandleCommands.hpp"
#include "Module.hpp"
#include "Core.hpp"
#include "Common.hpp"
#include <QtGui>

using namespace FreeMat;

void InterpreterThread::run() {
  //  qDebug("interpreter thread on standby...\n");
  eval->sendGreeting();
  emit Ready();
  forever {
    QString cmdline;
    mutex.lock();
    if (cmd_buffer.empty())
      condition.wait(&mutex);
    cmdline = *cmd_buffer.begin();
    cmd_buffer.erase(cmd_buffer.begin());
    mutex.unlock();
    eval->ExecuteLine(cmdline);
    emit Ready();
  }
}

void InterpreterThread::Setup() {
  Context *context = new Context;
  LoadModuleFunctions(context);
  LoadClassFunction(context);
  LoadCoreFunctions(context);
  LoadFNFunctions(context);
  if (guimode) {
    LoadGUICoreFunctions(context);
    LoadHandleGraphicsFunctions(context);  
  }
  QStringList basePath;
  if (inBundleMode()) {
    QDir dir(QApplication::applicationDirPath());
    dir.cdUp();
    dir.cd("Plugins");
    QString dummy(dir.absolutePath());
    QApplication::setLibraryPaths(QStringList(dir.absolutePath()));
    QDir dir1(qApp->applicationDirPath() + "/../Resources/toolbox");
    if (dir1.exists()) {
      QString path1(dir1.canonicalPath());
      basePath += GetRecursiveDirList(path1);
    }
    QDir dir2(qApp->applicationDirPath() + "/../Resources/help/text");
    if (dir2.exists()) {
      QString path2(dir2.canonicalPath());
      basePath += GetRecursiveDirList(path2);
    }
  } else {
    QDir dir1(QApplication::applicationDirPath()+"/"+
	      QString(BASEPATH)+"/toolbox");   
    if (dir1.exists()) {
      QString path1(dir1.canonicalPath());
      basePath += GetRecursiveDirList(path1);
    }
    QDir dir2(QApplication::applicationDirPath()+"/"+
	      QString(BASEPATH) + "/help/text");
    if (dir2.exists()) {
      QString path2(dir2.canonicalPath());
      basePath += GetRecursiveDirList(path2);
    }
  }
  QSettings settings("FreeMat", Interpreter::getVersionString());
  QStringList userPath = settings.value("interpreter/path").toStringList();
  eval = new Interpreter(context);
  eval->setBasePath(basePath);
  eval->setUserPath(userPath);
  eval->setAppPath(qApp->applicationDirPath().toStdString());
  eval->rescanPath();
}

void InterpreterThread::ExecuteLine(QString cmd) {
  mutex.lock();
  cmd_buffer.push_back(cmd);
  condition.wakeAll();
  mutex.unlock();
}
