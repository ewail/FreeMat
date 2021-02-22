function success = tjit4
  success = 0;
  try
    A = zeros(1,1000);
    j = 0;
    for i=1:1000
      j = j + A(3,i);
    end
  catch
    success = 1;
  end
