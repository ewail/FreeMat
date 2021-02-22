0
t = linspace(0,1);
red = t'*t;
green = t'*(t.^2);
blue = t'*(0*t+1);
A(:,:,1) = red; 
A(:,:,2) = green; 
A(:,:,3) = blue;
image(A);
mprint image2
