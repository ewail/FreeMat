function test_val = test_nargin1
  test_val = (sub_test_nargin1(3) == 1) && (sub_test_nargin1(3,'h',1.34,pi,e) == 5);
end

function x = sub_test_nargin1(a1,a2,a3,a4,a5)
  x = nargin;
end
