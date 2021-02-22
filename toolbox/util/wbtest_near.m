function c = wbtest_near(a,b)
  if (issame(a,b)) 
    c = true;
    return;
  end
  if (~issame(size(a),size(b))) 
    c = false;
    return;
  end
  if (isa(a,'cell') || isa(b,'cell'))
    c = issame(a,b);
    return;
  end;
  % screen out the nans
  p = (isnan(a));
  q = (isnan(b));
  if (numel(find(q)) ~= numel(find(p)))
     c = false;
     return;
  end;
  if (any(p~=q)) 
     c = false; 
     return; 
  end
  a(p) = 0;
  b(p) = 0;
  % screen out the infs
  p = (isinf(a));
  q = (isinf(b));
  if (numel(find(q)) ~= numel(find(p)))
     c = false;
     return;
  end;
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
