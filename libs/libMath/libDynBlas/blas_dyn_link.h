/*
 * Copyright (c) 2002-2007 Samit Basu, Eugene Ingerman
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

#ifndef __BLAS_DYN_LINK_H
#define __BLAS_DYN_LINK_H

#include <QString>
#include <QLibrary> 
#include <vector>
typedef int integer;
typedef char *address;
typedef short shortint;
typedef float real;
typedef double doublereal;
typedef struct { real r, i; } complex;
typedef struct { doublereal r, i; } doublecomplex;
typedef short shortlogical;
typedef char logical1;
typedef char integer1;

class BlasWrapper{
    struct LibConf{
	QString Name;
	QString fileName;
	QString prefix;
	QString suffix;
	bool capitalized;
	QString desc;
    };
    std::list<LibConf> libList;
    LibConf currentLib;
    QLibrary* blasLib;
    bool useReference;
public:
    BlasWrapper();
    ~BlasWrapper() { if(blasLib && blasLib->isLoaded()) blasLib->unload(); delete blasLib; blasLib = NULL;};
    void Init( void );
    bool LoadLibByName( const std::string& name);
    void* Resolve( const char* function_name, void (*default_fcn) () );
    void ListLibraries( std::string& msg );

private:
    void DiscoverBlasLibrary( void );
    void LoadLib( const LibConf& );
    void InitFunctions( void );

private:
    std::string ComputerType( void );

public:
    std::vector<void*> fn;
};
#endif

