#!/usr/bin/python
import os
import sys
import re

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
        self.docblock =     prefix+'!'
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
        self.eqnin =        prefix+'\['
        self.eqnout =       prefix+'\]';
        self.fnin =         prefix+'@{\s*(\S+)'
        self.fnout =        prefix+'@}'
        self.enumeratein =  prefix+'\\begin{enumerate}'
        self.enumerateout = prefix+'\\end{enumerate}'
        self.itemizein =    prefix+'\\begin{itemize}'
        self.itemizeout =   prefix+'\\end{itemize}'
        self.item =         prefix+'\s*\\item(.*)'
    

class Writer:
    """The base class for writers..."""
    def begingroup(self,groupname):
        return
    def beginmodule(self,sourcepath,modname,moddesc,secname,section_descriptors):
        return
    def beginverbatim(self):
        return
    def docomputeblock(self,filename):
        return
    def doenumerate(self,enums):
        return
    def doequation(self,eqn):
        return
    def dofunction(self,figname):
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

class TextWriter:
    myfile = []
    sectables = {}
    eqnlist = []
    verbatim = False
    modulename = ''
    groupname = ''
    ignore = False
    section_descriptors = {}
    sourcepath = ''
    def expand_codes(self,text):
        return re.sub(r'\@\|([^\|]*)\|',r'\1',text)
    def begingroup(self,groupname):
        self.groupname = groupname
        if (groupname.lower() != 'usage'):
            self.ignore = True
        else:
            self.myfile.write('Usage\n\n')
            self.ignore = False
        return
    def beginmodule(self,sourcepath,modname,moddesc,secname,section_descriptors):
        self.moddesc = moddesc
        self.secname = secname.lower()
        self.modulename = modname.lower()
        self.section_descriptors = section_descriptors
        self.sourcepath = sourcepath;
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
    def docomputeblock(self,filename):
        ## self.myfile.write('<<<< ' + filename + ' >>>>')
        return
    def doenumerate(self,enums):
        if (self.ignore):
            return
        for i,item in enumerate(enums):
            self.myfile.write('  %d. %s\n'%(i,self.expand_codes(item)))
        return
    def doequation(self,eqn):
        return
    def dofigure(self,figname):
        return
    def dofile(self,filename,text):
        return
    def doitemize(self,enums):
        if (self.ignore):
            return
        for item in enums:
            self.myfile.write('  - %s\n'%(item))
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
    def writeindex(self):
        return

class HelpGen:
    pline = ''
    fp = []
    pset = []
    writers = []
    section_descriptors = {}
    sourcepath = ''
    finished = False 
    genfiles = []
    fragment_counter = 0
    modname = ''
    secname = ''
    def __init__(self,prefix,sourcepath):
        self.writers = TextWriter()
        self.pset = ExpressionSet(prefix)
        self.sourcepath = sourcepath
        fp = open(sourcepath + '/src/toolbox/help/section_descriptors.txt','r')
        for line in fp:
            m = re.search(r'(\w*)\s*([^\n]*)',line)
            self.section_descriptors[m.group(1)] = m.group(2)
        fp.close()
        return
    def testmatch(self,rtext):
        return re.search(rtext,self.pline,re.DOTALL)
        return
    def mustmatch(self,rtext):
        match = re.search(rtext,self.pline,re.DOTALL)
        if (match):
            return match.group(1)
        else:
            exit('Error: match failed to find ' + rtext + ' expression in ' + self.pline)
        return
    def getline(self):
        ##return self.fp.readline().rstrip()
        return self.fp.readline()
    def nextline(self):
        self.pline = self.getline()
        #print('Line:%s'%(self.pline))
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
        fname = self.mustmatch(self.pset.fnin);
        self.nextline();
        fn = '';
        while not self.testmatch(self.pset.fnout):
            fn += self.mustmatch(self.pset.ccomment)
            self.nextline()
        self.genfiles.append(fname)
        zp = open(fname,'w+')
        zp.write(fn)
        zp.close()
        if (re.search('test_\w+',fname)):
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
        ###if (feof(fp)), error('unmatched enumeration block'); end
        self.nextline()
        self.writers.doitemize(itemlist)
        return
    def handle_exec(self):
        match = self.mustmatch(self.pset.execin)
        if (isinstance(match,int)):
            errors_expected = int(self.mustmatch(self.pset.execin))
        else:
            errors_expected = 0
        self.nextline()
        cmdlist = [];
        while not self.testmatch(self.pset.execout):
            cmdlist.append(self.mustmatch(self.pset.ccomment))
            self.nextline()
        fragnum = '{:03d}'.format(self.fragment_counter)
        fragname = 'frag_%s_%s_%s'%(self.secname,self.modname,fragnum)
        fragfile = self.sourcepath + '/help/fragments/' + fragname + '.m'
        fp = open(fragfile,'w')
        fp.write('%errorcount is ' + str(errors_expected) + '\n')
        for cmd in cmdlist:
            fp.write(cmd)
        fp.close()
        self.fragment_counter = self.fragment_counter + 1
        self.writers.docomputeblock(fragfile)
        self.nextline()
        return
    def process_file(self,filename):
        self.fp = open(filename,'r')
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
                self.writers.beginmodule(self.sourcepath,self.modname,moddesc,self.secname,self.section_descriptors)
                self.nextline()
                while not self.testmatch(self.pset.docblock):
                    groupname = self.mustmatch(self.pset.groupname)
                    print('  Group %s...'%(groupname.lower()))
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

h = HelpGen('//','/home/sbasu/Devel/FreeMat')
h.process_file(sys.argv[1])

