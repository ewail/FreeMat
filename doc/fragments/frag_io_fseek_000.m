0
% First we create the file
fp = fopen('test.dat','wb');
fwrite(fp,float(rand(4096,1)));
fclose(fp);
% Now we open it
fp = fopen('test.dat','rb');
% Read the whole thing
x = fread(fp,[1,inf],'float');
% Rewind to the beginning
fseek(fp,0,'bof');
% Read part of the file
y = fread(fp,[1,1024],'float');
who x y
% Seek 2048 bytes into the file
fseek(fp,2048,'cof');
% Read 512 floats from the file
x = fread(fp,[512,1],'float');
% Close the file
fclose(fp);
