% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

% Process the fragment files to generate the
% text fragments that are inserted into the 
% help files.
function fraggen(fname)
  % Get the list of all fragment groups
  cd(fname)
  fraglist = dir('frag_*_000.m');
  for i=1:numel(fraglist)
    % Create a new thread for this fragment group
    exec_id = threadnew;
    % Get the set of fragments for this group
    n = strfind(fraglist(i).name,'000.m');
    groupbase = fraglist(i).name(1:(n-1));
    grouplist = dir([groupbase,'*.m']);
    for j=1:numel(grouplist)
      printf('Processing fragment %s\n',grouplist(j).name)
      fp = fopen(grouplist(j).name,'r');
      pline = fgetline(fp);
      errors_expected = str2num(pline);
      cmdlist = {};
      while (~feof(fp))
	pline = fgetline(fp);
	cmdlist = [cmdlist,{pline}];
      end
      fclose(fp);
      wd = pwd;
      etext = threadcall(exec_id,100000,'simkeys',cmdlist);
      cd(wd);
      etext = regexprep(etext,'(--> mprint[^\n]*\n)','');
      etext = regexprep(etext,'[\n]*--> quit\n',sprintf('\n'));
      errors_occured = threadcall(exec_id,100000,'errorcount');
      if (errors_occured ~= errors_expected)
	     keyboard
        error('Error count mismatch on block');
      end
      fp = fopen([grouplist(j).name,'.out'],'w');
      fprintf(fp,'%s',etext);
      fclose(fp);
    end
    threadfree(exec_id);
    close all;
  end
