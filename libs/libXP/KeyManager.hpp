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

#ifndef __KeyManager_hpp__
#define __KeyManager_hpp__

#include <list>
#include "Scope.hpp"
#include <QEventLoop>
#include "Context.hpp"

#define KM_CTRLA     0x01
#define KM_CTRLB     0x02
#define KM_CTRLZ     0x1A
#define KM_CTRLD     0x04
#define KM_CTRLE     0x05
#define KM_TAB       0x09
#define KM_CTRLK     0x0b
#define KM_CTRLW     0x17
#define KM_CTRLY     0x19
#define KM_BACKSPACE 0x08
#define KM_BACKSPACEALT 0x7f
#define KM_ESC       0x1b
#define KM_NEWLINE   0x0d
#define KM_LEFT      0x101
#define KM_RIGHT     0x102
#define KM_UP        0x103
#define KM_DOWN      0x104
#define KM_DELETE    0x108
#define KM_INSERT    0x105
#define KM_HOME      0x106
#define KM_END       0x107

class KeyManager : public QObject
{
  Q_OBJECT
public:
  KeyManager();
  virtual ~KeyManager();
  void Redisplay();
  void setTerminalWidth(int w);
  void AddCharToLine(QChar c);
  void ReplacePrompt(QString prmt);
  int DisplayPrompt();
  void OutputChar(QChar c, QChar pad);
  void OutputString(QString msg, QChar c);
  void TerminalMove(int n);
  void SetTermCurpos(int n);
  void PlaceCursor(int n);
  int DisplayedCharWidth(QChar c, int aterm_curpos);
  int DisplayedStringWidth(QString s, int nc, int offset);
  int BuffCurposToTermCurpos(int n);
  void DeleteChars(int nc, int cut);
  void TruncateDisplay();
  void EndOfLine();
  void KillLine();
  int getTerminalWidth();
  void RegisterTerm(QObject* term);
  void SetCompletionContext(Context* ctxt);
  Context* GetCompletionContext();
  void getKeyPress();
 private:
  Context *context;
  StringVector GetCompletions(QString line, int word_end, 
			      QString &matchString);
  void CursorLeft();
  void CursorRight();
  void BeginningOfLine();
  void BackwardDeleteChar();
  void ForwardDeleteChar();

  void HistorySearchBackward();
  void HistorySearchForward();
  void AddHistory(QString line);
  void HistoryFindForwards();
  void HistoryFindBackwards();
  void AddStringToLine(QString s);
  void SearchPrefix(QString aline, int alen);
  void Yank();
  void ClearCurrentLine();
  void ListCompletions(StringVector completions);
  void CompleteWord();
 protected:
  void EraseCharacters(int pos, int count);
  void InsertCharacter(int pos, QChar c);
  void SetCharacter(int pos, QChar c);
  void InsertString(int pos, QString s);
  
  void NewLine();
  void ResetLineBuffer();
  StringVector enteredLines;  
  bool enteredLinesEmpty;  
  // the size (in text coords) of the window
  int nline;
  int ncolumn;
  // the text
  StringVector history;
  // The new line buffer
  QString lineData;
  // The maximum allowed line length
  int linelen;
  // The cut buffer
  QString cutbuf;
  // number of characters in line
  int ntotal;
  // Current position in the buffer
  int buff_curpos;
  // Current position on the terminal
  int term_curpos;
  // Number of characters used to display the current input line
  int term_len;
  // Marker location in the buffer
  int buff_mark;
  // Cursor position at start of insert
  int insert_curpos;
  int keyseq_count;
  int last_search;
  QString prefix;
  int prefix_len;
  // True in insert mode
  bool insert;
  int startsearch;
  // The prompt
  QString prompt;
  // length of the prompt string
  int prompt_len;
  // Are we waiting for input?
  int loopactive;
  // Are we waiting for a key press
  bool keypresswait;
signals:
  void CWDChanged();
  void UpdateCWD(QString);
  void updateDirView(QVariant);
  void updateVarView(QVariant);
  void updateStackView(QStringList);
  void MoveDown();
  void MoveUp();
  void MoveRight();
  void MoveLeft();
  void ClearEOL();
  void ClearEOD();
  void MoveBOL();
  void ClearDisplay();
  void OutputRawString(QString txt);
  void SendCommand(QString);
  void Interrupt();
  void UpdateInfoViews();
  void ExecuteLine(QString txt);
  void PopCompletions(QStringList txt);
public slots:
  void Ready();
  void ClearHistory();
  void WriteHistory();
  void OnChar( int c );
  void SetTermWidth(int w);
  void QueueString(QString);
  void QueueMultiString(QString);
  void QueueCommand(QString);
  void QueueSilent(QString);
  void ContinueAction();
  void StopAction();
  void DbStepAction();
  void DbTraceAction();
  void SetPrompt(QString);
  void ClearDisplayCommand();
  void ChangeDir(const QString& dir);
  void PlaceCursorDXDY(int dx, int dy);
  void updateStackDepth(int);
signals:
  void UpdateTermWidth(int);
  void RegisterInterrupt();
};

#endif
