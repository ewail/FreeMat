#!/usr/bin/python
# This script extracts the fragments from doc files and
# writes them to fragment directories.  
import os
import sys
import re
import shutil
import glob

basedir = ''

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
        if ext=='.doc':
            print('Processing file %s'%(name))
            f = open(dirname + '/' + name, 'rU')
            line = f.readline()
            while line:
                if (line.find('if FRAGMENT') != -1):
                    fragfilename = f.readline()[:-1]
                    print('Fragment %s'%(fragfilename))
                    q = open(basedir+'/doc/fragments/'+fragfilename,'w')
                    line = f.readline()
                    while (line.find(r'\endif') == -1):
                        q.write(line)
                        line = f.readline()
                    q.close()
                if (line.find('if FILE') != -1):
                    filefilename = f.readline()[:-1]
                    print('File %s'%(filefilename))
                    q = open(basedir+'/doc/fragments/'+filefilename,'w')
                    line = f.readline()
                    while (line.find(r'\endif') == -1):
                        q.write(line)
                        line = f.readline()
                    q.close()
                line = f.readline()

basedir = sys.argv[1]
makepath(basedir+'/doc/fragments/')
os.path.walk(basedir, visit, '')


