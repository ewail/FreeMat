0
a = threadnew;
threadstart(a,'svd',1,A);
sigmas = threadvalue(a);
threadfree(a)
