0
A = float(randn(512));
fp = fopen('test.dat','w');
fwrite(fp,A,'single');
fclose(fp);
