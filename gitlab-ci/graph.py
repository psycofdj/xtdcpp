#!/usr/bin/env python

import sys
import os
import json
import argparse

l_parser = argparse.ArgumentParser()
l_parser.add_argument("--report-dir",   action="store", help ="path to xtdmake reports", required=True)
l_result = l_parser.parse_args()


for c_root, c_dirs, c_files in os.walk(l_result.report_dir):
  for c_file in c_files:
    if c_file == "status.json":
      l_module=os.path.basename(os.path.dirname(c_root))
      l_kpi=os.path.basename(c_root)
      print(l_module + ":" + l_kpi)
      print(c_root + "/" + c_file)
      l_file = open(os.path.join(c_root, c_file), "r")
      l_content = l_file.read()
      l_data = json.loads(l_content)
      print(l_data["axes"])
  




