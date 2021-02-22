function res = testmatch(line,pattern)
  res = ~isempty(regexp(line,pattern));
