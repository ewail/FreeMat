function y = horzcat(varargin)
  y = [];
for i = 1:length(varargin)
  y = [y,double(varargin{i})];
end
y = mat(y);
