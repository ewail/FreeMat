0
x = rand(512);
x((-64:63)+256,(-128:127)+256) = 1.0;
figure
image(x)
colormap(gray)
mprint image1
