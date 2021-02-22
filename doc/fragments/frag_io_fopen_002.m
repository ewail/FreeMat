0
fp = fopen('test.dat','a+','le')
fwrite(fp,float([pi,e]))
fclose(fp)
