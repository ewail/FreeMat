% update a testmatrix with a fieldname
function wbgentests(fname,fieldname,funcname)
% Load the test inputs
fprintf('loading %s..\n',fname);
load(fname)
fprintf('done\n');
if (exist('funcname'))
  func_only = true;
else
  func_only = false;
end
for ndx=1:size(recs,1)
  if (rem(ndx,1000)==0)
    fprintf('Processing test record %d of %d...\r',ndx,size(recs,1));
  end
  rec = recs{ndx};
  error_flag = 0;
  if (~func_only || strstr(rec.expr,funcname))
    if (numel(rec.inputs) > 0)
      x1 = inputs{rec.inputs(1)};
    end
    if (numel(rec.inputs) > 1)
      x2 = inputs{rec.inputs(2)};
    end
    eval([rec.expr,';'],'error_flag = 1;');
    evt.success = error_flag;
    if (error_flag == 0)
      if (rec.out_count == 1)
        outpod = {y1};
      elseif (rec.out_count == 2)
        outpod = {y1,y2};
      elseif (rec.out_count == 3)
        outpod = {y1,y2,y3};
      elseif (rec.out_count == 4)
        outpod = {y1,y2,y3,y4};
      elseif (rec.out_count == 5)
        outpod = {y1,y2,y3,y4,y5};
      else
        outpod = {};
      end
      evt.result = outpod;
    end
    rec.(fieldname) = evt;
    recs{ndx} = rec;
  else
    recs{ndx} = rec;
  end
end
fprintf('\nsaving %s...\n',fname);
save(fname,'recs','inputs');
fprintf('done\n');
