function A = jit_test039
  i = 1;
  A = 0;
  while (i<1000000)
     if ((i > 5) && (i < 3000))
       A = A + i;
     end
     i = i + 1;
  end
