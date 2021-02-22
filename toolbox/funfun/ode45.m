% DOCBLOCK num_ode45
function varargout = ode45(f,tspan,y0,options,varargin)
if(nargin<4) options={}; end
abstol=generic_get(options,'AbsTol',1e-6);
reltol=generic_get(options,'RelTol',1e-3);
maxstep=generic_get(options,'MaxStep',(tspan(2)-tspan(1))/10);
h=generic_get(options,'InitialStep',maxstep/100);
stepper=generic_get(options,'Stepper',@fehlberg_step);
event=generic_get(options,'Events',[]);
project=generic_get(options,'Projection',[]);
absoverrel=abstol/reltol;
k=1;
t(1,1)=tspan(1);
y(:,1)=reshape(y0,length(y0),1);
te=[]; ye=[]; we=[]; terminate=0;
f0=feval(f,t,y0,varargin{:});
tnext=tspan(1)+h;
while t(k)<tspan(2) & terminate==0
    [yn,error,fs,interp,pwr]=feval(stepper,t(k),y(:,k),h,f0,f,varargin);
    err=h*max(abs(error)./max(max(abs(y(:,k)),abs(yn)),abstol/reltol));
    if err>reltol
       h=h/2;
    else
       k=k+1;
       y(:,k)=yn;
       t(k)=t(k-1)+h;
       if size(fs,2)==6 fs(:,7)=feval(f,t(k),yn,varargin{:}); end;
       f0=fs(:,7);
       f3d(k,:,:)=fs;
       if ~ isempty(event)
          [ts,ys,ws,terminate]= ...
                findallzeros(t(k-1),t(k),y(:,k-1),feval(interp,y(:,k-1),yn,h,fs),...
                abstol,event,varargin);
            te=[te;ts]; ye=[ye,ys]; we=[we;ws];
        end;
        if ~ isempty(project)
           yn=feval(project,t,yn,varargin{:});
           if ~ isempty(event)
              [v1,isterminal,direction]=feval(event,t(k),y(:,k),varargin{:});
              [v2,isterminal,direction]=feval(event,t(k),yn,varargin{:});
              ws=v1*v2<0 & (v2-v1)*direction>=0;
              if ws
                 te=[te;t(k)];
                 ye=[ye,yn];
                 we=[we;ws]
              end;
           end;
           y(:,k)=yn;
           fs(:,7)=feval(f,t(k),yn,varargin{:});
        end;
        foo=((err/reltol)^pwr);
        if foo>0.2 h=0.8*h/foo; else h=h*5; end;
        h=min(min(h,maxstep),tspan(2)-t(k));
    end
end
if nargout>=2
    varargout{1}=t;
    varargout{2}=y';
    if nargout>=3
        varargout{3}=te;
        if nargout>=4
            varargout{4}=ye;
            if nargout>=5 varargout{5}=we; end;
        end;
    end;
elseif nargout==1
    sol.x=t; sol.y=y; sol.xe=te; sol.ye=ye; sol.ie=we;
    sol.solver='generic_ode_solver';
    sol.interpolant=interp;
    sol.idata.f3d=f3d;
    varargout{1}=sol;
end;
 
function val=generic_get(options,name,default);
if isempty(options) val=default; return; end;
if isfield(options,name)
    if isempty(getfield(options,name)) val=default;
    else val=getfield(options,name); end;
else val=default;
end;

function [ynext,err,fs,interp,pwr]=fehlberg_step(t,y,h,f0,f,args);
A=([0 0 0 0 0 0
    1/4 0 0 0 0 0
    3/32 9/32 0 0 0 0
    1932/2197 -7200/2197 7296/2197 0 0 0
    439/216 -8 3680/513 -845/4104 0 0
    -8/27 2 -3544/2565 1859/4104 -11/40 0]')*h;
b=[0;1/4;3/8;12/13;1;1/2]*h;
d=([25/216 0 1408/2565 2197/4104 -1/5 0
    16/135 0 6656/12825 28561/56430 -9/50 2/55]')*h;
fs=zeros(size(f0,1),6);
fs(:,1)=f0;
for i=2:6
    fs(:,i)=feval(f,t+b(i),y+fs*A(:,i),args{:});
end;
ynext=(y+fs*d(:,2));
ynext4=(y+fs*d(:,1));
err=ynext-ynext4;
interp=@fehlberg_interpolant;
pwr=0.2;

function coeffs=fehlberg_interpolant(y,ynext,h,fs);
K=[1 0 0 0 0 0 0 0 0
   -1081/720 0 38464/4275 68107/150480 -27/200 -117/110 3/2 33/4 -33/4
   1/360 0 -76928/4275 -68107/75240 27/100 117/55 -4 -41/2 41/2
   359/720 0 38464/4275 68107/150480 -27/200 -117/110 5/2 45/4 -45/4];
data=(h.*fs)';
data(8,:)=y;
data(9,:)=ynext;
coeffs=(K*data)';

function [zs,ys,which,terminate]= ...
    findallzeros(t0,t1,y0,coeffs,tol,fun,args);
x5=[-1;(1-sqrt(5))/2;0;(sqrt(5)-1)/2;1];
A5 = [0., 0., 1., 0., 0.
0.309016994374947424102293414712, -1.30901699437494742410229341718, ...
    0., 1.30901699437494742410229341758, -.309016994374947424102293417691
-.309016994374947424102293416536, 2.11803398874989484820458683438, ...
    -3.61803398874989484820458683568, 2.11803398874989484820458683503, ...
    -.309016994374947424102293417308
-.809016994374947424102293417510, 1.30901699437494742410229341718, 0., ...
    -1.30901699437494742410229341758, 0.809016994374947424102293417426
0.809016994374947424102293417365, -2.11803398874989484820458683438, ...
    2.61803398874989484820458683516, -2.11803398874989484820458683503, ...
    0.809016994374947424102293417445];

x7=[-1;(-51235)/63889;(-41137)/92434;0;41137/92434;51235/63889;1];
A7 = [0., 0., 0., 1., 0., 0., 0.
-.222520933635899601186999247177, .777479065038070461054386787189, ...
    -2.02445866950865623828054626939, 0., 2.02445866950865623828054626939, ...
    -.777479065038070461054386787189, .222520933635899601186999247177
.222520933635899601186999247177, -.969500536473451423564042499204, ...
    4.54891734101570680237314373593, -7.60387547635630995999220096780, ...
    4.54891734101570680237314373593, -.969500536473451423564042499204, ...
    .222520933635899601186999247177
1.46950053661402731443823941200, -4.70290660118097799137857043054, ...
    5.17240714108433185119475716907, 0., -5.17240714108433185119475716907, ...
    4.70290660118097799137857043054, -1.46950053661402731443823941200
-1.46950053661402731443823941200, 5.86442861018544945625423023786, ...
    -11.6222933534042134898834670532, 14.4547305596655826961349524546, ...
    -11.6222933534042134898834670532, 5.86442861018544945625423023786, ...
    -1.46950053661402731443823941200
-1.74697960297812771325124016482, 3.92542753614290753032418364335, ...
    -3.14794847157567561291421089968, 0., 3.14794847157567561291421089968, ...
    -3.92542753614290753032418364335, 1.74697960297812771325124016482
1.74697960297812771325124016482, -4.89492807371199803269018773865, ...
    7.07337601238850668751032331724, -7.85085508330927273614275148682, ...
    7.07337601238850668751032331724, -4.89492807371199803269018773865, ...
    1.74697960297812771325124016482];
% Note, x5(1)=x7(1)=-1, x5(3)=x7(4)=0, x5(5)=x7(7)=1
[f0,isterminal,direction]=feval(fun,t0,y0,args{:});
tnext=t1; h=t1-t0;
zs=[]; which=[]; ys=[];
tstart=t0; terminate=0;
k=0;
while t0<t1
    ys7(1,:)=f0;
    for i=2:7
        s=((x7(i)+1)/2*h+t0-tstart)/(t1-tstart);
        yval7(:,i)=y0+coeffs*cumprod(s(ones(size(coeffs,2),1),1));
        ys7(i,:)=(feval(fun,(x7(i)+1)*(tnext-t0)/2+t0,yval7(:,i),args{:}))';
    end;
    ys5=zeros(5,size(ys7,2));
    ys5([1,3,5],:)=ys7([1,4,7],:);
    for i=[2,4]
        s=((x5(i)+1)/2*h+t0-tstart)/(t1-tstart);
        yval5(:,i)=y0+coeffs*cumprod(s(ones(size(coeffs,2),1),1));
        ys5(i,:)=(feval(fun,(x5(i)+1)*(tnext-t0)/2+t0,yval5(:,i),args{:}))';
    end;
    C7=A7*ys7;
    C5=zeros(size(C7));
    C5(1:5,:)=A5*ys5;
    D5=C7-C5;
    differentiator=(1:6)';
    DD5=differentiator(:,ones(size(D5,2))).*D5(2:7,:);
    err=0;
%    if k==0
%        xs=-1:0.01:1; x0=xs*(t1-t0)/2+(t0+t1)/2;
%        ss=((xs+1)/2*h+t0-tstart)/(t1-tstart);
%        yvals=y0(:,ones(length(ss),1))+coeffs*cumprod(ss(ones(size(coeffs,2),1),:));
%        for i=1:length(xs)
%            vs(:,i)=feval(fun,x0(i),yvals(:,i),args{:});
%        end;
%        plot(xs,polyval(C5(5:-1:1,1),xs)-0.05,'r-',xs,polyval(C7(7:-1:1,1),xs),'g-',xs,vs+0.05,'b-');
%        drawnow;
%    end;
    k=k+1;
    for i=1:size(C5,2)
        DD50=real(roots(DD5(6:-1:1,i)));
        DD50=DD50(find(DD50>=-1 & DD50<=1));
        err=max(max([ones(size(DD50,1),1),DD50,DD50.^2,DD50.^3,DD50.^4,DD50.^5,DD50.^6]*D5),err);
    end;
    if err>tol
        foo=(err/tol)^0.2;
        h=h/max(min(1.25*foo,10),2);
        if t0+h>=t1
            h=t1-t0; tnext=t1;
        else
            tnext=t0+h;
        end;
    else
        for i=1:size(C5,2)
            C70=sort(real(roots(C7(7:-1:1,i))));
            C70=[C70(find(C70(2:end,1)-C70(1:(length(C70)-1),1)>tol),1);C70(end,1)];
            mids=(C70(1:(length(C70)-1),1)+C70(2:end,1))/2;
            samples=[-1.25*abs(C70(1,1));mids;1.25*abs(C70(end,1))];
            vals=polyval(C7(7:-1:1,i),samples);
            matches=find(vals(2:end,1).*vals(1:(length(vals)-1),1)<0 & ...
                C70>=-1 & C70<=1 & ...
                (vals(2:end,1)-vals(1:(length(vals)-1),1))*direction(i)>=0 & ...
                abs(vals(2:end,1))>10*tol & abs(vals(1:(length(vals)-1)))>10*tol);
%            if 0
%                hold on;
%                ass1=samples;%(find(samples>-3 & samples<3));
%                ass2=C70;%(find(C70>-3 & C70<3));
%                samples
%                vals
%                C70
%                vals(2:end,1).*vals(1:(length(vals)-1),1)<0
%                vals(2:end,1)-vals(1:(length(vals)-1),1)
%                plot(ass1,zeros(size(ass1)),'xr',ass2,zeros(size(ass2)),'xb',...
%                    C70(matches),zeros(size(C70(matches))),'og');
%                hold off;
%                drawnow;
%            end;
            if matches
                zs=[zs;(C70(matches)+1)*h/2+t0];
                who=zeros(1,size(C70,2));
                who(1,i)=1;
                which=[which;who];
                s=((C70(matches)'+1)/2*h+t0-tstart)/(t1-tstart);
                yvals=y0+coeffs*cumprod(s(ones(size(coeffs,2),1),:));
                ys=[ys,yvals];
                if isterminal(i) terminate=1; end;
            end;
        end;
        t0=tnext;
        f0=ys7(7,:);
        foo=(err/tol)^0.2;
        h=h/max(min(1.25*foo,10),0.2);
%        y0=yval7(:,7);
        if t0+h>=t1
            h=t1-t0; tnext=t1;
        else
            tnext=t0+h;
        end;
    end;
end;
[zs,idx]=sort(zs);
which=which(idx,:);
