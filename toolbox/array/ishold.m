% DOCBLOCK handle_ishold

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function k = ishold
k = strcmp(get(gca,'nextplot'),'add');
