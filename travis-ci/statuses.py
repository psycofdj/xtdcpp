#!/usr/bin/env python
#------------------------------------------------------------------#

__author__    = "Xavier MARCELET <xavier@marcelet.com>"

#------------------------------------------------------------------#

import json
import os
import sys
import requests
import argparse
import subprocess
import urllib
import glob
from lxml import etree

l_path = os.path.realpath(os.path.dirname(__file__))
os.chdir(os.path.dirname(l_path))
sys.path.append(".")

#------------------------------------------------------------------#

class StatusHelper:
  def __init__(self):
    self.m_dryrun = False
    self.m_parser = argparse.ArgumentParser("xtd build checker")
    self.m_parser.add_argument("--token",    help="Github API secret token",                 dest="m_token",     required=True)
    self.m_parser.add_argument("--build-dir",help="Travis build directory",                  dest="m_dir",       required=True)
    self.m_parser.add_argument("--build-id", help="Travis build-id",                         dest="m_buildID",   required=True)
    self.m_parser.add_argument("--branch",   help="Target branch",                           dest="m_branch",    required=True)
    self.m_parser.add_argument("--commit",   help="Current git commit hash",                 dest="m_commit",    required=True)
    self.m_parser.add_argument("--pull-id",  help="Current pull request, false if not a PR", dest="m_prid",      required=True)
    self.m_parser.add_argument("--dry-run",  help="Do not push statuses to github",          dest="m_dryrun",    action="store_true")
    self.m_parser.parse_args(sys.argv[1:], self)
    self.m_comment = ""

  def get_pr_commit(self):
    if self.m_dryrun:
      return {}

    l_params  = { "access_token" : self.m_token }
    l_headers = { "Content-Type" : "application/json" }
    l_url     = "https://api.github.com/repos/psycofdj/xtdcpp/pulls/%(prid)s" % {
      "prid"   : self.m_prid
    }

    try:
      l_req = requests.get(l_url, params=l_params, headers=l_headers)
      l_data = l_req.json()
      return l_data["head"]["sha"]
    except BaseException as l_error:
      print("error while sending comment to github : %s" % str(l_error))
      sys.exit(1)


  def getTargetUrl(self):
    l_url = "https://travis-ci.org/psycofdj/xtdcpp/builds/%(buildID)s"
    return l_url % {
      "buildID" : self.m_buildID
    }

  def make_badge(self, p_title, p_label, p_value, p_status, p_link = "#"):
    if p_status == "error":
      l_color = "red"
    elif p_status == "failure":
      l_color = "lightgrey"
    elif p_status == "warning":
      l_color = "yellow"
    else:
      l_color = "brightgreen"
    l_url = "https://img.shields.io/badge/%(label)s-%(value)s-%(color)s.svg" % {
      "label" : urllib.quote(p_label),
      "value" : urllib.quote(p_value),
      "color" : l_color
    }

    return "[![%(title)s](%(url)s)](%(link)s)" % {
      "title" : p_title,
      "url"   : l_url,
      "link"  : p_link
    }

  def comment_pr(self, p_body):
    if self.m_dryrun:
      return {}

    l_params  = { "access_token" : self.m_token }
    l_headers = { "Content-Type" : "application/json" }
    l_url     = "https://api.github.com/repos/psycofdj/xtdcpp/issues/%(prid)s/comments" % {
      "prid"   : self.m_prid
    }
    l_data = {
      "body" : p_body
    }

    try:
      l_req = requests.post(l_url, params=l_params, headers=l_headers, data=json.dumps(l_data))
    except BaseException as l_error:
      print("error while sending comment to github : %s" % str(l_error))
      sys.exit(1)
    return l_req.json()

  def comment_commit(self, p_body):
    if self.m_dryrun:
      return {}

    l_params  = { "access_token" : self.m_token }
    l_headers = { "Content-Type" : "application/json" }
    l_url     = "https://api.github.com/repos/psycofdj/xtdcpp/commits/%(commitID)s/comments" % {
      "user"     : "psycofdj",
      "repo"     : "xtd",
      "commitID" : self.m_commit
    }
    l_data = {
      "body" : p_body
    }
    try:
      l_req = requests.post(l_url, params=l_params, headers=l_headers, data=json.dumps(l_data))
    except BaseException as l_error:
      print("error while sending comment to github : %s" % str(l_error))
      sys.exit(1)
    return l_req.json()


  def send_status(self, p_status, p_tag, p_description):
    if self.m_dryrun:
      return {}
    l_url    = "https://api.github.com/repos/psycofdj/xtdcpp/statuses/%(commit)s" % {
      "commit" : self.m_commit
    }
    l_params  = { "access_token" : self.m_token }
    l_headers = { "Content-Type" : "application/json" }
    l_data    = {
      "state"       : p_status,
      "target_url"  : self.getTargetUrl(),
      "description" : p_description,
      "context"     : p_tag
    }

    try:
      l_req = requests.post(l_url, params=l_params, headers=l_headers, data=json.dumps(l_data))
    except BaseException as l_error:
      print("error while sending comment to github : %s" % str(l_error))
      sys.exit(1)
    return l_req.json()

  def collect_unittests(self):
    print("--------------------")
    print("Collecting unittests")
    print("--------------------")
    try:
      l_tests  = glob.glob("%s/reports/*/check/tests.xml" % self.m_dir)
      l_total  = 0
      l_failed = 0
      l_passed = 0
      for c_test in l_tests:
        l_root = etree.parse(c_test)
        l_total += len(l_root.xpath("/Site/Testing/Test"))
        l_failed += len(l_root.xpath("/Site/Testing/Test[@Status!='passed']"))
        l_passed += len(l_root.xpath("/Site/Testing/Test[@Status='passed']"))
      if l_failed == 0:
        l_status      = "success"
        l_badge_descr = "passed (%d/%d) " % (l_passed, l_total)
        l_description = "Tests : %d failed, %d passed" % (l_failed, l_passed)
      else:
        l_status      = "error"
        l_badge_descr = "failed (%d/%d) " % (l_failed, l_total)
        l_description = "Tests : %d failed, %d passed" % (l_failed, l_passed)
      print("%s : %s" % (l_status, l_description))
    except BaseException as l_error:
      print("caught exception : %s" % str(l_error))
      l_status      = "failure"
      l_badge_descr = "error while collect tests"
      l_description = "error while collect tests"

    l_badge = self.make_badge("UnitTests", "unittests", l_badge_descr, l_status)
    self.m_comment += "%s\n" % l_badge
    self.send_status(l_status, "kpi/unittests", l_description)
    print("")


  def collect_coverage(self):
    print("-------------------")
    print("Collecting coverage")
    print("-------------------")
    try:
      l_tests   = glob.glob("%s/reports/*/coverage/coverage.xml" % self.m_dir)
      l_total   = 0
      l_covered = 0
      for c_test in l_tests:
        l_root     = etree.parse(c_test)
        l_coverage = l_root.xpath("/coverage")[0]
        l_total   += int(l_coverage.get("lines-valid"))
        l_covered += int(l_coverage.get("lines-covered"))

      l_status = "success"
      l_percent = 0
      if l_total != 0:
        l_percent = (float(l_covered) / float(l_total)) * 100
      l_badge_status = "success"
      if l_percent < 75.0:
        l_badge_status = "warning"
      l_description = "Coverage : %.2f %%" % l_percent
      print("%s : %s" % (l_status, l_description))
    except BaseException as l_error:
      print("caught exception : %s" % str(l_error))
      l_percent      = 0
      l_badge_status = "failure"
      l_status       = "failure"
      l_description  = "Unable to collect test results"
    l_badge = self.make_badge("Coverage", "coverage", "%.2f %%" % l_percent, l_badge_status)
    self.m_comment += "%s\n" % l_badge
    self.send_status(l_status, "kpi/coverage", l_description)
    print("")

  def collect_cppcheck(self):
    print("-------------------")
    print("Collecting cppcheck")
    print("-------------------")
    try:
      l_tests   = glob.glob("%s/reports/*/cppcheck/cppcheck.xml" % self.m_dir)
      l_total   = 0
      for c_test in l_tests:
        l_root   = etree.parse(c_test)
        l_total += len(l_root.xpath("/results/error"))
      l_status       = "success"
      l_badge_status = "success"
      if l_total != 0:
        l_badge_status = "warning"
      l_badge_descr = "%d error(s)" % l_total
      l_description = "Cppcheck detected %d error(s)" % l_total
      print("%s : %s" % (l_status, l_description))
    except BaseException as l_error:
      print("caught exception : %s" % str(l_error))
      l_badge_status = "failure"
      l_status       = "failure"
      l_description  = "Unable to collect cppcheck"
    l_badge = self.make_badge("Cppcheck", "cppcheck", l_badge_descr, l_badge_status)
    self.m_comment += "%s\n" % l_badge
    self.send_status(l_status, "kpi/cppcheck", l_description)
    print("")


  def collect_cloc(self):
    print("---------------")
    print("Collecting cloc")
    print("---------------")
    try:
      l_tests   = glob.glob("%s/reports/*/cloc/cloc.xml" % self.m_dir)
      l_code    = 0
      l_comment = 0
      for c_test in l_tests:
        l_root     = etree.parse(c_test)
        l_elem     = l_root.xpath("/results/languages/total")[0]
        l_code    += int(l_elem.get("code"))
        l_comment += int(l_elem.get("comment"))

      l_percent = 0
      if l_code != 0:
        l_percent = float(l_comment) / float(l_code) * 100
      l_status       = "success"
      l_badge_status = "success"
      if l_percent < 50:
        l_badge_status = "warning"
      l_badge_descr = "%d / %d (%.2f %%)" % (l_comment, l_code, l_percent)
      l_description = "Code statistics : %d / %d (%.2f %%)" % (l_comment, l_code, l_percent)
      print("%s : %s" % (l_status, l_description))
    except BaseException as l_error:
      print("caught exception : %s" % str(l_error))
      l_badge_status = "failure"
      l_status       = "failure"
      l_description  = "Unable to collect cloc"
      l_badge_descr  = "unable to collect"
    l_badge = self.make_badge("Code statistics", "cloc", l_badge_descr, l_badge_status)
    self.m_comment += "%s\n" % l_badge
    self.send_status(l_status, "kpi/cloc", l_description)
    print("")


  def collect_doc(self):
    print("--------------")
    print("Collecting doc")
    print("--------------")

    l_status       = "success"
    l_badge_status = "success"
    l_badge_descr  = "success"
    l_description  = "Documentation successfully generared"
    l_badge = self.make_badge("Documentation", "doc", l_badge_descr, l_badge_status)
    print("%s : %s" % (l_status, l_description))
    self.m_comment += "%s\n" % l_badge
    self.send_status(l_status, "kpi/doc", l_description)
    print("")


  def collect_doc_coverage(self):
    print("-----------------------")
    print("Collecting doc-coverage")
    print("-----------------------")
    try:
      l_tests   = glob.glob("%s/reports/*/doc-coverage/doc-coverage.xml" % self.m_dir)
      l_total   = 0
      l_covered = 0
      for c_test in l_tests:
        l_root     = etree.parse(c_test)
        l_coverage = l_root.xpath("/coverage")[0]
        l_total   += int(l_coverage.get("lines-valid"))
        l_covered += int(l_coverage.get("lines-covered"))

      l_status = "success"
      l_percent = 0
      if l_total != 0:
        l_percent = (float(l_covered) / float(l_total)) * 100
      l_badge_status = "success"
      if l_percent < 50:
        l_badge_status = "warning"
      l_description = "Documentation coverage : %.2f %%" % l_percent
      print("%s : %s" % (l_status, l_description))
    except BaseException as l_error:
      print("caught exception : %s" % str(l_error))
      l_percent      = 0
      l_badge_status = "failure"
      l_status       = "failure"
      l_description  = "Unable to collect test results"

    l_badge = self.make_badge("Documentation coverage", "doc--coverage", "%.2f %%" % l_percent, l_badge_status)
    self.m_comment += "%s\n" % l_badge
    self.send_status(l_status, "kpi/doc-coverage", l_description)
    print("")

  def run(self):
    self.m_pr_commit       = "false"
    self.m_doCommentCommit = True
    if self.m_prid != "false" :
      l_commit = self.get_pr_commit()
      self.m_doCommentCommit = (l_commit != self.m_commit)
      self.m_commit = l_commit

    self.send_status("pending", "kpi/unittests",    "collecting unittests")
    self.send_status("pending", "kpi/coverage",     "collecting coverage")
    self.send_status("pending", "kpi/cppcheck",     "collecting cppcheck")
    self.send_status("pending", "kpi/cloc",         "collecting cloc")
    self.send_status("pending", "kpi/doc",          "collecting documentation")
    self.send_status("pending", "kpi/doc-coverage", "collecting doc-coverage")

    self.collect_unittests()
    self.collect_coverage()
    self.collect_cppcheck()
    self.collect_cloc()
    self.collect_doc()
    self.collect_doc_coverage()

    if self.m_prid != "false" :
      self.comment_pr("""
Build report #%(buildid)s for PR %(prid)s.

%(results)s

[Reports](https://psycofdj.github.io/xtdcpp/pr/%(prid)s/)
      """ % {
        "prid"    : self.m_prid,
        "buildid" : self.m_buildID,
        "results" : self.m_comment
      })

    if self.m_doCommentCommit:
      self.comment_commit("""
Build report #%(buildid)s for sha %(commit)s:

%(results)s

[Reports](https://psycofdj.github.io/xtdcpp/%(branch)s/)
      """ % {
        "branch" : self.m_branch,
        "commit" : self.m_commit,
        "buildid" : self.m_buildID,
        "results" : self.m_comment
      })

if __name__ == "__main__":
  l_app = StatusHelper()
  l_app.run()
