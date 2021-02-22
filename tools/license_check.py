import os

ignore_list = ['llvm', 'dependencies', 'levmar-2.3', 'libLAPACK_C',  'thirdparty', 'libBLAS_C', 'libARPACK_C']

def visit( arg,  dirname,  names):
    out = open('miss', 'a')
    
    for name in names:
        (root, ext) = os.path.splitext(name)

        res=[(dirname.find(lst) != -1) for lst in ignore_list]
        if any(res):
            continue
        if ext=='.m' or ext=='.cpp' or ext=='.c' or ext=='.h' or ext=='.hpp':
            path = os.path.join(dirname, name)
            licensed = False
            f = open(path, 'r')
            for line in f:
                if line.find('GPL') !=-1 or line.find('GNU General Public License')!=-1:
                    licensed=True
            f.close()
            if licensed==False:
                print path
                out.write(path)
                out.write('\n')
            
            
    out.close()
    
os.path.walk('c:\\freemat_src\\FreeMat4',  visit,  '')
