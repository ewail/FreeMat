% Test FFT running in parallel threads
function test_val = test_parallel_fft1
a = rand(1000,1000);
b = rand(500,1000);
id1 = threadnew;
id2 = threadnew;
threadstart(id1,'fft',1,a);
threadstart(id2,'fft',1,b);
A = threadvalue(id1);
B = threadvalue(id2);
threadfree(id1);
threadfree(id2);
test_val = issame(A,fft(a)) && issame(B,fft(b));
