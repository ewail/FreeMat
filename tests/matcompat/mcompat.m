function mcompat
  fp = fopen('funlist.txt');
  test_out = {};
  ptr = 1;
  found = 0;
  total = 0;
  while (~feof(fp))
    g = fgetline(fp);
%    g = fgets(fp);
%    g(end) = [];
    if (~feof(fp))
      test_out{ptr,1} = deblank(g);
      test_out{ptr,2} = exist(deblank(g));
      if (test_out{ptr,2}>0)
        found = found+1;
      end
      ptr = ptr + 1;
      total = total + 1;
    end
  end
  fclose(fp);
  for i=1:size(test_out,1)
    printf('Function %30s status %d\n',test_out{i,1},test_out{i,2});
  end
  printf('Net fraction = %0.2f\n',found/total*100.0);

