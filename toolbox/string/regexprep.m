% DOCBLOCK string_regexprep

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function outputs = regexprep(instring,pattern,replacement,varargin)
if (iscellstr(instring))
  outputs = {};
  for i=1:numel(instring)
     outputs = [outputs,regexprep_helper(instring{i},pattern,replacement,varargin{:})];
  end;
  outputs = reshape(outputs,size(instring));
elseif (isstr(instring))
  outputs = regexprep_helper(instring,pattern,replacement,varargin{:});
else
  error('input instring must be either a string or a cell array of strings');
end

function outputs = regexprep_helper(instring,pattern,replacement,varargin)
if (iscellstr(pattern) && iscellstr(replacement))
  if (numel(pattern) ~= numel(replacement))
    error('when pattern and replacement are both cell-arrays of strings, they must both be the same size');
  end
  outputs = instring;
  for i=1:numel(pattern)
    outputs = regexprepdriver(outputs,pattern{i},replacement{i},varargin{:});
  end;
elseif (isstr(pattern) && (iscellstr(replacement) || isstr(replacement)))
  outputs = regexprepdriver(instring,pattern,replacement,varargin{:});
elseif (iscellstr(pattern) && isstr(replacement))
  outputs = instring;
  for i=1:numel(pattern)
    outputs = regexprepdriver(outputs,pattern{i},replacement,varargin{:});
  end;
else
  error('pattern and replacement must both be either cell-arrays of strings or strings');
end

