0
fp = fopen('test.dat','r');
x = fread(fp,[512,inf],'float');
fclose(fp);
who x
