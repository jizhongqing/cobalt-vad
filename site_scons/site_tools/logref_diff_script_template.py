#!/usr/bin/env python

reffile = ''
trialfile = ''

import sys, subprocess

cmd = ['diff', trialfile, reffile]

if len(sys.argv) > 1:
    if sys.argv[1] == 'accept':
        cmd= ['cp', trialfile, reffile]

subprocess.call(cmd)
