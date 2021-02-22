% DOCBLOCK io_input

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function y = input(prompt,stringflag)
  if (isset('stringflag') && ischar(stringflag) && (strcmp(stringflag,'s') | strcmp(stringflag,'S')))
    y = getline(prompt);
    y(end) = [];
  elseif isset('stringflag')
    error 'second argument to input must be the string ''s'' or ''S''.'
  else
    needval = 1;
    while (needval)
      a = getline(prompt);
      a(end) = [];
      needval = 0;
      if (~isempty(a))
        y = evalin('caller',a,'0;printf(''%s\n'',lasterr);needval=1;');
      else
        y = [];
      end
    end
  end
