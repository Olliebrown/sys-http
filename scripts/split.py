#!/usr/bin/env python3

"""This script splits httplib.h into .h and .cc parts."""

import argparse
import os
import sys

lib_name = 'httplib'
lib_path = ''

border = '// ----------------------------------------------------------------------------'

# Configure arguments parser
args_parser = argparse.ArgumentParser(description=__doc__)
args_parser.add_argument(
  "-e", "--extension", help="extension of the implementation file (default: cc)",
  default="cc"
)
args_parser.add_argument(
  "-o", "--out", help="where to write the files (default: out)", default="out"
)

# Parse arguments
args = args_parser.parse_args()

# get the current directory and configure paths
cur_dir = os.path.dirname(sys.argv[0])
header_name = '/' + lib_name + '.h'
source_name = '/' + lib_name + '.' + args.extension

# get the input file
in_file = cur_dir + '/' + lib_path + header_name

# get the output file
h_out = args.out + header_name
cc_out = args.out + source_name

# if the modification time of the out file is after the in file,
# don't split (as it is already finished)
do_split = True

if os.path.exists(h_out):
  in_time = os.path.getmtime(in_file)
  out_time = os.path.getmtime(h_out)
  do_split = in_time > out_time

# Split the file
if do_split:
  with open(in_file) as f:
    lines = f.readlines()

  python_version = sys.version_info[0]
  if python_version < 3:
    os.makedirs(args.out)
  else:
    os.makedirs(args.out, exist_ok=True)

  # Open the output files for writing
  in_implementation = False
  cc_out = args.out + source_name
  with open(h_out, 'w') as fh, open(cc_out, 'w') as fc:
    # Write include and namespace to the implementation file
    fc.write('#include "httplib.h"\n')
    fc.write('namespace httplib {\n')

    # Examine line by line
    for line in lines:
      # Is this the border line?
      is_border_line = border in line
      if is_border_line:
        in_implementation = not in_implementation
      elif in_implementation:
        # Write to implementation (remove any 'inline' keywords)
        fc.write(line.replace('inline ', ''))
      else:
        # Write to header
        fh.write(line)

    # Write namespace closing bracket to implementation file
    fc.write('} // namespace httplib\n')

  # Print success message
  print("Wrote {} and {}".format(h_out, cc_out))

else:
  # Print up to date message
  print("{} and {} are up to date".format(h_out, cc_out))
