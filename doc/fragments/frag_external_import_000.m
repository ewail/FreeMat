0
if (strcmp(computer,'MAC')) system('gcc -bundle -flat_namespace -undefined suppress -o add.so addArrays.c'); end;
if (~strcmp(computer,'MAC')) system('gcc -shared -fPIC -o add.so addArrays.c'); end;
import('add.so','addArrays','addArrays','void','int32 N, float[N] a, float[N] b, float[N] &c');
a = [3,2,3,1];
b = [5,6,0,2]; 
c = [0,0,0,0];
addArrays(length(a),a,b,c)
c
