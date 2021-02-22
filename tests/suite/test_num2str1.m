function x = test_num2str1
   % Added for bug 1918315
   inum = 100;
   x = all('100.data' == [num2str(inum),'.data']);
