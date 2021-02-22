function A = jit_test030
  P = [1,2,3,4,5];
  B = [6,5,3,4,5];
  A = [0,0,0,0,0];
  for i=1:10000
    C = P.*B;
    D = P./B;
    E = P.\B;
    A = A + C + D - E;
  end
