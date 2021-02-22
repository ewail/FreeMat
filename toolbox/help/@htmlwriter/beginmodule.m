
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function beginmodule(&p,sourcepath,modname,moddesc,secname,section_descriptors)
    p.moddesc = moddesc;
    p.secname = lower(secname);
    p.modulename = lower(modname);
    p.section_descriptors = section_descriptors;
    p.sourcepath = sourcepath;
    p.eqnlist = {};
    filename = [sourcepath '/help/html/' p.secname '_' p.modulename '.html'];
    p.myfile = fopen(filename,'w');
    if (p.myfile < 0) 
      error(sprintf('unable to open %s for output',filename)); 
    end	
    fprintf(p.myfile,'<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">\n');
    fprintf(p.myfile,'\n');
    fprintf(p.myfile,'<HTML>\n');
    fprintf(p.myfile,'<HEAD>\n');
    fprintf(p.myfile,'<TITLE>%s</TITLE>\n',moddesc);
    fprintf(p.myfile,'</HEAD>\n');
    fprintf(p.myfile,'<BODY>\n');
    fprintf(p.myfile,'<H2>%s</H2>\n',moddesc);
    if (~isfield(p.sectables,p.secname))
      p.sectables.(p.secname) = {};
    end
    p.sectables.(p.secname) = [p.sectables.(p.secname);{p.modulename, ...
                        p.moddesc}];
    fprintf(p.myfile,'<P>\n');
    fprintf(p.myfile,'Section: <A HREF=sec_%s.html> %s </A>\n',lower(secname),section_descriptors.(lower(secname)));
    p.verbatim = 0;
    p.ignore = 0;
