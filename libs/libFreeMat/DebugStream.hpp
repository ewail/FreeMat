/*
* Copyright (c) 2008 Eugene Ingerman
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

#ifndef __DEBUGSTREAM_HPP__
#define __DEBUGSTREAM_HPP__

#include <ostream>
#include <iostream>
#include <sstream>
#include <string>
#include <QObject>
#include <QTextCursor>
#include <QString>
#include <QTextStream>
#include "DebugWin.hpp"

//#ifndef NDEBUG
#define ENABLEDBSTREAM (true)
//#else
//#define ENABLEDBSTREAM (false)
//#endif

class DebugStream : public QObject 
{
    Q_OBJECT
private:
    DebugWin* dbwin;   
    QString stream;
    QTextStream ts;
    bool bEnabled;
signals:
    void SendMessage( const QString& msg );

public:
    DebugStream(){ dbwin=0; bEnabled = true; ts.setString( &stream, QIODevice::Unbuffered ); };
    ~DebugStream() { QObject::disconnect( this, 0, 0, 0 ); };

    inline void setEnabled( void ) { bEnabled=true; }
    inline void setDisabled( void ) { bEnabled=false; }
    inline bool isEnabled( void ) { return bEnabled && ENABLEDBSTREAM; }

    void setWin( DebugWin* _dbwin ) { 
	dbwin = _dbwin; 
	QObject::connect( this, SIGNAL( SendMessage( const QString& ) ), 
			  dbwin, SLOT( addString( const QString& ) ) );
    };

    DebugStream &sync(void);
    inline DebugStream &operator<<(QChar t) { if( isEnabled() ) ts << "\'" << t << "\'"; return sync(); }
    inline DebugStream &operator<<(bool t) { if( isEnabled() ) ts << (t ? "true" : "false"); return sync(); }
    inline DebugStream &operator<<(char t) { if( isEnabled() ) ts << t; return sync(); }
    inline DebugStream &operator<<(signed short t) { if( isEnabled() ) ts << t; return sync(); }
    inline DebugStream &operator<<(unsigned short t) { if( isEnabled() ) ts << t; return sync(); }
    inline DebugStream &operator<<(signed int t) { if( isEnabled() ) ts << t; return sync(); }
    inline DebugStream &operator<<(unsigned int t) { if( isEnabled() ) ts << t; return sync(); }
    inline DebugStream &operator<<(signed long t) { if( isEnabled() ) ts << t; return sync(); }
    inline DebugStream &operator<<(unsigned long t) { if( isEnabled() ) ts << t; return sync(); }
    inline DebugStream &operator<<(qint64 t)
    { if( isEnabled() ) ts << QString::number(t); return sync(); }
    inline DebugStream &operator<<(quint64 t)
    { if( isEnabled() ) ts << QString::number(t); return sync(); }
    inline DebugStream &operator<<(float t) { if( isEnabled() ) ts << t; return sync(); }
    inline DebugStream &operator<<(double t) { if( isEnabled() ) ts << t; return sync(); }
    inline DebugStream &operator<<(const char* t) { if( isEnabled() ) ts << QString::fromAscii(t); return sync(); }
    inline DebugStream &operator<<(const QString & t) { if( isEnabled() ) ts << "\"" << t  << "\""; return sync(); }
    inline DebugStream &operator<<(const QStringList & t) { 
      for (int i=0;i<t.size();i++) 
	if( isEnabled() ) ts << "\"" << t[i]  << "\"\n"; 
      return sync(); 
    }
    inline DebugStream &operator<<(const std::string & t) { if( isEnabled() ) ts << "\"" << t.c_str()  << "\""; return sync(); }
    inline DebugStream &operator<<(const QLatin1String &t) { if( isEnabled() ) ts << "\""  << t.latin1() << "\""; return sync(); }
    inline DebugStream &operator<<(const QByteArray & t) { if( isEnabled() ) ts  << "\"" << t << "\""; return sync(); }
    inline DebugStream &operator<<(const void * t) { if( isEnabled() ) ts << t; return sync(); }
    inline DebugStream &operator<<(QTextStreamFunction f) {
	if( isEnabled() ) ts << f;
	return *this;
    }
    inline DebugStream &operator<<(QTextStreamManipulator m)
    { if( isEnabled() ) ts << m; return *this; }

};

extern DebugStream dbout;
#endif /* __DEBUGSTREAM_HPP__ */
