function test_val = do_jit_test(test_name)
printf('Running %s without JIT...',test_name);
jitcontrol off
tic; A1 = feval(test_name); nojit = toc;
jitcontrol on
% Run it twice to remove the compile time
tic; A2 = feval(test_name); wjit = toc;
tic; A2 = feval(test_name); wjit = toc;
printf('Speedup is %g...',nojit/wjit);
test_val = issame(A1,A2);
if (test_val)
  printf('PASS\n');
else
  printf('FAIL\n');
end
