
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function writeindex(&p)
  secnames = fieldnames(p.section_descriptors);
  fp = fopen(sprintf('%s/help/latex/FreeMat-%s.tex',p.sourcepath,version),'w');
  fprintf(fp,'\\documentclass{book}\n');
  fprintf(fp,'\\usepackage{graphicx}\n');
  fprintf(fp,'\\usepackage{amsmath}\n');
  fprintf(fp,'\\renewcommand{\\topfraction}{0.9}\n');
  fprintf(fp,'\\renewcommand{\\floatpagefraction}{0.9}\n');
  fprintf(fp,'\\oddsidemargin 0.0in\n');
  fprintf(fp,'\\evensidemargin 1.0in\n');
  fprintf(fp,'\\textwidth 6.0in\n');
  fprintf(fp,'\\title{%s Documentation}\n',verstring);
  fprintf(fp,'\\author{Samit Basu}\n');
  fprintf(fp,'\\begin{document}\n');
  fprintf(fp,'\\maketitle\n');
  fprintf(fp,'\\tableofcontents\n');
  for n=1:numel(secnames)
    section = secnames{n};
    fprintf(fp,'\\chapter{%s}\n',p.section_descriptors.(section));
    modulelist = sort(p.sectables.(section)(:,1));
    for k=1:numel(modulelist)
      fprintf(fp,'\\input{%s_%s}\n',section,lower(modulelist{k}));
    end
  end
  fprintf(fp,'\\end{document}\n');
  fclose(fp);

  
  
