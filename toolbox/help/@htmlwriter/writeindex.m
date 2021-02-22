
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function writeindex(&p)
  secnames = fieldnames(p.section_descriptors);
  for n=1:numel(secnames)
    writesectiontable(secnames{n},p.sectables.(secnames{n}),p);
  end
  fp = fopen(sprintf('%s/help/html/index.html',p.sourcepath),'w');
  fprintf(fp,['<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">\n']);
  fprintf(fp,'\n');
  fprintf(fp,'<HTML>\n');
  fprintf(fp,'<HEAD>\n');
  fprintf(fp,'<TITLE>%s Documentation</TITLE>\n',verstring);
  fprintf(fp,'</HEAD>\n');
  fprintf(fp,'<BODY>\n');
  fprintf(fp,'<H1>%s Documentation</H1>\n',verstring);
  fprintf(fp,'<P>\n');
  fprintf(fp,'<H2> Documentation Sections </H2>\n');
  fprintf(fp,'<UL>\n');
  for n=1:numel(secnames)
    fprintf(fp,'<LI> <A HREF=sec_%s.html> %s </A> </LI>\n',secnames{n}, ...
            p.section_descriptors.(secnames{n}));
  end
  fprintf(fp,'</UL>\n');
  fprintf(fp,'</BODY>\n');
  fprintf(fp,'</HTML>\n');
  fclose(fp);
  
  fp = fopen(sprintf('%s/help/html/modules.txt',p.sourcepath),'w');
  modulenames = {};
  for n=1:numel(secnames)
    section = secnames{n};
    modules = p.sectables.(secnames{n});
    for k=1:size(modules,1);
      modulenames = [modulenames;{[modules{k,1}  ' (' section ')']}];
    end
  end
  modulenames = sort(modulenames);
  for n = 1:numel(modulenames)
    fprintf(fp,'%s\n',modulenames{n});
  end
  fclose(fp);

  fp = fopen(sprintf('%s/help/html/sectable.txt',p.sourcepath),'w');
  for n=1:numel(secnames)
    section = secnames{n};
    fprintf(fp,'%s\n',p.section_descriptors.(section));
    modulelist = sort(p.sectables.(section)(:,2));
    for k=1:numel(modulelist)
      fprintf(fp,'+(%s) %s',section,modulelist{k});
    end
  end
  fclose(fp);
  
function writesectiontable(secname,modules,&p)
  fp = fopen(sprintf('%s/help/html/sec_%s.html',p.sourcepath,secname), ...
             'w');
  fprintf(fp,['<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">\' ...
              'n']);
  fprintf(fp,'\n');
  fprintf(fp,'<HTML>\n');
  fprintf(fp,'<HEAD>\n');
  fprintf(fp,'<TITLE>%s</TITLE>\n',p.section_descriptors.(secname));
  fprintf(fp,'</HEAD>\n');
  fprintf(fp,'<BODY>\n');
  fprintf(fp,'<H2>%s</H2>\n',p.section_descriptors.(secname));
  fprintf(fp,'<P>\n');
  fprintf(fp,'<A HREF=index.html> Main Index </A>\n');
  fprintf(fp,'<P>\n');
  fprintf(fp,'<UL>\n');
  for n=1:size(modules,1)
    fprintf(fp,'<LI> <A HREF=%s_%s.html> %s </A> %s </LI>\n',secname, ...
            modules{n,1},modules{n,1},modules{n,2});
  end
  fprintf(fp,'</UL>\n');
  fprintf(fp,'</BODY>\n');
  fprintf(fp,'</HTML>\n');
  fclose(fp);

  
  
