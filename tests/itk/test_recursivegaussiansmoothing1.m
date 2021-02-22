function t = test_recursivegaussiansmoothing1
  a = zeros(5);
  a(3,3) = 1;
  b = smoothingrecursivegaussian(a,1,1);
  [x,y] = meshgrid(-2:2);
  h = exp(-(x.^2+y.^2)/2)/(2*pi);
  t = norm(b-h,'fro') < 0.01;

  