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
#include <QDir>
#include <QtGui>
#include <QDebug>
#if !defined(_MSC_VER ) 
#include <unistd.h>
#else
#include <fstream>
#include <iostream>
#endif
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "MainApp.hpp"
#include <qapplication.h>
#include "Exception.hpp"
#include "application.hpp"
#include "FuncMode.hpp"
#include "ScriptMode.hpp"


MainApp *m_app;
FuncMode *m_func;
ScriptMode *m_script;

extern int __global_return_value;

void usage() {
  printf("%s\n  Command Line Help\n",qPrintable(Interpreter::getVersionString()));
  printf(" You can invoke FreeMat with the following command line options:\n");
  printf("     -f <command>  Runs FreeMat in command mode.  FreeMat will \n");
  printf("                   startup, run the given command, and then quit.\n");
  printf("                   Note that this option uses the remainder of the\n");
  printf("                   command line, so use it last.\n");
#ifdef Q_WS_X11
  printf("     -nogui        Suppress the GUI for FreeMat.\n");
#endif
  printf("     -noX          Disables the graphics subsystem.\n");
  printf("     -nogreet      Skips the greeting when starting.\n");
  printf("     -t            Test mode -- by default FreeMat will\n");
  printf("                   exit with code 1 in this mode.\n");
  printf("     -e            uses a dumb terminal interface \n");
  printf("                   (no command line editing, etc.)\n");
  printf("                   This flag is primarily used when \n");
  printf("                   you want to capture input/output\n");
  printf("                   to FreeMat from another application.\n");
  printf("     -s <file>     Script mode.  If you put a #!FreeMat -s at the\n");
  printf("                   beginning of a .m file, and make it executable\n");
  printf("                   in Unix-like OSes, FreeMat will start up, execute\n");
  printf("                   the script, and quit automatically.\n");
  printf("     -i <path>     Install FreeMat - provide the path to the\n");
  printf("                   FreeMat data directory (containing the\n");
  printf("                   scripts, help and other files.).  Normally\n");
  printf("                   these are installed in /usr/local/share/\n");
  printf("                   but regardless, you must run FreeMat -i once\n");
  printf("                   to indicate the location of this directory.\n");
  printf("                   Note that in this mode, FreeMat will only \n");
  printf("                   update its internal configuration and then\n");
  printf("                   exit.\n");
  printf("     -p <path>     Set the FreeMat path to the given pathspec.\n");
  printf("     -r <path>     Set the FreeMat root path to the given pathspec.\n");
  printf("     -help         Get this help text\n");
  exit(0);
}


// Search through the arguments to freemat... look for the given
// flag.  if the flagarg variable is true, then an argument must
// be provided to the flag.  If the flag is not found, then a 
// 0 is returned.  Otherwise, the index into argv of the flag is
// returned.
int parseFlagArg(int argc, char *argv[], const char* flagstring, bool flagarg) {
  bool flagFound = false;
  int ndx;
  ndx = 1;
  while (!flagFound && ndx < argc) {
    flagFound = strcmp(argv[ndx],flagstring) == 0;
    if (!flagFound) ndx++;
  }
  if (flagFound && flagarg && (ndx == argc-1)) {
    fprintf(stderr,"Error: flag %s requires an argument!\n",flagstring);
    exit(1);
  }
  if (!flagFound)
    ndx = 0;
  return ndx;
}

void sigDoNothing(int arg) {
}

int main(int argc, char *argv[]) {  
  QCoreApplication *app;
  int nogui = parseFlagArg(argc,argv,"-nogui",false);
  int dumbTerminal = parseFlagArg(argc,argv,"-e",false); 
  int noX = parseFlagArg(argc,argv,"-noX",false);
  int help = parseFlagArg(argc,argv,"-help",false);
  int help2 = parseFlagArg(argc,argv,"--help",false);
  int funcMode = parseFlagArg(argc,argv,"-f",true);
  int nogreet = parseFlagArg(argc,argv,"-nogreet",false);
  int installMode = parseFlagArg(argc,argv,"-i",true);
  int pathMode = parseFlagArg(argc,argv,"-p",true);
  int scriptMode = parseFlagArg(argc,argv,"-s",true);
  int rootpathMode = parseFlagArg(argc,argv,"-r",true);
  bool testMode = parseFlagArg(argc,argv,"-t",false);

  signal(SIGINT,sigDoNothing);
  signal(SIGFPE,sigDoNothing);
  
  if (testMode)
    __global_return_value = 1;
  else
    __global_return_value = 0;

  if (installMode) {
    app = new QCoreApplication(argc, argv);
    QSettings settings("FreeMat", Interpreter::getVersionString());
    settings.setValue("root",argv[installMode+1]);
    dbout << "FreeMat root path set to '" << argv[installMode+1] << "'\n";
    return 0;
  }

  if (help || help2) usage();
  if (!noX) {
    app = new QApplication(argc, argv);
  } else {
    app = new QCoreApplication(argc, argv);
    nogui = true;
  }
  
  QLocale::setDefault( QLocale::C );


  if (pathMode) {
    QSettings settings("FreeMat", Interpreter::getVersionString());
    settings.setValue("interpreter/path",QString::fromStdString(argv[pathMode+1]).split(":"));
  }

  if (rootpathMode) {
    QSettings settings("FreeMat", Interpreter::getVersionString());
    settings.setValue("root",QString::fromStdString(argv[rootpathMode+1]));
  }

  if (dumbTerminal) nogui = 1;
  if (scriptMode) 
    {
      nogui = 1;
      nogreet = 1;
      funcMode = 0;
    }
  m_app = new MainApp;
  if (!nogui)
    m_app->SetupGUICase();
  else if (!dumbTerminal) 
    m_app->SetupInteractiveTerminalCase();
  else
    m_app->SetupDumbTerminalCase();
  m_app->SetGUIMode(!noX);
  m_app->SetSkipGreeting(nogreet);
  if (scriptMode)
    m_app->SetNoPrompt(true);
  m_app->Run();
  //  QTimer::singleShot(0,m_app,SLOT(Run()));
  // In function mode, we need to send a command to the GUI
  if (funcMode) {
    m_func = new FuncMode(argv[funcMode+1]);
    QObject::connect(m_func,SIGNAL(SendCommand(QString)),
 		     m_app->GetKeyManager(),SLOT(QueueSilent(QString)));
    QTimer::singleShot(0,m_func,SLOT(Fire()));
  }
  if (scriptMode) {
    m_script = new ScriptMode(argv[scriptMode+1]);
    QObject::connect(m_script,SIGNAL(SendCommand(QString)),
 		     m_app->GetKeyManager(),SLOT(QueueSilent(QString)));
    QTimer::singleShot(0,m_script,SLOT(Fire()));
  }
  app->exec();
  return __global_return_value;
}
