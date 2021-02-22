function A = jit_test014
  N = 500;
  A = zeros(N,N);
  for i=1:N;
    for j=1:N;
      A(j,i) = abs(i-j);
    end
  end
