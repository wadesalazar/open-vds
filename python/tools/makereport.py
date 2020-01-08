import sys
from glob import glob
from os import path

total_not_implemented = 0
print("\n *** OpenVDS wrapper generator report ***\n")
sourcefiles = glob('../OpenVDS/Py*.cpp')
for f in sourcefiles:
    with open(f) as infile:
        lines = infile.readlines()
    autogen_fail = [l[l.index(':')+1:].strip() for l in lines if '// AUTOGENERATE FAIL :' in l]
    implemented = [l[l.index(':')+1:].strip() for l in lines if '// IMPLEMENTED :' in l]
    not_implemented=[l for l in autogen_fail if l not in implemented]
    p,n = path.split(f)
    print("{:40} {} items left unimplemented out of {} autogenerate failures.".format(n, len(not_implemented), len(autogen_fail)))
    if not_implemented:
        for n in not_implemented:
            print("  Autogenerate failure: {}".format(n))
        print()
    total_not_implemented += len(not_implemented)
print("\nTotal: {} items not implemented.".format(total_not_implemented))
    