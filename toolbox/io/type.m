% DOCBLOCK io_type

function type(filename)
fp = fopen(filename,'r');
if (fp == -1)
  f = which(filename);
  if (isempty(f)), return; end
  filename = f;
  fp = fopen(filename,'r');
end
if (fp == -1), return; end
while (~feof(fp))
  printf('%s',fgetline(fp));
end
fclose(fp);
