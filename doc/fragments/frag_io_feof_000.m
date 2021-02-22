0
fp = fopen('test.dat','rb');
x = fread(fp,[512,inf],'float');
feof(fp)
x = fread(fp,[1,1],'float');
feof(fp)
