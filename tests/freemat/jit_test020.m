function a = jit_test020
  a = zeros(1,10000);
  for i=1:10000;
    a(i) = sec(i/10000);
  end
