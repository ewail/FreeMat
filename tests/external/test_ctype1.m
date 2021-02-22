function test_val = test_ctype1
  ctypedefine('struct','tstruct','v1','int32','v2','double','v3','float','v4','uint32[5]');
  g.v1 = int32(54); g.v2 = pi; g.v3 = float(exp(1)); g.v4 = uint32(1:5);
  h = ctypefreeze(g,'tstruct');
  k = ctypethaw(h,'tstruct');
  test_val = issame(k,g);
