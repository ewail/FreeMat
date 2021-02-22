% Test reading and writing a scalar to a file
function test_val = test_file1
fp = fopen('junk.dat','w');
p = uint32(4);
fwrite(fp,p);
fclose(fp);
fp = fopen('junk.dat','r');
q = fread(fp,1,'uint32');
fclose(fp);
return (p==q);
