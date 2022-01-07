import sys

section = sys.argv[1] # .data, .text, etc
mod_objs = sys.argv[2:]

s = []

for obj in mod_objs:
    s.append(f"{obj}({section});")

print('"' + ''.join(s) + '"')
