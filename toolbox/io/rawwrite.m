% DOCBLOCK io_rawwrite

% Contributed by Bruno DeMan
% Licensed under the GPL

function rawwrite(fname,x,byteorder)
  try 
    if (isset('byteorder'))
      fp = fopen(fname,'wb',byteorder);
    else
      fp = fopen(fname,'wb');
    end
    fwrite(fp,x);
    fclose(fp);
  catch
    error(['Unable to write file ' fname ' in function rawwrite']);
  end

