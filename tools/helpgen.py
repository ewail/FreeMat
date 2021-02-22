#!/usr/bin/python
import os
import sys
import re

version = '4.1'
verstring = 'FreeMat v'+version


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


class ExpressionSet:
    docblock = ''
    modulename = ''
    moduledesc = ''
    sectionname = ''
    groupname = ''
    execin = ''
    execout = ''
    verbatimin = ''
    verbatimout = ''
    ccomment = ''
    figure = ''
    eqnin = ''
    eqnout = ''
    fnin = ''
    fnout = ''
    enumeratein = ''
    enumerateout = ''
    itemizein = ''
    itemizeout = ''
    item = ''
    def __init__(self,prefix):
        self.docblock =     '^' + prefix+'!'
        self.modulename =   prefix+'@Module\s*(\w*)'
        self.moduledesc =   prefix+'@Module\s*(.*)'
        self.sectionname =  prefix+'@@Section\s*(\w*)'
        self.groupname =    prefix+'@@([ \w]*)'
        self.execin =       prefix+'@<(\d+)?'
        self.execout =      prefix+'@>'
        self.verbatimin =   prefix+'@\['
        self.verbatimout =  prefix+'@\]'
        self.ccomment =     prefix+'(.*)'
        self.figure =       prefix+'@figure\s*(\w+)'
        self.eqnin =        prefix+'\\\\\['
        self.eqnout =       prefix+'\\\\\]';
        self.fnin =         prefix+'@{\s*(\S+)'
        self.fnout =        prefix+'@}'
        self.enumeratein =  prefix+'\\begin{enumerate}'
        self.enumerateout = prefix+'\\end{enumerate}'
        self.itemizein =    prefix+'\\\\begin{itemize}'
        self.itemizeout =   prefix+'\\\\end{itemize}'
        self.item =         prefix+'\s*\\\\item(.*)'
    
class Writer:
    """The base class for writers..."""
    def setsectioninfo(self,section_descriptors,section_order,sourcepath):
        return
    def begingroup(self,groupname):
        return
    def beginmodule(self,sourcepath,modname,moddesc,secname):
        return
    def beginverbatim(self):
        return
    def docomputeblock(self,cmds,errorsexpected,filename):
        return
    def doenumerate(self,enums):
        return
    def doequation(self,eqn):
        return
    def dofigure(self,figname):
        return
    def dofile(self,filename,text):
        return
    def doitemize(self,enums):
        return
    def endmodule(self):
        return
    def endverbatim(self):
        return
    def outputtext(self,text):
        return
    def writeindex(self):
        return

class WriterGroup(Writer):
    writers = []
    def addwriter(self,writer):
        self.writers.append(writer)
        return
    """The base class for writers..."""
    def setsectioninfo(self,section_descriptors,section_order,sourcepath):
        for writer in self.writers:
            writer.setsectioninfo(section_descriptors,section_order,sourcepath)
        return
    def begingroup(self,groupname):
        for writer in self.writers:
            writer.begingroup(groupname)
        return
    def beginmodule(self,sourcepath,modname,moddesc,secname):
        for writer in self.writers:
            writer.beginmodule(sourcepath,modname,moddesc,secname)
        return
    def beginverbatim(self):
        for writer in self.writers:
            writer.beginverbatim()
        return
    def docomputeblock(self,cmds,errorsexpected,filename):
        for writer in self.writers:
            writer.docomputeblock(cmds,errorsexpected,filename)
        return
    def doenumerate(self,enums):
        for writer in self.writers:
            writer.doenumerate(enums)
        return
    def doequation(self,eqn):
        for writer in self.writers:
            writer.doequation(eqn)
        return
    def dofigure(self,figname):
        for writer in self.writers:
            writer.dofigure(figname)
        return
    def dofile(self,filename,text):
        for writer in self.writers:
            writer.dofile(filename,text)
        return
    def doitemize(self,enums):
        for writer in self.writers:
            writer.doitemize(enums)
        return
    def endmodule(self):
        for writer in self.writers:
            writer.endmodule()
        return
    def endverbatim(self):
        for writer in self.writers:
            writer.endverbatim()
        return
    def outputtext(self,text):
        for writer in self.writers:
            writer.outputtext(text)
        return
    def writeindex(self):
        for writer in self.writers:
            writer.writeindex()
        return


class DocWriter(Writer):
    myfile = []
    sectables = {}
    eqnlist = []
    verbatim = False
    modulename = ''
    groupname = ''
    ignore = False
    section_descriptors = {}
    section_order = []
    sourcepath = ''
    fp = -1
    def expand_codes(self,text):
        text = re.sub(r'\@\|([^\|]*)\|','<tt>'r'\1''</tt>',text)
        return text
    def setsectioninfo(self,section_descriptors,section_order,sourcepath):
        self.section_descriptors = section_descriptors
        self.section_order = section_order
        return
    def begingroup(self,groupname):
        self.groupname = groupname
        if (groupname.lower() == 'tests'):
            self.ignore = True
        else:
            self.fp.write('\\section %s\n'%(groupname))
            self.ignore = False
        return
    def beginmodule(self,sourcepath,modname,moddesc,secname):
        self.moddesc = moddesc
        self.secname = secname.lower()
        self.modulename = modname.lower()
        self.sourcepath = sourcepath
        self.eqnlist = []
        makepath(sourcepath+'/doc/'+self.secname+'/')
        filename = sourcepath + '/doc/' + self.secname + '/' + self.modulename + '.doc'
        if (not self.secname in self.sectables):
            self.sectables[self.secname] = {}
        self.sectables[self.secname][self.modulename] = self.moddesc;        
        self.fp = open(filename,'w')
        self.fp.write('/*!\n');
        self.fp.write('\\page %s %s\n'%(self.secname+'_'+self.modulename,moddesc))
        self.fp.write('<p>\n')
        self.fp.write('Section: \\ref sec_%s "%s"\n'%(secname.lower(),self.section_descriptors[secname.lower()]))
        self.verbatim = False
        self.ignore = False
        return
    def beginverbatim(self):
        self.fp.write('\\verbatim\n')
        self.verbatim = True
        return
    def docomputeblock(self,cmds,errorsexpected,filename):
        (head,tail) = os.path.split(filename)
        # Read the fragment file in
        fragfile = open(filename,'rU')
        self.fp.write('\n\if FRAGMENT\n')
        self.fp.write(tail + '\n')
        for line in fragfile:
            self.fp.write(line)
        self.fp.write('\endif\n\n')
        fragfile.close()
        self.fp.write('\n\\verbinclude ' + tail + '.out \n\n')
        return
    def doenumerate(self,enums):
        self.fp.write('<OL>\n')
        for item in enums:
            self.fp.write('<LI> %s </LI>\n'%(self.expand_codes(item)))
        self.fp.write('</OL>\n')
        return
    def doequation(self,eqn):
        self.fp.write('\\f[\n%s\\f]\n'%(eqn))
        return
    def dofigure(self,figname):
        self.fp.write('\image html %s.png\n'%(figname))
        self.fp.write('\image latex %s.eps "%s" width=12cm\n'%(figname,figname))
        return
    def dofile(self,filename,text):
        if (self.ignore):
            return
        self.fp.write('\n\\if FILE\n')
        self.fp.write(filename+'\n%s\\endif\n'%(text))
        self.fp.write('\\verbatim\n     %s\n\\endverbatim\n'%(filename))
        self.fp.write('\n\\verbinclude ' + filename + '\n\n')
        return
    def doitemize(self,enums):
        self.fp.write('<UL>\n')
        for item in enums:
            self.fp.write('<LI> %s </LI>\n'%(self.expand_codes(item)))
        self.fp.write('</UL>\n')
        return
        return
    def endmodule(self):
        self.fp.write('*/\n')
        self.fp.close()
        self.fp = -1
        return
    def endverbatim(self):
        self.fp.write('\\endverbatim\n')
        self.verbatim = False
        return
    def outputtext(self,text):
        if (self.ignore):
            return
        if (self.verbatim):
            self.fp.write(text)
        else:
            self.fp.write(self.expand_codes(text))
        return
    def writeindex(self):
        for secname in self.section_descriptors:
            if secname in self.sectables:
                self.writesectiontable(secname,self.sectables[secname])
        f = open('%s/doc/index.doc'%(self.sourcepath),'w')
        f.write('/*!\n')
        f.write('\\mainpage FreeMat Documentation\n')
        f.write('\n')
        f.write('<H2> Documentation Sections </H2>\n')
        f.write('<UL>\n')
        for secname in self.section_order:
            if (len(secname) > 0):
                f.write('<LI> \\subpage sec_%s "%s" </LI>\n'%(secname,self.section_descriptors[secname]))
        f.write('</UL>\n')
        f.write('*/')
        f.close()
        return
    def writesectiontable(self,secname,modules):
        makepath(self.sourcepath+'/doc/sections/')
        f = open('%s/doc/sections/sec_%s.doc'%(self.sourcepath,secname),'w')
        f.write('/*!\n')
        f.write('\\page sec_%s %s\n'%(secname.lower(),self.section_descriptors[secname.lower()]))
        f.write('\n')
        f.write('<P>\n')
        f.write('<UL>\n')
        for module in sorted(modules.iterkeys()):
            f.write('<LI> \\subpage %s_%s "%s" </LI>\n'%(secname,module,self.sectables[secname][module][:-1]))
        f.write('</UL>\n')
        f.write('*/')
        f.close()
        return


def latin_filter(text):
    text = text.replace('&','&amp;')
    text = text.replace('<','&lt;')
    text = text.replace('>','&gt;')
    return text

class HTMLWriter(Writer):
    myfile = []
    sectables = {}
    eqnlist = []
    verbatim = False
    modulename = ''
    groupname = ''
    ignore = False
    section_descriptors = {}
    section_order = []
    sourcepath = ''
    fp = -1
    def expand_codes(self,text):
        text = re.sub(r'\@\|([^\|]*)\|','<code>'r'\1''</code>',text)
        while (re.search('\\\\f\$(.*?)\\\\f\$',text)):
            eqn = re.search('\\\\f\$(.*?)\\\\f\$',text).group(1)
            self.eqnlist.append(eqn)
            subtxt = '<IMG STYLE="vertical-align:middle" SRC="%s_eqn%d.png"/>'%(self.modulename,len(self.eqnlist))
            text = re.sub('\\\\f\$(.*?)\\\\f\$',text,subtxt,1)
        if (text == '\n'):
            text = '<P>\n'
        return text
    def setsectioninfo(self,section_descriptors,section_order,sourcepath):
        self.section_descriptors = section_descriptors
        self.section_order = section_order
        return
    def begingroup(self,groupname):
        self.groupname = groupname
        if (groupname.lower() == 'tests'):
            self.ignore = True
        else:
            self.fp.write('<H3>%s</H3>\n'%(groupname))
            self.ignore = False
        return
    def beginmodule(self,sourcepath,modname,moddesc,secname):
        self.moddesc = moddesc
        self.secname = secname.lower()
        self.modulename = modname.lower()
        self.sourcepath = sourcepath
        self.eqnlist = []
        filename = sourcepath + '/help/html/' + self.secname + '_' + self.modulename + '.html.in'
        self.fp = open(filename,'w')
        self.fp.write('<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">\n')
        self.fp.write('\n')
        self.fp.write('<HTML>\n')
        self.fp.write('<HEAD>\n')
        self.fp.write('<TITLE>%s</TITLE>\n'%(moddesc))
        self.fp.write('</HEAD>\n')
        self.fp.write('<BODY>\n')
        self.fp.write('<H2>%s</H2>\n'%(moddesc))
        if (not self.secname in self.sectables):
            self.sectables[self.secname] = {}
        self.sectables[self.secname][self.modulename] = self.moddesc;
        self.fp.write('<P>\n')
        self.fp.write('Section: <A HREF=sec_%s.html> %s </A>\n'%(secname.lower(),self.section_descriptors[secname.lower()]))
        self.verbatim = False
        self.ignore = False
        return
    def beginverbatim(self):
        self.fp.write('<PRE>\n')
        self.verbatim = True
        return
    def docomputeblock(self,cmds,errorsexpected,filename):
        self.fp.write('<PRE>\n')
        self.fp.write('<<<< ' + filename + '.out >>>>\n')
        self.fp.write('</PRE>\n')
        return
    def doenumerate(self,enums):
        self.fp.write('<OL>\n')
        for item in enums:
            self.fp.write('<LI> %s </LI>\n'%(self.expand_codes(latin_filter(item))))
        self.fp.write('</OL>\n')
        return
    def doequation(self,eqn):
        self.eqnlist.append(eqn)
        self.fp.write('<P>\n')
        self.fp.write('<DIV ALIGN="CENTER">\n')
        self.fp.write('<IMG SRC="%s_eqn%d.png">\n'%(self.modulename,len(self.eqnlist)))
        self.fp.write('</DIV>\n')
        self.fp.write('<P>\n')
        return
    def dofigure(self,figname):
        self.fp.write('<P>\n');
        self.fp.write('<DIV ALIGN="CENTER">\n');
        self.fp.write('<IMG SRC="%s.png">\n'%(figname))
        self.fp.write('</DIV>\n');
        self.fp.write('<P>\n');
        return
    def dofile(self,filename,text):
        if (self.ignore):
            return
        self.fp.write('<P>\n<PRE>\n');
        self.fp.write('     %s\n'%(filename))
        self.fp.write('%s\n'%(latin_filter(text)))
        self.fp.write('</PRE>\n')
        self.fp.write('<P>\n')
        return
    def doitemize(self,enums):
        self.fp.write('<UL>\n')
        for item in enums:
            self.fp.write('<LI> %s </LI>\n'%(self.expand_codes(latin_filter(item))))
        self.fp.write('</UL>\n')
        return
        return
    def endmodule(self):
        self.fp.write('</BODY>\n')
        self.fp.write('</HTML>\n')
        self.fp.close()
        self.fp = -1
        if (len(self.eqnlist) > 0):
            f = open(self.sourcepath + '/help/tmp/%s_eqn.tex'%(self.modulename),'w')
            f.write('\\documentclass{article}\n')
            f.write('\\usepackage{amsmath}\n');
            f.write('\\pagestyle{empty}\n');
            f.write('\\begin{document}\n');
            for eqn in self.eqnlist:
                f.write('\\[\n');
                f.write(eqn.rstrip() + '\n');
                f.write('\\]\n');
                f.write('\\pagebreak\n');
            f.write('\\end{document}\n');
            f.close()
        return
    def endverbatim(self):
        self.fp.write('</PRE>\n<P>\n')
        self.verbatim = False
        return
    def outputtext(self,text):
        if (self.ignore):
            return
        text = latin_filter(text)
        if (self.verbatim):
            self.fp.write(text)
        else:
            self.fp.write(self.expand_codes(text))
        return
    def writeindex(self):
        for secname in self.section_descriptors:
            if secname in self.sectables:
                self.writesectiontable(secname,self.sectables[secname])
        f = open(self.sourcepath + '/help/html/index.html','w')
        f.write('<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">\n')
        f.write('\n')
        f.write('<HTML>\n')
        f.write('<HEAD>\n')
        f.write('<TITLE>%s Documentation</TITLE>\n'%(verstring))
        f.write('</HEAD>\n')
        f.write('<BODY>\n')
        f.write('<H1>%s Documentation</H1>\n'%(verstring))
        f.write('<P>\n')
        f.write('<H2> Documentation Sections </H2>\n')
        f.write('<UL>\n')
        for secname in self.section_order:
            f.write('<LI> <A HREF=sec_%s.html> %s </A> </LI>\n'%(secname,self.section_descriptors[secname]))
        f.write('</UL>\n')
        f.write('</BODY>\n')
        f.write('</HTML>\n')
        f.close()
        f = open(self.sourcepath + '/help/html/modules.txt','w')
        modulenames = []
        for section in self.section_order:
            if (section in self.sectables):
                modules = sorted(self.sectables[section].iterkeys())
                for module in modules:
                    modulenames.append(module + ' (' + section + ')')
        for mod in sorted(modulenames):
            f.write(mod + '\n')
        f.close()
        f = open(self.sourcepath + '/help/html/sectable.txt','w')
        for section in self.section_order:
            f.write(self.section_descriptors[section] + '\n')
            if (section in self.sectables):
                for module in self.sectables[section]:
                    f.write('+(%s) %s\n'%(section,module))
        f.close()
        return
    def writesectiontable(self,secname,modules):
        f = open('%s/help/html/sec_%s.html'%(self.sourcepath,secname),'w')
        f.write('<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">\n')
        f.write('\n')
        f.write('<HTML>\n')
        f.write('<HEAD>\n')
        f.write('<TITLE>%s</TITLE>\n'%(self.section_descriptors[secname]))
        f.write('</HEAD>\n')
        f.write('<BODY>\n')
        f.write('<H2>%s</H2>\n'%(self.section_descriptors[secname]))
        f.write('<P>\n')
        f.write('<A HREF=index.html> Main Index </A>\n')
        f.write('<P>\n')
        f.write('<UL>\n')
        for module in sorted(modules.iterkeys()):
            f.write('<LI> <A HREF=%s_%s.html> %s </A> %s </LI>\n'%(secname,module,module,self.sectables[secname][module]))
        f.write('</UL>\n')
        f.write('</BODY>\n')
        f.write('</HTML>\n')
        f.close()
        return

def refilter(txt):
    ret = ''
    ineqn = False
    protect = ['^','_','&','#']
    prev = ''
    while (txt):
        c = txt[0]
        txt = txt[1:]
        if (c == '$' or (c == '[' and prev == '\\') or (c == ']' and prev == '\\')):
            ineqn = not ineqn
        if ((c in protect) and not ineqn and prev != '\\'):
            ret += '\\'
        ret += c
        prev = c
    return ret

class LaTeXWriter(Writer):
    myfile = []
    sectables = {}
    eqnlist = []
    verbatim = False
    modulename = ''
    groupname = ''
    ignore = False
    section_descriptors = {}
    section_order = []
    sourcepath = ''
    fp = -1
    def expand_codes(self,text):
        text = re.sub(r'\@\|([^\|]*)\|','\\\\verb|'r'\1''|',text)
        text = re.sub('\\\\f\$(.*?)\\\\f\$','$'r'\1''$',text)
        text = re.sub('\\\\f\[','\\\\[',text)
        text = re.sub('\\\\f\]','\\\\]',text)
        text = re.sub('"',"''",text)
        text = re.sub('<pre>','\\\\begin{verbatim}',text)
        text = re.sub('\\\\verbatim','\\\\begin{verbatim}',text)
        text = re.sub('\\\\endverbatim','\\\\end{verbatim}',text)
        text = re.sub('</pre>','\\\\end{verbatim}',text)
        text = re.sub('\\\\dot','\\\\begin{verbatim}',text)
        text = re.sub('\\\\enddot','\\\\end{verbatim}',text)
        text = re.sub('\\\\pre (.*)','',text)
        text = re.sub('\\\\post (.*)','',text)
        text = re.sub('\\\\image (.*)','',text)
        text = re.sub('\\\\p','',text)
        text = re.sub('\\\\a','',text)
        text = re.sub('\\\\n','\\\\\\\\n',text)
        text = re.sub('\$renWin','\\\\$renWin',text)
        text = re.sub('\\\\<','',text)
        text = re.sub('\\\\>','',text)
        text = re.sub('\\\\li','',text)
        text = refilter(text)
        return text
    def setsectioninfo(self,section_descriptors,section_order,sourcepath):
        self.section_descriptors = section_descriptors
        self.section_order = section_order
        return
    def begingroup(self,groupname):
        self.groupname = groupname
        if (groupname.lower() == 'tests'):
            self.ignore = True
        else:
            self.fp.write('\\subsection{%s}\n\n'%(groupname))
            self.ignore = False
        return
    def beginmodule(self,sourcepath,modname,moddesc,secname):
        self.moddesc = moddesc
        self.secname = secname.lower()
        self.modulename = modname.lower()
        self.sourcepath = sourcepath
        self.eqnlist = []
        filename = sourcepath + '/help/latex/' + self.secname + '_' + self.modulename + '.tex.in'
        self.fp = open(filename,'w')
        self.fp.write('\\section{%s}\n\n'%(moddesc.rstrip()))
        if (not self.secname in self.sectables):
            self.sectables[self.secname] = {}
        self.sectables[self.secname][self.modulename] = self.moddesc;
        self.verbatim = False
        self.ignore = False
        return
    def beginverbatim(self):
        self.fp.write('\\begin{verbatim}\n')
        self.verbatim = True
        return
    def docomputeblock(self,cmds,errorsexpected,filename):
        self.fp.write('\\begin{verbatim}\n')
        self.fp.write('<<<< ' + filename + '.out >>>>\n')
        self.fp.write('\\end{verbatim}\n')
        return
    def doenumerate(self,enums):
        self.fp.write('\\begin{enumerate}\n')
        for item in enums:
            self.fp.write('\\item %s\n'%(self.expand_codes(item)))
        self.fp.write('\\end{enumerate}\n')
        return
    def doequation(self,eqn):
        self.fp.write('\\[\n%s\\]\n'%(eqn))
        return
    def dofigure(self,figname):
        self.fp.write('\n\n');
        self.fp.write('\\centerline{\\includegraphics[width=8cm]{%s}}\n\n'%(figname))
        return
    def dofile(self,filename,text):
        if (self.ignore):
            return
        self.fp.write('\\begin{verbatim}\n')
        self.fp.write('    %s\n'%(filename))
        self.fp.write('%s'%(text))
        self.fp.write('\\end{verbatim}\n')
        return
    def doitemize(self,enums):
        self.fp.write('\\begin{itemize}\n')
        for item in enums:
            self.fp.write('\\item %s\n'%(self.expand_codes(item)))
        self.fp.write('\\end{itemize}\n')
        return
    def endmodule(self):
        self.fp.close()
        self.fp = -1
        return
    def endverbatim(self):
        self.fp.write('\\end{verbatim}\n')
        self.verbatim = False
        return
    def outputtext(self,text):
        if (self.ignore):
            return
        if (self.verbatim):
            self.fp.write(text)
        else:
            self.fp.write(self.expand_codes(text))
        return
    def writeindex(self):
        f = open(self.sourcepath + '/help/latex/FreeMat-'+version+'.tex','w')
        f.write('\\documentclass{book}\n')
        f.write('\\usepackage{graphicx}\n')
        f.write('\\usepackage{amsmath}\n')
        f.write('\\usepackage[letterpaper]{geometry}\n')
        f.write('\\geometry{top=1in,bottom=1in,left=1in,right=1in}\n')
        f.write('\\usepackage{tocloft}\n')
        f.write('\\setlength{\\cftpartnumwidth}{4em}')
        f.write('\\setlength{\\cftsecnumwidth}{4em}')
        f.write('\\setlength{\\cftsubsecnumwidth}{4em}')
        f.write('\\title{%s Documentation}\n'%(verstring))
        f.write('\\author{Samit Basu}\n')
        f.write('\\begin{document}\n')
        f.write('\\maketitle\n')
        f.write('\\setcounter{tocdepth}{1}\n')
        f.write('\\tableofcontents\n')
        for section in self.section_order:
            if section in self.sectables:
                f.write('\\chapter{%s}\n'%(self.section_descriptors[section]))
                for mod in sorted(self.sectables[section].iterkeys()):
                    f.write('\\input{%s_%s}\n'%(section,mod.lower()))
        f.write('\\end{document}\n')
        f.close()
        return

class BBTestWriter(Writer):
    moddesc = ''
    empty = True
    modulename = ''
    blacklist = {'retall','keyboard','return','where'}
    secname = ''
    section_descriptors = {}
    section_order = []
    sourcepath = ''
    filename = ''
    fp = []
    def setsectioninfo(self,section_descriptors,section_order,sourcepath):
        self.section_descriptors = section_descriptors
        self.section_order = section_order
        return
    def docomputeblock(self,cmds,errorsexpected,filename):
        if (self.modulename not in self.blacklist):
            self.fp.write('NumErrors = 0;\n')
            for cmd in cmds:
                self.fp.write('try\n')
                self.fp.write('  ' + cmd)
                self.fp.write('catch\n')
                self.fp.write('  NumErrors = NumErrors + 1;\n')
                self.fp.write('end\n')
            self.fp.write('if (NumErrors ~= %d) bbtest_success = 0; return; end\n'%(errorsexpected))
        self.empty = False
    def beginmodule(self,sourcepath,modname,moddesc,secname):
        self.moddesc = moddesc
        self.secname = secname.lower()
        self.modulename = modname.lower()
        self.sourcepath = sourcepath
        self.filename = sourcepath + '/toolbox/test/bbtest_' + self.secname + '_' + self.modulename + '.m'
        self.fp = open(self.filename,'w+')
        self.fp.write('%% Regression test function (black box) for %s\n'%(self.modulename))
        self.fp.write('% This function is autogenerated by helpgen.py\n')
        self.fp.write('function bbtest_success = bbtest_%s_%s\n'%(self.secname,self.modulename))
        self.fp.write('  bbtest_success = 1;\n')
        self.empty = True
        return
    def endmodule(self):
        self.fp.close()
        if (self.empty):
            os.unlink(self.filename)
            
class TestWriter(Writer):
    moddesc = ''
    empty = True
    modulename = ''
    blacklist = {'retall','keyboard','return','where'}
    secname = ''
    section_descriptors = {}
    section_order = []
    sourcepath = ''
    filename = ''
    fp = []
    num = 1
    def setsectioninfo(self,section_descriptors,section_order,sourcepath):
        self.section_descriptors = section_descriptors
        self.section_order = section_order
        for section in section_descriptors:
            cml = sourcepath + "/tests/" + section + "/CMakeLists.txt"
            makepath(cml)
            sp = open(cml,'w')
            sp.write('ENABLE_TESTING()\n');
            sp.close()
        sp = open(sourcepath + "/tests/CMakeLists.txt",'w')
        sp.write('ENABLE_TESTING()\n')
        for section in section_descriptors:
            if (section):  sp.write('add_subdirectory(' + section + ')\n')
        sp.close
        return
    def begingroup(self,groupname):
        self.groupname = groupname
        if (groupname.lower() == 'tests'):
            self.ignore = False
        else:
            self.ignore = True
        return
    def outputtext(self,text):
        if (self.ignore):
            return
        text = text[:-1]
        v = re.search('\@\$([^#]*)#([^#]*)#?(.*)',text)
        if (not v):
            exit('Error: bad line ' + text + ' in module ' + self.modulename)
        ttype = v.group(1)
        expr = v.group(2)
        exclude = v.group(3)
        if (not exclude):
            exclude = 'false'
        sexpr = re.sub(r"'","''",expr)
        if (not ttype in ["near","exact","near_permute"]):
            exit('Error: bad test type in line ' + text)
        in_count = 0
        ptext = ' ' + text + ' '
        for i in range(1,9):
            if (re.search("\Wx"+str(i)+"\W",ptext)):
                in_count = in_count + 1
        out_count = 0
        for i in range(1,9):
            if (re.search("\Wy"+str(i)+"\W",ptext)):
                out_count = out_count + 1
        funcname = 'wbtest_%s_%d'%(self.modulename,self.num)
        makepath('%s/toolbox/test/reference'%(self.sourcepath))
        # Write the test file
        clistname = self.sourcepath + '/tests/' + self.secname + '/CMakeLists.txt'
        makepath(clistname)
        zp = open(clistname,'a+')
        ptext = text.replace(r"'",r"''")
        ptext = ptext.replace("\\","\\\\\\\\")
        zp.write('ADD_TEST(%s ${FreeMat_Loc} "-e" "-nogui" "-t" "-nogreet" "-p" "${CMAKE_SOURCE_DIR}/tests/%s" "-f" "wb_test(\'%s\',\'%s\',\'${CMAKE_SOURCE_DIR}/tests/reference\')")\n'%(funcname,self.secname,ptext,funcname))
        zp.close()
        filename = '%s/toolbox/test/%s.m'%(self.sourcepath,funcname)
        fp = open(filename,'w')
        if (not fp):
            exit('unable to open %s for output'%(filename))
        fp.write('function test_val = %s(verbose)\n'%(funcname))
        fp.write("  myloc=fileparts(which('%s'));\n"%(funcname))
        fp.write("  load([myloc,dirsep,'reference',dirsep,'wbinputs.mat'])\n")
        fp.write("  load([myloc,dirsep,'reference',dirsep,'%s_ref.mat'])\n"%(funcname))
        fp.write("  fail_count = 0;\n")
        if (in_count == 0):
            fp.write('  error_flag = 0;\n');
            for k in range(1,out_count+1):
                fp.write('  y%d = [];\n'%(k))
            fp.write('  try\n')
            fp.write('    %s;\n'%(expr))
            fp.write('  catch\n')
            fp.write('    error_flag = 1;\n')
            fp.write('  end\n') 
            fp.write('  if (error_flag && ~error_refs)\n')
            fp.write("     printf('Mismatch Errors: %s\\n');\n"%(sexpr))
            fp.write('     fail_count = fail_count + 1;\n')
            for k in range(1,out_count+1):
                fp.write('  elseif (~error_flag && ~error_refs && ~wbtest_%s(y%d,y%d_refs{1}))\n'%(ttype,k,k))
                fp.write("    printf('Mismatch (%s): output %d %s\\n');\n"%(ttype,k,sexpr))
                fp.write('    fail_count = fail_count + 1;\n')
            fp.write('  end\n')
        elif (in_count == 1):
            fp.write('  for loopi=1:numel(wbinputs)\n')
            fp.write('    x1 = wbinputs{loopi};\n')
            fp.write('    error_flag = 0;\n')
            for k in range(1,out_count+1):
                fp.write('  y%d = [];\n'%(k))
            fp.write('    try\n')
            fp.write('      %s;\n'%(expr))
            fp.write('    catch\n')
            fp.write('      error_flag = 1;\n')
            fp.write('    end\n')
            fp.write('    if (error_flag && ~error_refs(loopi) && ~(%s))\n'%(exclude))
            fp.write("       printf('Mismatch Errors: input %%d %s\\n',loopi);\n"%(sexpr))
            fp.write('       fail_count = fail_count + 1;\n')
            for k in range(1,out_count+1):
                fp.write('    elseif (~error_flag && ~error_refs(loopi) && ~wbtest_%s(y%d,y%d_refs{loopi}) && ~(%s))\n'%(ttype,k,k,exclude))
                fp.write("      printf('Mismatch (%s): input %%d output %d %s\\n',loopi);\n"%(ttype,k,sexpr))
                fp.write('      fail_count = fail_count + 1;\n')
            fp.write('    end\n')
        elif (in_count == 2):
            fp.write('  for loopi=1:numel(wbinputs)\n')
            fp.write('    for loopj=1:numel(wbinputs)\n')
            fp.write('      x1 = wbinputs{loopi};\n')
            fp.write('      x2 = wbinputs{loopj};\n')
            fp.write('      error_flag = 0;\n')
            for k in range(1,out_count+1):
                fp.write('     y%d = [];\n'%(k))
            fp.write('      try\n')
            fp.write('        %s;\n'%(expr))
            fp.write('      catch\n')
            fp.write('        error_flag = 1;\n')
            fp.write('      end\n')
            fp.write('    if (error_flag && ~error_refs(loopi,loopj) && ~(%s))\n'%(exclude))
            fp.write("       printf('Mismatch Errors: input %%d, %%d %s\\n',loopi,loopj);\n"%(sexpr))
            fp.write('        fail_count = fail_count + 1;\n')
            for k in range(1,out_count+1):
                fp.write('  elseif (~error_flag && ~error_refs(loopi,loopj) && ~wbtest_%s(y%d,y%d_refs{loopi,loopj}) && ~(%s))\n'%(ttype,k,k,exclude))
                fp.write("    printf('Mismatch (%s): input %%d,%%d output %d %s\\n',loopi,loopj);\n"%(ttype,k,sexpr))
                fp.write('    fail_count = fail_count + 1;\n')
            fp.write('    end\n')
            fp.write('  end\n')
        fp.write('  test_val = (fail_count == 0);\n')
        fp.write('end\n')
        fp.close()
        # Write the reference file
        filename = '%s/toolbox/test/gen_%s.m'%(self.sourcepath,funcname)
        fp = open(filename,'w')
        if (not fp):
            exit('unable to open %s for output'%(filename))
        fp.write('function gen_%s(verbose)\n'%(funcname))
        fp.write("  myloc=fileparts(which('%s'));\n"%(funcname))
        fp.write("  load([myloc,dirsep,'reference',dirsep,'wbinputs.mat'])\n");
        if (in_count == 0):
            fp.write('  error_refs = 0;\n')
            for k in range(1,out_count+1):
                fp.write('  y%d = []; y%d_refs = {};\n'%(k,k))
            fp.write('  try\n')
            fp.write('    %s;\n'%(expr))
            fp.write('  catch\n')
            fp.write('    error_refs = 1;\n')
            fp.write('  end\n')
            fp.write('  if (~error_refs)\n')
            for k in range(1,out_count+1):
                fp.write('  y%d_refs = {y%d};\n'%(k,k))
            fp.write('  end\n')
        elif (in_count == 1):
            fp.write('  n_ = numel(wbinputs);\n')
            fp.write('  error_refs = zeros(n_,1);\n')
            for k in range(1,out_count+1):
                fp.write('  y%d_refs = cell(n_,1);\n'%(k))
            fp.write('  for loopi=1:n_\n')
            fp.write('    x1 = wbinputs{loopi};\n')
            fp.write('    error_refs(loopi) = 0;\n')
            for k in range(1,out_count+1):
                fp.write('    y%d = [];\n'%(k))
            fp.write('    try\n')
            fp.write('      %s;\n'%(expr))
            fp.write('    catch\n')
            fp.write('      error_refs(loopi) = 1;\n')
            fp.write('    end\n')
            fp.write('    if (~error_refs(loopi))\n')
            for k in range(1,out_count+1):
                fp.write('     y%d_refs(loopi) = {y%d};\n'%(k,k))
            fp.write('    end\n')
            fp.write('  end\n')
        elif (in_count == 2):
            fp.write('  n_ = numel(wbinputs);\n')
            fp.write('  error_refs = zeros(n_,n_);\n')
            for k in range(1,out_count+1):
                fp.write('  y%d_refs = cell(n_,n_);\n'%(k))
            fp.write('  for loopi=1:n_\n')
            fp.write('    for loopj=1:n_\n')
            fp.write('      x1 = wbinputs{loopi};\n')
            fp.write('      x2 = wbinputs{loopj};\n')
            for k in range(1,out_count+1):
                fp.write('      y%d = [];\n'%(k))
            fp.write('      try\n')
            fp.write('        %s;\n'%(expr))
            fp.write('      catch\n')
            fp.write('        error_refs(loopi,loopj) = 1;\n')
            fp.write('      end\n')
            fp.write('      if (~error_refs(loopi,loopj))\n')
            for k in range(1,out_count+1):
                fp.write('       y%d_refs(loopi,loopj) = {y%d};\n'%(k,k))
            fp.write('      end\n')
            fp.write('    end\n')
            fp.write('  end\n')
        fp.write('  save reference/%s_ref.mat error_refs'%(funcname))
        for k in range(1,out_count+1):
            fp.write('  y%d_refs '%(k))
        fp.write('\nend\n')
        fp.close()
        self.num = self.num + 1;
        return
    def beginmodule(self,sourcepath,modname,moddesc,secname):
        self.moddesc = moddesc
        self.secname = secname.lower()
        self.modulename = modname.lower()
        self.sourcepath = sourcepath
        self.num = 1
        return


class TextWriter(Writer):
    myfile = []
    sectables = {}
    eqnlist = []
    verbatim = False
    modulename = ''
    groupname = ''
    ignore = False
    section_descriptors = {}
    section_order = []
    sourcepath = ''
    def expand_codes(self,text):
        return re.sub(r'\@\|([^\|]*)\|',r'\1',text)
    def setsectioninfo(self,section_descriptors,section_order,sourcepath):
        self.section_descriptors = section_descriptors
        self.section_order = section_order
        return
    def begingroup(self,groupname):
        self.groupname = groupname
        if ((groupname.lower() != 'usage') and (groupname.lower() != 'methods')):
            self.ignore = True
        else:
            self.myfile.write('Usage\n\n')
            self.ignore = False
        return
    def beginmodule(self,sourcepath,modname,moddesc,secname):
        self.moddesc = moddesc
        self.secname = secname.lower()
        self.modulename = modname.lower()
        self.sourcepath = sourcepath
        filename = sourcepath + '/help/text/' + self.modulename + '.mdc'
        self.myfile = open(filename,'w+')
        self.myfile.write('%s\n'%(moddesc))
        self.verbatim = False
        self.ignore = True
        if (not self.secname in self.sectables):
            self.sectables[self.secname] = {}
        self.sectables[self.secname][self.modulename] = self.moddesc
        return
    def beginverbatim(self):
        if (self.ignore):
            return
        self.verbatim = True
        self.myfile.write('\n')
        return
    def docomputeblock(self,cmds,errorsexpected,filename):
#        self.myfile.write('<<<< ' + filename + ' >>>>\n')
        return
    def doenumerate(self,enums):
        if (self.ignore):
            return
        for i,item in enumerate(enums):
            self.myfile.write('  %d. %s\n'%(i,self.expand_codes(item)))
        return
    def doitemize(self,enums):
        if (self.ignore):
            return
        for item in enums:
            self.myfile.write('  - %s\n'%(self.expand_codes(item)))
        return
    def endmodule(self):
        self.myfile.close()
        return
    def endverbatim(self):
        if (self.ignore):
            return
        self.myfile.write('\n')
        self.verbatim = False
        return
    def outputtext(self,text):
        if (self.ignore):
            return
        self.myfile.write('%s'%(self.expand_codes(text)))
        return

class HelpGen:
    pline = ''
    fp = []
    pset = []
    writers = []
    section_descriptors = {}
    section_order = []
    sourcepath = ''
    finished = False 
    genfiles = []
    fragment_counter = 0
    modname = ''
    secname = ''
    def __init__(self,sourcepath):
        self.writers = WriterGroup()
        self.writers.addwriter(TextWriter())
        self.writers.addwriter(BBTestWriter())
        self.writers.addwriter(HTMLWriter())
        self.writers.addwriter(DocWriter())
        self.writers.addwriter(LaTeXWriter())
        self.writers.addwriter(TestWriter())
        self.sourcepath = sourcepath
        fp = open(sourcepath + '/src/toolbox/help/section_descriptors.txt','r')
        for line in fp:
            m = re.search(r'(\w*)\s*([^\n]*)',line)
            self.section_descriptors[m.group(1)] = m.group(2)
            self.section_order.append(m.group(1))
        fp.close()
        self.writers.setsectioninfo(self.section_descriptors,self.section_order,sourcepath)
        return
    def testmatch(self,rtext):
        return re.search(rtext,self.pline,re.DOTALL)
    def mustmatch(self,rtext):
        match = re.search(rtext,self.pline,re.DOTALL)
        if (match):
            return match.group(1)
        else:
            exit('Error: match failed to find ' + rtext + ' expression in ' + self.pline)
        return
    def getline(self):
        return self.fp.readline()
    def nextline(self):
        self.pline = self.getline()
        return
    def handle_output(self):
        self.pline = self.mustmatch(self.pset.ccomment)
        self.writers.outputtext(self.pline)
        self.nextline()
        return
    def handle_verbatim(self):
        self.nextline()
        self.writers.beginverbatim()
        while not self.testmatch(self.pset.verbatimout):
            self.writers.outputtext(self.mustmatch(self.pset.ccomment))
            self.nextline()
        self.writers.endverbatim()
        self.nextline()
        return
    def handle_figure(self):
        self.writers.dofigure(self.mustmatch(self.pset.figure))
        self.nextline()
    def handle_equation(self):
        self.nextline()
        eqn = ''
        while not self.testmatch(self.pset.eqnout):
            eqn += self.mustmatch(self.pset.ccomment)
            self.nextline()
        self.writers.doequation(eqn)
        self.nextline()
        return
    def handle_filedump(self):
        fname = self.mustmatch(self.pset.fnin)
        self.nextline()
        fn = ''
        while not self.testmatch(self.pset.fnout):
            fn += self.mustmatch(self.pset.ccomment)
            self.nextline()
        self.genfiles.append(fname)
        zp = open(self.sourcepath + '/help/tmp/' + fname,'w+')
        zp.write(fn)
        zp.close()
        zp = open(self.sourcepath + '/toolbox/test/' + fname,'w+')
        zp.write(fn)
        zp.close()
        pname = self.sourcepath + '/tests/' + self.secname + '/' + fname
        makepath(pname)
        zp = open(pname,'w')
        zp.write(fn)
        zp.close()
        if (re.search('^test_',fname,re.DOTALL)):
            clistname = self.sourcepath + '/tests/' + self.secname + '/CMakeLists.txt'
            zp = open(clistname,'a')
            zp.write('ADD_TEST(%s ${FreeMat_Loc} "-e" "-nogui" "-t" "-nogreet" "-p" "${CMAKE_SOURCE_DIR}/tests/%s" "-f" "wrap_test(\'%s\')")\n'%(fname[:-2],self.secname,fname[:-2]))
            zp.close()
        if (not re.search('test_\w+',fname)):
            self.writers.dofile(fname,fn)
        self.nextline()
    def handle_itemize(self):
        self.nextline()
        itemlist = []
        while not self.testmatch(self.pset.itemizeout):
            item = self.mustmatch(self.pset.item)
            self.nextline()
            while not self.testmatch(self.pset.item) and not self.testmatch(self.pset.itemizeout):
                item  += self.mustmatch(self.pset.ccomment)
                self.nextline()
            itemlist.append(item)
        ###if (feof(fp)), error('unmatched enumeration block') end
        self.nextline()
        self.writers.doitemize(itemlist)
        return
    def handle_exec(self):
        match = self.mustmatch(self.pset.execin)
        if (isinstance(match,str)):
            errors_expected = int(self.mustmatch(self.pset.execin))
        else:
            errors_expected = 0
        self.nextline()
        cmdlist = []
        while not self.testmatch(self.pset.execout):
            cmdlist.append(self.mustmatch(self.pset.ccomment))
            self.nextline()
        fragnum = '{:03d}'.format(self.fragment_counter)
        fragname = 'frag_%s_%s_%s'%(self.secname,self.modname,fragnum)
        fragfile = self.sourcepath + '/help/tmp/' + fragname + '.m'
        fp = open(fragfile,'w')
        fp.write(str(errors_expected) + '\n')
        for cmd in cmdlist:
            fp.write(cmd)
        fp.close()
        self.fragment_counter = self.fragment_counter + 1
        self.writers.docomputeblock(cmdlist,errors_expected,fragfile)
        self.nextline()
        return
    def writeindex(self):
        self.writers.writeindex()
        return
    def process_dir(self,dirname):
        if (re.search('\.svn',dirname)):
            return
        if (re.search('\+octave',dirname)):
            return
        print('Processing dir %s'%(dirname))
        names = os.listdir(dirname)
        blacklist = {'dependencies','private'}
        for name in names:
            fname = os.path.join(dirname,name)
            if os.path.isdir(fname) and not name in blacklist:
                self.process_dir(fname)
            else:
                (root, ext) = os.path.splitext(fname)
                if ext == '.cpp':
                    self.pset = ExpressionSet('\s*//')
                    self.process_file(fname)
                elif ext == '.m':
                    self.pset = ExpressionSet('\s*%')
                    self.process_file(fname)
    def process_file(self,filename):
        print('Processing file %s'%(filename))
        if (re.search('(\+octave)',filename)):
            print('skipped');
            return
        self.fp = open(filename,'rU')
        self.finished = False
        while not self.finished:
            self.pline = self.fp.readline()
            self.finished = self.pline == ''
            if (self.testmatch(self.pset.docblock)):
                self.nextline()
                self.modname = self.mustmatch(self.pset.modulename).lower()
                print('    Module %s...'%(self.modname.lower()))
                self.fragment_counter = 0
                moddesc = self.mustmatch(self.pset.moduledesc)
                self.nextline()
                self.secname = self.mustmatch(self.pset.sectionname).lower()
                self.writers.beginmodule(self.sourcepath,self.modname,moddesc,self.secname)
                self.nextline()
                while not self.testmatch(self.pset.docblock):
                    groupname = self.mustmatch(self.pset.groupname)
                    if (re.search('Signature',groupname)):
                        self.nextline()
                        while (not self.testmatch(self.pset.groupname) and not self.testmatch(self.pset.docblock)):
                            self.nextline()
                    else:
                        self.writers.begingroup(groupname)
                        self.nextline()
                        while (not self.testmatch(self.pset.groupname) and not self.testmatch(self.pset.docblock)):
                            if (self.testmatch(self.pset.execin)):
                                self.handle_exec()
                            elif (self.testmatch(self.pset.verbatimin)):
                                self.handle_verbatim()
                            elif (self.testmatch(self.pset.figure)):
                                self.handle_figure()
                            elif (self.testmatch(self.pset.eqnin)):
                                self.handle_equation()
                            elif (self.testmatch(self.pset.fnin)):
                                self.handle_filedump()
                            elif (self.testmatch(self.pset.enumeratein)):
                                self.handle_enumerate()
                            elif (self.testmatch(self.pset.itemizein)):
                                self.handle_itemize()
                            elif (self.testmatch(self.pset.ccomment)):
                                self.handle_output()
                            else:
                                exit('Unprocessed line:' + self.pline)
                self.writers.endmodule()
        self.fp.close()    

makepath('../help/tmp/')
dirname = sys.argv[1]
h = HelpGen(dirname)
h.process_dir(dirname)
h.writeindex()
