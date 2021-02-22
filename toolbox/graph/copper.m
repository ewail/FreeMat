% DOCBLOCK handle_copper

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function map = copper
basecolor = [184,115,51]/255.0; basecolor = basecolor/basecolor(1);
map = linspace(0,1,256)'*basecolor;
