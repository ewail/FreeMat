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

#include "File.hpp"
#include "Exception.hpp"

File::File(QString filename, QString accessmode) {
  fp = fopen(qPrintable(filename),qPrintable(accessmode));
  if (fp == NULL)
    throw Exception(QString("unable to open file ") + filename + 
		    QString(" with file mode ") + accessmode);
  autoclose = true;
}

File::File(FILE*afp) {
  fp = afp;
  autoclose = false;
}

File::~File() {
  if (autoclose)
    fclose(fp);
}

void File::writeBytes(const void* data, int len) {
  fwrite(data,sizeof(char),len,fp);
}

void File::readBytes(void* data, int len) {
  fread(data,sizeof(char),len,fp);
}
