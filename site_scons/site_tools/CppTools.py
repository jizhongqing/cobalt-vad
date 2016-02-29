# Tools for compiling CPP.
import os, sys

'''
util_libs = ['libutil.a',]
config_libs = ['libconfig.a',]
model_libs = ['libmodel.a',]
result_libs = ['libresult.a',]
recognizer_libs = ['librecognizer.a',]
cobalt_api_libs = ['libcobalt_api.a',]
cobalt_client_libs = ['libcobalt_client.a',]
decoder_libs = ['libdecoder.a'],
'''

json_cpp_libs = ['libjson_cpp.a',]

kaldi_libs = ['kaldi-feat.a',
              'kaldi-transform.a',
              'kaldi-thread.a',
              'kaldi-hmm.a',
              'kaldi-tree.a',
              'kaldi-matrix.a',
              'kaldi-util.a',
              'kaldi-base.a',]

blas_libs = ['liblapack.a',
             'libcblas.a',
             'libatlas.a',]

boost_libs = ['libboost_program_options.a',
              'libboost_filesystem.a',
              'libboost_system.a',
              'libboost_thread.a',
              'libboost_timer.a',
              'libboost_chrono.a',]

global_depends_dict = dict()

def find_lib_path(env, lib_name, libs, potential_paths):
    first_lib = libs[0]
    target_path = None
    for path in potential_paths:
        if os.path.isfile(os.path.join(path, first_lib)):
            target_path = path
    if target_path is None:
        raise Exception('Could not find ' + first_lib + ' on any of these paths: ' + str(potential_paths))

    lib_nodes = list()
    for lib in libs:
        lib_nodes.append(env.File(os.path.join(target_path, lib)))
    global_depends_dict[lib_name] = lib_nodes


def add_external_libs(env):

    potential_paths = list()
    external_libs_paths_single = env['EXTERNAL_LIB_DIRS']
    if external_libs_paths_single != '':
        external_libs_paths = external_libs_paths_single.split(':')
        for lib in external_libs_paths:
            potential_paths.append(lib)
    else: # kaldi
        if sys.platform == 'darwin':
            kaldi_path = '/opt/local/lib'
        else:
            kaldi_path = '/usr/local/lib'

        potential_paths.append(kaldi_path)
    find_lib_path(env, 'kaldi', kaldi_libs, potential_paths)
    find_lib_path(env, 'boost', boost_libs , potential_paths)
    find_lib_path(env, 'jsoncpp', json_cpp_libs , potential_paths)
    if 'linux' in str(sys.platform):
        find_lib_path(env, 'blas', blas_libs , potential_paths)

def get_dependencies_for_module(module):
    module_depends = list()
    elif module == "cobalt_client":
        module_depends.append('cobalt_api')
    elif module == "cobalt_api":
        module_depends.append('recognizer')
    elif module == "recognizer":
        module_depends.append('model')
    elif module == 'model':
        module_depends.append('decoder')
        module_depends.append('results')
    elif module == 'results':
        module_depends.append('config')
    elif module == "decoder":
        module_depends.append('config')
    elif module == 'config':
        module_depends.append('util')
    elif module == 'util':
         module_depends.append('boost')
         module_depends.append('jsoncpp')
         module_depends.append('kaldi')
    elif module == 'kaldi':
        if 'linux' in str(sys.platform):
            module_depends.append('blas')
    elif module == 'boost':
        pass
    elif module =='jsoncpp':
        pass
    elif module == 'blas':
        pass
    else:
        raise Exception('unknown module (directory) name '+ module)
    return module_depends

def get_all_modules(module_name):
    all_modules = list()
    module_queue = [module_name,]
    while len(module_queue) > 0:
        # popping the first element makes this BFS
        current_module = module_queue.pop(0)
        if current_module not in all_modules:
            # it is ok for a module to be included twice, some base modules will be included a few times.
            all_modules.append(current_module)
            module_depends = get_dependencies_for_module(current_module)
            module_queue.extend(module_depends)
    return all_modules

def get_all_libs(module_name, all_libs):
    libs_for_module = global_depends_dict[module_name]
    for lib in libs_for_module:
        if lib not in all_libs:
            all_libs.append(lib)

def add_compile_flags(cpp_flags, build_type, with_valgrind):
    # add some flags for kaldi to compile
    if build_type == 'debug':
        cpp_flags.append('-g')
    elif build_type == 'release':
        cpp_flags.append('-O2')
    else:
        raise Exception('Unknown build type' + build_type)

    cpp_flags.append('-DHAVE_ATLAS')
    cpp_flags.append('-flax-vector-conversions')
    cpp_flags.append('-DHAVE_POSIX_MEMALIGN')
    if with_valgrind:
        cpp_flags.append('-DVALGRIND')
    if 'linux' in sys.platform:
        cpp_flags.append('-fPIC')

def add_blas_links(cpp_flags):
    if sys.platform == 'darwin':
        cpp_flags.append('-framework Accelerate')

def check_kaldi_depends():
    # TODO, check the kaldi runtime libs that we need.
    pass

def setup_include_paths(env):
    env['INCLUDE_PATHS'] = list()
    external_inc_paths_single = env['EXTERNAL_INC_DIRS']
    if external_inc_paths_single != '':
        external_libs_paths = external_inc_paths_single.split(':')
        env['INCLUDE_PATHS'].append(external_libs_paths)
    else:
        import sys
        if sys.platform == 'darwin':
            env['INCLUDE_PATHS'].append('/opt/local/include')
        else:
            env['INCLUDE_PATHS'].append('/usr/local/include')

def generate(env):
    setup_include_paths(env)
    add_external_libs(env)
    # compiles c++ library, returns static and shared libraries
    # runs log-ref tests
    def CompileToLibraries(cpp_files, module_name, include_dirs, cpp_flags=list(), include_all_dependent_libs=False):
        libenv = env.Clone()
        include_dirs.extend(env['INCLUDE_PATHS'])
        cpp_file_and_libs = [cpp_files]

        if include_all_dependent_libs:
            modules = get_all_modules(module_name)
            # remove the current module.
            modules.remove(module_name)
            for module in modules:
                get_all_libs(module, cpp_file_and_libs)

        add_compile_flags(cpp_flags, env['BUILD_TYPE'], env['VALGRIND'])
        static_lib_node = libenv.StaticLibrary(module_name, cpp_file_and_libs, CPPPATH=include_dirs, CPPFLAGS=cpp_flags)
        global_depends_dict[module_name] = static_lib_node
        return static_lib_node
    env.CompileToLibraries = CompileToLibraries

    def CompileToSharedLibraries(cpp_files, module_name, include_dirs, cpp_flags=list()):
        libenv = env.Clone()
        modules = get_all_modules(module_name)
        cpp_file_and_libs = [cpp_files]
        include_dirs.extend(env['INCLUDE_PATHS'])

        for module in modules:
            get_all_libs(module, cpp_file_and_libs)

        add_compile_flags(cpp_flags, env['BUILD_TYPE'], env['VALGRIND'])
        if 'linux' in sys.platform:
            libenv.Append(LINKFLAGS=['-fPIC'])
        if sys.platform == 'darwin':
            libenv.Append(LINKFLAGS=['-framework', 'Accelerate'])
        libs=['pthread','dl']

        shared_lib_node = libenv.SharedLibrary(module_name, cpp_file_and_libs, CPPPATH=include_dirs, CPPFLAGS=cpp_flags)
        return shared_lib_node
    env.CompileToSharedLibraries = CompileToSharedLibraries

    def BuildExecutable(executable_name, cpp_file, include_dirs, module_name, cpp_flags):
        envExe = env.Clone()
        modules = get_all_modules(module_name)
        cpp_file_and_libs = [cpp_file]
        include_dirs.extend(env['INCLUDE_PATHS'])

        for module in modules:
            get_all_libs(module, cpp_file_and_libs)
            #cpp_file_and_libs.extend(global_depends_dict[module])
        add_compile_flags(cpp_flags, env['BUILD_TYPE'], env['VALGRIND'])
        if sys.platform == 'darwin':
            envExe.Append(LINKFLAGS=['-framework', 'Accelerate'])
        #if 'linux' in str(sys.platform):
        libs=['pthread','dl']
        exe = envExe.Program(executable_name, cpp_file_and_libs,
                   CPPPATH=include_dirs,
                   LIBS=libs,
                   CPPFLAGS=cpp_flags)

        for module in modules:
            env.Depends(exe, global_depends_dict[module])
        return exe
    env.BuildExecutable = BuildExecutable

    # installs all this lib and all dependent libs.
    def InstallAllLibs(module_name):
        modules = get_all_modules(module_name)
        libs = list()

        for module in modules:
            get_all_libs(module, libs)
        for lib in libs:
            env.Install(os.path.join(env.Dir('#').abspath, env['BUILD_TOP'], 'lib'), lib)
    env.InstallAllLibs = InstallAllLibs

    def run_exe(source, target, env, for_signature):
        args = env['test_args']
        one_arg = ''
        if args: one_arg = ' '.join(args)
        runarg = '%s %s > %s' % (source[0], one_arg, target[0])
        return '%s %s > %s' % (source[0], one_arg, target[0])

    # builder that runs with the executable with arguments.
    env.Append(BUILDERS = {'RunExe': env.Builder(generator = run_exe,)
                })

    def RunLogrefTest(test_exe, test_prefix, test_args=None):
        if env['NO_TESTS']:
            return
        # run the test to generate a trial log file, then verify this file
        # against the reference; note: use 0 index on SOURCES to ensure we are using
        # just the executable item in test_exe
        ref_file_name = test_prefix + '.log-ref'
        trial_name = test_prefix + '.log-trial'
        trial_name_norm = test_prefix + '.log-trail-norm'
        diff_file_name = test_prefix + '.log-diff'
        logref_env = env.Clone()
        logref_env['test_args'] = test_args

        logref_env.RunExe(trial_name, test_exe)
        logref_env.LogRefNorm(trial_name_norm, trial_name)
        logref_env.LogRefDiff(diff_file_name, [ref_file_name, trial_name_norm])
        #env.Command(trial_name, test_exe, '$SOURCE > $TARGET')
        #logref_env.Diff(diff_file_name, [ref_file_name, trial_name_norm])
    env.RunLogrefTest = RunLogrefTest

def exists(env):
    """
    Make sure that CppTools has the resources it needs
    """
    return True
