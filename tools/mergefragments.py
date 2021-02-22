import os
import sys
import re
import shutil
import glob

def latin_filter(text):
    text = text.replace('&','&amp;')
    text = text.replace('<','&lt;')
    text = text.replace('>','&gt;')
    return text

def visit(arg, dirname, names):
    for name in names:
        (root, ext) = os.path.splitext(name)
        if ext=='.in':
            (junk, true_ext) = os.path.splitext(root)
            inpath = os.path.join(dirname, name)
            outpath = os.path.join(dirname, root)
            f = open(inpath, 'r')
            g = open(outpath, 'w')
            for line in f:
                if line.find('<<<<') != -1:
                    filename = re.search('<<<< (.*) >>>>',line).group(1);
                    print("Inserting %s into %s"%(filename,outpath))
                    h = open(filename)
                    for junk in h:
                        if (true_ext=='.html'):
                            junk = latin_filter(junk)
                        g.write(junk)
                    h.close()
                else:
                    g.write(line)
            g.close()
            f.close()
            os.unlink(inpath)

basedir = sys.argv[1]
# Merge in the fragment text
os.path.walk(basedir, visit, '')
# Copy the png files from tmp to the latex and html directories
pngs = glob.glob(basedir+'/tmp/*.png')
for png in pngs:
    fname = os.path.basename(png)
    print("Copying file %s to latex/html directories"%(fname))
    shutil.copyfile(png,basedir+'/html/'+fname)
jpgs = glob.glob(basedir+'/tmp/*.jpg')
for jpg in jpgs:
    fname = os.path.basename(jpg)
    print("Copying file %s to latex directories"%(fname))
    shutil.copyfile(jpg,basedir+'/latex/'+fname)
os.chdir(basedir+'/tmp')
eqns = glob.glob('*.tex')
for eqn in eqns:
    fname = os.path.basename(eqn)
    print("Generating equation images for %s"%(fname))
    os.system("latex %s"%(fname))
    (root,ext) = os.path.splitext(fname)
    os.system("dvipng -T tight %s.dvi"%(root))
pngs = glob.glob('*.png')
for png in pngs:
    fname = os.path.basename(png)
    print("Copying file %s to html directory"%(fname))
    shutil.copyfile(png,'../html/'+fname)
