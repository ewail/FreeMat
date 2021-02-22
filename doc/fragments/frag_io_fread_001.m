0
fp = fopen('test.dat','r');
x = fread(fp,[1,inf],'float');
fclose(fp);
who x
