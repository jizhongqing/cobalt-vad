import os, sys
import SCons
def generate(env):
    managedmodules = list()

    # Action function for compiling PYC files, these are done in typical SCons
    # fashion one at a time using the Python that's running SCons
    def pyc_compile_function(target, source, env):
        target, = target
        source, = source
        import py_compile
        py_compile.compile(source.path, target.path)

    pyo_source_files = env['PYO_SOURCE_FILES'] = list()
    def pyo_register_function(target, source, env):
        target, = target
        source, = source
        # an enabler of the hack: we use the env.Pyo builder's action to grow the
        # list of pyo_source_files with PYO filenames for out-of-date PYOs
        pyo_source_files.append(source.path)

    env.Append(BUILDERS = {
        'Pyc': env.Builder(
             src_suffix = 'py',
             single_source = True,
             suffix = 'pyc',
             ensure_suffix = True,

             # this compiles the PY file using the Python that's running SCons
             action = SCons.Action.Action(pyc_compile_function, 'py_compile_pyc( $SOURCE )'),
            ),
        'Pyo': env.Builder(
             src_suffix = 'py',
             single_source = True,

             suffix = 'pyo',
             ensure_suffix = True,

             # this action just collects the names of the PYOs
             action = SCons.Action.Action(pyo_register_function, 'register py_compile_pyo $SOURCE'),
            ),
        })
    pyo_compile = env.Command('pyo_compile', [],
                         'python' + " -S -O -m py_compile ${' '.join(PYO_SOURCE_FILES)}")

    def run_python(source, target, env, for_signature):
        args = env['test_args']
        one_arg = ''
        if args: one_arg = ' '.join(args)
        runarg = '%s %s > %s' % (source[0], one_arg, target[0])
        return 'python %s %s > %s' % (source[0], one_arg, target[0])
    # builder that runs with the executable with arguments.
    env.Append(BUILDERS = {'RunPython': env.Builder(generator = run_python,)
                })

    def RunPythonLogrefTest(python_file, test_prefix, test_args=None, dependencies=None):
        ref_file_name = test_prefix + '.log-ref'
        trial_name = test_prefix + '.log-trial'
        trial_name_norm = test_prefix + '.log-trail-norm'
        diff_file_name = test_prefix + '.log-diff'
        logref_env = env.Clone()
        logref_env['test_args'] = test_args

        runpython = logref_env.RunPython(trial_name, python_file)

        for dependency in dependencies:
            env.Depends(runpython, dependency)

        logref_env.LogRefNorm(trial_name_norm, trial_name)
        return logref_env.LogRefDiff(diff_file_name, [ref_file_name, trial_name_norm])

    def PyFile(pyfile, dirname, is_tool_file=False, tool_files=None,
               log_ref_name_and_args=None, depends=None):
        def build(pyfile):
            # explicit mention of the file to SCons
            pynode = env.File(pyfile)

            # global accumulation
            managedmodules.append(pyfile)
            # cause byte compiles to happen
            pycfile = env.Pyc(pyfile)
            pyofile = env.Pyo(pyfile)
            pycofiles = pycfile + pyofile

            from modulefinder import ModuleFinder
            scan_dir = os.path.join(str(env.Dir('#')), dirname)
            scan_file = os.path.join(scan_dir, pyfile)
            finder = ModuleFinder(scan_dir)
            finder.run_script(scan_file)
            dependencies = set()
            if depends:
                for item in depends:
                    dependencies.add(item)
            for value in finder.modules.values():
                filename = value.__file__
                if filename is not None and filename.lower().endswith('.py'):
                    #if filename.lower().startswith(onyx_home_l):
                    #  filename = filename[len(onyx_home_l):]
                    # returning filenames in the build tree causes the dependencies
                    # to get built; as of SCons 1.2.0, alternatives, like returning
                    # the filenames in the source tree, do not work; this feels like
                    # a very grey area of SCons, that is, running the scanner on
                    # targets of other builders rather than on sources, and returning
                    # '#' prefixed filenames
                    #dependency = os.path.join('#', onyx_build, filename)
                    dependency = os.path.join(str(env['COBALT_BUILD']), dirname, filename)
                    if os.path.exists(dependency):
                        dependencies.add(dependency)
                        env.Depends(pycofiles, dependency)
            for value in finder.badmodules.iterkeys():
                if not value.endswith('.py'):
                    filename = value + '.py'
                dependency = os.path.join(os.path.abspath(str(env.Dir('.'))), filename)
                if os.path.exists(dependency):
                    dependencies.add(dependency)
                    dependencies.add(dependency+'c')
                    env.Depends(pycofiles, dependency)
                    env.Depends(pycofiles, dependency+'c')
             # make sure targets get rebuilt if tools have changed
            if not is_tool_file:
                # tool_file is True for tools used by SCons in order to avoid
                # circular dependencies
                env.Depends(pycofiles, tool_files)

            if log_ref_name_and_args is not None:
                python_exe_path = os.path.join(os.path.abspath(str(env.Dir('.'))), pyfile+'c')
                log_test = RunPythonLogrefTest(python_exe_path, log_ref_name_and_args[0],
                                               log_ref_name_and_args[1], dependencies=dependencies)
                env.Depends(log_test, pycofiles)
                for item in dependencies:
                    env.Depends(log_test, item)

            # we make pyo_compile depend on all source files, so it doesn't execute
            # until all Pyo builders have executed (and their execution builds the
            # list of sources that need their PYOs built)
            env.Depends(pyo_compile, pynode)

            return pycofiles

        # compile the py{co} files
        pyc_node = build(pyfile)[0:1]

        # note: create a copy so that we can append to exposed_targets without
        # messing up pyc_node
        exposed_targets = pyc_node[:]
        return exposed_targets

    env.PyFile=PyFile

def exists(env):
    """
    Make sure that PyTools has the resources it needs
    """
    return True
