function wrap_jit_test(name,speedtarget,shouldjit)
try
jcs = jitcontrol;
jitcontrol('off');
tic;
A1 = feval(name);
base = toc;
jitcontrol('flush')
jcount = jitstat;
jitcontrol('on');
A2 = feval(name);
jcountafter = jitstat;
tic;
A2 = feval(name);
up = toc;
printf('Speedup base %f target %f up %f\n',base,speedtarget,up);
printf('Jit succeeded %d\n',jcountafter > jcount);
printf('Issame %d\n',issame(A1,A2));
if (issame(A1,A2) && (base > speedtarget*up) && (shouldjit && (jcountafter > jcount)))
  saveretvalue(0);
end
catch
end
