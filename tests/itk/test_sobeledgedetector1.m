function t = test_sobeledgedetector1
  A = zeros(7);
  A(2:6,2:6) = 1;
  B = sobeledgedetector(A);
  C = zeros(7,'logical');
  C(2,2) = 1;
  C(2,6) = 1;
  C(6,2) = 1;
  C(6,6) = 1;
  D = B > 4;
  t = issame(D,C);

  