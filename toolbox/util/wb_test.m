function wb_test(text,testname,refloc)
  v = regexp(text,'@\$([^#]*)#([^#]*)#?(.*)','tokens');
  ttype = v{1}{1};
  expr = v{1}{2};
  exclude = v{1}{3};
  if (isempty(exclude)) exclude = '0'; end
  load([refloc,dirsep,'wbinputs.mat'])
  load([refloc,dirsep,testname,'_ref.mat'])
  fail_count = 0;
  in_count = 0;
  vars = symvar(expr);
  pnt = regexp(vars,'x.');
  for i=1:numel(pnt)
    if (~isempty(pnt{i}))
      in_count = in_count + 1; 
    end; 
  end
  pnt = regexp(vars,'y.');
  out_count = 0;
  for i=1:numel(pnt)
    if (~isempty(pnt{i}))
      out_count = out_count + 1;
    end
  end
  if (in_count == 0)
    error_flag = 0;
    for k=1:out_count
      eval(sprintf('y%d = [];\n',k));
    end
    eval([expr,';'],'error_flag=1;');
    if (error_flag && ~error_refs)
      return;
    elseif (~error_flag && ~error_refs)
      for k=1:out_count
        if (~eval(sprintf('wbtest_%s(y%d,y%d_refs{1})',ttype,k,k)))
          return;
        end
      end
    end
  end
  if (in_count == 1)
    for loopi=1:numel(wbinputs)
      x1 = wbinputs{loopi};
      error_flag = 0;
      for k=1:out_count
        eval(sprintf('y%d = [];\n',k));
      end
      eval([expr,';'],'error_flag=1;');
      if (error_flag && ~error_refs(loopi) && ~eval(exclude))
        return;
      elseif (~error_flag && ~error_refs(loopi) && ~eval(exclude))
        for k=1:out_count
          if (~eval(sprintf('wbtest_%s(y%d,y%d_refs{loopi})',ttype,k,k)))
            return;
          end
        end
      end
    end
  end
  if (in_count == 2)
    for loopi=1:numel(wbinputs)
      for loopj=1:numel(wbinputs)
        x1 = wbinputs{loopi};
        x2 = wbinputs{loopj};
        error_flag = 0;
        for k=1:out_count
            eval(sprintf('y%d = [];\n',k));
        end
        eval([expr,';'],'error_flag=1;');
        if (error_flag && ~error_refs(loopi,loopj) && ~eval(exclude))
            return;
        elseif (~error_flag && ~error_refs(loopi,loopj) && ~eval(exclude))
            for k=1:out_count
                if (~eval(sprintf('wbtest_%s(y%d,y%d_refs{loopi,loopj})',ttype,k,k)))
                    return;
                end
            end
        end
      end
    end
  end
  saveretvalue(0);
