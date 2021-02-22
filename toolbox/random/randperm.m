% DOCBLOCK random_randperm

% Copyright (c) 2008 Samit Basu
% Licensed under the GPL

function result = randperm(n)
% %     Slow version
%     m = floor(n);
%     result = 1:m;
%     for ii = 1:m
%         jj = round(rand(1,1)*(m-1))+1;
%         temp = result(ii);
%         result(ii) = result(jj);
%         result(jj) = temp;
%     end

%   This is much faster
    [A, result] = sort(rand(1,n));
end
