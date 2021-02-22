#include <stdio.h>
#include <ctype.h>
#include <string.h>

typedef void (*strfunc)(const char*);

strfunc PrintMessage;
strfunc PrintWarning;
strfunc PrintError;

void freemat_io_handler(strfunc message, strfunc warning, strfunc error) {
  PrintMessage = message;
  PrintWarning = warning;
  PrintError = error;
}

int addArrays(int N, float *a, float *b, float *c) {
  int i;
 
  for (i=0;i<N;i++)
   c[i] = a[i] + b[i];
  return N;
}

int sum_string(char* p) {
  PrintMessage(p);
  PrintMessage("\n");
  return strlen(p);
}

int upper_string(char* p) {
  while (*p) {
    *p = toupper(*p);
    p++;
  }
  PrintError("Say what?\n");
  return strlen(p);
}

void square_it(double *t) {
  PrintWarning("About to square a number!\n");
  *t = (*t)*(*t);
}

