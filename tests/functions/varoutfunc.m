function [varargout] = varoutfunc
  switch(nargout)
    case 1
      varargout = {'one of one'};
    case 2
      varargout = {'one of two','two of two'};
    case 3
      varargout = {'one of three','two of three','three of three'};
  end
