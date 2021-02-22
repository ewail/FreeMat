% Test the imwrite and imread capability with random grayscale image
function test_val = test_imwrite_imread
   a =  uint8(255*rand(64));
   imwrite(a, 'test.bmp')
   b = imread('test.bmp');
   test_val = issame(a,b);
