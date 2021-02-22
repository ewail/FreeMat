function A = jit_test035
  A = zeros(2,100000);
  C = [5,6];
  for i=1:100000
    C(2) = i;
    A(1:2,i) = C';
  end
