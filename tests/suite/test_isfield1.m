function test_val = test_isfield1
  a.foo = 53;
  a.goo = 43;
  test_val = isfield(a,'foo') && isfield(a,'goo') && ~isfield(a,'round');
