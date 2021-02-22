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
#ifndef __MainApp_hpp__
#define __MainApp_hpp__

#include <string>
#include <QObject>
#include <QFileDialog>
#include "KeyManager.hpp"
#include "application.hpp"
#include "Interpreter.hpp"

class MainApp : public QObject
{
  Q_OBJECT
  KeyManager* m_keys;
  QObject *m_term;
  bool guimode;
  bool skipGreeting;
  bool noPrompt;
  ApplicationWindow *m_win;
  Interpreter *m_eval;
  bool GUIHack;
  void CheckNonClosable();
  ScopePtr m_global;
  QStringList basePath, userPath;
  QTimer *profilerTimer, *refreshTimer;
 
public:
  MainApp();
  ~MainApp();
  void SetKeyManager(KeyManager* term);
  void SetSkipGreeting(bool skip);
  void SetNoPrompt(bool noprompt);
  void SetupGUICase();
  void SetupInteractiveTerminalCase();
  void SetupDumbTerminalCase();
  KeyManager* GetKeyManager();
  ApplicationWindow* getApplicationWindow();
  void SetGUIMode(bool mode);
  void TerminalReset();
  Context *NewContext();
  int  StartNewInterpreterThread();
  void UpdatePaths();
  void UpdateBasePath(QString rootpath);
  void SetRootPath(QString path, Interpreter* eval);
  bool debugwin;
  void EnableProfiler( bool bEnable );
public slots:
  int Run();
  void OpenFile();
  void NewFile();
  void HelpWin();
  void PathTool();
  void Editor();
  void ExecuteLine(QString txt);
  void UpdateTermWidth(int);
  void DoGraphicsCall(Interpreter*, FuncPtr f, ArrayVector m, int narg);
  void Quit();
  void Crashed();
  void RegisterInterrupt();
  void EnableRepaint();
  void DisableRepaint();
  void CollectProfileSample();
  void RefreshFigures();
  void showFileAtLine(QString fileName, int lineNumber);
  void ControlProfiler(bool enableflag);
signals:
  void Shutdown();
  void Initialize();
  void SetEnable( bool );
};

#endif
