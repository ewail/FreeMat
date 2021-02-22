% DOCBLOCK os_mkdir

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function status = mkdir(parentdir,dirname)
  if (~exist('dirname'))
    status = mkdir_core(parentdir);
  else
    status = mkdir_core([parentdir dirsep dirname]);
  end
  
  
