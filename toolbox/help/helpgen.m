% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function helpgen(source_path,test_only)
  global sourcepath section_descriptors genfiles skipexec

  diary([source_path,'/help_diary.txt']);

  genfiles = {};
  printf('Cleaning up help directories...\n');
  delete_set(helpgen_rdir([source_path,'/help']));
  printf('Cleaning up toolbox directories...\n');
  delete_set(helpgen_rdir([source_path,'/toolbox']));
  
  % This one is not monitored by svn -- we can just delete it.
  rmdir([source_path,'/help/tmp']);

  mkdir([source_path,'/help/html']);
  mkdir([source_path,'/help/tmp']);
  mkdir([source_path,'/help/latex']);
  mkdir([source_path,'/help/text']);
  mkdir([source_path,'/toolbox/test']);
  mkdir([source_path,'/toolbox']);

  sourcepath = source_path;
  read_section_descriptors;
  if (exist('test_only'))
    p = groupwriter({testwriter,bbtestwriter});
    skipexec = true;
  else
    p = groupwriter({htmlwriter,latexwriter,bbtestwriter,textwriter, ...
                     testwriter});
    skipexec = false;
  end
  file_list = {};
  file_list = [file_list;helpgen_rdir([source_path,'/libs/libXP'])];
  file_list = [file_list;helpgen_rdir([source_path,'/libs/libCore'])];
  file_list = [file_list;helpgen_rdir([source_path,'/libs/libFN'])];
  file_list = [file_list;helpgen_rdir([source_path,'/libs/libFreeMat'])];
  file_list = [file_list;helpgen_rdir([source_path,'/libs/libGraphics'])];
  file_list = [file_list;helpgen_rdir([source_path,'/libs/libMatC'])];
  file_list = [file_list;helpgen_rdir([source_path,'/libs/libMath'])];
  file_list = [file_list;helpgen_rdir([source_path,'/libs/libMex'])];
  file_list = [file_list;helpgen_rdir([source_path,'/src'])];

  cd([sourcepath,'/help/tmp']);

  for i=1:numel(file_list)
    [mpath,name,suffix] = fileparts(file_list{i});
    if (~strcmp(name,'MPIWrap'))
      helpgen_processfile(file_list{i},p);
    end
  end

  if (~skipexec)
    file_list = helpgen_rdir([source_path,'/src/toolbox']);
    for i=1:numel(file_list)
      merge_mfile(file_list{i});
    end
  end
  writeindex(p);
  if (skipexec), return; end
  copyfile([source_path,'/help/tmp/*.jpg'],[source_path,'/help/latex'])
  copyfile([source_path,'/help/tmp/*.png'],[source_path,'/help/html'])
  copyfile([source_path,'/src/toolbox/help/run_tests.m'],[source_path,'/toolbox/test'])
  delete([source_path,'/toolbox/help/run_tests.m']);
  for i=1:numel(genfiles)
    copyfile([source_path,'/help/tmp/',genfiles{i}],[source_path,'/toolbox/test']);
  end
  copyfile([source_path,'/src/toolbox/test/reference/*.mat'],[source_path,'/toolbox/test/reference'])
  printf('To complete documentation generation -- run pdflatex on main.tex\n');
  printf('in help/latex directory -- several runs are required to get index\n');
  printf('and table of content generation.\n');
  diary off
  
function merge_mfile(filename)
  global sourcepath
  [mpath,name,suffix] = fileparts(filename);
  if (~strcmp(suffix,'.m')) return; end
  printf('Merging comments with file %s\n',filename);
  headertext = [];
  try 
    fp = fopen([sourcepath,'/help/text/',name,'.mdc'],'r');
    while (~feof(fp))
      text = getline(fp);
      headertext = [headertext,'% ',text];
    end
    fclose(fp);
  catch
  end
  h = fopen(filename,'r');
  newname = strrep(filename,'src/toolbox','toolbox');
  [mpath,name,suffix] = fileparts(newname);
  mkdir(mpath);
  g = fopen(newname,'w');
  while (~feof(h))
    text = getline(h);
    if (~isempty(regexp(text,'^\s*%!')))
      text = getline(h);
      while (isempty(regexp(text,'^\s*%!')))
        text = getline(h);
      end
      fprintf(g,'%s',headertext);
    else
      fprintf(g,'%s',text);
    end
  end
  fclose(h);
  fclose(g);
  
function read_section_descriptors
  global sourcepath section_descriptors
  fp = fopen([sourcepath,'/src/toolbox/help/section_descriptors.txt'],'r');
  while (~feof(fp))
    pline = fgetline(fp);
    p = regexp(pline,'(\w*)\s*([^\n]*)','tokens');
    section_descriptors.(p{1}{1}) = p{1}{2};
  end
  fclose(fp);
  

function helpgen_processfile(filename,&writers)
  global sourcepath section_descriptors
  [mpath,name,suffix] = fileparts(filename);
  if (strcmp(suffix,'.cpp'))
    comment = '//';
  elseif (strcmp(suffix,'.m'))
    comment = '%';
  else
    return;
  end
  prefix = ['^\s*' comment];
  pset = get_pattern_set(prefix);
  printf('Processing file %s...\n',filename);
  try
    fp = fopen(filename,'r');
  catch
    printf('Error %s...\n',lasterr);
    return;
  end
  while (1)
    if (feof(fp))
      fclose(fp);
      return;
    end
    pline = getline(fp);
    if (testmatch(pline,pset.docblock))
      pline = getline(fp);
      modname = mustmatch(pline,pset.modulename);
      printf('    Module %s...\n',lower(modname));
      moddesc = mustmatch(pline,pset.moduledesc);
      pline = getline(fp);
      secname = mustmatch(pline,pset.sectionname);
      beginmodule(writers,sourcepath,modname,moddesc,secname,section_descriptors);
      pline = getline(fp);
      exec_id = threadnew;
      while (~feof(fp) && ~testmatch(pline,pset.docblock))
         groupname = mustmatch(pline,pset.groupname);
         if (testmatch(groupname,'Signature'))
             pline = getline(fp);
             while (~feof(fp) && ~testmatch(pline,pset.groupname) ...
                 && ~testmatch(pline,pset.docblock))
                 pline = getline(fp);
             end
         else
             begingroup(writers,groupname);	 
             pline = getline(fp);
             while (~feof(fp) && ~testmatch(pline,pset.groupname) ...
                 && ~testmatch(pline,pset.docblock))
                 if (testmatch(pline,pset.execin))
                     handle_exec(pline,fp,pset,writers,exec_id);
                 elseif (testmatch(pline,pset.verbatimin)) 
                     handle_verbatim(pline,fp,pset,writers);
                 elseif (testmatch(pline,pset.figure))
                     handle_figure(pline,fp,pset,writers);
                 elseif (testmatch(pline,pset.eqnin))
                     handle_equation(pline,fp,pset,writers);
                 elseif (testmatch(pline,pset.fnin))
                     handle_filedump(pline,fp,pset,writers);
                 elseif (testmatch(pline,pset.enumeratein))
                     handle_enumerate(pline,fp,pset,writers);
                 elseif (testmatch(pline,pset.itemizein))
                     handle_itemize(pline,fp,pset,writers);
                 elseif (testmatch(pline,pset.ccomment))
                     handle_output(pline,fp,pset,writers);
                 else
                     error('Unprocessed line:%s',pline);
                 end
             end
         end
      end
      threadfree(exec_id);
      close all;
      endmodule(writers);
    end
  end

function pset = get_pattern_set(prefix)
  pset.docblock =     [prefix '!'];
  pset.modulename =   [prefix '@Module\s*(\w*)'];
  pset.moduledesc =   [prefix '@Module\s*(.*)'];
  pset.sectionname =  [prefix '@@Section\s*(\w*)'];
  pset.groupname =    [prefix '@@([ \w]*)'];
  pset.execin =       [prefix '@<(\d*)?'];
  pset.execout =      [prefix '@>'];
  pset.verbatimin =   [prefix '@\['];
  pset.verbatimout =  [prefix '@\]'];
  pset.ccomment =     [prefix '(.*)'];
  pset.figure =       [prefix '@figure\s*(\w+)'];
  pset.eqnin =        [prefix '\\\['];
  pset.eqnout =       [prefix '\\\]'];
  pset.fnin =         [prefix '@{\s*(\S+)'];
  pset.fnout =        [prefix '@}'];
  pset.enumeratein =  [prefix '\\begin{enumerate}'];
  pset.enumerateout = [prefix '\\end{enumerate}'];
  pset.itemizein =    [prefix '\\begin{itemize}'];
  pset.itemizeout =   [prefix '\\end{itemize}'];
  pset.item =         [prefix '\s*\\item(.*)'];

function pline = getline(fp)
  persistent version
  if (~exist('version'))
    b = regexp(verstring,'v(.*)','tokens');
    version = b{:}{1};
  end;
  pline = fgetline(fp);
  pline = strrep(pline,'<VERSION_NUMBER>',version);

function tok = mustmatch(pline,pattern)
  toks = regexpi(pline,pattern,'tokens');
  if (isempty(toks)) error(sprintf('Error on line: %s',pline)); end;
  tok = toks{1}{1};

function handle_filedump(&pline,fp,pset,&writers)
  global genfiles
  fname = mustmatch(pline,pset.fnin);
  pline = getline(fp);
  fn = '';
  while (~feof(fp) && ~testmatch(pline,pset.fnout))
    fn = [fn mustmatch(pline,pset.ccomment)];
    pline = getline(fp);
  end
  genfiles = [genfiles,{fname}];
  zp = fopen(fname,'w');
  fprintf(zp,'%s',fn);
  fclose(zp);
  if (isempty(regexp(fname,'test_\w+')))
    dofile(writers,fname,fn);
  end
  pline = getline(fp);

function handle_equation(&pline,fp,pset,&writers)
  pline = getline(fp);
  eqn = '';
  while (~feof(fp) && ~testmatch(pline,pset.eqnout))
     eqn = [eqn,mustmatch(pline,pset.ccomment)];
	pline = getline(fp);
  end
  doequation(writers,eqn);
  pline = getline(fp);

function handle_figure(&pline,fp,pset,&writers)
  dofigure(writers,mustmatch(pline,pset.figure));
  pline = getline(fp);

function handle_verbatim(&pline,fp,pset,&writers)
  pline = getline(fp);
  beginverbatim(writers);
  while (~feof(fp) && ~testmatch(pline,pset.verbatimout))
    outputtext(writers,mustmatch(pline,pset.ccomment));
    pline = getline(fp);
  end
  endverbatim(writers);
  if (feof(fp))
     error('unmatched verbatim block detected!');
  end
  pline = getline(fp);

function handle_enumerate(&pline,fp,pset,&writers)
  pline = getline(fp);
  itemlist = {};
  while (~feof(fp) && ~testmatch(pline,pset.enumerateout))
    item = mustmatch(pline,pset.item);
    pline = getline(fp);
    while (~testmatch(pline,pset.item) && ...
           ~testmatch(pline,pset.enumerateout) && ...
	   ~feof(fp))
      item = [item, mustmatch(pline,pset.ccomment)];
      pline = getline(fp);
    end
    itemlist = [itemlist,{item}];
  end
  if (feof(fp)), error('unmatched enumeration block'); end
  pline = getline(fp);
  doenumerate(writers,itemlist);
 
function handle_itemize(&pline,fp,pset,&writers)
  pline = getline(fp);
  itemlist = {};
  while (~feof(fp) && ~testmatch(pline,pset.itemizeout))
    item = mustmatch(pline,pset.item);
    pline = getline(fp);
    while (~testmatch(pline,pset.item) && ...
           ~testmatch(pline,pset.itemizeout) && ...
	   ~feof(fp))
      item = [item, mustmatch(pline,pset.ccomment)];
      pline = getline(fp);
    end
    itemlist = [itemlist,{item}];
  end
  if (feof(fp)), error('unmatched enumeration block'); end
  pline = getline(fp);
  doitemize(writers,itemlist);

function handle_output(&pline,fp,pset,&writers)
  pline = mustmatch(pline,pset.ccomment);
  outputtext(writers,pline);
  pline = getline(fp);

function handle_exec(&pline,fp,pset,&writers,exec_id)
  global sourcepath skipexec
  pline = mustmatch(pline,pset.execin);
  errors_expected = str2num(pline);
  cmdlist = {};
  pline = getline(fp);
  while (~feof(fp) && ~testmatch(pline,pset.execout))
    if (testmatch(pline,pset.ccomment))
      cmdlist = [cmdlist,{mustmatch(pline,pset.ccomment)}];
    end
    pline = getline(fp);
  end
  docomputeblock(writers,cmdlist,errors_expected);
  if (skipexec) return; end;
  cd([sourcepath,'/help/tmp']);
  beginverbatim(writers);
  etext = threadcall(exec_id,100000,'simkeys',cmdlist);
  etext = regexprep(etext,'(--> mprint[^\n]*\n)','');
  etext = regexprep(etext,'[\n]*--> quit\n',sprintf('\n'));
  outputtext(writers,etext);
  endverbatim(writers);
  if (threadcall(exec_id,100000,'errorcount') ~= errors_expected)
    printf('Error count mismatch on block\n');
    for (i=1:numel(cmdlist))
      printf('%s\n',cmdlist{i});
    end
  end
  cd([sourcepath,'/help/tmp']);
  pline = getline(fp);

  
function delete_set(p)
for i=1:numel(p)
  delete(p{i});
end
