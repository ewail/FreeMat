function x = test_power1
invD_11 = [0.3529    0.4028    0.5812    0.3333];
invD_12 = [0.0707   -0.0334   -0.8818    0.0000];
invD_22 = [0.4942    0.4028    2.3447    0.5000];
d1_k = [-1.2335   -0.3520   -1.5988   -1.8315];
d2_k = [1.5265   -1.5861    0.0067    1.0221];
vec_max = d1_k.^2.0.*invD_11+2*d1_k.*d2_k.*invD_12+d2_k.^2.0.*invD_22;
[value, pos1] = max(vec_max);
vec_max = d1_k  .^  2.0   .*invD_11+2*d1_k.*d2_k.*invD_12+d2_k  .^  2.0   .*invD_22;
[value, pos2] = max(vec_max);
vec_max = d1_k.^2.*invD_11+2*d1_k.*d2_k.*invD_12+d2_k.^2.*invD_22;
[value, pos3] = max(vec_max);
vec_max = d1_k.^2    .* invD_11+2*d1_k.*d2_k.*invD_12+d2_k.^2   .*invD_22;
[value, pos4] = max(vec_max);
x = (pos1 == 4) && (pos2 == 4) && (pos3 == 4) && (pos4 == 4);
