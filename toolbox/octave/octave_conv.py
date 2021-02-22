import os,  os.path
import re
import sys

#p='C:\\freemat_src\\Freemat4\\src\\toolbox\\octave\\nnet'
p=os.getcwd()
if len(sys.argv) > 1:
    p = sys.argv[1]

keyword_dict = { 'endif':'end',  'endfor':'end',  'endfunction':'end',  'endswitch':'end',  "isn't":"is not",  "doesn't":"does not", r"'%s'":r"''%s''" }
syntax_dict = { '#':'%', '!':'~', '"':"'" }

def process_comments( script ):
    return script
    
def process_globals( script ):
    glob_re = re.compile("(?<=global).+=.*(?=\s|\Z|;])") #look for global keyword
    matches = glob_re.search(script) 
    glob_var_list = []
    assgn_list = []
    if matches != None:
        print matches.start(0), matches.end(0)
        # parse 'global a, b=2;'
        clauses = re.split('[,;\r\n]*', script[ matches.start(0) : matches.end(0) ]) 
        for expr in clauses:
            e = expr.strip()
            if e.find('=')>0:
                var_name = e.split('=')[0].strip()
                glob_var_list.append(var_name)
                assgn_list.append(e)
            else:
                if len(e)>0:
                    glob_var_list.append( e )
            
        out_script = script[:matches.start(0)]
        for e in glob_var_list:
            out_script = out_script + ' ' + e + ','
        out_script = out_script[:-1] + ';'
        out_script = out_script + '\n'
        for e in assgn_list:
            out_script = out_script + ' ' + e + ';\n'
        out_script = out_script + '\n' + script[matches.end(0)+1:]
        script = out_script
    return script
    
  
def process_syntax( script ):
    for kwd,  subst in syntax_dict.items():
        script, nsub=re.subn('%s'%kwd,  subst,  script)
    return script

def process_keywords( script ):
    for kwd,  subst in keyword_dict.items():
        script, nsub=re.subn('(?<=\s)%s(?=\s|$)'%kwd,  subst,  script)
    return script

def process_dbl_underscore( script ):
    undrscr_re = re.compile('__[^\(\s]+')
    scr_srch = script
    matches = undrscr_re.search(scr_srch)
    fnames = []
    while matches != None:
        fnames.append( scr_srch[ matches.start(0) : matches.end(0) ] ) 
        scr_srch = scr_srch[matches.end(0):]
        matches = undrscr_re.search(scr_srch)
        
    for fname in fnames:
        script = script.replace(fname,  fname[2:]+'__priv')
    return script
    
def process_script( script ):
    script = process_keywords( script )
    script = process_syntax( script )
    script = process_globals( script )
    script = process_comments( script )
    script = process_dbl_underscore( script )
    return script 
    
def process_files( fpaths ):
    for fname,  fpath in fpaths.items():
        f = open(fpath, 'r')
        script = f.read()
        f.close()
        script = process_script( script )
        
        if fname[0:2]=='__':
            f = open(os.path.join(os.path.dirname(fpath), fname[2:-2]+'__priv'+'.m'), 'w')
        else:
            f = open(fpath, 'w')
        f.write(script)
        f.close()
    

exclude_pth = re.compile(r"""\.svn""")
mat_files = dict()

for root, dirs, files in os.walk(p):
    if exclude_pth.search(root)==None:
        for file in files:
            if file[-2:]=='.m':
                print file
                mat_files[file] = os.path.join(root, file)

process_files(mat_files)
