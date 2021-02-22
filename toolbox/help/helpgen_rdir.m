function file_list = helpgen_rdir(basedir)
  file_list = {};
  avec = dir(basedir);
  for (i=1:numel(avec))
    if (~(strcmp(avec(i).name,'.')  || (strcmp(avec(i).name,'..'))))
      cpath = [basedir dirsep avec(i).name];
      if (avec(i).isdir)
        if (~strcmp(avec(i).name,'.svn'))
          subdir_list = helpgen_rdir(cpath);
          file_list = [file_list;subdir_list];
        end
      else
        file_list = [file_list;{cpath}];
      end
    end
  end
