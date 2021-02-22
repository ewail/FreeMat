% DOCBLOCK handle_newplot

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function h = newplot
fig = gcf;
fg_mode = get(fig,'nextplot');
if (strcmp(fg_mode,'replace'))
     clf;
end
ax = gca;
ax_mode = get(ax,'nextplot');
if (strcmp(ax_mode,'replace'))
     cla;
end
h = gca;



