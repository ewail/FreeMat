% DOCBLOCK mathfunctions_angle

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function p = angle(c)

p = atan2(imag(c), real(c));

