
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function beginmodule(&p,sourcepath,modname,moddesc,secname, ...
                     section_descriptors)
    p.moddesc = moddesc;
    p.secname = lower(secname);
    p.modulename = lower(modname);
    p.section_descriptors = section_descriptors;
    p.sourcepath = sourcepath;
    filename = [sourcepath '/help/latex/' p.secname '_' p.modulename '.tex'];
    p.myfile = fopen(filename,'w');
    if (p.myfile < 0) 
      error(sprintf('unable to open %s for output',filename)); 
    end	
    fprintf(p.myfile,'\\section{%s}\n\n',moddesc(1:(end-1)));
    if (~isfield(p.sectables,p.secname))
      p.sectables.(p.secname) = {};
    end
    p.sectables.(p.secname) = [p.sectables.(p.secname);{p.modulename, ...
                        p.moddesc}];
    p.verbatim = 0;
    p.ignore = 0;
    
  
  
  
