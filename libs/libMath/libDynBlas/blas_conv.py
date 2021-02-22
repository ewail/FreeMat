#!/usr/bin/env python

import os
import re

class Fn:
    def __init__(self, function_name, parameter_string, return_type, call_params ):
        self.function_name = function_name
        self.parameter_string = parameter_string
        self.return_type = return_type
        self.call_params = call_params

f=file('blas_header','r');
lines=f.readlines()
f.close()

fns=[]

for line in lines:
    function_name=re.search('\w+(?=\()',line).group().lower()
    parameter_string=re.search('\(.+\)',line).group()
    return_type = re.search('(?<=extern\s)\S+',line).group()
    call_params = re.findall('(?<=[*,\s])\w+(?=[\,,\)])',line)
    call_params = '( ' + ", ".join(call_params) + ' )'
    fns.append( Fn( function_name, parameter_string, return_type, call_params ) )

fout=file('blas_wrap.cpp','w')

preamble= \
"""//This is a generated file. Do not edit!
#include <QString>
#include <QLibrary> 
#include "blas_wrap.h"
#include "blas_dyn_link.h"
#include <iostream>
#include <vector>

BlasWrapper wrapper;

#ifdef __cplusplus
extern "C" { 
#endif /* __cplusplus */ 
"""        
fout.write(preamble)


# Put function typedefs
for fn in fns:
    fdef = 'typedef ' + fn.return_type + ' (* p_' + fn.function_name + ' )' + fn.parameter_string + ';\n'
    fout.write(fdef)
fout.write('\n\n')

# Put reference BLAS declarations
fout.write('extern "C" {\n');
for fn in fns:
    fdef = fn.return_type + ' REF_' + fn.function_name + '_' + fn.parameter_string + ';\n'
    fout.write(fdef)
fout.write('}\n\n')

# put wrapper function definitions
i = 0
for fn in fns:
    fdecl = fn.return_type + ' ' + fn.function_name + '_' + fn.parameter_string + '\n{\n'
    if ((fn.function_name != 'zdotu') and (fn.function_name != 'zdotc') and \
            (fn.function_name != 'cdotu') and (fn.function_name != 'cdotc')):
        fdecl = fdecl + '\tif( !wrapper.fn['+ str(i) + '] )\n'
        fdecl = fdecl + '\t\twrapper.fn['+ str(i) + '] = wrapper.Resolve(\"' + fn.function_name + '\",(void (*)()) REF_' + fn.function_name + '_);\n'
        if fn.return_type != 'void':
            fdecl = fdecl+ '\treturn '
        else:
            fdecl = fdecl+ '\t'
        fdecl = fdecl + \
                '(* reinterpret_cast<p_'+fn.function_name+'>(wrapper.fn[' + str(i) + ']))' + fn.call_params +';\n'
    else:
        fdecl = fdecl + '\tREF_' + fn.function_name + '_' + fn.call_params  + ';';
    fout.write(fdecl+'\n}\n\n')
    i=i+1
nblasfns = i    
fout.write('\n\n')


postamble=\
"""#ifdef __cplusplus 
}
#endif /* __cplusplus */
"""
fout.write( postamble )

fout.close()

fout=file('blas_wrap.h','w')
fout.write("""#ifndef __BLAS_WRAP_H
#define __BLAS_WRAP_H
""")
fout.write("#define BLAS_NUM_FNS %d \n" % nblasfns)
fout.write("#endif /*_BLAS_WRAP_H*/\n\n")
fout.close()

fout=file('blas.h','w')
preamble= \
"""//This is a generated file. Do not edit!
#ifndef __BLAS_H
#define __BLAS_H

#ifdef __cplusplus
extern "C" { 
#endif /* __cplusplus */

"""        
fout.write(preamble)

# Put function typedefs
for fn in fns:
    fdef = fn.return_type + ' ' + fn.function_name + '_' + fn.parameter_string + ';\n'
    fout.write(fdef)
fout.write('\n\n')

postamble=\
"""#ifdef __cplusplus 
}
#endif /* __cplusplus */

#endif /*_BLAS_H*/
"""
fout.write( postamble )

fout=file('../libBLAS_C/blas_ref.def','w')
fout.write("""
LIBRARY blas_ref
EXPORTS
""")
i=1
for fn in fns:
    fout.write(fn.function_name + '_' + '\t@' + str(i) + '\n')
    i=i+1
fout.write('\n\n');
fout.close()
