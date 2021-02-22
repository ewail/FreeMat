function test_val = test_gausfit1
t = linspace(-1,1);
y = exp(-t.^2/8);
[mu,sigma,dc,gain,yhat] = gausfit(t,y)
test_val = (abs(mu) < .0001) && (abs(sigma-2) < 0.0001);
