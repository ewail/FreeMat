function A = jit_test005
  A = zeros(1,1000);
  for i=1:1000
    for m=1:100
      if (i > 50)
        k = 55;
      elseif (i > 40)
        k = 45;
      elseif (i > 30)
        k = 35;
      else 
        k = 15;
      end
    end
    A(i) = k;
  end
