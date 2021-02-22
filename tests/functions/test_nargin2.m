function test_val = test_nargin2
  test_val = (sub_test_nargin2(3) == 1) && (sub_test_nargin2(3,'h',1.34,pi,e) == 5);
end

function x = sub_test_nargin2(varargin)
  x = nargin;
end
