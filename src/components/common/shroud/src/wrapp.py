#!/bin/env python3
"""
Generate Python module for C++ code.

Entire library in a single header.
One Extension module per class

"""
from __future__ import print_function

import util
from util import wformat, append_format

class Wrapp(util.WrapperMixin):
    """Generate Python bindings.
    """

    def __init__(self, tree, config, splicers):
        self.tree = tree    # json tree
        self.patterns = tree['patterns']
        self.config = config
        self.log = config.log
        self.typedef = tree['types']
        self._init_splicer(splicers)
        self.comment = '//'

    def _begin_output_file(self):
        """Start a new class for output"""
        self.f_type_decl = []
        self.c_interface = []
        self.impl = []         # implementation, after contains
        self.c_interface.append('interface')
        self.c_interface.append(1)

    def _end_output_file(self):
        self.c_interface.append(-1)
        self.c_interface.append('end interface')

    def _begin_class(self):
        pass

    def reset_file(self):
        self.PyMethodBody = []
        self.PyMethodDef = []

    def wrap_library(self):
        options = self.tree['options']
        fmt_library = self.tree['fmt']

        fmt_library.PY_prefix          = options.get('PY_prefix', 'PY_')
        fmt_library.PY_module_name     = fmt_library.lower_library
        util.eval_template(options, fmt_library, 'PY_module_filename', 'py{library}module.cpp')
        util.eval_template(options, fmt_library, 'PY_header_filename', 'py{library}module.hpp')
        util.eval_template(options, fmt_library, 'PY_helper_filename', 'py{library}helper.cpp')
        fmt_library.BBB = 'BBB'   # name of cpp class pointer in PyObject
        self.py_type_object_creation = []
        self.py_type_extern = []
        self.py_type_structs = []
        self.py_helper_definition = []
        self.py_helper_declaration = []
        self.py_helper_prototypes = []
        self.py_helper_functions = []

        # preprocess all classes first to allow them to reference each other
        for node in self.tree['classes']:
            typedef = self.typedef[node['name']]
            fmt = node['fmt']
            typedef.PY_format = 'O'

            # PyTypeObject for class
            util.eval_template(options, fmt,
                               'PY_PyTypeObject', '{PY_prefix}{cpp_class}_Type')
            typedef.PY_PyTypeObject = fmt.PY_PyTypeObject

            # PyObject for class
            util.eval_template(options, fmt,
                               'PY_PyObject', '{PY_prefix}{cpp_class}')
            typedef.PY_PyObject = fmt.PY_PyObject

            fmt.PY_to_object_func = typedef.PY_to_object = wformat('PP_{cpp_class}_to_Object', fmt)
            fmt.PY_from_object_func = typedef.PY_from_object = wformat('PP_{cpp_class}_from_Object', fmt)

        self._push_splicer('class')
        for node in self.tree['classes']:
            name = node['name']
            self.reset_file()
            self._push_splicer(name)
            self.wrap_class(node)
            self.write_extension_type(node)
            self._pop_splicer(name)
        self._pop_splicer('class')

        self.reset_file()
        if self.tree['functions']:
            self._push_splicer('function')
            self._begin_class()
            for node in self.tree['functions']:
                self.wrap_method(None, node)
            self._pop_splicer('function')

        self.write_header(self.tree)
        self.write_module(self.tree)
        self.write_helper()

    def wrap_class(self, node):
        self.log.write("class {1[name]}\n".format(self, node))
        name = node['name']
        unname = util.un_camel(name)
        typedef = self.typedef[name]

        options = node['options']
        fmt_class = node['fmt']

        util.eval_template(options, fmt_class,
                           'PY_type_filename', 'py{cpp_class}type.cpp')

        self.create_class_helper_functions(node)

        self.py_type_object_creation.append(wformat("""
// {cpp_class}
    {PY_PyTypeObject}.tp_new   = PyType_GenericNew;
    {PY_PyTypeObject}.tp_alloc = PyType_GenericAlloc;
    if (PyType_Ready(&{PY_PyTypeObject}) < 0)
        return RETVAL;
    Py_INCREF(&{PY_PyTypeObject});
    PyModule_AddObject(m, "{cpp_class}", (PyObject *)&{PY_PyTypeObject});
""", fmt_class))
        self.py_type_extern.append(wformat('extern PyTypeObject {PY_PyTypeObject};', fmt_class))


        self._create_splicer('C_declaration', self.py_type_structs)
        self.py_type_structs.append('')
        self.py_type_structs.append('typedef struct {')
        self.py_type_structs.append('PyObject_HEAD')
        self.py_type_structs.append(1)
        append_format(self.py_type_structs, '{cpp_class} * {BBB};', fmt_class)
        self._create_splicer('C_object', self.py_type_structs)
        self.py_type_structs.append(-1)
        self.py_type_structs.append(wformat('}} {PY_PyObject};', fmt_class))

        # wrap methods
        self._push_splicer('method')
        for method in node['methods']:
            self.wrap_method(node, method)
        self._pop_splicer('method')

    def create_class_helper_functions(self, node):
        """Create some helper functions to and from a PyObject.
        These functions are used by PyArg_ParseTupleAndKeywords and Py_BuildValue
        node is a C++ class.
        """
        fmt = node['fmt']
        
        fmt.PY_capsule_name = wformat('PY_{cpp_class}_capsule_name', fmt)

        self._push_splicer('helper')
        append_format(self.py_helper_definition, 'const char *{PY_capsule_name} = "{cpp_class}";', fmt)
        append_format(self.py_helper_declaration, 'extern const char *{PY_capsule_name};', fmt)

        # To
        to_object = wformat("""PyObject *voidobj;
PyObject *args;
PyObject *rv;

voidobj = PyCapsule_New(addr, {PY_capsule_name}, NULL);
args = PyTuple_New(1);
PyTuple_SET_ITEM(args, 0, voidobj);
rv = PyObject_Call((PyObject *) &{PY_PyTypeObject}, args, NULL);
Py_DECREF(args);
return rv;""", fmt)
        to_object = to_object.split('\n')
        

        proto = wformat('PyObject *{PY_to_object_func}({cpp_class} *addr)', fmt)
        self.py_helper_prototypes.append(proto + ';')

        self.py_helper_functions.append('')
        self.py_helper_functions.append(proto)
        self.py_helper_functions.append('{')
        self.py_helper_functions.append(1)
        self._create_splicer('to_object', self.py_helper_functions, default=to_object)
        self.py_helper_functions.append(-1)
        self.py_helper_functions.append('}')

        # From
        from_object = wformat("""if (obj->ob_type != &{PY_PyTypeObject}) {{
    // raise exception
    return 0;	
}}
{PY_PyObject} * self = ({PY_PyObject} *) obj;
*addr = self->{BBB};
return 1;""", fmt)
        from_object = from_object.split('\n')

        proto = wformat('int {PY_from_object_func}(PyObject *obj, void **addr)', fmt)
        self.py_helper_prototypes.append(proto + ';')

        self.py_helper_functions.append('')
        self.py_helper_functions.append(proto)
        self.py_helper_functions.append('{')
        self.py_helper_functions.append(1)
        self._create_splicer('from_object', self.py_helper_functions, default=from_object)
        self.py_helper_functions.append(-1)
        self.py_helper_functions.append('}')

        self._pop_splicer('helper')

    def wrap_method(self, cls, node):
        """
        cls  - class node or None for functions
        node - function/method node
        """
        options = node['options']
        if not options.wrap_python:
            return

        if cls:
            cls_function = 'method'
        else:
            cls_function = 'function'
        if 'decl' in node:
            self.log.write("Python {0} {1[decl]}\n".format(cls_function, node))
        else:
            self.log.write("Python {0} {1[result][name]}\n".format(cls_function, node))

        fmt_func = node['fmt']
        fmt = util.Options(fmt_func)
        fmt.doc_string = 'documentation'

        result = node['result']
        result_type = result['type']
        result_is_ptr = result['attrs'].get('ptr', False)
        result_is_ref = result['attrs'].get('reference', False)

        if node.get('return_this', False):
            result_type = 'void'
            result_is_ptr = False

        result_typedef = self.typedef[result_type]
        is_ctor  = result['attrs'].get('constructor', False)
        is_dtor  = result['attrs'].get('destructor', False)
#        is_const = result['attrs'].get('const', False)
        if is_ctor:   # or is_dtor:
            # XXX - have explicit delete
            # need code in __init__ and __del__
            return

        # XXX if a class, then knock off const since the PyObject
        # is not const, otherwise, use const from result.
        if result_typedef.base == 'wrapped':
            is_const = False
        else:
            is_const = None
        fmt.rv_decl = self.std_c_decl('cpp_type', result, name='rv', const=is_const)  # return value

        if cls:
            util.eval_template(options, fmt,
                               'PY_name_impl', '{PY_prefix}{lower_class}_{underscore_name}{method_suffix}')
        else:
            util.eval_template(options, fmt,
                               'PY_name_impl', '{PY_prefix}{underscore_name}{method_suffix}')

        PY_decl = []     # variables for function
        PY_code = []
        format = []      # for PyArg_ParseTupleAndKeywords
        addrargs = []    # for PyArg_ParseTupleAndKeywords
        cpp_call_list = []

        # parse arguments
        optional = []
        post_parse = []
        args = node.get('args', [])
        if not args:
            fmt.ml_flags = 'METH_NOARGS'
        else:
            fmt.ml_flags = 'METH_VARARGS|METH_KEYWORDS'
            arg_names = []
            arg_offsets  = []
            offset = 0
            for arg in node.get('args', []):
                arg_name = arg['name']
                fmt.var = arg_name
                arg_names.append(arg_name)
                arg_offsets.append( '(char *) kwcpp+%d' % offset)
                offset += len(arg_name) + 1
                arg_typedef = self.typedef[arg['type']]

                attrs = arg['attrs']
                if 'default' in attrs:
                    fmt.default_value = attrs['default']
                    if not optional:
                        format.append('|')  # add once
                    append_format(optional, '{var} = {default_value};', fmt)

                format.append(arg_typedef.PY_format)
                if arg_typedef.PY_PyTypeObject:
                    # Expect object of given type
                    format.append('!')
                    addrargs.append('&' + arg_typedef.PY_PyTypeObject)
                elif arg_typedef.PY_from_object:
                    # Use function to convert object
                    format.append('&')
                    addrargs.append(arg_typedef.PY_from_object)
                addrargs.append('&' + arg_name)


                # argument for C++ function
                if arg_typedef.PY_PyTypeObject:
                    # A Python Object
                    fmt.var_ptr = fmt.var + '_ptr'
                    PY_decl.append(arg_typedef.PY_PyObject + ' * ' + arg_name + ';')
                    PY_decl.append(self.std_c_decl('cpp_type', arg, name=fmt.var_ptr) + ';')
                    append_format(post_parse, '{var_ptr} = ({var} ? {var}->{BBB} : NULL);', fmt)
                    cpp_call_list.append(fmt.var_ptr)
                elif arg_typedef.PY_from_object:
                    # already a C++ type
                    PY_decl.append(self.std_c_decl('cpp_type', arg) + ';')
                    cpp_call_list.append(fmt.var)
                else:
                    # convert to C++ type
                    PY_decl.append(self.std_c_decl('c_type', arg) + ';')
                    fmt.ptr=' *' if arg['attrs'].get('ptr', False) else ''
                    append_format(cpp_call_list, arg_typedef.c_to_cpp, fmt)


            if True:
                # jump through some hoops for char ** const correctness for C++
                # warning: deprecated conversion from string constant to 'char*' [-Wwrite-strings]
                PY_decl.append('const char *kwcpp = "%s";' % '\\0'.join(arg_names))
                PY_decl.append('char *kw_list[] = { ' + ','.join(arg_offsets) + ', NULL };')
            else:
                PY_decl.append('char * kw_list[] = { "' + '", "'.join(arg_names) + ', NULL" };')
            PY_decl.append('')
            format.extend([ ':', fmt.method_name])
            fmt.PyArg_format = ''.join(format)
            fmt.PyArg_addrargs = ', '.join(addrargs)
            PY_code.extend(optional)
            PY_code.append(wformat('if (!PyArg_ParseTupleAndKeywords(args, kwds, "{PyArg_format}", kw_list,', fmt))
            PY_code.append(1)
            PY_code.append(wformat('{PyArg_addrargs}))', fmt))
            PY_code.append(-1)
            PY_code.extend(['{', 1, 'return NULL;', -1, '}'])
            PY_code.extend(post_parse)
            
        fmt.call_list = ', '.join(cpp_call_list)

        if cls:
#                    template = '{C_const}{cpp_class} *{C_this}obj = static_cast<{C_const}{cpp_class} *>({C_this});'
#                fmt_func.C_object = wformat(template, fmt_func)
            fmt.PY_this_call = wformat('self->{BBB}->', fmt)  # call method syntax
        else:
            fmt.PY_this_call = ''  # call function syntax


        if is_dtor:
            append_format(PY_code, 'delete self->{BBB};', fmt)
            append_format(PY_code, 'self->{BBB} = NULL;', fmt)
        elif result_type == 'void' and not result_is_ptr:
            line = wformat('{PY_this_call}{CPP_name}({call_list});', fmt)
            PY_code.append(line)
        else:
            line = wformat('{rv_decl} = {PY_this_call}{CPP_name}({call_list});', fmt)
            PY_code.append(line)

        if 'PY_error_pattern' in node:
            lfmt = util.Options(fmt)
            lfmt.var = fmt.rv
            append_format(PY_code, self.patterns[node['PY_error_pattern']], lfmt)

        # return Object
        if result_type == 'void' and not result_is_ptr:
            PY_code.append('Py_RETURN_NONE;')
        elif result_typedef.base == 'wrapped':
            lfmt = util.Options(fmt)
            lfmt.rv_obj = 'rv_obj'
            lfmt.PY_PyObject = result_typedef.PY_PyObject
            lfmt.PY_PyTypeObject = result_typedef.PY_PyTypeObject
            append_format(PY_code, '{PY_PyObject} * {rv_obj} = PyObject_New({PY_PyObject}, &{PY_PyTypeObject});', lfmt)
            append_format(PY_code, '{rv_obj}->{BBB} = {rv};', lfmt)
            append_format(PY_code, 'return (PyObject *) {rv_obj};', lfmt)
        elif result_typedef.PY_ctor:
            fmt.var = fmt.rv
            fmt.var = wformat(result_typedef.cpp_to_c, fmt)  # if C++
            append_format(PY_code, 'return ' + result_typedef.PY_ctor + ';', fmt)
        else:
            fmt.var = 'rv'
            format = [ result_typedef.PY_format ]
            addrargs = [ ]
            if result_typedef.PY_to_object:
                format.append('&')
                addrargs.append(result_typedef.PY_to_object)
            if result_is_ptr:
                append_format(addrargs, result_typedef.cpp_to_c, fmt)  # if C++
            elif result_is_ref:
                append_format(addrargs, result_typedef.cpp_to_c, fmt)  # if C++
            else:
                # XXX intermediate variable?
                addrargs.append('rv')
            fmt.PyArg_format = ''.join(format)
            fmt.PyArg_addrargs = ', '.join(addrargs)
            PY_code.append(wformat('return Py_BuildValue("{PyArg_format}", {PyArg_addrargs});', fmt))

        PY_impl = [1] + PY_decl + PY_code + [-1]

        body = self.PyMethodBody
        body.append(wformat("""
static char {PY_name_impl}__doc__[] =
"{doc_string}"
;

static PyObject *
{PY_name_impl}(""", fmt))
        if cls:
            body.append(wformat('  {PY_PyObject} *self,', fmt))
        else:
            body.append('  PyObject *self,    /* not used */')
        body.append('  PyObject *args,')
        body.append('  PyObject *kwds)')
        body.append('{')
        self._create_splicer(fmt.CPP_name, self.PyMethodBody, default=PY_impl)
        self.PyMethodBody.append('}')

        self.PyMethodDef.append( wformat('{{"{CPP_name}{method_suffix}", (PyCFunction){PY_name_impl}, {ml_flags}, {PY_name_impl}__doc__}},', fmt))

    def write_tp_func(self, node, fmt, fmt_type, output):
        # fmt is a dictiony here.
        # update with type function names
        # type bodies must be filled in by user, no real way to guess
        PyObj  = fmt.PY_PyObject
        selected = node.get('python', {}).get('type', [])

        # Dictionary of methods for bodies
        default_body = dict(
            richcompare = self.not_implemented
            )

        self._push_splicer('type')
        for typename in typenames:
            tp_name = 'tp_' + typename
            if typename not in selected:
                fmt_type[tp_name] = '0'
                continue
            func_name = wformat('{PY_prefix}{cpp_class}_tp_', fmt) + typename
            fmt_type[tp_name] = func_name
            tup = typefuncs[typename]
            output.append('static ' + tup[0])
            output.append(('{name} ' + tup[1]).format(name=func_name, object=PyObj))
            output.append('{')
            default = default_body.get(typename, self.not_implemented_error)
            default = default(typename, tup[2])
            self._create_splicer(typename, output, default=default)
            output.append('}')
        self._pop_splicer('type')

    def write_extension_type(self, node):
        fmt = node['fmt']
        fname = fmt.PY_type_filename

        output = []

        output.append(wformat('#include "{PY_header_filename}"', fmt))
        self._push_splicer('impl')
        self._create_splicer('include', output)
        self.namespace(node, 'begin', output)
        self._create_splicer('C_definition', output)
        self._create_splicer('additional_methods', output)
        self._pop_splicer('impl')

        fmt_type = dict(
            PY_module_name  = fmt.PY_module_name,
            PY_PyObject     = fmt.PY_PyObject,
            PY_PyTypeObject = fmt.PY_PyTypeObject,
            cpp_class       = fmt.cpp_class,
            )
        self.write_tp_func(node, fmt, fmt_type, output)

        output.extend(self.PyMethodBody)

        self._push_splicer('impl')
        self._create_splicer('after_methods', output)
        self._pop_splicer('impl')

        fmt_type['tp_methods'] = wformat('{PY_prefix}{cpp_class}_methods', fmt)
        output.append(wformat('static PyMethodDef {tp_methods}[] = {{', fmt_type))
        output.extend(self.PyMethodDef)
        self._create_splicer('PyMethodDef', output)
        output.append('{NULL,   (PyCFunction)NULL, 0, NULL}            /* sentinel */')
        output.append('};')

        output.append(wformat(PyTypeObject_template, fmt_type))
        self.namespace(node, 'end', output)

        self.write_output_file(fname, self.config.binary_dir, output)

    def write_header(self, node):
        options = node['options']
        fmt = node['fmt']
        fname = fmt.PY_header_filename

        output = []

        output.append("""
/*
 * This is generated code.
 * Any edits must be made between the splicer.begin and splicer.end blocks.
 * All other edits will be lost.
 * Once a block is edited remove the 'UNMODIFIED' on the splicer.begin
 * comment to allow the block to be preserved when it is regenerated.
 */

#ifndef HDR_BASISMODULE
#define HDR_BASISMODULE
#include <Python.h>
#if PY_MAJOR_VERSION >= 3
#define IS_PY3K
#endif""")
        
        self._push_splicer('header')
        self._create_splicer('include', output)
        self.namespace(node, 'begin', output)
        output.extend(self.py_type_extern)
        self._create_splicer('C_declaration', output)
        self._pop_splicer('header')

        output.append('')
        output.append('// helper functions')
        output.extend(self.py_helper_declaration)
        output.extend(self.py_helper_prototypes)

        output.append('')
        output.extend(self.py_type_structs)
        output.append(wformat("""
extern PyObject *{PY_prefix}error_obj;

#ifdef __cplusplus
extern "C" {{
#endif
#ifdef IS_PY3K
#define MOD_INITBASIS PyInit_{PY_module_name}
#else
#define MOD_INITBASIS init{PY_module_name}
#endif
PyMODINIT_FUNC MOD_INITBASIS(void);
#endif
#ifdef __cplusplus
}}
#endif
""", fmt))
        self.namespace(node, 'end', output)
        self.write_output_file(fname, self.config.binary_dir, output)

    def write_module(self, node):
        options = node['options']
        fmt = node['fmt']
        fname = fmt.PY_module_filename

        fmt.PY_library_doc = 'library documentation'

        output = []

        output.append(wformat('#include "{PY_header_filename}"', fmt))
        self._create_splicer('include', output)
        output.append('')
        self.namespace(node, 'begin', output)
        self._create_splicer('C_definition', output)

        output.append(wformat('PyObject *{PY_prefix}error_obj;', fmt))

        self._create_splicer('additional_functions', output)
        output.extend(self.PyMethodBody)

        output.append(wformat('static PyMethodDef {PY_prefix}methods[] = {{', fmt))
        output.extend(self.PyMethodDef)
        output.append('{NULL,   (PyCFunction)NULL, 0, NULL}            /* sentinel */')
        output.append('};')

        output.append(wformat(module_begin, fmt))
        self._create_splicer('C_init_locals', output)
        output.append(wformat(module_middle, fmt))
        output.extend(self.py_type_object_creation)
        output.append(wformat(module_middle2, fmt))
        self._create_splicer('C_init_body', output)
        output.append(wformat(module_end, fmt))
        self.namespace(node, 'end', output)

        self.write_output_file(fname, self.config.binary_dir, output)

    def write_helper(self):
        node = self.tree
        fmt = node['fmt']
        output = []
        output.append(wformat('#include "{PY_header_filename}"', fmt))
        self.namespace(node, 'begin', output)
        output.extend(self.py_helper_definition)
        output.append('')
        output.extend(self.py_helper_functions)
        self.namespace(node, 'end', output)
        self.write_output_file(fmt.PY_helper_filename, self.config.binary_dir, output)

    def not_implemented_error(self, msg, ret):
        '''A standard splicer for unimplemented code
        ret is the return value (NULL or -1)
        '''
        return [
            "    PyErr_SetString(PyExc_NotImplementedError, \"%s\");" % msg,
            "    return %s;" % ret
            ]

    def not_implemented(self, msg, ret):
        '''A standard splicer for rich comparison
        '''
        return [
            'Py_INCREF(Py_NotImplemented);',
            'return Py_NotImplemented;'
            ]




#### Python boiler plate

typenames = ['dealloc', 'print', 'compare',
             'getattr', 'setattr',  # deprecated
             'getattro', 'setattro',
             'repr', 'hash', 'call', 'str',
             'init', 'alloc', 'new', 'free', 'del',
             'richcompare']

# return type, prototype, default return value
typefuncs = {
    'dealloc':  ('void',       '({object} *self)',                                  ''),
    'print':    ('int',        '({object} *self, FILE *fp, int flags)',             '-1'),
    'getattr':  ('PyObject *', '({object} *self, char *name)',                      'NULL'),
    'setattr':  ('int',        '({object} *self, char *name, PyObject *value)',     '-1'),
    'compare':  ('int',        '({object} *self, PyObject *)',                      '-1'),
    'repr':     ('PyObject *', '({object} *self)',                                  'NULL'),
    'hash':     ('long',       '({object} *self)',                                  '-1'),
    'call':     ('PyObject *', '({object} *self, PyObject *args, PyObject *kwds)',  'NULL'),
    'str':      ('PyObject *', '({object} *self)',                                  'NULL'),
    'getattro': ('PyObject *', '({object} *self, PyObject *name)',                  'NULL'),
    'setattro': ('int',        '({object} *self, PyObject *name, PyObject *value)', '-1'),
    'init':     ('int',        '({object} *self, PyObject *args, PyObject *kwds)',  '-1'),
    'alloc':    ('PyObject *', '(PyTypeObject *type, Py_ssize_t nitems)',                 'NULL'),
    'new':      ('PyObject *', '(PyTypeObject *type, PyObject *args, PyObject *kwds)',    'NULL'),
    'free':     ('void',       '(void *op)',                                        ''),
    'del':      ('void',       '({object} *self)',                                  ''),
    'richcompare': ('PyObject *', '({object} *self, PyObject *other, int opid)',      ''),
    }

PyTypeObject_template = """
static char {cpp_class}__doc__[] =
"virtual class"
;

/* static */
PyTypeObject {PY_PyTypeObject} = {{
        PyVarObject_HEAD_INIT(NULL, 0)
        "{PY_module_name}.{cpp_class}",                       /* tp_name */
        sizeof({PY_PyObject}),         /* tp_basicsize */
        0,                              /* tp_itemsize */
        /* Methods to implement standard operations */
        (destructor){tp_dealloc},                 /* tp_dealloc */
        (printfunc){tp_print},                   /* tp_print */
        (getattrfunc){tp_getattr},                 /* tp_getattr */
        (setattrfunc){tp_setattr},                 /* tp_setattr */
#ifdef IS_PY3K
        0,                               /* tp_reserved */
#else
        (cmpfunc){tp_compare},                     /* tp_compare */
#endif
        (reprfunc){tp_repr},                    /* tp_repr */
        /* Method suites for standard classes */
        0,                              /* tp_as_number */
        0,                              /* tp_as_sequence */
        0,                              /* tp_as_mapping */
        /* More standard operations (here for binary compatibility) */
        (hashfunc){tp_hash},                    /* tp_hash */
        (ternaryfunc){tp_call},                 /* tp_call */
        (reprfunc){tp_str},                    /* tp_str */
        (getattrofunc){tp_getattro},                /* tp_getattro */
        (setattrofunc){tp_setattro},                /* tp_setattro */
        /* Functions to access object as input/output buffer */
        0,                              /* tp_as_buffer */
        /* Flags to define presence of optional/expanded features */
        Py_TPFLAGS_DEFAULT,             /* tp_flags */
        {cpp_class}__doc__,         /* tp_doc */
        /* Assigned meaning in release 2.0 */
        /* call function for all accessible objects */
        (traverseproc)0,                /* tp_traverse */
        /* delete references to contained objects */
        (inquiry)0,                     /* tp_clear */
        /* Assigned meaning in release 2.1 */
        /* rich comparisons */
        (richcmpfunc){tp_richcompare},                 /* tp_richcompare */
        /* weak reference enabler */
        0,                              /* tp_weaklistoffset */
        /* Added in release 2.2 */
        /* Iterators */
        (getiterfunc)0,                 /* tp_iter */
        (iternextfunc)0,                /* tp_iternext */
        /* Attribute descriptor and subclassing stuff */
        {tp_methods},                             /* tp_methods */
        0,                              /* tp_members */
        0,                             /* tp_getset */
        0,                              /* tp_base */
        0,                              /* tp_dict */
        (descrgetfunc)0,                /* tp_descr_get */
        (descrsetfunc)0,                /* tp_descr_set */
        0,                              /* tp_dictoffset */
        (initproc){tp_init},                   /* tp_init */
        (allocfunc){tp_alloc},                  /* tp_alloc */
        (newfunc){tp_new},                    /* tp_new */
        (freefunc){tp_free},                   /* tp_free */
        (inquiry)0,                     /* tp_is_gc */
        0,                              /* tp_bases */
        0,                              /* tp_mro */
        0,                              /* tp_cache */
        0,                              /* tp_subclasses */
        0,                              /* tp_weaklist */
        (destructor){tp_del},                 /* tp_del */
        0,                              /* tp_version_tag */
#ifdef IS_PY3K
        (destructor)0,                  /* tp_finalize */
#endif
}};
"""


module_begin = """
/*
 * init{lower_library} - Initialization function for the module
 * *must* be called init{lower_library}
 */
static char {PY_prefix}_doc__[] =
"{PY_library_doc}"
;

struct module_state {{
    PyObject *error;
}};

#ifdef IS_PY3K
#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct module_state _state;
#endif

#ifdef IS_PY3K
static int {lower_library}_traverse(PyObject *m, visitproc visit, void *arg) {{
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}}

static int {lower_library}_clear(PyObject *m) {{
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}}

static struct PyModuleDef moduledef = {{
    PyModuleDef_HEAD_INIT,
    "{lower_library}", /* m_name */
    {PY_prefix}_doc__, /* m_doc */
    sizeof(struct module_state), /* m_size */
    {PY_prefix}methods, /* m_methods */
    NULL, /* m_reload */
    {lower_library}_traverse, /* m_traverse */
    {lower_library}_clear, /* m_clear */
    NULL  /* m_free */
}};

#define RETVAL m
#define INITERROR return NULL
#else
#define RETVAL
#define INITERROR return
#endif

#ifdef __cplusplus
extern "C" {{
#endif
PyMODINIT_FUNC
MOD_INITBASIS(void)
{{
    PyObject *m = NULL;
    const char * error_name = "{lower_library}.Error";
"""

module_middle = """

    /* Create the module and add the functions */
#ifdef IS_PY3K
    m = PyModule_Create(&moduledef);
#else
    m = Py_InitModule4("{PY_module_name}", {PY_prefix}methods,
                       {PY_prefix}_doc__,
                       (PyObject*)NULL,PYTHON_API_VERSION);
#endif
    if (m == NULL)
        return RETVAL;
    struct module_state *st = GETSTATE(m);
"""

module_middle2 = """
    {PY_prefix}error_obj = PyErr_NewException((char *) error_name, NULL, NULL);
    if ({PY_prefix}error_obj == NULL)
        return RETVAL;
    st->error = {PY_prefix}error_obj;
    PyModule_AddObject(m, "Error", st->error);
"""

module_end = """
    /* Check for errors */
    if (PyErr_Occurred())
        Py_FatalError("can't initialize module {PY_module_name}");
    return RETVAL;
}}
#ifdef __cplusplus
}}
#endif
"""
