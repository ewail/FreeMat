function wbtestcompare(fname_input,fieldname_base,fieldname_test)
  diary wbtest.txt
  load(fname_input);
  for ndx=1:size(recs,1)
    if (~recs{ndx}.(fieldname_base).success && recs{ndx}.(fieldname_test).success)
      inputs = recs{ndx}.inputs; 
      printf('Failed: %d expr %s\n',ndx,recs{ndx}.expr);
    elseif (~recs{ndx}.(fieldname_base).success && ~recs{ndx}.(fieldname_test).success)
      if (strcmp(recs{ndx}.test,'exact'))
        if (~issame(recs{ndx}.(fieldname_base).result{1},recs{ndx}.(fieldname_test).result{1}))
          printf('Mismatch: %d expr: %s\n',ndx,recs{ndx}.expr);
        end       
      elseif (strcmp(recs{ndx}.test,'near'))
        a = recs{ndx}.(fieldname_base).result;
	b = recs{ndx}.(fieldname_test).result;
	if (~isempty(a) && ~isempty(b))
	  a = a{1};
          b = b{1};
          if (~test_near(a,b))
            printf('Mismatch (norm): %d expr: %s\n',ndx,recs{ndx}.expr);
          end
        end
      elseif (strcmp(recs{ndx}.test,'near_permute'))
        a = recs{ndx}.(fieldname_base).result;
	b = recs{ndx}.(fieldname_test).result;
	if (~isempty(a) && ~isempty(b))
	  a = sort(a{1}(:));
          b = sort(b{1}(:));
          if (~test_near(a,b))
            printf('Mismatch (near permute): %d expr: %s\n',ndx,recs{ndx}.expr);
          end
        end       
      end
    end
  end
%
%  outputs_base = outputs;
%  recs_base = recs;
%  load(fname_ref);
%  outputs_ref = outputs;
%  recs_ref = recs;
%  e1count = 0;
%  e2count = 0;
%  for ndx=1:size(recs_ref,1);
%    rec_ref = recs_ref(ndx,:);
%    rec_base = recs_base(ndx,:);
%    expr = exprs{rec_ref(4)}; expr(end) = [];
%    if ((rec_ref(5) == -1) || (rec_base(5) == -1))
%      continue;
%    end
%    if (rec_ref(5) && ~rec_base(5))
%      printf('Failed: %d expr: %s inputs %d %d output %d\n',ndx,expr,rec_ref(1),rec_ref(2),rec_base(6));
%      e1count = e1count + 1;
%    elseif (~rec_ref(5) && ~rec_base(5))
%      if (~issame(outputs_base{rec_base(6)},outputs_ref{rec_ref(6)}))
%        printf('Mismatch: %d expr: %s inputs %d %d outputs %d %d\n',ndx,expr,rec_ref(1),rec_ref(2),rec_ref(6),rec_base(6));
%        e2count = e2count + 1;
%      end
%    end
%  end
  diary off


function c = test_near(a,b)
  % screen out the nans
  p = (isnan(a));
  q = (isnan(b));
  if (any(p~=q)) 
     c = false; 
     return; 
  end
  a(p) = 0;
  b(p) = 0;
  % screen out the infs
  p = (isinf(a));
  q = (isinf(b));
  if (any(a(p) ~= b(q)))
     c = false; 
    return; 
  end
  a(p) = 0;
  b(p) = 0;
  nb = xnrm2(b);
  if (nb == 0)
    if (isa(a,'double'))
      c = (xnrm2(a(:)) < 1e-10);
    else
      c = (xnrm2(a(:)) < 1e-6);
    end
    return;
  end
  if (isa(a,'double'))
    c = (xnrm2(a(:)-b(:))/nb) < 1e-10;
  else
    c = (xnrm2(a(:)-b(:))/nb) < 1e-6;
  end

  
