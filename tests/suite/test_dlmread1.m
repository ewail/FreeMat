function test_val = test_dlmread1
  fp = fopen('test.csv','w');
  fprintf(fp,'1;2;3;4;5\n');
  fprintf(fp,'6;7;8;9;0\n');
  fprintf(fp,'4;3;2;4;1\n');
  fclose(fp);
  y = dlmread('test.csv',';');
  A = [1,2,3,4,5;6,7,8,9,0;4,3,2,4,1];
  test_val = issame(y,A);
