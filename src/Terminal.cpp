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
#include <qapplication.h>
#include "Serialize.hpp"
#include "Terminal.hpp"
#include <errno.h>
#if !defined(_MSC_VER ) 
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef Q_WS_X11
#include <term.h>
#include <curses.h>
#include <sys/ioctl.h>
#include "Exception.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <glob.h>
#endif
#include "File.hpp"

#define KM_ESC       0x1b

// Automake on OS X does not support conditionals in sources... so we
// have the braindead solution of ifdef-ing out all the stuff that
// will choke. Sigh.

// Set up the terminal in raw mode, and initialize the control
// strings.
Terminal::Terminal() {
}

void Terminal::Initialize() {
  RetrieveTerminalName();
  SetRawMode();
  SetupControlStrings();
  ResizeEvent();
  //    SetInterface(this);
  state = 0;
}

Terminal::~Terminal() {
}

void Terminal::SetRawMode() {
#ifdef Q_WS_X11
  tcgetattr(STDIN_FILENO, &oldattr);
  newattr = oldattr;
  newattr.c_lflag &= ~(ECHO | ICANON | IEXTEN);
  newattr.c_iflag &= ~(ICRNL | INPCK | ISTRIP);
  newattr.c_cflag &= ~(CSIZE | PARENB);
  newattr.c_cflag |= CS8;
  newattr.c_oflag &= ~(OPOST);  
  newattr.c_cc[VMIN] = 1;
  newattr.c_cc[VTIME] = 0;
  while (tcsetattr(STDIN_FILENO, TCSADRAIN, &newattr)) {
    if (errno != EINTR) 
      throw Exception(QString("Unable to set up terminal attributes: tcsetattr error:") + QString::fromStdString(strerror(errno)));
  }
#endif
}
  
void Terminal::RestoreOriginalMode() {
#ifdef Q_WS_X11
  // Restore the original terminal setttings
  while (tcsetattr(STDIN_FILENO, TCSADRAIN, &oldattr)) {
    if (errno != EINTR)
      throw Exception(QString("Unable to set up terminal attributes: tcsetattr error:") + QString::fromStdString(strerror(errno)));
  }
#endif
}

void Terminal::RetrieveTerminalName() {
#ifdef Q_WS_X11
  term = getenv("TERM");
  if (!term)
    throw Exception("Unable to retrieve terminal name!");
  if (setupterm((char*) term, STDIN_FILENO, NULL) == ERR)
    throw Exception(QString("Unable to retrieve terminal info for ") + term);
#endif
}

const char* Terminal::LookupControlString(const char *name) {
#ifdef Q_WS_X11
  const char* value = tigetstr((char*)name);
  if (!value || value == (char*) -1)
    return NULL;
  else
    return strdup(value);
#else
	return NULL;
#endif
}

void Terminal::SetupControlStrings() {
#ifdef Q_WS_X11
  left = LookupControlString("cub1");
  right = LookupControlString("cuf1");
  up = LookupControlString("cuu1");
  down = LookupControlString("cud1");
  home = LookupControlString("home");
  clear_eol = LookupControlString("el");
  clear_eod = LookupControlString("ed");
  u_arrow = LookupControlString("kcuu1");
  d_arrow = LookupControlString("kcud1");
  l_arrow = LookupControlString("kcub1");
  r_arrow = LookupControlString("kcuf1");
  const char* delstr = LookupControlString("kdch1");
  // Store these in the esc_seq_array for later lookup
  esc_seq_array = (mapping*) malloc(13*sizeof(mapping));
  esc_seq_count = 13;
  // These are the ones provided by terminfo
  esc_seq_array[0].sequence = l_arrow;
  esc_seq_array[0].keycode = 0x101;
  esc_seq_array[1].sequence = r_arrow;
  esc_seq_array[1].keycode = 0x102;
  esc_seq_array[2].sequence = u_arrow;
  esc_seq_array[2].keycode = 0x103;
  esc_seq_array[3].sequence = d_arrow;
  esc_seq_array[3].keycode = 0x104;
  // These are defaults...
  esc_seq_array[4].sequence = "\033[D";
  esc_seq_array[4].keycode = 0x101;
  esc_seq_array[5].sequence = "\033[C";
  esc_seq_array[5].keycode = 0x102;
  esc_seq_array[6].sequence = "\033[A";
  esc_seq_array[6].keycode = 0x103;
  esc_seq_array[7].sequence = "\033[B";
  esc_seq_array[7].keycode = 0x104;
  // These are defaults...
  esc_seq_array[8].sequence = "\033OD";
  esc_seq_array[8].keycode = 0x101;
  esc_seq_array[9].sequence = "\033OC";
  esc_seq_array[9].keycode = 0x102;
  esc_seq_array[10].sequence = "\033OA";
  esc_seq_array[10].keycode = 0x103;
  esc_seq_array[11].sequence = "\033OB";
  esc_seq_array[11].keycode = 0x104;
  // The delete character...
  esc_seq_array[12].sequence = delstr;
  esc_seq_array[12].keycode = 0x108;
#endif
}

// Translate the given character (which is a raw
// character) - this traps escape sequences and
// maps them to integer constants (as expected
// by, e.g. KeyManager).
void Terminal::ProcessChar(char c) {
  // Check our current state, if it is zero, we are not
  // in the middle of an escape sequence.  
  if (state == 0) {
    if (c != KM_ESC) {
      emit OnChar(c);
      return;
    } else {
      escseq[0] = KM_ESC;
      state = 1;
      return;
    }
  }
  // We are in the middle of an escape sequence.
  // Append the current character to the escseq
  // buffer, increase the state number.
  escseq[state] = c; escseq[state+1] = 0;
  state++;
  // Now we check against the contents of esc-seq-array for
  // a match
  int matchnum = 0;
  bool isprefix = false;
  bool matchfound = false;
  while (!matchfound && (matchnum < esc_seq_count)) {
    matchfound = (strcmp(escseq,esc_seq_array[matchnum].sequence)==0);
    isprefix = isprefix | (strncmp(escseq,esc_seq_array[matchnum].sequence,state)==0);
    if (!matchfound) matchnum++;
  }
  // Did we find a match? If so, convert to an extended key
  // code (by adding 0x101), reset state to 0, and call the callback.
  if (matchfound) {
    state = 0;
    emit OnChar(esc_seq_array[matchnum].keycode);
    return;
  }
  // Not a prefix of any known codes... ignore it
  if (!isprefix) {
    state = 0;
  }
}

// The terminal has been resized - calculate the 
void Terminal::ResizeEvent() {
#ifdef Q_WS_X11
  struct winsize size;  /* The new size information */
  int ncolumns = 0;
  /*
   * Query the new terminal window size. Ignore invalid responses.
   */
  if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == 0 &&
     size.ws_row > 0 && size.ws_col > 0) {
    nline = size.ws_row;
    ncolumns = size.ws_col;
  };
  emit SetTextWidth(ncolumns);
#endif
}

void Terminal::MoveDown() {
#ifdef Q_WS_X11
  tputs(down,1,putchar);
  fflush(stdout);
#endif
}

void Terminal::MoveUp() {
#ifdef Q_WS_X11
  tputs(up,1,putchar);
  fflush(stdout);
#endif
}

void Terminal::MoveRight() {
#ifdef Q_WS_X11
  tputs(right,1,putchar);
  fflush(stdout);
#endif
}

void Terminal::MoveLeft() {
#ifdef Q_WS_X11
  tputs(left,1,putchar);
  fflush(stdout);
#endif
}

void Terminal::ClearEOL() {
#ifdef Q_WS_X11
  tputs(clear_eol,1,putchar);
  fflush(stdout);
#endif
}

void Terminal::ClearEOD() {
#ifdef Q_WS_X11
  tputs(clear_eod,nline,putchar);
  fflush(stdout);
#endif
}

void Terminal::MoveBOL() {
#ifdef Q_WS_X11
  //  tputs(home,1,putchar);
  putchar('\r');
  fflush(stdout);
#endif
}

void Terminal::OutputRawString(QString txt) {
#ifdef Q_WS_X11
  int ndone = 0;   /* The number of characters written so far */
  /*
   * How long is the string to be written?
   */
  int slen = txt.size();
  /*
   * Attempt to write the string to the terminal, restarting the
   * write if a signal is caught.
   */
  while(ndone < slen) {
    int nnew = write(STDOUT_FILENO, qPrintable(txt) + ndone, 
		     sizeof(char)*(slen-ndone));
    if(nnew > 0)
      ndone += nnew;
    else if(errno != EINTR && errno != EAGAIN) {
      perror("write");
      throw Exception("stop");
    }
  }
#endif
}

void Terminal::DoRead() {
#ifdef Q_WS_X11
  char c;
  while (read(STDIN_FILENO, &c, 1) == 1)
    ProcessChar(c);
#endif
}

void Terminal::ClearDisplay() {
#ifdef Q_WS_X11
  tputs(home,1,putchar);
  tputs(clear_eod,nline,putchar);
  fflush(stdout);
#endif
}
