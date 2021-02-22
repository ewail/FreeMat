function x = test_conv2_1
   % In response to 1928542
   A = [1,3;4,5]+i*[5,2;7,8];
   B = [0,2;5,0]+i*[9,3;5,2];
   C = conv2(A,B);
   D = conv2(real(A),real(B))-conv2(imag(A),imag(B));
   E = conv2(real(A),imag(B))+conv2(imag(A),real(B));
   x = wbtest_near(C,D+E*i);
