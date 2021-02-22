function success = test_jit
  tests = dir('jit_test*.m');
  success = true;
  for i=1:numel(tests)
    [direc,fname] = fileparts(tests(i).name);
    success = success && do_jit_test(fname);
  end
