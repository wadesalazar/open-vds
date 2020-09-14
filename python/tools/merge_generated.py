import sys
from glob import glob
from os import path

merge_begin = '//AUTOGEN-BEGIN'
merge_end   = '//AUTOGEN-END'

sourcefiles = glob('generated/*.h')
targetfiles = glob('../OpenVDS/Py*.cpp')
for f in sourcefiles:
    tmp = 'Py' + path.split(f)[1].replace('.h', '.cpp') 
    target = [t for t in targetfiles if t.endswith(tmp)]
    if target:
        t = target[0]
        print("Merging: {}".format(t), end=' ')
        source_contents = ''
        target_contents = ''
        with open(f, 'rb') as infile:
            source_contents = infile.read().decode().replace('\r', '')
        with open(t, 'rb') as targetfile:
            target_contents = targetfile.read().decode().replace('\r', '')
        target_lines = target_contents.splitlines()
        implemented = [l[l.index(':')+1:].strip() for l in target_lines if '// IMPLEMENTED :' in l]
        source_lines = source_contents.splitlines()
        unimplemented = [l for l in source_lines if not l.strip() in implemented]
        source_lines = '\n'.join(unimplemented) + '\n' if unimplemented else ''
        if merge_begin in target_contents and merge_end in target_contents:
            prefix = target_contents[:target_contents.index(merge_begin)+len(merge_begin)+1]
            suffix = target_contents[target_contents.index(merge_end):]
            merged = prefix + source_lines + suffix
            if merged == target_contents:
                print("[unchanged]")
            else:
                with open(t, 'wb') as outfile:
                    outfile.write(merged.encode())
                    print("[written]")
        else:
            print('Merge patterns not found in {}, skipping.'.format(t), file=sys.stdout)
            
