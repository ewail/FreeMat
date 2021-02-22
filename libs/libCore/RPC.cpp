/*
 * Copyright (c) 2002-2007 Samit Basu
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

#include <QtNetwork>
#include "Array.hpp"
#include "Malloc.hpp"
#include "HandleList.hpp"
#include "RPC.hpp"
#include <QList>
#include "Print.hpp"

template <class T>
void getArray(int N, T* c, QDataStream &in) {
  for (int i=0;i<N;i++)
    in >> c[i];
}

template <class T>
void getSparseArray(int cols, T** c, QDataStream &in) {
  for (int i=0;i<cols;i++) {
    T len;
    in >> len;
    c[i] = new T[(uint32)len];
    getArray((int)len,c[i],in);
  }
}

template <class T>
void putArray(int N, const T* c, QDataStream &out) {
  for (int i=0;i<N;i++)
    out << c[i];
}

template <class T>
void putSparseArray(int cols, const T** c, QDataStream &out) {
  for (int i=0;i<cols;i++) {
    out << 1+c[i][0];
    for (int j=0;j<1+c[i][0];j++)
      out << c[i][j];
  }
}

void getArrayFromQDS(QDataStream &in, Array& dat) {
  Class dclass;
  bool sparseFlag;
  Dimensions dims;
  uint8 dimCount;
  uint8 t;
  in >> t; dclass = (Class) t;
  in >> t; sparseFlag = (bool) t;
  in >> dimCount;
  for (int i=0;i<dimCount;i++) {
    uint32 dimVal;
    in >> dimVal;
    dims.setDimensionLength(i,dimVal);
  }
  int elCount(dims.getElementCount());
  if (elCount == 0) {
    dat = Array(dclass,dims,NULL);
    return;
  }
  switch(dclass) {
  default: throw Exception("unhandled type in getarrayfromqds");
  case FM_CELL_ARRAY: {
    Array *dp = new Array[elCount];
    for (int i=0;i<elCount;i++)
      getArrayFromQDS(in,dp[i]);
    dat = Array(dclass,dims,dp);
    return;
  }
  case FM_STRUCT_ARRAY: {
    StringVector fnames;
    quint32 ncount;
    in >> ncount;
    for (quint32 i=0;i<ncount;i++) {
      char *dp;
      in >> dp;
      fnames.push_back(dp);
      delete dp;
    }
    Array *dp = new Array[elCount*ncount];
    for (unsigned i=0;i<elCount*ncount;i++)
      getArrayFromQDS(in,dp[i]);
    dat = Array(dclass,dims,dp,false,fnames);
    return;
  }
  case FM_LOGICAL: {
    logical *dp = (logical*) Malloc(sizeof(logical)*elCount);
    getArray(elCount, dp, in);
    dat = Array(dclass,dims,dp);
    return;
  }
  case FM_STRING:
  case FM_UINT8: {
    uint8 *dp = (uint8*) Malloc(sizeof(uint8)*elCount);
    getArray(elCount, dp, in);
    dat = Array(dclass,dims,dp);
    return;
  }
  case FM_INT8: {
    int8 *dp =  (int8*) Malloc(sizeof(int8)*elCount);
    getArray(elCount, dp, in);
    dat = Array(dclass,dims,dp);
    return;
  }
  case FM_UINT16: {
    uint16 *dp = (uint16*) Malloc(sizeof(uint16)*elCount);
    getArray(elCount, dp, in);
    dat = Array(dclass,dims,dp);
    return;
  }
  case FM_INT16: {
    int16 *dp = (int16*) Malloc(sizeof(int16)*elCount);
    getArray(elCount, dp, in);
    dat = Array(dclass,dims,dp);
    return;
  }
  case FM_UINT32: {
    uint32 *dp = (uint32*) Malloc(sizeof(uint32)*elCount);
    getArray(elCount, dp, in);
    dat = Array(dclass,dims,dp);
    return;
  }
  case FM_UINT64: {
    uint64 *dp = (uint64*) Malloc(sizeof(uint64)*elCount);
    getArray(elCount, dp, in);
    dat = Array(dclass,dims,dp);
    return;
  }
  case FM_INT64: {
    uint64 *dp = (uint64*) Malloc(sizeof(uint64)*elCount);
    getArray(elCount, dp, in);
    dat = Array(dclass,dims,dp);
    return;
  }
  case FM_INT32: {
    if (!sparseFlag) {
      int32 *dp = (int32*) Malloc(sizeof(int32)*elCount);
      getArray(elCount, dp, in);
      dat = Array(dclass,dims,dp);
    } else {
      int32 **dp = new int32*[dims.getColumns()];
      getSparseArray(dims.getColumns(), dp, in);
      dat = Array(dclass,dims,dp,true);
    }
    return;
  }
  case FM_FLOAT: {
    if (!sparseFlag) {
      float *dp =  (float*) Malloc(sizeof(float)*elCount);
      getArray(elCount, dp, in);
      dat = Array(dclass,dims,dp);
    } else {
      float **dp = new float*[dims.getColumns()];
      getSparseArray(dims.getColumns(), dp, in);
      dat = Array(dclass,dims,dp,true);
    }
    return;
  }
  case FM_DOUBLE: {
    if (!sparseFlag) {
      double *dp = (double*) Malloc(sizeof(double)*elCount);
      getArray(elCount, dp, in);
      dat = Array(dclass,dims,dp);
    } else {
      double **dp = new double*[dims.getColumns()];
      getSparseArray(dims.getColumns(), dp, in);
      dat = Array(dclass,dims,dp,true);
    }
    return;
  }
  case FM_COMPLEX: {
    if (!sparseFlag) {
      float *dp = (float*) Malloc(sizeof(float)*elCount*2);
      getArray(elCount*2, dp, in);
      dat = Array(dclass,dims,dp);
    } else {
      float **dp = new float*[dims.getColumns()];
      getSparseArray(dims.getColumns(), dp, in);
      dat = Array(dclass,dims,dp,true);
    }
    return;
  }
  case FM_DCOMPLEX: {
    if (!sparseFlag) {
      double *dp = (double*) Malloc(sizeof(double)*elCount*2);
      getArray(elCount*2, dp, in);
      dat = Array(dclass,dims,dp);
    } else {
      double **dp = new double*[dims.getColumns()];
      getSparseArray(dims.getColumns(), dp, in);
      dat = Array(dclass,dims,dp,true);
    }
    return;
  }
  }  
}

void putArrayToQDS(QDataStream &out, const Array& dat) {
  out << (uint8) dat.dataClass();
  out << (uint8) dat.sparse();
  out << (uint8) dat.dimensions().getLength();
  for (int i=0;i<dat.dimensions().getLength();i++)
    out << (uint32) dat.dimensions().getDimensionLength(i);
  int elCount(dat.getLength());
  if (dat.isEmpty()) return;
  switch(dat.dataClass()) {
  default: throw Exception("unhandled type in putarraytoqds");
  case FM_CELL_ARRAY: {
    const Array *dp=((const Array *) dat.getDataPointer());
    for (int i=0;i<elCount;i++)
      putArrayToQDS(out,dp[i]);
    return;
  }
  case FM_STRUCT_ARRAY: {
    StringVector fnames(dat.fieldNames());
    int ncount(fnames.size());
    out << (quint32) ncount;
    int i;
    for (i=0;i<ncount;i++)
      out << fnames.at(i).c_str();
    const Array *dp=((const Array *) dat.getDataPointer());
    for (i=0;i<elCount*ncount;i++)
      putArrayToQDS(out,dp[i]);
    return;
  }
  case FM_LOGICAL:
    putArray(elCount,(const logical *)dat.getDataPointer(),out);
    return;
  case FM_STRING:
  case FM_UINT8: 
    putArray(elCount,(const uint8 *)dat.getDataPointer(),out);
    return;
  case FM_UINT16:
    putArray(elCount,(const uint16 *)dat.getDataPointer(),out);
    return;
  case FM_UINT32: 
    putArray(elCount, (const uint32 *)dat.getDataPointer(),out);
    return;
  case FM_UINT64: 
    putArray(elCount, (const uint64 *)dat.getDataPointer(),out);
    return;    
  case FM_INT8: 
    putArray(elCount, (const int8 *)dat.getDataPointer(),out);
    return;
  case FM_INT16:
    putArray(elCount, (const int16 *)dat.getDataPointer(),out);
    return;
  case FM_INT32: 
    if (!dat.sparse()) 
      putArray(elCount, (const int32 *)dat.getDataPointer(),out);
    else 
      putSparseArray(dat.getDimensionLength(1), (const int32 **) dat.getSparseDataPointer(), out);
    return;
  case FM_INT64: 
    putArray(elCount, (const int64 *)dat.getDataPointer(), out);
    return;
  case FM_FLOAT: 
    if (!dat.sparse()) 
      putArray(elCount, (const float *)dat.getDataPointer(), out);
    else 
      putSparseArray(dat.getDimensionLength(1), (const float **) dat.getSparseDataPointer(), out);
    return;
  case FM_DOUBLE: 
    if (!dat.sparse()) 
      putArray(elCount, (const double *)dat.getDataPointer(), out);
    else 
      putSparseArray(dat.getDimensionLength(1), (const double **) dat.getSparseDataPointer(), out);
    return;
  case FM_COMPLEX: 
    if (!dat.sparse()) 
      putArray(elCount*2, (const float *)dat.getDataPointer(), out);
    else 
      putSparseArray(dat.getDimensionLength(1), (const float **) dat.getSparseDataPointer(), out);
    return;
  case FM_DCOMPLEX: 
    if (!dat.sparse()) 
      putArray(elCount*2, (const double *)dat.getDataPointer(), out);
    else 
      putSparseArray(dat.getDimensionLength(1), (const double **) dat.getSparseDataPointer(), out);
    return;
  }
}


// Here is the idea behind FreeMat's RPC service.
//
//  1. It is enabled and controlled via functions
//  2. It acts as an independant entity - driven by the main event loop
//  3. It supports simple Get/Put operations for arrays.
//  4. It is more of a peer-to-peer service than a centralized one
//
// Here is the mock-up:
//
// rpccontrol enable on queue 10 port 3254 % turn on our RPC service
//                                         % Allow a queue depth of 10
//                                         % set our RPC port to 3254
// recon = rpcreg('192.168.0.100:2950')    % returns an integer ID for the remote process
// rpcping(recon)                          % query the state of the remote RPC service
// rpcput(recon,A)                         % throws an exception if the put fails
// cnt = rpcpeek(recon)                    % how many messages are available from recon?
// A = rpcget(recon)                       % retrieve next message from recon
//
// With these commands, a simple RPC server looks like this:
// id = rpcinit(5237);                     % Open an RPC server on port 5237
// [A,id] = rpcget(0)                      % Get the next message (from anyone)
// [var{:}] = feval(A.funcname,A.data);    % Evaluate the function (whatever it is)
// A.data = var;                           % replace the arguments with the reply
// rpcput(id,A);                           % Send it back.
//
// A slot is a place to store a received array, or the fact than an error occured 
// trying to receive/decode it
//
// The current design has flaws... In particular... Suppose we issue 2 gets to 
// a remote client in sequence:
//   rpcput(id,var1)
//   rpcput(id,var2)
//
// Because of the lack of a queue structure, there is no guarantee that var1 will complete
// (and thus free up the server) before we attempt to put var2.  
//
// The right way to handle this is to have a send and receive queue associated with each
// rpc server.  Also, how will a server return a message?  The current "push" model assumes
// that a uni-directional push is adequate to implement all desired features.  
//
// One option is to have two global queues "out" and "in". We could also have a thread
// for each RPC server.  For some reason, I don't like that idea.  
//
// OK - so what is the problem with the current model?  It will push data to as many different
// slots as possible concurrently.  Actually - that shouls be fine - the example I listed
// above is wrong.  If we attempt to push var1 and var2 to the same server, they will
// succeed and show up in that server's queue as successive entries - precisely because
// that server will spawn RPCClients to manage the connections
// 
// Then in principle it is fine... If you want to call an RPC server, you have to start your
// own RPC service first.  It would be nice if you could start multiple services on different
// ports.  That can be done easily enough.  The problem with the current system, then, is that
// the "get-s" are not working.
//
// OK - as originally envisioned - the put/get mechanism appears to work... But it leaves
// somethings to be desired.  First of all, it means we have to have our own rpc server running
// (and get the remote server to register us) before the remote server can push an answer to us.
// That means publishing an address where the remote server can reach us.  Something like this:
//
// a = rpcdial(remoteaddress,portnum)
//   -- rpcinit(portnum)
//   -- a = rpcreg(remoteaddress,portnum)
//      cmd.type = 'register'
//      cmd.address = <our ip address>
//      cmd.portnum = portnum
//   -- rpcput(a,cmd)
//
// To make an rpc call, we could then do
//
// vars = rpcfeval(a,'cos',pi)
//   -- cmd.type = 'call'
//   -- cmd.name = 'cos'
//   -- cmd.args = {pi}
// reply = rpcget(a)
//   if (~reply.success) error('')
//
// It could work.  The missing piece would be detecting errors on the rpcput side,
// since these errors do not appear anywhere right now.
//
//
// It would be far easier (and simpler) to start with a synchronous RPC mechanism
// instead of an asynchronous one.  This would be something like:
//
// a = rpcreg(remoteIP, remote port)
// G = rpcfeval(a,'cos',pi)
// 
// The way this would work is to block the calling socket until the function completes.
// 
// I could expose the socket interface at the FreeMat level with functions like:
//
//   a = tcpserver(5890);
//   while (1)
//     try
//       g = tcpaccept(a);
//       cmd = tcpget(g);
//       try
//         cmd.type = 'reply'
//         cmd.args = feval(cmd.name,cmd.args);
//       catch
//         cmd.type = 'error';
//         cmd.error = lasterror;
//       end
//       tcpput(g,cmd);
//       tcpclose(g);
//     catch
//     end
//   end


HandleList<QTcpServer*> m_servers;
HandleList<QTcpSocket*> m_sockets;

//@Module TCPSERVER Start a TCP Server on a designated port
//@@Section IO
//@@Usage
//Sets up a TCP server on a specified port.  The syntax for its
//use is:
//@[
//   handle = tcpserver(portnum)
//@]
//where @|portnum| is the port number to set up the tcp server on.
//It returns a @|handle| to the tcpserver.  To actually accept
//a connection on the server requires a call to @|tcpaccept|.  To
//close the server down, you need to call @|tcpserverclose|.  It is
//perfectly acceptable to have multiple @|tcpserver| open simultaneously,
//but they must be on different portnumbers.
//@@Example
//See @|rpcserver| for an example of how to use @|tcpserver|.  To 
//close the server down, you must call @|tcpserverclose|.  
//The following example works on a single machine, only because of
//buffering in the TCP implementation.  In practice, the 
//server and send sockets would be on different machines
//@<
//server = tcpserver(6010);             % Start up the server
//send = tcpconnect('127.0.0.1',6010);  % Connect to the server just started
//                                      % Will succeed because the server is running
//recv = tcpaccept(server);             % Accept the connection we just tried to make
//                                      % Will succeed because of the tcpconnect call
//msg = 'Hello';                        % Create a message to send through the loop
//tcpsend(send,msg);                    % Push the message through the socket
//tcprecv(recv)                         % Out it comes through the other side
//tcpsend(recv,msg);                    % Sockets are bi-directional
//tcprecv(send)
//tcpclose(recv); tcpclose(send);       % Close the tcp sockets
//tcpserverclose(server);               % Close the server socket
//@>
ArrayVector TCPServerFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    throw Exception("tcpserver requires one address - the port to set up the server on");
  unsigned int port = arg[0].asInteger();
  QTcpServer *server = new QTcpServer;
  if (!server->listen(QHostAddress::Any,port))
    throw Exception("unable to create a tcp server to listen to the given address");
  return ArrayVector() << Array(double(m_servers.assignHandle(server)));
}

//@Module TCPACCEPT Accept a connection on a TCP server
//@@Section IO
//@@Usage
//Accepts a connection on the given @|tcpserver|, and returns a
//handle to the connected @|tcpsocket|.  This function requires
//a timeout (in milliseconds), and will block until either a
//connection arrives or the timeout elapses.  The syntax for the
//command is 
//@[
//   handle = tcpaccept(server_handle)
//@]
//where @|server_handle| is the handle returned by @|tcpserver|.
//The output of @|tcpaccept| can be used with the socket functions
//@|tcpsend| and @|tcprecv| to send data between FreeMat instances.
//Optionally, you can specify the timeout in milliseconds for the 
//command to fail
//@[
//   handle = tcpaccept(server_handle, timeout)
//@]
//The default timeout is set to 30 seconds.  To
//close the socket returned by @|tcpaccept| you must call @|tcpclose|.
//The resulting handle is identical to one returned by @|tcpconnect|.
ArrayVector TCPAcceptFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("tcpaccept requires one argument - the handle of the server to read, and an optional timeout to wait before failure (in milliseconds)");
  unsigned int server_handle = arg[0].asInteger();
  unsigned int timeout = 30000;
  if (arg.size() == 2)
    timeout = arg[1].asInteger();
  QTcpServer *server = m_servers.lookupHandle(server_handle);
  if (!server->waitForNewConnection(timeout))
    throw Exception("Wait for connection in tcpaccept timed out");
  QTcpSocket *sock = server->nextPendingConnection();
  return ArrayVector() << Array(double(m_sockets.assignHandle(sock));
}

//@Module TCPCONNECT Connect to a remote TCP server
//@@Section IO
//@@Usage
//Attempts to open a tcp socket to a remote ip address and portnumber
//within a given timeout.  The general syntax for its use is
//@[
//   handle = tcpconnect(remote_address,remote_port,timeout)
//@]
//where @|timeout| is in milliseconds.  The @|remote_address|
//must be a string containing either an IP address (e.g., @|'192.168.0.1'|),
//or a name (e.g., @|'foo.goo.com'|).  The resulting socket can 
//be closed using @|tcpclose|.  If you do not specify a 
//@|timeout|, then a default of 30 seconds is used.
//@@Example
//See @|rpceval| for an example of how to use @|tcpconnect|.
//The following example works on a single machine, only because of
//buffering in the TCP implementation.  In practice, the 
//server and send sockets would be on different machines
ArrayVector TCPConnectFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 2)
    throw Exception("tcpconnect requires two arguments - the remote address of the server to connect to and the port number - an optional timeout can be specified also");
  QString host = arg[0].asString();
  unsigned int port = arg[1].asInteger();
  int timeout = 30000;
  if (arg.size() == 3)
    timeout = arg[2].asInteger();
  QTcpSocket *a_sock = new QTcpSocket;
  a_sock->connectToHost(host,port);
  if (!a_sock->waitForConnected(timeout))
    throw Exception(QString("tcpconnect failed to connect to ") + host + " on port " + port);
  return ArrayVector() << Array(double(m_sockets.assignHandle(a_sock));
}

//@Module TCPCLOSE Close a TCP socket
//@@Section IO
//@@Usage
//Closes a tcp socket that is returned either from @|tcpconnect|
//or from @|tcpaccept|.  The general syntax for its use is either
//@[
//   tcpclose(handle)
//@]
//which closes a specific @|handle| or 
//@[
//   tcpclose all
//@]
//to close all open sockets.  Each close operation has a timeout
//associated with it.  You can modify the timeout using the following
//forms of the command:
//@[
//   tcpclose(handle,timeout)
//@]
//where @|timeout| is in milliseconds, and
//@[
//   tcpclose all timeout
//@]
//or
//@[
//   tcpclose('all',timeout)
//@]
//which are equivalent, and will close all sockets, each with the given timeout.
ArrayVector TCPCloseFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    throw Exception("tcpclose requires at least one argument - the handle to close, or the string 'all' to close all tcp socket handles");
  int timeout = 30000;
  if (arg.size() >= 2)
    timeout = arg[1].asInteger();
  if (arg[0].isString()) {
    QString txtval = arg[0].asString().toUpper();
    if (txtval != "ALL")
      throw Exception(QString("Unrecognized argument to tcpclose ") + txtval);
    // Close all sockets
    for (unsigned i=0;i<=m_sockets.maxHandle();i++) {
      try {
	QTcpSocket *sock = m_sockets.lookupHandle(i);
	if (sock) {
	  sock->disconnectFromHost();
	  if (sock->state() != QAbstractSocket::UnconnectedState)
	    if (!sock->waitForDisconnected(timeout))
	      throw Exception(QString("Failed to disconnect socket: ") + 
			      sock->errorString());
	  delete sock;
	}
	m_sockets.deleteHandle(i);
      } catch (Exception &e) {
      }
    }
    return ArrayVector();
  }
  int handle = arg[0].asInteger();
  QTcpSocket *sock = m_sockets.lookupHandle(handle);
  sock->disconnectFromHost();
  if (sock->state() != QAbstractSocket::UnconnectedState)
    if (!sock->waitForDisconnected(timeout))
      throw Exception(string("Failed to disconnect socket: ") + sock->errorString());
  delete sock;
  m_sockets.deleteHandle(handle);
  return ArrayVector();
}

//@Module TCPSERVERCLOSE Close a TCP server socket
//@@Section IO
//@@Usage
//Closes a @|tcpserver| socket.  The general syntax
//for its use is either
//@[
//   tcpserverclose(handle)
//@]
//which closes a specific @|handle| or
//@[
//   tcpserverclose all
//@]
//to close all open servers.
ArrayVector TCPServerCloseFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    throw Exception("tcpserverclose requires at least one argument - the handle to close, or the string 'all' to close all tcp socket handles");
  qDebug() << "Closing server";
  if (arg[0].isString()) {
    QString txtval = arg[0].asString().toUpper();
    if (txtval != "ALL")
      throw Exception(QString("Unrecognized argument to tcpserverclose ") + txtval);
    // Close all sockets
    for (unsigned i=0;i<=m_servers.maxHandle();i++) {
      try {
	QTcpServer *sock = m_servers.lookupHandle(i);
	if (sock)
	  sock->close();
	m_servers.deleteHandle(i);
      } catch (Exception &e) {
      }
    }
    return ArrayVector();
  }
  int handle = arg[0].asInteger();
  QTcpServer *sock = m_servers.lookupHandle(handle);
  m_servers.deleteHandle(handle);
  sock->close();
  return ArrayVector();
}

//@Module TCPSEND Send an array over a TCP socket
//@@Section IO
//@@Usage
//Sends an array over a TCP socket.  The encoding of the
//array is done in a manner such that arrays can be 
//transparently sent between different machine types
//(endianness, word size, etc.).  The general syntax for its 
//use is
//@[
//  tcpsend(handle,array)
//@]
//where @|handle| is a connected socket returned from a 
//successful @|tcpconnect| or @|tcpaccept| call.  By 
//default the @|tcpsend| operation has a 30 second timeout.
//You can specify the timeout using the following syntax for it
//@[
//  tcpsend(handle,array,timeout)
//@]
//where @|timeout| is in milliseconds.
ArrayVector TCPSendFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 2)
    throw Exception("tcpsend requires two arguments - the handle of the connection to use, and the array to send - an optional timeout can be specified also");
  qDebug() << "Start send";
  unsigned int handle = arg[0].asInteger();
  QTcpSocket *sock = m_sockets.lookupHandle(handle);
  if (sock->state() != QAbstractSocket::ConnectedState)
    throw Exception("tcpsend only works on connected sockets");
  int timeout = 30000;
  if (arg.size() == 3)
    timeout = arg[2].asInteger();
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_4_2);
  out << (quint64)0;
  out << (quint32) 0xFEEDADAD;
  putArrayToQDS(out,arg[1]);
  out.device()->seek(0);
  out << (quint64)(block.size() - sizeof(quint64));
  qDebug() << "block size -> " << (quint64)(block.size() - sizeof(quint64));
  sock->write(block);
  //  sock->flush();
  qDebug() << "send: bytes to write: " << sock->bytesToWrite();
  if (!sock->waitForBytesWritten(timeout))
    throw Exception("timeout on tcpsend function:" + sock->errorString().toStdString());
  qDebug() << "(after) send: bytes to write: " << sock->bytesToWrite();
  qDebug() << "Done send";
  return ArrayVector();
}

//@Module TCPRECV Receive an array over a TCP socket
//@@Section IO
//@@Usage
//Receives an array over from a TCP socket.  The encoding of the
//array is done in a manner such that arrays can be 
//transparently sent between different machine types
//(endianness, word size, etc.).  There must be a matching
//@|tcpsend| call for it to work.  The general syntax for its 
//use is
//@[
//  array = tcprecv(handle)
//@]
//where @|handle| is a connected socket returned from a 
//successful @|tcpconnect| or @|tcpaccept| call.  By 
//default the @|tcprecv| operation has a 30 second timeout.
//You can specify the timeout using the following syntax for it
//@[
//  array = tcprecv(handle,timeout)
//@]
//where @|timeout| is in milliseconds.
ArrayVector TCPRecvFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("tcprecv requires one argument - the handle of the connection to use - an optional timeout can be specified also.");
  qDebug() << "Start recv";
  unsigned int handle = ArrayToInt32(arg[0]);
  int timeout = 30000;
  if (arg.size() == 2)
    timeout = ArrayToInt32(arg[1]);
  QTcpSocket *a_sock = m_sockets.lookupHandle(handle);
  qDebug() << "receive socket state: " << a_sock->state() << " " << a_sock->isValid();
  while (a_sock->bytesAvailable() < (int)sizeof(quint64)) {
    qDebug() << "bytes available = " << a_sock->bytesAvailable();
     if (!a_sock->waitForReadyRead(timeout)) {
       qDebug() << "TIMEOUT: bytes available = " << a_sock->bytesAvailable();
       qDebug() << "TIMEOUT: receive socket state: " << a_sock->state() << " " << a_sock->isValid();
       throw Exception(string("tcprecv failed to get blocksize:") + a_sock->errorString().toStdString() + " with " + a_sock->bytesAvailable() + " bytes available");
     }
//    sleep(1);
  }
  QDataStream in(a_sock);
  in.setVersion(QDataStream::Qt_4_2);
  quint64 blockSize;
  in >> blockSize;
  qDebug() << "block size = " << blockSize;
  while ((quint64) a_sock->bytesAvailable() < blockSize) {
    if (!a_sock->waitForReadyRead(timeout))
      throw Exception(string("tcprecv failed to get data block prior to timeout"));
  }
  quint32 magic;
  in >> magic;
  if (magic != 0xFEEDADAD) 
    throw Exception(string("tcprecv failed to get proper magic number"));
  Array ret;
  getArrayFromQDS(in,ret);
  qDebug() << "bytes left after get operation: " << a_sock->bytesAvailable();
  qDebug() << "Done recv";
  return ArrayVector() << ret;
}

//@Module TCPSTATE State of a TCP socket
//@@Section IO
//@@Usage
//Returns the state of a TCP socket given the handle (returned
//either by @|tcpaccept| or @|tcpconnect|.  The general syntax
//for its use is 
//@[
//   state = tcpstate(handle)
//@]
//where @|state| is a string that is either:
//\begin{itemize}
//   \item @|'unconnected'| if the socket is unconnected
//   \item @|'hostlookup'| if the socket is performing a host name lookup
//   \item @|'connecting'| if the socket has started establishing a connection
//   \item @|'connected'| if the socket is connected
//   \item @|'closing'| if the socket is about to close.
//\end{itemize}
ArrayVector TCPStateFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("tcpstate requires one argument - the handle of the socket to examine");
  unsigned int handle = arg[0].asInteger();
  QTcpSocket *a_sock = m_sockets.lookupHandle(handle);
  switch (a_sock->state()) {
  case QAbstractSocket::UnconnectedState:
    return ArrayVector() << Array(QString("unconnected"));
  case QAbstractSocket::HostLookupState:
    return ArrayVector() << Array(QString("hostlookup"));
  case QAbstractSocket::ConnectingState:
    return ArrayVector() << Array(QString("connecting"));
  case QAbstractSocket::ConnectedState:
    return ArrayVector() << Array(QString("connected"));
  case QAbstractSocket::BoundState:
    return ArrayVector() << Array(QString("bound"));
  case QAbstractSocket::ClosingState:
    return ArrayVector() << Array(QString("closing"));
  case QAbstractSocket::ListeningState:
    return ArrayVector() << Array(QString("listening"));
  }
  return ArrayVector() << Array(QString("unknown"));
}

