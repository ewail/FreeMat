%
%@Module BIND Bind Standalone Executable
%@@Section FREEMAT
%@@Usage
%The @|bind| function allows you to build a stand alone
%executable on Linux.  The syntax for its use is
%@[
%   bind(output,mainexe,startcommand,varargin)
%@]
%where @|output| is the name of the output binary, 
%@|mainexe| is the location of the FreeMat intepreter to be
%used (e.g., @|'/usr/local/bin/FreeMat'|).  The next argument
%is the start command, i.e., the command to execute once the
%interpreter is started.  Generally speaking, it should be
%@|-f main| where @|main| is the name of the routine to start
%up.  The last argument is the list of 
%M-files to bind into the executable as p-code.  Note that
%the resulting binary executable will pass arguments to your main
%function automatically, see the example for example.  Bound
%executables can use graphics, MPI, and any other FreeMat 
%capabilities.
%@@Example
%Here is the ubiquitous "hello world" example done FreeMat
%style.  Note that this example will only work under Linux at
%the moment.  Support for standalone executables on Mac OS and
%Windows is scheduled for a later version of FreeMat.
%@{  hello.m
%function hello(x)
%  printf(' Hello %s\n',x);
%@}
%Here is the @|hello| function from within FreeMat
%@<
%hello('world!')
%@>
%Now we bind it into an executable.
%@<
%bind('hello_bind','../../build/src/x11/FreeMat','-f hello','hello');
%@>
%We have to make the output executable and run it.
%@<
%system('chmod +x hello_bind');
%system('./hello_bind world!')
%@>
%
function bind(output,mainexe,startcommand,varargin)
% Open up the output stream
fpo = fopen(output,'wb');
% Read the main program in
fpi = fopen(mainexe,'rb');
printf('Reading main executable %s\n',mainexe);
pdata = fread(fpi,[1,inf],'int8');
fclose(fpi);
% Write the main program to the output stream
printf('Copying interpreter to output file %s\n',output);
fwrite(fpo,pdata);
if (~isempty(varargin))
  fwrite(fpo,uint32(length(varargin)));
  for i=1:length(varargin)
    printf('Pcoding support routine %s\n',varargin{i});
    pcode(varargin{i});
    printf('Copying Pcode for %s into output bundle...\n',varargin{i});
    fpi = fopen(sprintf('%s.p',varargin{i}),'rb');
    fwrite(fpo,fread(fpi,[1,inf],'int8'));
    fclose(fpi);
  end
else
  fwrite(fpo,uint32(0));
end
% Write the closing tag
printf('Writing closing tag information...\n');
fwrite(fpo,'fmexe');
fwrite(fpo,uint32(length(pdata)));
fwrite(fpo,uint32(length(startcommand)+1));
fwrite(fpo,startcommand);
fwrite(fpo,int8(zeros(1,1024-5-4-4-length(startcommand))));
fclose(fpo);
printf('Bundle successfully created.\n');

