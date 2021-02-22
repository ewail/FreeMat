function accum = break_ex
  accum = 0;
  i = 1;
  while (i<=10)
    accum = accum + i;
    if (i == 5)
      break;
    end
    i = i + 1;
  end
