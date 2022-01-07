import sys

outfile = sys.argv[1]
mods = sys.argv[2:]

with open(outfile, 'w') as f:
    for mod in mods:
        f.write(f'void {mod}_main(void);\n')
    
    f.write('void load_mods(void) {\n')

    for mod in mods:
        f.write(f'    {mod}_main();\n')

    f.write('}\n')
