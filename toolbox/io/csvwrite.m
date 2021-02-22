% DOCBLOCK io_csvwrite

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function csvwrite(filename,x,firstrow,firstcol)
    if (nargin == 2)
        firstrow = 0;
        firstcol = 0;
    elseif (nargin == 3)
        firstcol = 0;
    elseif (nargin < 2)
        error('csvwrite expects a matrix to write to the CSV file');
    end
    if (ndims(x) ~= 2)
        error('csvwrite cannot write multidimensional arrays');
    end
    if (~isnumeric(x))
        error('csvwrite cannot write non-numerical arrays');
    end
    if (isreal(x))
      csvwrite_real(filename,x,firstrow,firstcol);
    else
      csvwrite_complex(filename,x,firstrow,firstcol);
    end
    
function csvwrite_real(filename,x,firstrow,firstcol)
    fp = fopen(filename,'w');
    for i=0:(firstrow-1)
        fprintf(fp,'\n');
    end
    if (firstcol > 0)
        prepend_string = repmat(',',[1,(firstcol-1)]);
    else
        prepend_string = '';
    end
    for j=1:size(x,1)
        fprintf(fp,prepend_string);
        for k=1:(size(x,2)-1)
            fprintf(fp,'%.20g',x(j,k));
            fprintf(fp,',');
        end
        fprintf(fp,'%.20g',x(j,end));
        fprintf(fp,'\n');
    end
    fclose(fp);
    
function csvwrite_complex(filename,x,firstrow,firstcol)
    fp = fopen(filename,'w');
    for i=0:(firstrow-1)
        fprintf(fp,'\n');
    end
    if (firstcol > 0)
        prepend_string = repmat(',',[1,(firstcol-1)]);
    else
        prepend_string = '';
    end
    for j=1:size(x,1)
        fprintf(fp,prepend_string);
        for k=1:(size(x,2)-1)
            fprintf(fp,['%.20g','+','%.20g','i'],real(x(j,k)),imag(x(j,k)));
            fprintf(fp,',');
        end
        fprintf(fp,['%.20g','+','%.20g','i'],real(x(j,end)),imag(x(j,end)));
        fprintf(fp,'\n');
    end
    fclose(fp);
    
