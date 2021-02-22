# python utility to strip documentation from source files.
# Creates links to the documentation using the "DOCLINK" mark.
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


def matches(line,pattern):
    return re.search(pattern,line,re.DOTALL)

def getfirstgroup(line,pattern):
    return re.search(pattern,line,re.DOTALL).group(1)

def visit(arg, dirname, names):
    for name in names:
        if (re.search('\+octave',dirname)):
            return
        (root, ext) = os.path.splitext(name)
        if (ext == '.cpp'):
            return
            print('Stripping file %s/%s'%(dirname,name))
            iname = '%s/%s'%(dirname,name)
            h = open(iname,'rU')
            oname = '%s/%s.out'%(dirname,name)
            g = open(oname,'w')
            inblock = False
            insig = False
            repcount = 0
            for line in h:
                if (matches(line,r'^//!')):
                    inblock = not inblock
                    if (not inblock): 
                        insig = False
                        line = '//DOCBLOCK %s_%s\n'%(section.lower(),module.lower())
                        repcount = repcount+1
                if (inblock and matches(line,r'^//@@Signature')):
                    insig = True
                if (inblock and matches(line,r'^//@Module\s*(\w*)')):
                    module = getfirstgroup(line,r'^//@Module\s*(\w*)')
                if (inblock and matches(line,r'^//@@Section\s*(\w*)')):
                    section = getfirstgroup(line,r'^//@@Section\s*(\w*)')
                if (insig and inblock):
                    g.write(line)
                if (not inblock):
                    g.write(line)
            g.close()
            h.close()
            if (repcount == 0):
                os.unlink(oname)
            else:
                os.unlink(iname)
                os.rename(oname,iname)
        if (ext == '.m'):
            print('Stripping file %s/%s'%(dirname,name))
            found = False
            h = open('%s/%s'%(dirname,name),'rU')
            for line in h:
                line = line[:-1]
                if matches(line,r'@Module\s*(\w*)'):
                    module = getfirstgroup(line,r'@Module\s*(\w*)')
                    found = True
                if matches(line,r'@@Section\s*(\w*)'):
                    section = getfirstgroup(line,r'@@Section\s*(\w*)')
                    found = True
            h.close()
            if (found): 
                print('File %s --> DOCBLOCK %s_%s'%(name,section,module))
            else:
                print('Warning: %s is not doc-linked'%(name))
            state = 0
            oname = '%s/%s.out'%(dirname,name)
            iname = '%s/%s'%(dirname,name)
            g = open(oname,'w')
            h = open(iname,'rU')
            for line in h:
                if (matches(line,r'^%!')): 
                    state = state + 1
                    if (state == 2):
                        g.write(r'%% DOCBLOCK %s_%s'%(section.lower(),module.lower()))
                        g.write('\n')
                        line = ''
                if (state >=2): g.write(line)
            h.close()
            g.close()
            os.unlink(iname)
            os.rename(oname,iname)


basedir = sys.argv[1]
os.chdir(basedir)
os.path.walk('.', visit, '')
