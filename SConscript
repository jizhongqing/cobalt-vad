Import('env')

# directories with a SConscript file in them
dirs = (
    # SCons initialization and tools
    './site_scons/site_tools',

    # CPP Tools
    # TODO, right now, the ordering of the directories is also the ordering of dependencies
    # SCons needs to scan the SConscript of a dependent directory first.
    # We should use the global function in CppTools to re-order and make things automatic.
    './cpp/util',
    './cpp/config',
    './cpp/api',
    './cpp/detector',
    './cpp/client',
    )

# We want these top-level files to appear in the build directory:
env.Local('SConstruct')
env.Local('changes.txt')
env.Local('README.txt')
env.Local('LICENSE.txt')
env.Local('NOTICE.txt')

# We want these directories to appear in the build directory, even though there
# are no explicit dependencies on them.
bin_dir = env.Dir('#bin')
scons_dir = env.Dir('#scons')

import os, os.path
# run the SConscript files using the env that SConstruct has set up
env.SConscript(list(os.path.join(dir, 'SConscript') for dir in dirs))

