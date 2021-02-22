
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function writeindex(&p)
fname = sprintf('%s/toolbox/test/run_gen_all.m', ...
                p.sourcepath);
fp = fopen(fname,'w');
if (fp < 0)
  error(sprintf('unable to open %s for output',filename));
end
fprintf(fp,'tlist = dir(''gen_*.m'');\n');
fprintf(fp,'for i=1:numel(tlist)\n');
fprintf(fp,'  [m,n,o,p] = fileparts(tlist(i).name);\n');
fprintf(fp,'  fprintf(''Running %%s...\\n'',n);\n');
fprintf(fp,'  feval(n);\n');
fprintf(fp,'end\n');
fclose(fp);
