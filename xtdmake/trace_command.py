#!/usr/bin/env python3

import os
import sys
import fcntl
import time
import json
import errno

l_filename = "%s/compile_commands.json" % sys.argv[1]
l_mode     = "r+"
if not os.path.exists(l_filename):
  l_mode = "w+"

with open(l_filename, l_mode) as l_file:
  while True:
    try:
      fcntl.flock(l_file, fcntl.LOCK_EX | fcntl.LOCK_NB)
      break
    except IOError as e:
      # raise on unrelated IOErrors
      if e.errno != errno.EAGAIN:
        raise
      else:
        time.sleep(0.1)

  l_content = "".join(l_file.readlines())
  if not len(l_content):
    l_data = []
  else:
    l_data = json.loads(l_content)


  l_newData = []
  for c_item in l_data:
    if not c_item["file"] == sys.argv[3]:
      l_newData.append(c_item)
  l_newData.append({
    "directory" : sys.argv[2],
    "file" : sys.argv[3],
    "command" : sys.argv[4]
  })

  l_file.seek(0)
  l_file.write(json.dumps(l_newData, indent=2))
  fcntl.flock(l_file, fcntl.LOCK_UN)

