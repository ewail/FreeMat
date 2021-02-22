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

#ifndef __File_hpp__
#define __File_hpp__
#include <stdio.h>
#include "Stream.hpp"
#include <QString>

class File : public Stream{
  bool autoclose;
  FILE *fp;
public:
  // Create a file with the given access code
  File(QString filename, QString accessmode);
  File(FILE*afp);
  // Close the file
  virtual ~File();
  // Write a sequence of bytes to the file
  virtual void writeBytes(const void* data, int len);
  // Read a sequence of bytes from the file
  virtual void readBytes(void* data, int len);
};

#endif
