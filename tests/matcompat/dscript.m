a = {1,-1,inf,-inf,0,nan};
b = {};
for i=1:6
  for j=1:6
    b = [b,{complex(a{i},a{j})}];
  end
end
c = {};

for k=1:numel(b);
  for l=1:numel(b);
    fprintf('%f+i%f * %f+i%f = %f+i%f\n',real(b{k}),imag(b{k}),real(b{l}),imag(b{l}),real(b{k}*b{l}),imag(b{k}*b{l}));
    c = [c,{b{k}*b{l}}];
  end
end

