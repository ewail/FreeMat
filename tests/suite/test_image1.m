    % test image of an empty argument
function test_val = test_image1
a = [];
try
  % If this causes a segfault, it won't be caught.
  image(a);
catch
end
close all;
test_val = 1;

