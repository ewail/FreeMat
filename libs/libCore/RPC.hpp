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
#ifndef __RPC_hpp__
#define __RPC_hpp__

#include "Array.hpp"

ArrayVector TCPServerFunction(int nargout, const ArrayVector& arg);
ArrayVector TCPAcceptFunction(int nargout, const ArrayVector& arg);
ArrayVector TCPConnectFunction(int nargout, const ArrayVector& arg);
ArrayVector TCPSendFunction(int nargout, const ArrayVector& arg);
ArrayVector TCPRecvFunction(int nargout, const ArrayVector& arg);
ArrayVector TCPStateFunction(int nargout, const ArrayVector& arg);
ArrayVector TCPCloseFunction(int nargout, const ArrayVector& arg);
ArrayVector TCPServerCloseFunction(int nargout, const ArrayVector& arg);
#endif
