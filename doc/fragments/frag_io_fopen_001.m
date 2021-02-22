0
fp = fopen('test.dat','r','ieee-le')
fread(fp,[1,3],'float')
fclose(fp)
