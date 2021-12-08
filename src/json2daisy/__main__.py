import os

from .json2daisy import *

if __name__ == '__main__':
  import argparse

  parser = argparse.ArgumentParser(prog='python3 -m json2daisy',
                                  description='Convert JSON board descriptions to C++ headers.')
  parser.add_argument('source', type=str,
                      help='Daisy board name or path to JSON board description (e.g. "patch" or "path/to/myboard.json")')
  parser.add_argument('-o', metavar='outfile', type=str, default=None,
                      help='output file name')
  
  args = parser.parse_args()
  if os.path.exists(args.source) and os.path.isfile(args.source):
    header, info = generate_header_from_file(args.source)
  else:
    # In case a file is passed but does not exist
    if args.source[-4:] == '.json':
      print(f'Error: cannot find file "{args.source}"')
      exit(1)
    header, info = generate_header_from_name(args.source)
    
  outfile = args.o if args.o is not None else f'j2daisy_{info["name"]}.h'
  with open(outfile, 'w') as file:
    file.write(header)
  
  print(f'Generated Daisy C++ header in "{outfile}"')
