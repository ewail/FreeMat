import xml.dom.minidom
import sys


filename = sys.argv[1]
outfile = sys.argv[2]
x = xml.dom.minidom.parse(filename)
compounds = x.getElementsByTagName("compound")
fp = open(outfile,"w")
for compound in compounds:
    if (compound.getAttribute("kind") == "page"):
        name = compound.getElementsByTagName("name")[0].firstChild.nodeValue
        # Name is of the form section_node
        if (name.find('_') != -1):
            (section,node) = name.split('_')
            if section != "sec":
                fp.write("%s (%s)\n"%(node,section))
fp.close()


        
