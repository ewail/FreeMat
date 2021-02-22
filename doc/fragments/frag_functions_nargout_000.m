0
a1 = nargouttest
[a1,a2] = nargouttest
[a1,a2,a3] = nargouttest
[a1,a2,a3,a4,a5] = nargouttest
nargout('sin')
y = @sin
nargout(y)
