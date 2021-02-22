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

#ifndef __HelpWidget_hpp__
#define __HelpWidget_hpp__

#include <QObject>
#include <QMainWindow>
#include <QDockWidget>
#include <QWebView>
#include <QMenu>
#include <QToolBar>
#include <QAction>
#include <QTabWidget>
#include <QListWidget>
#include <QTreeWidget>
#include <QLineEdit>

class HelpWindow;

class HelpSearcher : public QWidget {
  Q_OBJECT
  public:
  HelpSearcher(QWidget *parent, QString basepath, HelpWindow *mgr);
private:
  QListWidget *m_results_list;
  QLineEdit *m_search_word;
  QString m_basepath;
  QString *searchString;
private slots:
  void updateSearch();
};

class HelpWidget : public QDockWidget {
  Q_OBJECT
public:
  HelpWidget(QString url, HelpWindow *mgr);
  QListWidget *m_flist;
private:
  QTabWidget *m_browser;
};

class HelpWindow : public QMainWindow {
  Q_OBJECT

public:
  HelpWindow(QString baseURL);
  void helpText(QString fulltext);
  QString searchString;
private slots:
  void activateModule(QListWidgetItem* item);
  void activateModule(QTreeWidgetItem* item, int);
  void activateModuleSearch(QListWidgetItem* item);
  void execSelected();
  void helpOnSelection();
signals:
  void EvaluateText(QString);
private:
  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();
  void writeSettings();
  void readSettings();
  void closeEvent(QCloseEvent* ce);

  QWebView *tb;
  HelpWidget *m_helpwidget;
  QMenu *fileMenu, *editMenu, *goMenu, *helpMenu;
  QToolBar *editToolBar;
  QToolBar *goToolBar;
  QAction *copyAct, *exitAct;
  QAction *backAct, *forwardAct, *homeAct;
  QAction *zoominAct, *zoomoutAct;
  QAction *executeSelectionAct, *helpOnSelectionAct;
  QString m_initial;
};

#endif
