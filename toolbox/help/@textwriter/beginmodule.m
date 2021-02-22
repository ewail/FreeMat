
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function beginmodule(&p,sourcepath,modname,moddesc,secname,section_descriptors)
    p.moddesc = moddesc;
    p.secname = lower(secname);
    p.modulename = lower(modname);
    p.section_descriptors = section_descriptors;
    p.sourcepath = sourcepath;
    filename = [sourcepath '/help/text/' p.modulename '.mdc'];
    p.myfile = fopen(filename,'a');
    if (p.myfile < 0) 
      error(sprintf('unable to open %s for output',filename)); 
    end	
    fprintf(p.myfile,'%s %s\n',modname,moddesc);
    p.verbatim = 0;
    p.ignore = 1;
    if (~isfield(p.sectables,p.secname))
      p.sectables.(p.secname) = {};
    end
    p.sectables.(p.secname) = [p.sectables.(p.secname);{p.modulename, ...
                        p.moddesc}];
