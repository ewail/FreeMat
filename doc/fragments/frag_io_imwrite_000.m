0
a =  uint8(255*rand(64));
figure(1), image(a), colormap(gray)
title('image to save')
imwrite(a, 'test.bmp')
