% DOCBLOCK io_str2num
function y = str2num(x)
if (~isstr(x)) 
  y = []; 
  return;
end
try
  y = eval(['[',x,']'],'[]');
catch
  y = [];
end
