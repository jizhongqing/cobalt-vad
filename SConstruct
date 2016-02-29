
# show the default environment
#print Environment().Dump()

# top-level SConscript that dispatches to directory-specific SConscript files
top_level_sconscript = './SConscript'

# set up command-line variables
vars = Variables()

# this default build-top must be used when making logref reference log files
BUILD_TOP_DEFAULT = 'build'
vars.Add(PathVariable('BUILD_TOP',
                      'Name of subdirectory, relative to the project, under '
                      'which the platform-specific build should occur',
                      BUILD_TOP_DEFAULT,
                      PathVariable.PathAccept))

vars.Add(BoolVariable('NO_DOC',
                      'Set to True to skip documentation building',
                      False))

vars.Add(EnumVariable('BUILD_TYPE', 'build type', 'debug',
                      allowed_values=('debug', 'release'),
                      map={}, ignorecase=2))

vars.Add(PathVariable('EXTERNAL_LIB_DIRS',
                      'paths were external libs are installed, paths are separated by semicolons, will seek based on order',
                      '',
                      PathVariable.PathAccept))

vars.Add(PathVariable('EXTERNAL_INC_DIRS',
                      'paths were external headers are installed, paths are separated by semicolons, will seek based on order',
                      '',
                      PathVariable.PathAccept))

if vars.UnknownVariables():
    raise ValueError("unhandled command-line variables: %s" % ' '.join(sorted(vars.UnknownVariables())))

# show the environment with variables loaded
## env0 = Environment(tools=[], variables=vars)
## print
## print env0.Dump()
## del env0

# mechanism to override default tools
# XXX really need to have separate builds that all get done, e.g. with
# multiple environments

# SCons tools that we use
tools = (

    # note: we are relying on SCons to implement that the order in
    # which the tools get generated (initialized) to be the same as
    # the order here.

    # SCons default and/or other built-in tools first; that way, the
    # items we prepend to PATH etc when our tools are loaded come
    # before the default items
    'default',

    # ProjectTools sets up project-level entities in the env
    'ProjectTools',

    # PyTools is a set of Scons tools for Python files
    'PyTools',

    # CppTools
    'CppTools',
    )

env = Environment(tools=tools,
                  variables=vars,
                  CPPFLAGS='$CPPFLAGS -Wall -Werror, -Wno-sign-compare')

# generate help for command-line variables
Help(vars.GenerateHelpText(env))

Export('env')

# for debugging, show the environment we've created
if False:
    print
    print env.Dump()
    print
    print dir(env)
    print
    print dir()
    print

env['STATIC_AND_SHARED_OBJECTS_ARE_THE_SAME']=1

# call the top-level SConscript file
env.SConscript([top_level_sconscript],
               variant_dir=env['COBALT_BUILD'],
               duplicate=1)

