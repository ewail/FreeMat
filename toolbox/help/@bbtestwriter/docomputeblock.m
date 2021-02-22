
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function docomputeblock(&p,cmds,errorsexpected)
  if ((strcmp(p.modulename,'retall')  == 0) && ...
      (strcmp(p.modulename,'keyboard') == 0) && ...
      (strcmp(p.modulename,'return') == 0) && ...
      (strcmp(p.modulename,'where') == 0))
    fprintf(p.myfile,'NumErrors = 0;\n');
    for i=1:numel(cmds)
      fprintf(p.myfile,'try\n');
      fprintf(p.myfile,'  %s\n',cmds{i});
      fprintf(p.myfile,'catch\n');
      fprintf(p.myfile,'  NumErrors = NumErrors + 1;\n');
      fprintf(p.myfile,'end\n');
    end
    fprintf(p.myfile,['if (NumErrors ~= %d) bbtest_success = 0; return; ' ...
                      'end\n'],errorsexpected);
    p.empty = 0;
  end
  
