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
#ifndef __Terminal_hpp__
#define __Terminal_hpp__

#include <qglobal.h>
#include <QObject>
#include <QString>
#include <QVector>
#ifdef Q_WS_X11
#include <termios.h>
#endif
#include <vector>
#include <list>
#include <string>

typedef struct {
  const char *sequence;
  int keycode;
} mapping;

#ifdef Q_WS_X11
typedef struct termios Termios;
#else
typedef int Termios;
#endif

class Terminal : public QObject {
  Q_OBJECT
protected:
  int state;
  Termios oldattr;
  Termios newattr;
  const char *term;
  const char *left, *right, *up, *down, *home, *bol;
  const char *clear_eol, *clear_eod;
  const char *u_arrow, *d_arrow, *l_arrow, *r_arrow;
  char *tgetent_buf, *tgetstr_buf;
  char escseq[50];
  mapping *esc_seq_array;
  int esc_seq_count;
  int nline;
public:
  Terminal();
  virtual ~Terminal();
  virtual void Initialize();
  virtual void SetRawMode();
  virtual void RestoreOriginalMode();
  virtual void RetrieveTerminalName();
  const char* LookupControlString(const char *name);
  virtual void SetupControlStrings();
  virtual void ProcessChar(char c);
  virtual void ResizeEvent();
public slots:
  void MoveDown();
  void MoveUp();
  void MoveRight();
  void MoveLeft();
  void ClearEOL();
  void ClearEOD();
  void MoveBOL();
  void OutputRawString(QString txt);
  void DoRead();
  void ClearDisplay();
signals:
  void OnChar(int c);
  void SetTextWidth(int);
  void PlaceCursorDXDY(int, int);
};

#endif
