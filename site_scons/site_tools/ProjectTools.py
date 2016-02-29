# Parts of this file are derived from the following licence.
#
###########################################################################
#
# Author:       Hugh Secker-Walker
# Description:  SCons support for work
#
# This file is part of Onyx   http://onyxtools.sourceforge.net
#
# Copyright 2007 - 2009 The Johns Hopkins University
#
# Licensed under the Apache License, Version 2.0 (the "License").
# You may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
# implied.  See the License for the specific language governing
# permissions and limitations under the License.
#
###########################################################################


from __future__ import with_statement
import re
from functools import partial
from itertools import count
import distutils.sysconfig

import timestamp, os

StrictError = type('StrictError', (ValueError,), {})
def izipstrict(*args):
    """
    XXX broken!  this function does not always raise the error it claims to
    implement; it needs a layer of counters to see how much was used from each
    iterable....

    Returns a generator very like the itertools izip function.  The arguments
    are treated like arguments to izip.  Unlike izip this function ensures that
    all the iterators finished after the same number of items.  It raises a
    StrictError if not all the ierators have finished.

    If StrictError is raised, in addition to a human-readable message, the
    exception will have attributes 'finished', 'unfinished' and 'values', all
    tuples corresponding respectively to the indices in the argument list of the
    iterators that had finished, the indices of the iterators that had not
    finished, and the initial yields from those unfinished iterators.  Note that
    process of getting each initial yield can itself finish iterators in the
    unfinished set.

    >>> tuple(izipstrict(xrange(5), xrange(5)))
    ((0, 0), (1, 1), (2, 2), (3, 3), (4, 4))

    Uncaught error

    >>> tuple(izipstrict(xrange(5), xrange(4), xrange(1,6), xrange(4)))
    Traceback (most recent call last):
       ...
    StrictError: out of 4 iterators, 2 were unfinished: at argument indices (0, 2)

    Caught error and attributes

    >>> try: tuple(izipstrict(xrange(5), xrange(4), xrange(1,6), xrange(4)))
    ... except StrictError, e: print e.finished, e.unfinished, e.values
    (1, 3) (0, 2) (4, 5)
    """
    nexts = tuple(iter(arg).next for arg in args)
    finished = list()
    build = list()
    build_append = build.append
    while True:
        del build[:]
        for index, next in enumerate(nexts):
            try:
                build_append(next())
            except StopIteration:
                finished.append(index)
        if finished and build:
            unfinished = tuple(sorted(frozenset(xrange(len(nexts))) - frozenset(finished)))
            assert len(unfinished) == len(build)
            err = StrictError("out of %d iterators, %d were unfinished: at argument indices %s"
                              % (len(nexts), len(unfinished), tuple(unfinished)))
            err.finished = tuple(finished)
            err.unfinished = unfinished
            err.values = tuple(build)
            raise err
        if build:
            yield tuple(build)
        else:
            assert len(finished) == len(nexts)
            raise StopIteration

# a simple diff tool
def diff_streams(env, target, source_ref, source_trial):

    # XXX: we need a proper normalization target to deal with all normalization
    # ; *this* function should be like diff

    assert target.name and source_ref.name and source_trial.name

    # mask out timestamps
    norm_timestamp = '###TIMESTAMP###'
    timestamp_subber = partial(timestamp.TIMESTAMP_RE.sub, norm_timestamp)

    # mask out pointer values, e.g. in 'ptr="0x00abcdef"'
    ptr_subber = partial(re.compile(r'\bptr\=\"0x[0-9a-fA-F]{1,8}\"').sub, r'###POINTER###')

    source_ref_iter = iter(source_ref)
    source_trial_iter = iter(source_trial)
    iters = count(1), source_ref_iter, source_trial_iter
    # assignment is necessary in case a file is empty
    lineno = 0
    try:
        for lineno, source_ref_line, source_trial_line in izipstrict(*iters):
            # XXX this would be useful if it did some string alignment work showing all diffs....

            source_ref_line = source_ref_line.rstrip('\n\r').replace('\\', '/')
            source_trial_line = source_trial_line.rstrip('\n\r').replace('\\', '/')

            source_ref_line = timestamp_subber(source_ref_line)
            source_trial_line = timestamp_subber(source_trial_line)

            source_ref_line = ptr_subber(source_ref_line)
            source_trial_line = ptr_subber(source_trial_line)

            if source_ref_line != source_trial_line:
                target.write("reference and trial files first differ on line %d\n" % (lineno,))
                target.write("%s(%d) : %s\n" % (source_ref.name, lineno, source_ref_line))
                target.write("%s(%d) : %s\n" % (source_trial.name, lineno, source_trial_line))
                return 1
    except StrictError, e:
        # we expect the count iterator to not have finished
        finished, unfinished, values = e.finished, e.unfinished, e.values
        assert unfinished and unfinished[0] == 0 and values[0] == lineno + 1
        if len(unfinished) > 1:
            assert len(finished) == 1
            target.write("%s ends at line %d : the other file continues\n" % (iters[finished[0]].name, lineno,))
            target.write("%s(%d) : %s\n" % (iters[unfinished[1]].name, lineno+1, values[1]))
            return 1
    else:
        assert False, "unreachable"

    return 0

from contextlib import nested
def diff(target, source, env):
    target, = target

    source_ref, source_trial, = source
    with nested(open(str(target), 'wt'),
                open(str(source_ref), 'rt'),
                open(str(source_trial), 'rt')) as files:
        return diff_streams(env, *files)

def diff_norm(target, source, env):
    import filecmp
    target, = target
    source_ref, source_trial = source
    if filecmp.cmp(str(source_ref), str(source_trial)):
        return 0

    # generate target file, a shell script will work on linux, but not windows!!!
    script_template = os.path.join('data/scripts','logref_diff_script_template.py')

    if not os.path.isfile(script_template): raise Exception('template script not found ' + script_template)

    with open(script_template) as in_file, open(str(target), 'wt') as out_file:
        print str(source_ref), str(source_trial)
        for line in in_file:
            if line.startswith('reffile'):
                line = 'reffile = "' + str(source_ref) + '"\n'
            if line.startswith('trialfile'):
                line = 'trialfile = "' + str(source_trial) + '"\n'
            out_file.write(line)
    # should probably use os.chmod instead of subprocess
    import subprocess
    subprocess.call(['chmod', 'a+x', str(target)])
    return 1

def normalize_trial(target, source, env):

    regexs = list()
    # mask out timestamps
    norm_timestamp = '###TIMESTAMP###'
    timestamp_subber = partial(timestamp.TIMESTAMP_RE.sub, norm_timestamp)
    regexs.append(timestamp_subber)

    # mask out pointer values, e.g. in 'ptr="0x00abcdef"'
    ptr_subber = partial(re.compile(r'\bptr\=\"0x[0-9a-fA-F]{1,8}\"').sub, r'###POINTER###')
    regexs.append(ptr_subber)

    with open(str(target[0]), 'wt') as normalized_out_file, open(str(source[0]), 'rt') as in_file:
        for line in in_file:
            norm_line = line
            for regex in regexs:
                norm_line = regex(line)
            normalized_out_file.write(norm_line)
    return 0

def generate(env):
    """
    Add construction variables and set up environment for Cobalt SCons work
    """
    # set this to True for some logging while SCons is initializing

    debug = False

    import os, sys
    import SCons

    # set up some project-specific names in the SCons environment
    COBALT_BUILD = env['BUILD_TOP'] + '/' + sys.platform + '/' + env['BUILD_TYPE']
    COBALT_INSTALL = env['BUILD_TOP'] +'/' + env['BUILD_TYPE']
    env.SConsignFile(COBALT_BUILD + '/' + '.sconsign')

    env['COBALT_BUILD'] = env.Dir(COBALT_BUILD)
    env['COBALT_INSTALL'] = COBALT_INSTALL
    env['COBALT_SOURCE_PYLIB'] = 'cpp/pylib/' + sys.platform
    env['COBALT_BUILD_PYLIB'] = env.Dir(COBALT_BUILD + '/' + env['COBALT_SOURCE_PYLIB'])

    # SCons view of platform.
    PLATFORM = env['PLATFORM']
    if PLATFORM == 'darwin':
        env.PrependENVPath('PATH', '/opt/local/bin')

    shlibsuffix = distutils.sysconfig.get_config_var('SO')
    if shlibsuffix is None:
        raise ValueError("Don't know the proper extension for Python shared libraries on this platform")

    # _foo ==> _foo.so or _foo.pyd, depending on platform
    def make_pylib_name(basename):
        return basename + shlibsuffix

    # Add correct extension for shared libraries and a function for making names to env
    env.make_pylib_name = make_pylib_name

    # *this* generic tools file
    tools_mod = env.tools_mod = '#' + 'site_scons/site_tools/ProjectTools.py'

    def tools_dependency(target, source, env):
        if len(source) != 2:
            raise ValueError("expected exactly 2 sources for Diff builder, got %d" % (len(source),))
        return target, source + [tools_mod]

    # the Diff builder uses the above diff() function
    # note: subscripting of SOURCES so as to avoid echoing the onyxtools_mod source
    env.Append(BUILDERS = {
        'Diff': env.Builder(
        action = SCons.Action.Action(diff, 'Diff ${SOURCES[:2]} > $TARGET '),
        emitter = tools_dependency,
        )
        })

    env.Append(BUILDERS = {
        'LogRefNorm': env.Builder(
            action = SCons.Action.Action(normalize_trial, 'LogRefNorm ${SOURCES[0]} > $TARGET '),
        )
        })
    env.Append(BUILDERS = {
        'LogRefDiff': env.Builder(
            action = SCons.Action.Action(diff_norm, 'LogRefDiff ${SOURCES[:2]} > $TARGET '),
        )
        })
    # meta tools
    # managedfiles = list()
    # managedfiles.append('foo')
    # note: env.managed_sources gets built up while the nodes are getting defined as part of
    # SConscripts being run
    env.managed_sources = list()

    def ManagedFiles(filename):
        # print 'ManagedFiles():'

        def write_managedfiles_list(env, target, source):
            # print 'write_managedfiles_list():', 'len(env.managed_sources)', len(env.managed_sources)
            with open(str(*target), 'wt') as outfile:
                for item in sorted(frozenset(env.managed_sources)):
                    outfile.write(str(item))
                    outfile.write('\n')

        from time import time
        # env.Command(filename, managed_sources, write_managedfiles_list)
        # XXX fix this so it actually does something
        #env.Command(filename, [env.managed_sources, env.Value(time())], write_managedfiles_list)

    env.ManagedFiles = ManagedFiles

    def LocalAllFilesInDir(dir_node, root):
        print '>>>>', dir_node.abspath, dir_node.path, root
        assert dir_node.abspath == os.path.join(root, dir_node.path)
        for top, dirs, files in os.walk(dir_node.abspath):
            # print '>>>', root, dirs, files
            assert top.startswith(root)
            relative_top = top[len(root) + 1:]
            for f in files:
                # print '>>', os.path.join(relative_top, f)
                env.Local(os.path.join(relative_top, f))

    env.LocalAllFilesInDir = LocalAllFilesInDir



def exists(env):
    """
    Make sure that ProjectTools has the resources it needs
    """
    return True


