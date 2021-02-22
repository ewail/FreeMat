
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function endmodule(&p)
  for i=1:numel(p.clients)
    endmodule(p.clients{i});
  end
