0
fp = fopen('test.dat','r','ieee-le')
fread(fp,[1,5],'float')
fclose(fp)
