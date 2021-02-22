import os
import sys
import re
import shutil
import glob

def makepath(path):

    """ creates missing directories for the given path and
        returns a normalized absolute version of the path.

    - if the given path already exists in the filesystem
      the filesystem is not modified.

    - otherwise makepath creates directories along the given path
      using the dirname() of the path. You may append
      a '/' to the path if you want it to be a directory path.

    from holger@trillke.net 2002/03/18
    """

    from os import makedirs
    from os.path import normpath,dirname,exists,abspath
    dpath = normpath(dirname(path))
    if not exists(dpath): makedirs(dpath)
    return normpath(abspath(path))


def visit(arg, dirname, names):
    for name in names:
        (root, ext) = os.path.splitext(name)
        if (ext == '.m'):
            print('merging file %s'%(name))
            makepath('../'+dirname+'/')
            headertext = ''
            if os.path.exists('../help/text/%s.mdc'%(root)):
                h = open('../help/text/%s.mdc'%(root),'r')
                for line in h:
                    headertext = headertext + '% ' + line
                h.close()
            h = open('%s/%s'%(dirname,name),'r')
            g = open('../%s/%s'%(dirname,name),'w')
            skip = False
            for line in h:
                if (line.rstrip() == '%!'):
                    skip = not skip
                if (not skip):
                    if (line.rstrip() != '%!'):
                        g.write(line)
                    else:
                        g.write(headertext)
            g.close()
            h.close()
            

basedir = sys.argv[1]
os.chdir(basedir)
# Merge in the fragment text
os.path.walk('.', visit, '')
makepath('../toolbox/jit/')
jitheaders = glob.glob('toolbox/jit/*.hpp')
for jit in jitheaders:
    print("Copy file %s to toolbox directory"%(jit))
    shutil.copyfile(jit,'../'+jit)
