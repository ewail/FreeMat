function t = test_cannyedgedetector1
  A = zeros(7);
  A(2:6,2:6) = 1;
  B = cannyedgedetector(A,.1,.1,.5,0);
  C = zeros(7);
  C(2:6,2) = 1; C(2:6,6) = 1; C(2,2:6) = 1; C(6,2:6) = 1;
  t = issame(B,C);
