
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function endmodule(&p)
  fclose(p.myfile);
  if (p.empty)
    delete(p.filename);
  end
