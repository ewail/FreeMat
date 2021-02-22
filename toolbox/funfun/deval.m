function SX=deval(SOL,X);
for i=1:length(X)
    a=1; b=length(SOL.x); c=floor((a+b)/2);
    while SOL.x(c)>X(i) | SOL.x(c+1)<X(i)
        if SOL.x(c)>X(i) b=c; else a=c; end;
        c=floor((a+b)/2);
    end;
    coeffs=feval(SOL.interpolant,SOL.y(:,c),SOL.y(:,c+1), ...
        SOL.x(c+1)-SOL.x(c),shiftdim(SOL.idata.f3d(c+1,:,:),1));
    s=(X(i)-SOL.x(c))/(SOL.x(c+1)-SOL.x(c));
    SX(:,i)=SOL.y(:,c)+coeffs*cumprod(s(ones(size(coeffs,2),1),:));
end;
