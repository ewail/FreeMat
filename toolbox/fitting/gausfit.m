% DOCBLOCK curvefit_gausfit

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function [mu,sigma,dc,gain,yhat] = gausfit(t,y,w,mug,sigmag,dcg,gaing)
if (~isset('w')) 
  w = y*0+1; 
end
if (~isset('dcg')) 
  dcg = min(y(:)); 
end
ycor = y - dcg;
if (~isset('gaing')) 
  gaing = max(ycor); 
end
ycor = ycor/gaing;
if (~isset('mug')) 
  mug = sum(ycor.*t)/sum(ycor); 
end
if (~isset('sigmag')) 
  sigmag = sqrt(abs(sum((ycor).*(t-mug).^2)/sum(ycor))); 
end
[xopt,err] = fitfun('gfitfun',[mug,sigmag,dcg,gaing],y,w,eps,t);
mu = xopt(1);
sigma = xopt(2);
dc = xopt(3);
gain = xopt(4);
yhat = gfitfun(xopt,t);


