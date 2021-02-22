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
#include "DebugStream.hpp"
#include <QTime>

DebugStream &DebugStream::sync(void) { 
    if( isEnabled() ){
	if( dbwin ){
	    int endl_ind;
	    static QString output_buffer("");
	    ts.flush(); 
	    if( (endl_ind = stream.indexOf( '\n' )) == -1 ){
		output_buffer += stream;
		stream.clear();
	    }
	    else{
		output_buffer += stream.left( endl_ind+1 );
		int leftover_size = stream.size() - (endl_ind+1);
		stream = (leftover_size>0) ? stream.right( leftover_size ) : QString("");
		emit SendMessage( QTime::currentTime().toString("hh:mm:ss.zzz: ") + output_buffer );
		output_buffer.clear();
	    }
	}
    }
    return *this; 
}
