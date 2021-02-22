function tjit
double_test = 1:26;
jitcontrol on;
for i=double_test
%  s = sprintf('jittime tjit%d\n',i);
  s = sprintf('tjit%d\n',i);
  printf('Evaluating Test Module %d\n',i);
  eval(s);
end


