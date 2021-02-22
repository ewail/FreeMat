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
#ifndef __Editor_hpp__
#define __Editor_hpp__

#include <QTextDocument>
#include <QTextEdit>
#include <QMainWindow>
#include <QTabWidget>
#include <QLabel>
#include <QComboBox>
#include <QDialog>
#include <QCheckBox>
#include <QGroupBox>
#include "highlighter.hpp"
#include "findform.ui.h"
#include "replaceform.ui.h"
#include "Interpreter.hpp"
#include "synlightconf.ui.h"
#include "indentconf.ui.h"
#include "Context.hpp"

class FMFindDialog : public QDialog {
  Q_OBJECT

public:
  FMFindDialog(QWidget *parent = 0);
  void found();
  void notfound();
  void setFindText(QString text);
signals:
  void doFind(QString text, bool backwards, bool sensitive);
private slots:
  void find();

private:
  Ui::FMFindDialog ui;
};

class FMReplaceDialog : public QDialog {
  Q_OBJECT

public:
  FMReplaceDialog(QWidget *parent = 0);
  void setReplaceText(QString text);
  void found();
  void notfound();
  void showrepcount(int cnt);
signals:
  void doFind(QString text, bool backwards, bool sensitive);
  void doReplace(QString text, QString replace, bool backwards, 
		 bool sensitive);
  void doReplaceAll(QString text, QString replace, bool backwards, 
		    bool sensitive);
private slots:
  void replace();
  void replaceAll();
  void find();

private:
  Ui::FMReplaceDialog ui;
};

class FMTextEdit : public QTextEdit {
  Q_OBJECT
  int indentSize;
  bool indentActive;
  bool matchActive;
  long matchingBegin, matchingEnd;
  QColor matchingColor;
  QString plainText, simpleCodes;
  QString Key, matchKey;
public:
  FMTextEdit();
  virtual ~FMTextEdit();
  void keyPressEvent(QKeyEvent *e);
  bool findmatch();
  int  priorMatchedKeyWordPosition();
  void paintEvent(QPaintEvent *event);
  void contextMenuEvent(QContextMenuEvent *e);
  void comment();
  void uncomment();
  void increaseIndent();
  void decreaseIndent();
  void smartIndent();
  bool replace(QString text, QString replace, QTextDocument::FindFlags flags);
  int  replaceAll(QString text, QString replace, QTextDocument::FindFlags flags);
  void fontUpdate();
  QTextCursor getLineCursor( int lineNumber ) const;
protected:
  bool event(QEvent *event);
private slots:
  void slotCursorOrTextChanged();
  void setMatchBracket(bool flag);
  void gotoLine(int lineNumber);
signals:
  void indent();
  void smart_Indent();
  void showDataTips(QPoint pos, QString textSelected);
};

class FMIndent : public QObject {
  Q_OBJECT
  FMTextEdit *m_te;
  int indentSize;
public:
  FMIndent();
  virtual ~FMIndent();
  void setDocument(FMTextEdit *te);
  FMTextEdit *document() const;
private slots:
  void update();
  void updateSelection();
};

class FMEditPane;

class BreakPointIndicator : public QWidget {
  Q_OBJECT
public:
  BreakPointIndicator(FMTextEdit *editor, FMEditPane* pane);
protected:
  virtual void paintEvent(QPaintEvent *);
  virtual void mousePressEvent(QMouseEvent *);
  //  virtual void mouseReleaseEvent(QMouseEvent *);
private:
  FMTextEdit *tEditor;
  FMEditPane *tPane;
};

class FMLineNumber : public QWidget {
  Q_OBJECT
public:
  FMLineNumber(FMTextEdit *editor);
protected:
  virtual void paintEvent(QPaintEvent *);
private:
  FMTextEdit *tEditor;
};

class FMEditPane : public QWidget {
  Q_OBJECT
  FMTextEdit *tEditor;
  QString curFile;
  int curLine;
  Interpreter *m_eval;
public:
  FMEditPane(Interpreter* eval);
  FMTextEdit* getEditor();
  void setFileName(QString filename);
  QString getFileName();
  int getLineNumber();
  Interpreter* getInterpreter();
  void setCurrentLine(int n);
  int currentLine();
signals:
  void gotoLine(int lineNumber);
};

class FMIndentConf : public QDialog {
  Q_OBJECT
public:
  FMIndentConf(QWidget *parent = 0);
private slots:
  void save();
private:
  Ui::FMIndentConf ui;
};

class FMSynLightConf : public QDialog {
  Q_OBJECT

public:
  FMSynLightConf(QWidget *parent = 0);

private slots:
  void setKeywordColor();
  void setCommentColor();
  void setStringColor();
  void setUntermStringColor();
  void save();
private:
  void setLabelColor(QLabel *l, QColor c);
  QColor getColor(QLabel *l);
  void querySetLabelColor(QLabel *l);
  Ui::FMSynLightConf ui;
};

class FMEditor : public QMainWindow {
  Q_OBJECT
  QMenu *fileMenu, *editMenu, *toolsMenu, *debugMenu, *helpMenu;
  QToolBar *editToolBar, *fileToolBar, *debugToolBar;
  QAction *newAct, *saveAct, *quitAct, *copyAct, *pasteAct;
  QAction *cutAct, *fontAct, *openAct, *saveAsAct, *closeAct, *closeAllAct;
  QAction *openNewAct, *findAct, *replaceAct, *commentAct, *uncommentAct;
  QAction *increaseIndentAct, *decreaseIndentAct, *smartIndentAct;
  QAction *helpWinAct, *helpOnSelectionAct, *openSelectionAct;
  QAction *dbStepAct, *dbTraceAct, *dbContinueAct;
  QAction *dbSetClearBPAct, *dbStopAct;
  QAction *redoAct, *undoAct, *colorConfigAct, *indentConfigAct;
  QAction *executeSelectionAct, *executeCurrentAct;
  QAction *separatorAct; 
  enum { MaxRecentFiles = 5 }; 
  QAction *recentFileActs[MaxRecentFiles]; 
  QAction *dataTipConfigAct;
  QAction *bracketMatchConfigAct;
  QAction *saveBeforeRunConfigAct;
  QAction *saveLastSessionConfigAct;
  QTabWidget *tab;
  FMTextEdit *prevEdit;
  QFont m_font;
  FMFindDialog *m_find;
  FMReplaceDialog *m_replace;
  QMenu *m_popup;
  Interpreter *m_eval;
  QMap<QString, QString> varTips;
  bool isShowToolTip;
  bool isMatchBracket;
  bool isSaveBeforeRun;
  bool isSaveLastSession;
  QStringList lastSessionList;
  QStringList pathList;
public:
  FMEditor(Interpreter* eval);
  virtual ~FMEditor();
  void loadFile(const QString& filename);
  void loadOrCreateFile(const QString& filename);
  QString getFullFileName(QString fname);
  void loadLastSession();
private:
  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();
  bool maybeSave();
  bool saveFile(const QString& filename);
  void setCurrentFile(const QString& filename);
  QString strippedName(const QString& fullfilename);
  FMTextEdit *currentEditor();
  FMEditPane *currentPane();
  void setCurrentFilename(QString filename);
  QString currentFilename();
  QString shownName();
  QString shownPath();
  void updateTitles();
  void readSettings();
  void writeSettings();
  void updateFont();
  void updateRecentFileActions();
  bool isFileOpened(const QString &fileName); 
signals:
  void EvaluateText(QString);
  void setMatchBracket(bool flag);
  void checkEditorExist(bool isExist);
protected:
  void contextMenuEvent(QContextMenuEvent *e);
private slots:
  bool save();
  bool saveAs();
  void open();
  void font();
  void addTab();
  void closeTab();
  void closeAllTabs();
  void tabChanged(int);
  void documentWasModified();
  void find();
  void replace();
  void doFind(QString text, bool backwards, bool sensitive);
  void doReplace(QString text, QString replace, 
		 bool backwards, bool sensitive);
  void doReplaceAll(QString text, QString replace, 
		    bool backwards, bool sensitive);
  void comment();
  void uncomment();
  void increaseIndent();
  void decreaseIndent();
  void smartIndent();
  void undo();
  void redo();
  void RefreshBPLists();
  void IllegalLineOrCurrentPath(QString name, int line);
  void dbstep();
  void dbtrace();
  void dbcontinue();
  void dbsetclearbp();
  void dbstop();
  void configcolors();
  void configindent();
  void execSelected();
  void execCurrent();
  void openRecentFile(); 
  void showDataTips(QPoint pos, QString textSelected);
  void configDataTip();
  void configBracketMatch();
  void configSaveBeforeRun();
  void configSaveLastSession();
  void helpWin();
  void helpOnSelection();
  void openSelection();
public:
  void closeEvent(QCloseEvent *event);
  void addTabIfEmpty();
  void addTabUntitled();
public slots:
  void ShowActiveLine(QString name, int line);
  void updateVarView(QVariant);
};

#endif
