def refilter(txt):
    ret = ''
    ineqn = False
    protect = ['^','_','&','|']
    while (txt):
        c = txt[0]
        txt = txt[1:]
        if (c == '$'):
            ineqn = not ineqn
        if ((c in protect) and not ineqn):
            ret += '\\'
        ret += c
    return ret
