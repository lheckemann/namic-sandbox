#ifdef __CPLUSPLUS__
extern "C" {
#endif

#ifndef __GNUC__
#pragma warning(disable: 4275)
#pragma warning(disable: 4101)

#endif
#include "Python.h"
#include "compile.h"
#include "frameobject.h"
#include <complex>
#include <math.h>
#include <string>
#include "scxx/object.h"
#include "scxx/list.h"
#include "scxx/tuple.h"
#include "scxx/dict.h"
#include <iostream>
#include <stdio.h>
#include "numpy/arrayobject.h"




// global None value for use in functions.
namespace py {
object None = object(Py_None);
}

const char* find_type(PyObject* py_obj)
{
    if(py_obj == NULL) return "C NULL value";
    if(PyCallable_Check(py_obj)) return "callable";
    if(PyString_Check(py_obj)) return "string";
    if(PyInt_Check(py_obj)) return "int";
    if(PyFloat_Check(py_obj)) return "float";
    if(PyDict_Check(py_obj)) return "dict";
    if(PyList_Check(py_obj)) return "list";
    if(PyTuple_Check(py_obj)) return "tuple";
    if(PyFile_Check(py_obj)) return "file";
    if(PyModule_Check(py_obj)) return "module";

    //should probably do more intergation (and thinking) on these.
    if(PyCallable_Check(py_obj) && PyInstance_Check(py_obj)) return "callable";
    if(PyInstance_Check(py_obj)) return "instance";
    if(PyCallable_Check(py_obj)) return "callable";
    return "unkown type";
}

void throw_error(PyObject* exc, const char* msg)
{
 //printf("setting python error: %s\n",msg);
  PyErr_SetString(exc, msg);
  //printf("throwing error\n");
  throw 1;
}

void handle_bad_type(PyObject* py_obj, const char* good_type, const char* var_name)
{
    char msg[500];
    sprintf(msg,"received '%s' type instead of '%s' for variable '%s'",
            find_type(py_obj),good_type,var_name);
    throw_error(PyExc_TypeError,msg);
}

void handle_conversion_error(PyObject* py_obj, const char* good_type, const char* var_name)
{
    char msg[500];
    sprintf(msg,"Conversion Error:, received '%s' type instead of '%s' for variable '%s'",
            find_type(py_obj),good_type,var_name);
    throw_error(PyExc_TypeError,msg);
}


class int_handler
{
public:
    int convert_to_int(PyObject* py_obj, const char* name)
    {
        // Incref occurs even if conversion fails so that
        // the decref in cleanup_code has a matching incref.
        
        if (!py_obj || !PyInt_Check(py_obj))
            handle_conversion_error(py_obj,"int", name);
        return (int) PyInt_AsLong(py_obj);
    }

    int py_to_int(PyObject* py_obj, const char* name)
    {
        // !! Pretty sure INCREF should only be called on success since
        // !! py_to_xxx is used by the user -- not the code generator.
        if (!py_obj || !PyInt_Check(py_obj))
            handle_bad_type(py_obj,"int", name);
        
        return (int) PyInt_AsLong(py_obj);
    }
};

int_handler x__int_handler = int_handler();
#define convert_to_int(py_obj,name) \
        x__int_handler.convert_to_int(py_obj,name)
#define py_to_int(py_obj,name) \
        x__int_handler.py_to_int(py_obj,name)


PyObject* int_to_py(PyObject* obj)
{
    return (PyObject*) obj;
}


class float_handler
{
public:
    double convert_to_float(PyObject* py_obj, const char* name)
    {
        // Incref occurs even if conversion fails so that
        // the decref in cleanup_code has a matching incref.
        
        if (!py_obj || !PyFloat_Check(py_obj))
            handle_conversion_error(py_obj,"float", name);
        return PyFloat_AsDouble(py_obj);
    }

    double py_to_float(PyObject* py_obj, const char* name)
    {
        // !! Pretty sure INCREF should only be called on success since
        // !! py_to_xxx is used by the user -- not the code generator.
        if (!py_obj || !PyFloat_Check(py_obj))
            handle_bad_type(py_obj,"float", name);
        
        return PyFloat_AsDouble(py_obj);
    }
};

float_handler x__float_handler = float_handler();
#define convert_to_float(py_obj,name) \
        x__float_handler.convert_to_float(py_obj,name)
#define py_to_float(py_obj,name) \
        x__float_handler.py_to_float(py_obj,name)


PyObject* float_to_py(PyObject* obj)
{
    return (PyObject*) obj;
}


class complex_handler
{
public:
    std::complex<double> convert_to_complex(PyObject* py_obj, const char* name)
    {
        // Incref occurs even if conversion fails so that
        // the decref in cleanup_code has a matching incref.
        
        if (!py_obj || !PyComplex_Check(py_obj))
            handle_conversion_error(py_obj,"complex", name);
        return std::complex<double>(PyComplex_RealAsDouble(py_obj),PyComplex_ImagAsDouble(py_obj));
    }

    std::complex<double> py_to_complex(PyObject* py_obj, const char* name)
    {
        // !! Pretty sure INCREF should only be called on success since
        // !! py_to_xxx is used by the user -- not the code generator.
        if (!py_obj || !PyComplex_Check(py_obj))
            handle_bad_type(py_obj,"complex", name);
        
        return std::complex<double>(PyComplex_RealAsDouble(py_obj),PyComplex_ImagAsDouble(py_obj));
    }
};

complex_handler x__complex_handler = complex_handler();
#define convert_to_complex(py_obj,name) \
        x__complex_handler.convert_to_complex(py_obj,name)
#define py_to_complex(py_obj,name) \
        x__complex_handler.py_to_complex(py_obj,name)


PyObject* complex_to_py(PyObject* obj)
{
    return (PyObject*) obj;
}


class unicode_handler
{
public:
    Py_UNICODE* convert_to_unicode(PyObject* py_obj, const char* name)
    {
        // Incref occurs even if conversion fails so that
        // the decref in cleanup_code has a matching incref.
        Py_XINCREF(py_obj);
        if (!py_obj || !PyUnicode_Check(py_obj))
            handle_conversion_error(py_obj,"unicode", name);
        return PyUnicode_AS_UNICODE(py_obj);
    }

    Py_UNICODE* py_to_unicode(PyObject* py_obj, const char* name)
    {
        // !! Pretty sure INCREF should only be called on success since
        // !! py_to_xxx is used by the user -- not the code generator.
        if (!py_obj || !PyUnicode_Check(py_obj))
            handle_bad_type(py_obj,"unicode", name);
        Py_XINCREF(py_obj);
        return PyUnicode_AS_UNICODE(py_obj);
    }
};

unicode_handler x__unicode_handler = unicode_handler();
#define convert_to_unicode(py_obj,name) \
        x__unicode_handler.convert_to_unicode(py_obj,name)
#define py_to_unicode(py_obj,name) \
        x__unicode_handler.py_to_unicode(py_obj,name)


PyObject* unicode_to_py(PyObject* obj)
{
    return (PyObject*) obj;
}


class string_handler
{
public:
    std::string convert_to_string(PyObject* py_obj, const char* name)
    {
        // Incref occurs even if conversion fails so that
        // the decref in cleanup_code has a matching incref.
        Py_XINCREF(py_obj);
        if (!py_obj || !PyString_Check(py_obj))
            handle_conversion_error(py_obj,"string", name);
        return std::string(PyString_AsString(py_obj));
    }

    std::string py_to_string(PyObject* py_obj, const char* name)
    {
        // !! Pretty sure INCREF should only be called on success since
        // !! py_to_xxx is used by the user -- not the code generator.
        if (!py_obj || !PyString_Check(py_obj))
            handle_bad_type(py_obj,"string", name);
        Py_XINCREF(py_obj);
        return std::string(PyString_AsString(py_obj));
    }
};

string_handler x__string_handler = string_handler();
#define convert_to_string(py_obj,name) \
        x__string_handler.convert_to_string(py_obj,name)
#define py_to_string(py_obj,name) \
        x__string_handler.py_to_string(py_obj,name)


               PyObject* string_to_py(std::string s)
               {
                   return PyString_FromString(s.c_str());
               }
               
class list_handler
{
public:
    py::list convert_to_list(PyObject* py_obj, const char* name)
    {
        // Incref occurs even if conversion fails so that
        // the decref in cleanup_code has a matching incref.
        
        if (!py_obj || !PyList_Check(py_obj))
            handle_conversion_error(py_obj,"list", name);
        return py::list(py_obj);
    }

    py::list py_to_list(PyObject* py_obj, const char* name)
    {
        // !! Pretty sure INCREF should only be called on success since
        // !! py_to_xxx is used by the user -- not the code generator.
        if (!py_obj || !PyList_Check(py_obj))
            handle_bad_type(py_obj,"list", name);
        
        return py::list(py_obj);
    }
};

list_handler x__list_handler = list_handler();
#define convert_to_list(py_obj,name) \
        x__list_handler.convert_to_list(py_obj,name)
#define py_to_list(py_obj,name) \
        x__list_handler.py_to_list(py_obj,name)


PyObject* list_to_py(PyObject* obj)
{
    return (PyObject*) obj;
}


class dict_handler
{
public:
    py::dict convert_to_dict(PyObject* py_obj, const char* name)
    {
        // Incref occurs even if conversion fails so that
        // the decref in cleanup_code has a matching incref.
        
        if (!py_obj || !PyDict_Check(py_obj))
            handle_conversion_error(py_obj,"dict", name);
        return py::dict(py_obj);
    }

    py::dict py_to_dict(PyObject* py_obj, const char* name)
    {
        // !! Pretty sure INCREF should only be called on success since
        // !! py_to_xxx is used by the user -- not the code generator.
        if (!py_obj || !PyDict_Check(py_obj))
            handle_bad_type(py_obj,"dict", name);
        
        return py::dict(py_obj);
    }
};

dict_handler x__dict_handler = dict_handler();
#define convert_to_dict(py_obj,name) \
        x__dict_handler.convert_to_dict(py_obj,name)
#define py_to_dict(py_obj,name) \
        x__dict_handler.py_to_dict(py_obj,name)


PyObject* dict_to_py(PyObject* obj)
{
    return (PyObject*) obj;
}


class tuple_handler
{
public:
    py::tuple convert_to_tuple(PyObject* py_obj, const char* name)
    {
        // Incref occurs even if conversion fails so that
        // the decref in cleanup_code has a matching incref.
        
        if (!py_obj || !PyTuple_Check(py_obj))
            handle_conversion_error(py_obj,"tuple", name);
        return py::tuple(py_obj);
    }

    py::tuple py_to_tuple(PyObject* py_obj, const char* name)
    {
        // !! Pretty sure INCREF should only be called on success since
        // !! py_to_xxx is used by the user -- not the code generator.
        if (!py_obj || !PyTuple_Check(py_obj))
            handle_bad_type(py_obj,"tuple", name);
        
        return py::tuple(py_obj);
    }
};

tuple_handler x__tuple_handler = tuple_handler();
#define convert_to_tuple(py_obj,name) \
        x__tuple_handler.convert_to_tuple(py_obj,name)
#define py_to_tuple(py_obj,name) \
        x__tuple_handler.py_to_tuple(py_obj,name)


PyObject* tuple_to_py(PyObject* obj)
{
    return (PyObject*) obj;
}


class file_handler
{
public:
    FILE* convert_to_file(PyObject* py_obj, const char* name)
    {
        // Incref occurs even if conversion fails so that
        // the decref in cleanup_code has a matching incref.
        Py_XINCREF(py_obj);
        if (!py_obj || !PyFile_Check(py_obj))
            handle_conversion_error(py_obj,"file", name);
        return PyFile_AsFile(py_obj);
    }

    FILE* py_to_file(PyObject* py_obj, const char* name)
    {
        // !! Pretty sure INCREF should only be called on success since
        // !! py_to_xxx is used by the user -- not the code generator.
        if (!py_obj || !PyFile_Check(py_obj))
            handle_bad_type(py_obj,"file", name);
        Py_XINCREF(py_obj);
        return PyFile_AsFile(py_obj);
    }
};

file_handler x__file_handler = file_handler();
#define convert_to_file(py_obj,name) \
        x__file_handler.convert_to_file(py_obj,name)
#define py_to_file(py_obj,name) \
        x__file_handler.py_to_file(py_obj,name)


               PyObject* file_to_py(FILE* file, char* name, char* mode)
               {
                   return (PyObject*) PyFile_FromFile(file, name, mode, fclose);
               }
               
class instance_handler
{
public:
    py::object convert_to_instance(PyObject* py_obj, const char* name)
    {
        // Incref occurs even if conversion fails so that
        // the decref in cleanup_code has a matching incref.
        
        if (!py_obj || !PyInstance_Check(py_obj))
            handle_conversion_error(py_obj,"instance", name);
        return py::object(py_obj);
    }

    py::object py_to_instance(PyObject* py_obj, const char* name)
    {
        // !! Pretty sure INCREF should only be called on success since
        // !! py_to_xxx is used by the user -- not the code generator.
        if (!py_obj || !PyInstance_Check(py_obj))
            handle_bad_type(py_obj,"instance", name);
        
        return py::object(py_obj);
    }
};

instance_handler x__instance_handler = instance_handler();
#define convert_to_instance(py_obj,name) \
        x__instance_handler.convert_to_instance(py_obj,name)
#define py_to_instance(py_obj,name) \
        x__instance_handler.py_to_instance(py_obj,name)


PyObject* instance_to_py(PyObject* obj)
{
    return (PyObject*) obj;
}


class numpy_size_handler
{
public:
    void conversion_numpy_check_size(PyArrayObject* arr_obj, int Ndims,
                                     const char* name)
    {
        if (arr_obj->nd != Ndims)
        {
            char msg[500];
            sprintf(msg,"Conversion Error: received '%d' dimensional array instead of '%d' dimensional array for variable '%s'",
                    arr_obj->nd,Ndims,name);
            throw_error(PyExc_TypeError,msg);
        }
    }

    void numpy_check_size(PyArrayObject* arr_obj, int Ndims, const char* name)
    {
        if (arr_obj->nd != Ndims)
        {
            char msg[500];
            sprintf(msg,"received '%d' dimensional array instead of '%d' dimensional array for variable '%s'",
                    arr_obj->nd,Ndims,name);
            throw_error(PyExc_TypeError,msg);
        }
    }
};

numpy_size_handler x__numpy_size_handler = numpy_size_handler();
#define conversion_numpy_check_size x__numpy_size_handler.conversion_numpy_check_size
#define numpy_check_size x__numpy_size_handler.numpy_check_size


class numpy_type_handler
{
public:
    void conversion_numpy_check_type(PyArrayObject* arr_obj, int numeric_type,
                                     const char* name)
    {
        // Make sure input has correct numeric type.
        int arr_type = arr_obj->descr->type_num;
        if (PyTypeNum_ISEXTENDED(numeric_type))
        {
        char msg[80];
        sprintf(msg, "Conversion Error: extended types not supported for variable '%s'",
                name);
        throw_error(PyExc_TypeError, msg);
        }
        if (!PyArray_EquivTypenums(arr_type, numeric_type))
        {

        const char* type_names[23] = {"bool", "byte", "ubyte","short", "ushort",
                                "int", "uint", "long", "ulong", "longlong", "ulonglong",
                                "float", "double", "longdouble", "cfloat", "cdouble",
                                "clongdouble", "object", "string", "unicode", "void", "ntype",
                                "unknown"};
        char msg[500];
        sprintf(msg,"Conversion Error: received '%s' typed array instead of '%s' typed array for variable '%s'",
                type_names[arr_type],type_names[numeric_type],name);
        throw_error(PyExc_TypeError,msg);
        }
    }

    void numpy_check_type(PyArrayObject* arr_obj, int numeric_type, const char* name)
    {
        // Make sure input has correct numeric type.
        int arr_type = arr_obj->descr->type_num;
        if (PyTypeNum_ISEXTENDED(numeric_type))
        {
        char msg[80];
        sprintf(msg, "Conversion Error: extended types not supported for variable '%s'",
                name);
        throw_error(PyExc_TypeError, msg);
        }
        if (!PyArray_EquivTypenums(arr_type, numeric_type))
        {
            const char* type_names[23] = {"bool", "byte", "ubyte","short", "ushort",
                                    "int", "uint", "long", "ulong", "longlong", "ulonglong",
                                    "float", "double", "longdouble", "cfloat", "cdouble",
                                    "clongdouble", "object", "string", "unicode", "void", "ntype",
                                    "unknown"};
            char msg[500];
            sprintf(msg,"received '%s' typed array instead of '%s' typed array for variable '%s'",
                    type_names[arr_type],type_names[numeric_type],name);
            throw_error(PyExc_TypeError,msg);
        }
    }
};

numpy_type_handler x__numpy_type_handler = numpy_type_handler();
#define conversion_numpy_check_type x__numpy_type_handler.conversion_numpy_check_type
#define numpy_check_type x__numpy_type_handler.numpy_check_type


class numpy_handler
{
public:
    PyArrayObject* convert_to_numpy(PyObject* py_obj, const char* name)
    {
        // Incref occurs even if conversion fails so that
        // the decref in cleanup_code has a matching incref.
        Py_XINCREF(py_obj);
        if (!py_obj || !PyArray_Check(py_obj))
            handle_conversion_error(py_obj,"numpy", name);
        return (PyArrayObject*) py_obj;
    }

    PyArrayObject* py_to_numpy(PyObject* py_obj, const char* name)
    {
        // !! Pretty sure INCREF should only be called on success since
        // !! py_to_xxx is used by the user -- not the code generator.
        if (!py_obj || !PyArray_Check(py_obj))
            handle_bad_type(py_obj,"numpy", name);
        Py_XINCREF(py_obj);
        return (PyArrayObject*) py_obj;
    }
};

numpy_handler x__numpy_handler = numpy_handler();
#define convert_to_numpy(py_obj,name) \
        x__numpy_handler.convert_to_numpy(py_obj,name)
#define py_to_numpy(py_obj,name) \
        x__numpy_handler.py_to_numpy(py_obj,name)


PyObject* numpy_to_py(PyObject* obj)
{
    return (PyObject*) obj;
}


class catchall_handler
{
public:
    py::object convert_to_catchall(PyObject* py_obj, const char* name)
    {
        // Incref occurs even if conversion fails so that
        // the decref in cleanup_code has a matching incref.
        
        if (!py_obj || !(py_obj))
            handle_conversion_error(py_obj,"catchall", name);
        return py::object(py_obj);
    }

    py::object py_to_catchall(PyObject* py_obj, const char* name)
    {
        // !! Pretty sure INCREF should only be called on success since
        // !! py_to_xxx is used by the user -- not the code generator.
        if (!py_obj || !(py_obj))
            handle_bad_type(py_obj,"catchall", name);
        
        return py::object(py_obj);
    }
};

catchall_handler x__catchall_handler = catchall_handler();
#define convert_to_catchall(py_obj,name) \
        x__catchall_handler.convert_to_catchall(py_obj,name)
#define py_to_catchall(py_obj,name) \
        x__catchall_handler.py_to_catchall(py_obj,name)


PyObject* catchall_to_py(PyObject* obj)
{
    return (PyObject*) obj;
}


    #define max(x,y)  ((x) > (y) ? (x) : (y))

    typedef struct { double _[3]; } vec_t;

    inline vec_t make_vec(double x, double y, double z)
    {
        vec_t m = { {x,y,z} };
        return m;
    }
    inline vec_t make_vec(double th, double phi)
    {
        double cosphi = cos(phi);
        double x = cosphi * cos(th), y = cosphi * sin(th), z = sin(phi);
        return make_vec(x,y,z);
    }
    inline vec_t make_vec_s(double *sph)
    {
        double th = sph[0], phi = sph[1];
        return make_vec(th,phi);
    }
    inline vec_t make_vec_c(double *m)
    {
        return make_vec(m[0],m[1],m[2]);
    }
    inline void vec2sph(vec_t m, double *sph)
    {
        double x = m._[0], y = m._[1], z = m._[2];
        sph[0] = atan2(y, x);
        sph[1] = atan2(z, hypot(x, y));
    }
    inline void vec2mem(vec_t v, double *m)
    {
        for (int i = 0; i < 3; i++)
            m[i] = v._[i];
    }


    // negation
    inline vec_t operator-(vec_t a)
    {
        return make_vec(-a._[0], -a._[1], -a._[2]);
    }

    // addition
    inline vec_t operator+(vec_t a, double b)
    {
        return make_vec(a._[0]+b, a._[1]+b, a._[2]+b);
    }
    inline vec_t operator+(double a, vec_t b) { return b + a; }
    inline void operator+=(vec_t &a, double b)
    {
        a._[0] += b;
        a._[1] += b;
        a._[2] += b;
    }

    // subtraction
    inline vec_t operator-(vec_t a, double b)
    {
        return make_vec(a._[0]-b, a._[1]-b, a._[2]-b);
    }
    inline vec_t operator-(double a, vec_t b)
    {
        return make_vec(a-b._[0], a-b._[1], a-b._[2]);
    }
    inline void operator-=(vec_t &a, double b)
    {
        a._[0] -= b;
        a._[1] -= b;
        a._[2] -= b;
    }

    // multiplication
    inline vec_t operator*(vec_t a, double b)
    {
        return make_vec(a._[0]*b, a._[1]*b, a._[2]*b);
    }
    inline vec_t operator*(double a, vec_t b) { return b * a; }
    inline void operator*=(vec_t &a, double b)
    {
        a._[0] *= b;
        a._[1] *= b;
        a._[2] *= b;
    }

    // division
    inline vec_t operator/(vec_t a, double b)
    {
        return make_vec(a._[0]/b, a._[1]/b, a._[2]/b);
    }
    inline vec_t operator/(double a, vec_t b)
    {
        return make_vec(a/b._[0], a/b._[1], a/b._[2]);
    }
    inline void operator/=(vec_t &a, double b)
    {
        a._[0] /= b;
        a._[1] /= b;
        a._[2] /= b;
    }


    inline double norm(vec_t a)
    {
        return sqrt(a._[0]*a._[0] + a._[1]*a._[1] + a._[2]*a._[2]);
    }


    inline double dot(vec_t a, vec_t b)
    {
        return a._[0]*b._[0] + a._[1]*b._[1] + a._[2]*b._[2];
    }

    typedef struct { double _[9]; } mat_t;

    inline mat_t make_mat(double a, double b, double c,
                          double d, double e, double f,
                          double g, double h, double i)
    {
        mat_t m = { {a,b,c, d,e,f, g,h,i} };
        return m;
    }
    inline mat_t diag(double a, double b, double c)
    {
        mat_t m = { {a,0,0, 0,b,0, 0,0,c} };
        return m;
    }
    inline mat_t t(mat_t m)
    {
        return make_mat(m._[0], m._[3], m._[6],
                        m._[1], m._[4], m._[7],
                        m._[2], m._[5], m._[8]);
    }

    // matrix-scalar
    inline mat_t operator*(mat_t a, double b)
    {
        return make_mat(a._[0]*b,  a._[1]*b, a._[2]*b,
                        a._[3]*b,  a._[4]*b, a._[5]*b,
                        a._[6]*b,  a._[7]*b, a._[8]*b);
    }
    inline mat_t operator*(double a, mat_t b)
    {
        return b * a;
    }

    // matrix-vector
    inline vec_t operator*(mat_t a, vec_t b)
    {
        return make_vec(a._[0]*b._[0] + a._[1]*b._[1] + a._[2]*b._[2],
                        a._[3]*b._[0] + a._[4]*b._[1] + a._[5]*b._[2],
                        a._[6]*b._[0] + a._[7]*b._[1] + a._[8]*b._[2]);
    }


    // matrix-matrix
    inline mat_t operator*(mat_t a, mat_t b)
    {
        return make_mat(a._[0]*b._[0] + a._[1]*b._[3] + a._[2]*b._[6],
                        a._[0]*b._[1] + a._[1]*b._[4] + a._[2]*b._[7],
                        a._[0]*b._[2] + a._[1]*b._[5] + a._[2]*b._[8],

                        a._[3]*b._[0] + a._[4]*b._[3] + a._[5]*b._[6],
                        a._[3]*b._[1] + a._[4]*b._[4] + a._[5]*b._[7],
                        a._[3]*b._[2] + a._[4]*b._[5] + a._[5]*b._[8],

                        a._[6]*b._[0] + a._[7]*b._[3] + a._[8]*b._[6],
                        a._[6]*b._[1] + a._[7]*b._[4] + a._[8]*b._[7],
                        a._[6]*b._[2] + a._[7]*b._[5] + a._[8]*b._[8]);
    }

    // determinant
    inline double det(mat_t M)
    {
        return M._[0]*(M._[4]*M._[8] - M._[5]*M._[7])
            -  M._[1]*(M._[3]*M._[8] - M._[5]*M._[6])
            +  M._[2]*(M._[3]*M._[7] - M._[4]*M._[6]);
    }

    // conjugate transpose
    inline mat_t ct(mat_t M)
    {
        return make_mat( M._[0], -M._[3],  M._[6],
                        -M._[1],  M._[4], -M._[7],
                         M._[2], -M._[5],  M._[8]);
    }

    // invert
    inline mat_t inv(mat_t M)
    {
        return (1 / det(M)) * ct(M);
    }

    inline mat_t diffusion(vec_t m, double l1, double l2)
    {
        double x = m._[0], y = m._[1], z = m._[2];
        mat_t R = make_mat(x,  y,            z          ,
                           y,  y*y/(1+x)-1,  y*z/(1+x)  ,
                           z,  y*z/(1+x),    z*z/(1+x)-1);
        return R * diag(l1,l2,l2) * t(R) * 1e-6;
    }

    inline mat_t rotation(double theta, double phi, double psi)
    {
        double c_th = cos(theta);
        double s_th = sin(theta);
        double c_ph = cos(phi);
        double s_ph = sin(phi);
        double c_ps = cos(psi);
        double s_ps = sin(psi);

        double q11 = c_th * c_ph * c_ps - s_ph * s_ps;
        double q21 = c_th * c_ps * s_ph + c_ph * s_ps;
        double q31 = -c_ps * s_th;
        double q12 = -c_ps * s_ph - c_th * c_ph * s_ps;
        double q22 = c_ph * c_ps - c_th * s_ph * s_ps;
        double q32 = s_th * s_ps;
        double q13 = c_ph * s_th;
        double q23 = s_th * s_ph;
        double q33 = c_th;

        mat_t Q = make_mat(q11, q12, q13,
                           q21, q22, q23,
                           q31, q32, q33);
        return Q;
    }

    // Calculates a rotation matrix from euler angles.
    inline mat_t diffusion_euler(double theta, double phi, double psi,
                                 double l1, double l2, double l3)
    {
        mat_t Q = rotation(theta, phi, psi);
        return Q * diag(l1,l2,l3) * t(Q) * 1e-6;
    }
    

static PyObject* c_model_1tensor_f(PyObject*self, PyObject* args, PyObject* kywds)
{
    py::object return_val;
    int exception_occured = 0;
    PyObject *py_local_dict = NULL;
    static char *kwlist[] = {"X","m","local_dict", NULL};
    PyObject *py_X, *py_m;
    int X_used, m_used;
    py_X = py_m = NULL;
    X_used= m_used = 0;
    
    if(!PyArg_ParseTupleAndKeywords(args,kywds,"OO|O:c_model_1tensor_f",kwlist,&py_X, &py_m, &py_local_dict))
       return NULL;
    try                              
    {                                
        py_X = py_X;
        PyArrayObject* X_array = convert_to_numpy(py_X,"X");
        conversion_numpy_check_type(X_array,PyArray_DOUBLE,"X");
        #define X1(i) (*((double*)(X_array->data + (i)*SX[0])))
        #define X2(i,j) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1])))
        #define X3(i,j,k) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1] + (k)*SX[2])))
        #define X4(i,j,k,l) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1] + (k)*SX[2] + (l)*SX[3])))
        npy_intp* NX = X_array->dimensions;
        npy_intp* SX = X_array->strides;
        int DX = X_array->nd;
        double* X = (double*) X_array->data;
        X_used = 1;
        py_m = py_m;
        int m = convert_to_int(py_m,"m");
        m_used = 1;
        /*<function call here>*/     
        
            for (int i = 0; i < m; ++i) {
                // normalize
                double m_norm = 1e-16;
                for (int i = 0; i < 3; ++i) {
                  double x = X[i];
                  m_norm += x * x;
                }
                m_norm = sqrt(m_norm);
                for (int i = 0; i < 3; ++i)
                  X[i] /= m_norm;
        
                // lambda (clamped)
                X[3] = max(X[3], 100);
                X[4] = max(X[4], 100);
        
                // prepare for next
                X += 5;
            }
        if(py_local_dict)                                  
        {                                                  
            py::dict local_dict = py::dict(py_local_dict); 
        }                                                  
    
    }                                
    catch(...)                       
    {                                
        return_val =  py::object();      
        exception_occured = 1;       
    }                                
    /*cleanup code*/                     
    if(X_used)
    {
        Py_XDECREF(py_X);
        #undef X1
        #undef X2
        #undef X3
        #undef X4
    }
    if(!(PyObject*)return_val && !exception_occured)
    {
                                  
        return_val = Py_None;            
    }
                                  
    return return_val.disown();           
}                                
static PyObject* c_model_1tensor_full_f(PyObject*self, PyObject* args, PyObject* kywds)
{
    py::object return_val;
    int exception_occured = 0;
    PyObject *py_local_dict = NULL;
    static char *kwlist[] = {"X","m","local_dict", NULL};
    PyObject *py_X, *py_m;
    int X_used, m_used;
    py_X = py_m = NULL;
    X_used= m_used = 0;
    
    if(!PyArg_ParseTupleAndKeywords(args,kywds,"OO|O:c_model_1tensor_full_f",kwlist,&py_X, &py_m, &py_local_dict))
       return NULL;
    try                              
    {                                
        py_X = py_X;
        PyArrayObject* X_array = convert_to_numpy(py_X,"X");
        conversion_numpy_check_type(X_array,PyArray_DOUBLE,"X");
        #define X1(i) (*((double*)(X_array->data + (i)*SX[0])))
        #define X2(i,j) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1])))
        #define X3(i,j,k) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1] + (k)*SX[2])))
        #define X4(i,j,k,l) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1] + (k)*SX[2] + (l)*SX[3])))
        npy_intp* NX = X_array->dimensions;
        npy_intp* SX = X_array->strides;
        int DX = X_array->nd;
        double* X = (double*) X_array->data;
        X_used = 1;
        py_m = py_m;
        int m = convert_to_int(py_m,"m");
        m_used = 1;
        /*<function call here>*/     
        
            for (int i = 0; i < m; ++i) {
                // lambda (clamped)
                X[3] = max(X[3], 100);
                X[4] = max(X[4], 100);
                X[5] = max(X[5], 100);
        
                // prepare for next
                X += 6;
            }
        if(py_local_dict)                                  
        {                                                  
            py::dict local_dict = py::dict(py_local_dict); 
        }                                                  
    
    }                                
    catch(...)                       
    {                                
        return_val =  py::object();      
        exception_occured = 1;       
    }                                
    /*cleanup code*/                     
    if(X_used)
    {
        Py_XDECREF(py_X);
        #undef X1
        #undef X2
        #undef X3
        #undef X4
    }
    if(!(PyObject*)return_val && !exception_occured)
    {
                                  
        return_val = Py_None;            
    }
                                  
    return return_val.disown();           
}                                
static PyObject* c_model_1tensor_h(PyObject*self, PyObject* args, PyObject* kywds)
{
    py::object return_val;
    int exception_occured = 0;
    PyObject *py_local_dict = NULL;
    static char *kwlist[] = {"s","X","u","b","n","m","local_dict", NULL};
    PyObject *py_s, *py_X, *py_u, *py_b, *py_n, *py_m;
    int s_used, X_used, u_used, b_used, n_used, m_used;
    py_s = py_X = py_u = py_b = py_n = py_m = NULL;
    s_used= X_used= u_used= b_used= n_used= m_used = 0;
    
    if(!PyArg_ParseTupleAndKeywords(args,kywds,"OOOOOO|O:c_model_1tensor_h",kwlist,&py_s, &py_X, &py_u, &py_b, &py_n, &py_m, &py_local_dict))
       return NULL;
    try                              
    {                                
        py_s = py_s;
        PyArrayObject* s_array = convert_to_numpy(py_s,"s");
        conversion_numpy_check_type(s_array,PyArray_DOUBLE,"s");
        #define S1(i) (*((double*)(s_array->data + (i)*Ss[0])))
        #define S2(i,j) (*((double*)(s_array->data + (i)*Ss[0] + (j)*Ss[1])))
        #define S3(i,j,k) (*((double*)(s_array->data + (i)*Ss[0] + (j)*Ss[1] + (k)*Ss[2])))
        #define S4(i,j,k,l) (*((double*)(s_array->data + (i)*Ss[0] + (j)*Ss[1] + (k)*Ss[2] + (l)*Ss[3])))
        npy_intp* Ns = s_array->dimensions;
        npy_intp* Ss = s_array->strides;
        int Ds = s_array->nd;
        double* s = (double*) s_array->data;
        s_used = 1;
        py_X = py_X;
        PyArrayObject* X_array = convert_to_numpy(py_X,"X");
        conversion_numpy_check_type(X_array,PyArray_DOUBLE,"X");
        #define X1(i) (*((double*)(X_array->data + (i)*SX[0])))
        #define X2(i,j) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1])))
        #define X3(i,j,k) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1] + (k)*SX[2])))
        #define X4(i,j,k,l) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1] + (k)*SX[2] + (l)*SX[3])))
        npy_intp* NX = X_array->dimensions;
        npy_intp* SX = X_array->strides;
        int DX = X_array->nd;
        double* X = (double*) X_array->data;
        X_used = 1;
        py_u = py_u;
        PyArrayObject* u_array = convert_to_numpy(py_u,"u");
        conversion_numpy_check_type(u_array,PyArray_DOUBLE,"u");
        #define U1(i) (*((double*)(u_array->data + (i)*Su[0])))
        #define U2(i,j) (*((double*)(u_array->data + (i)*Su[0] + (j)*Su[1])))
        #define U3(i,j,k) (*((double*)(u_array->data + (i)*Su[0] + (j)*Su[1] + (k)*Su[2])))
        #define U4(i,j,k,l) (*((double*)(u_array->data + (i)*Su[0] + (j)*Su[1] + (k)*Su[2] + (l)*Su[3])))
        npy_intp* Nu = u_array->dimensions;
        npy_intp* Su = u_array->strides;
        int Du = u_array->nd;
        double* u = (double*) u_array->data;
        u_used = 1;
        py_b = py_b;
        double b = convert_to_float(py_b,"b");
        b_used = 1;
        py_n = py_n;
        int n = convert_to_int(py_n,"n");
        n_used = 1;
        py_m = py_m;
        int m = convert_to_int(py_m,"m");
        m_used = 1;
        /*<function call here>*/     
        
            for (int i = 0; i < m; ++i) {
                // unpack and normalize orientations
                vec_t m1 = make_vec(X[0], X[1], X[2]); m1 /= norm(m1);
                double l11 = X[3], l12 = X[4];
        
                // ensure: lambda >= L
                double L = 100;
                if (l11 < L)   l11 = L;
                if (l12 < L)   l12 = L;
        
                // flip if necessary
                if (m1._[0] < 0)   m1 = -m1;
        
                // calculate diffusion matrix
                mat_t D = diffusion(m1, l11, l12);
        
                // reconstruct signal
                for (int i = 0; i < n; ++i, ++s) {
                    vec_t u_ = make_vec(u[3*i], u[3*i+1], u[3*i+2]);
                    *s = exp(-b*dot(u_,D*u_));
                }
        
                // prepare for next
                X += 5;
            }
        if(py_local_dict)                                  
        {                                                  
            py::dict local_dict = py::dict(py_local_dict); 
        }                                                  
    
    }                                
    catch(...)                       
    {                                
        return_val =  py::object();      
        exception_occured = 1;       
    }                                
    /*cleanup code*/                     
    if(s_used)
    {
        Py_XDECREF(py_s);
        #undef S1
        #undef S2
        #undef S3
        #undef S4
    }
    if(X_used)
    {
        Py_XDECREF(py_X);
        #undef X1
        #undef X2
        #undef X3
        #undef X4
    }
    if(u_used)
    {
        Py_XDECREF(py_u);
        #undef U1
        #undef U2
        #undef U3
        #undef U4
    }
    if(!(PyObject*)return_val && !exception_occured)
    {
                                  
        return_val = Py_None;            
    }
                                  
    return return_val.disown();           
}                                
static PyObject* c_model_1tensor_full_h(PyObject*self, PyObject* args, PyObject* kywds)
{
    py::object return_val;
    int exception_occured = 0;
    PyObject *py_local_dict = NULL;
    static char *kwlist[] = {"s","X","u","b","n","m","local_dict", NULL};
    PyObject *py_s, *py_X, *py_u, *py_b, *py_n, *py_m;
    int s_used, X_used, u_used, b_used, n_used, m_used;
    py_s = py_X = py_u = py_b = py_n = py_m = NULL;
    s_used= X_used= u_used= b_used= n_used= m_used = 0;
    
    if(!PyArg_ParseTupleAndKeywords(args,kywds,"OOOOOO|O:c_model_1tensor_full_h",kwlist,&py_s, &py_X, &py_u, &py_b, &py_n, &py_m, &py_local_dict))
       return NULL;
    try                              
    {                                
        py_s = py_s;
        PyArrayObject* s_array = convert_to_numpy(py_s,"s");
        conversion_numpy_check_type(s_array,PyArray_DOUBLE,"s");
        #define S1(i) (*((double*)(s_array->data + (i)*Ss[0])))
        #define S2(i,j) (*((double*)(s_array->data + (i)*Ss[0] + (j)*Ss[1])))
        #define S3(i,j,k) (*((double*)(s_array->data + (i)*Ss[0] + (j)*Ss[1] + (k)*Ss[2])))
        #define S4(i,j,k,l) (*((double*)(s_array->data + (i)*Ss[0] + (j)*Ss[1] + (k)*Ss[2] + (l)*Ss[3])))
        npy_intp* Ns = s_array->dimensions;
        npy_intp* Ss = s_array->strides;
        int Ds = s_array->nd;
        double* s = (double*) s_array->data;
        s_used = 1;
        py_X = py_X;
        PyArrayObject* X_array = convert_to_numpy(py_X,"X");
        conversion_numpy_check_type(X_array,PyArray_DOUBLE,"X");
        #define X1(i) (*((double*)(X_array->data + (i)*SX[0])))
        #define X2(i,j) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1])))
        #define X3(i,j,k) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1] + (k)*SX[2])))
        #define X4(i,j,k,l) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1] + (k)*SX[2] + (l)*SX[3])))
        npy_intp* NX = X_array->dimensions;
        npy_intp* SX = X_array->strides;
        int DX = X_array->nd;
        double* X = (double*) X_array->data;
        X_used = 1;
        py_u = py_u;
        PyArrayObject* u_array = convert_to_numpy(py_u,"u");
        conversion_numpy_check_type(u_array,PyArray_DOUBLE,"u");
        #define U1(i) (*((double*)(u_array->data + (i)*Su[0])))
        #define U2(i,j) (*((double*)(u_array->data + (i)*Su[0] + (j)*Su[1])))
        #define U3(i,j,k) (*((double*)(u_array->data + (i)*Su[0] + (j)*Su[1] + (k)*Su[2])))
        #define U4(i,j,k,l) (*((double*)(u_array->data + (i)*Su[0] + (j)*Su[1] + (k)*Su[2] + (l)*Su[3])))
        npy_intp* Nu = u_array->dimensions;
        npy_intp* Su = u_array->strides;
        int Du = u_array->nd;
        double* u = (double*) u_array->data;
        u_used = 1;
        py_b = py_b;
        double b = convert_to_float(py_b,"b");
        b_used = 1;
        py_n = py_n;
        int n = convert_to_int(py_n,"n");
        n_used = 1;
        py_m = py_m;
        int m = convert_to_int(py_m,"m");
        m_used = 1;
        /*<function call here>*/     
        
            for (int i = 0; i < m; ++i) {
                // unpack
                double theta = X[0], phi = X[1], psi = X[2];
                double l11 = X[3], l12 = X[4], l13 = X[5];
        
                // ensure: lambda >= L
                double L = 100;
                if (l11 < L)   l11 = L;
                if (l12 < L)   l12 = L;
                if (l13 < L)   l13 = L;
        
                // calculate diffusion matrix
                mat_t D = diffusion_euler(theta, phi, psi, l11, l12, l13);
        
                // reconstruct signal
                for (int i = 0; i < n; ++i, ++s) {
                    vec_t u_ = make_vec(u[3 * i], u[3 * i + 1], u[3 * i + 2]);
                    *s = exp(-b * dot(u_, D * u_));
                }
        
                // prepare for next
                X += 6;
            }
        if(py_local_dict)                                  
        {                                                  
            py::dict local_dict = py::dict(py_local_dict); 
        }                                                  
    
    }                                
    catch(...)                       
    {                                
        return_val =  py::object();      
        exception_occured = 1;       
    }                                
    /*cleanup code*/                     
    if(s_used)
    {
        Py_XDECREF(py_s);
        #undef S1
        #undef S2
        #undef S3
        #undef S4
    }
    if(X_used)
    {
        Py_XDECREF(py_X);
        #undef X1
        #undef X2
        #undef X3
        #undef X4
    }
    if(u_used)
    {
        Py_XDECREF(py_u);
        #undef U1
        #undef U2
        #undef U3
        #undef U4
    }
    if(!(PyObject*)return_val && !exception_occured)
    {
                                  
        return_val = Py_None;            
    }
                                  
    return return_val.disown();           
}                                
static PyObject* c_state2tensor1(PyObject*self, PyObject* args, PyObject* kywds)
{
    py::object return_val;
    int exception_occured = 0;
    PyObject *py_local_dict = NULL;
    static char *kwlist[] = {"X","y","m","l1","l2","local_dict", NULL};
    PyObject *py_X, *py_y, *py_m, *py_l1, *py_l2;
    int X_used, y_used, m_used, l1_used, l2_used;
    py_X = py_y = py_m = py_l1 = py_l2 = NULL;
    X_used= y_used= m_used= l1_used= l2_used = 0;
    
    if(!PyArg_ParseTupleAndKeywords(args,kywds,"OOOOO|O:c_state2tensor1",kwlist,&py_X, &py_y, &py_m, &py_l1, &py_l2, &py_local_dict))
       return NULL;
    try                              
    {                                
        py_X = py_X;
        PyArrayObject* X_array = convert_to_numpy(py_X,"X");
        conversion_numpy_check_type(X_array,PyArray_DOUBLE,"X");
        #define X1(i) (*((double*)(X_array->data + (i)*SX[0])))
        #define X2(i,j) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1])))
        #define X3(i,j,k) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1] + (k)*SX[2])))
        #define X4(i,j,k,l) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1] + (k)*SX[2] + (l)*SX[3])))
        npy_intp* NX = X_array->dimensions;
        npy_intp* SX = X_array->strides;
        int DX = X_array->nd;
        double* X = (double*) X_array->data;
        X_used = 1;
        py_y = py_y;
        PyArrayObject* y_array = convert_to_numpy(py_y,"y");
        conversion_numpy_check_type(y_array,PyArray_DOUBLE,"y");
        #define Y1(i) (*((double*)(y_array->data + (i)*Sy[0])))
        #define Y2(i,j) (*((double*)(y_array->data + (i)*Sy[0] + (j)*Sy[1])))
        #define Y3(i,j,k) (*((double*)(y_array->data + (i)*Sy[0] + (j)*Sy[1] + (k)*Sy[2])))
        #define Y4(i,j,k,l) (*((double*)(y_array->data + (i)*Sy[0] + (j)*Sy[1] + (k)*Sy[2] + (l)*Sy[3])))
        npy_intp* Ny = y_array->dimensions;
        npy_intp* Sy = y_array->strides;
        int Dy = y_array->nd;
        double* y = (double*) y_array->data;
        y_used = 1;
        py_m = py_m;
        PyArrayObject* m_array = convert_to_numpy(py_m,"m");
        conversion_numpy_check_type(m_array,PyArray_DOUBLE,"m");
        #define M1(i) (*((double*)(m_array->data + (i)*Sm[0])))
        #define M2(i,j) (*((double*)(m_array->data + (i)*Sm[0] + (j)*Sm[1])))
        #define M3(i,j,k) (*((double*)(m_array->data + (i)*Sm[0] + (j)*Sm[1] + (k)*Sm[2])))
        #define M4(i,j,k,l) (*((double*)(m_array->data + (i)*Sm[0] + (j)*Sm[1] + (k)*Sm[2] + (l)*Sm[3])))
        npy_intp* Nm = m_array->dimensions;
        npy_intp* Sm = m_array->strides;
        int Dm = m_array->nd;
        double* m = (double*) m_array->data;
        m_used = 1;
        py_l1 = py_l1;
        PyArrayObject* l1_array = convert_to_numpy(py_l1,"l1");
        conversion_numpy_check_type(l1_array,PyArray_DOUBLE,"l1");
        #define L11(i) (*((double*)(l1_array->data + (i)*Sl1[0])))
        #define L12(i,j) (*((double*)(l1_array->data + (i)*Sl1[0] + (j)*Sl1[1])))
        #define L13(i,j,k) (*((double*)(l1_array->data + (i)*Sl1[0] + (j)*Sl1[1] + (k)*Sl1[2])))
        #define L14(i,j,k,l) (*((double*)(l1_array->data + (i)*Sl1[0] + (j)*Sl1[1] + (k)*Sl1[2] + (l)*Sl1[3])))
        npy_intp* Nl1 = l1_array->dimensions;
        npy_intp* Sl1 = l1_array->strides;
        int Dl1 = l1_array->nd;
        double* l1 = (double*) l1_array->data;
        l1_used = 1;
        py_l2 = py_l2;
        PyArrayObject* l2_array = convert_to_numpy(py_l2,"l2");
        conversion_numpy_check_type(l2_array,PyArray_DOUBLE,"l2");
        #define L21(i) (*((double*)(l2_array->data + (i)*Sl2[0])))
        #define L22(i,j) (*((double*)(l2_array->data + (i)*Sl2[0] + (j)*Sl2[1])))
        #define L23(i,j,k) (*((double*)(l2_array->data + (i)*Sl2[0] + (j)*Sl2[1] + (k)*Sl2[2])))
        #define L24(i,j,k,l) (*((double*)(l2_array->data + (i)*Sl2[0] + (j)*Sl2[1] + (k)*Sl2[2] + (l)*Sl2[3])))
        npy_intp* Nl2 = l2_array->dimensions;
        npy_intp* Sl2 = l2_array->strides;
        int Dl2 = l2_array->nd;
        double* l2 = (double*) l2_array->data;
        l2_used = 1;
        /*<function call here>*/     
        
            // unpack
            double m_norm = 1e-10;
            for (int i = 0; i < 3; ++i) {
              double x = m[i] = X[i];
              m_norm += x * x;
            }
            // normalize
            m_norm = sqrt(m_norm);
            for (int i = 0; i < 3; ++i)
              m[i] /= m_norm;
        
            // flip orientation?
            if (m[0]*y[0] + m[1]*y[1] + m[2]*y[2] < 0) {
                for (int i = 0; i < 3; ++i)
                    m[i] = -m[i];
            }
        
            // lambda (clamped)
            *l1 = max(X[3], 100);
            *l2 = max(X[4], 100);
        if(py_local_dict)                                  
        {                                                  
            py::dict local_dict = py::dict(py_local_dict); 
        }                                                  
    
    }                                
    catch(...)                       
    {                                
        return_val =  py::object();      
        exception_occured = 1;       
    }                                
    /*cleanup code*/                     
    if(X_used)
    {
        Py_XDECREF(py_X);
        #undef X1
        #undef X2
        #undef X3
        #undef X4
    }
    if(y_used)
    {
        Py_XDECREF(py_y);
        #undef Y1
        #undef Y2
        #undef Y3
        #undef Y4
    }
    if(m_used)
    {
        Py_XDECREF(py_m);
        #undef M1
        #undef M2
        #undef M3
        #undef M4
    }
    if(l1_used)
    {
        Py_XDECREF(py_l1);
        #undef L11
        #undef L12
        #undef L13
        #undef L14
    }
    if(l2_used)
    {
        Py_XDECREF(py_l2);
        #undef L21
        #undef L22
        #undef L23
        #undef L24
    }
    if(!(PyObject*)return_val && !exception_occured)
    {
                                  
        return_val = Py_None;            
    }
                                  
    return return_val.disown();           
}                                
static PyObject* c_state2tensor1_full(PyObject*self, PyObject* args, PyObject* kywds)
{
    py::object return_val;
    int exception_occured = 0;
    PyObject *py_local_dict = NULL;
    static char *kwlist[] = {"X","y","m","l1","l2","l3","local_dict", NULL};
    PyObject *py_X, *py_y, *py_m, *py_l1, *py_l2, *py_l3;
    int X_used, y_used, m_used, l1_used, l2_used, l3_used;
    py_X = py_y = py_m = py_l1 = py_l2 = py_l3 = NULL;
    X_used= y_used= m_used= l1_used= l2_used= l3_used = 0;
    
    if(!PyArg_ParseTupleAndKeywords(args,kywds,"OOOOOO|O:c_state2tensor1_full",kwlist,&py_X, &py_y, &py_m, &py_l1, &py_l2, &py_l3, &py_local_dict))
       return NULL;
    try                              
    {                                
        py_X = py_X;
        PyArrayObject* X_array = convert_to_numpy(py_X,"X");
        conversion_numpy_check_type(X_array,PyArray_DOUBLE,"X");
        #define X1(i) (*((double*)(X_array->data + (i)*SX[0])))
        #define X2(i,j) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1])))
        #define X3(i,j,k) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1] + (k)*SX[2])))
        #define X4(i,j,k,l) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1] + (k)*SX[2] + (l)*SX[3])))
        npy_intp* NX = X_array->dimensions;
        npy_intp* SX = X_array->strides;
        int DX = X_array->nd;
        double* X = (double*) X_array->data;
        X_used = 1;
        py_y = py_y;
        PyArrayObject* y_array = convert_to_numpy(py_y,"y");
        conversion_numpy_check_type(y_array,PyArray_DOUBLE,"y");
        #define Y1(i) (*((double*)(y_array->data + (i)*Sy[0])))
        #define Y2(i,j) (*((double*)(y_array->data + (i)*Sy[0] + (j)*Sy[1])))
        #define Y3(i,j,k) (*((double*)(y_array->data + (i)*Sy[0] + (j)*Sy[1] + (k)*Sy[2])))
        #define Y4(i,j,k,l) (*((double*)(y_array->data + (i)*Sy[0] + (j)*Sy[1] + (k)*Sy[2] + (l)*Sy[3])))
        npy_intp* Ny = y_array->dimensions;
        npy_intp* Sy = y_array->strides;
        int Dy = y_array->nd;
        double* y = (double*) y_array->data;
        y_used = 1;
        py_m = py_m;
        PyArrayObject* m_array = convert_to_numpy(py_m,"m");
        conversion_numpy_check_type(m_array,PyArray_DOUBLE,"m");
        #define M1(i) (*((double*)(m_array->data + (i)*Sm[0])))
        #define M2(i,j) (*((double*)(m_array->data + (i)*Sm[0] + (j)*Sm[1])))
        #define M3(i,j,k) (*((double*)(m_array->data + (i)*Sm[0] + (j)*Sm[1] + (k)*Sm[2])))
        #define M4(i,j,k,l) (*((double*)(m_array->data + (i)*Sm[0] + (j)*Sm[1] + (k)*Sm[2] + (l)*Sm[3])))
        npy_intp* Nm = m_array->dimensions;
        npy_intp* Sm = m_array->strides;
        int Dm = m_array->nd;
        double* m = (double*) m_array->data;
        m_used = 1;
        py_l1 = py_l1;
        PyArrayObject* l1_array = convert_to_numpy(py_l1,"l1");
        conversion_numpy_check_type(l1_array,PyArray_DOUBLE,"l1");
        #define L11(i) (*((double*)(l1_array->data + (i)*Sl1[0])))
        #define L12(i,j) (*((double*)(l1_array->data + (i)*Sl1[0] + (j)*Sl1[1])))
        #define L13(i,j,k) (*((double*)(l1_array->data + (i)*Sl1[0] + (j)*Sl1[1] + (k)*Sl1[2])))
        #define L14(i,j,k,l) (*((double*)(l1_array->data + (i)*Sl1[0] + (j)*Sl1[1] + (k)*Sl1[2] + (l)*Sl1[3])))
        npy_intp* Nl1 = l1_array->dimensions;
        npy_intp* Sl1 = l1_array->strides;
        int Dl1 = l1_array->nd;
        double* l1 = (double*) l1_array->data;
        l1_used = 1;
        py_l2 = py_l2;
        PyArrayObject* l2_array = convert_to_numpy(py_l2,"l2");
        conversion_numpy_check_type(l2_array,PyArray_DOUBLE,"l2");
        #define L21(i) (*((double*)(l2_array->data + (i)*Sl2[0])))
        #define L22(i,j) (*((double*)(l2_array->data + (i)*Sl2[0] + (j)*Sl2[1])))
        #define L23(i,j,k) (*((double*)(l2_array->data + (i)*Sl2[0] + (j)*Sl2[1] + (k)*Sl2[2])))
        #define L24(i,j,k,l) (*((double*)(l2_array->data + (i)*Sl2[0] + (j)*Sl2[1] + (k)*Sl2[2] + (l)*Sl2[3])))
        npy_intp* Nl2 = l2_array->dimensions;
        npy_intp* Sl2 = l2_array->strides;
        int Dl2 = l2_array->nd;
        double* l2 = (double*) l2_array->data;
        l2_used = 1;
        py_l3 = py_l3;
        PyArrayObject* l3_array = convert_to_numpy(py_l3,"l3");
        conversion_numpy_check_type(l3_array,PyArray_DOUBLE,"l3");
        #define L31(i) (*((double*)(l3_array->data + (i)*Sl3[0])))
        #define L32(i,j) (*((double*)(l3_array->data + (i)*Sl3[0] + (j)*Sl3[1])))
        #define L33(i,j,k) (*((double*)(l3_array->data + (i)*Sl3[0] + (j)*Sl3[1] + (k)*Sl3[2])))
        #define L34(i,j,k,l) (*((double*)(l3_array->data + (i)*Sl3[0] + (j)*Sl3[1] + (k)*Sl3[2] + (l)*Sl3[3])))
        npy_intp* Nl3 = l3_array->dimensions;
        npy_intp* Sl3 = l3_array->strides;
        int Dl3 = l3_array->nd;
        double* l3 = (double*) l3_array->data;
        l3_used = 1;
        /*<function call here>*/     
        
            double theta = X[0], phi = X[1], psi = X[2];
            mat_t Q = rotation(theta, phi, psi);
        
            m[0] = Q._[0];
            m[1] = Q._[3];
            m[2] = Q._[6];
        
            // Flip orientation if necessary.
            //if (m[0] * y[0] + m[1] * y[1] + m[2] * y[2] < 0) {
            //    for (int i = 0; i < 3; ++i)
            //        m[i] = -m[i];
            //}
        
            // Clamp lambdas.
            *l1 = max(X[3], 100);
            *l2 = max(X[4], 100);
            *l3 = max(X[5], 100);
        if(py_local_dict)                                  
        {                                                  
            py::dict local_dict = py::dict(py_local_dict); 
        }                                                  
    
    }                                
    catch(...)                       
    {                                
        return_val =  py::object();      
        exception_occured = 1;       
    }                                
    /*cleanup code*/                     
    if(X_used)
    {
        Py_XDECREF(py_X);
        #undef X1
        #undef X2
        #undef X3
        #undef X4
    }
    if(y_used)
    {
        Py_XDECREF(py_y);
        #undef Y1
        #undef Y2
        #undef Y3
        #undef Y4
    }
    if(m_used)
    {
        Py_XDECREF(py_m);
        #undef M1
        #undef M2
        #undef M3
        #undef M4
    }
    if(l1_used)
    {
        Py_XDECREF(py_l1);
        #undef L11
        #undef L12
        #undef L13
        #undef L14
    }
    if(l2_used)
    {
        Py_XDECREF(py_l2);
        #undef L21
        #undef L22
        #undef L23
        #undef L24
    }
    if(l3_used)
    {
        Py_XDECREF(py_l3);
        #undef L31
        #undef L32
        #undef L33
        #undef L34
    }
    if(!(PyObject*)return_val && !exception_occured)
    {
                                  
        return_val = Py_None;            
    }
                                  
    return return_val.disown();           
}                                
static PyObject* c_model_2tensor_f(PyObject*self, PyObject* args, PyObject* kywds)
{
    py::object return_val;
    int exception_occured = 0;
    PyObject *py_local_dict = NULL;
    static char *kwlist[] = {"X","m","local_dict", NULL};
    PyObject *py_X, *py_m;
    int X_used, m_used;
    py_X = py_m = NULL;
    X_used= m_used = 0;
    
    if(!PyArg_ParseTupleAndKeywords(args,kywds,"OO|O:c_model_2tensor_f",kwlist,&py_X, &py_m, &py_local_dict))
       return NULL;
    try                              
    {                                
        py_X = py_X;
        PyArrayObject* X_array = convert_to_numpy(py_X,"X");
        conversion_numpy_check_type(X_array,PyArray_DOUBLE,"X");
        #define X1(i) (*((double*)(X_array->data + (i)*SX[0])))
        #define X2(i,j) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1])))
        #define X3(i,j,k) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1] + (k)*SX[2])))
        #define X4(i,j,k,l) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1] + (k)*SX[2] + (l)*SX[3])))
        npy_intp* NX = X_array->dimensions;
        npy_intp* SX = X_array->strides;
        int DX = X_array->nd;
        double* X = (double*) X_array->data;
        X_used = 1;
        py_m = py_m;
        int m = convert_to_int(py_m,"m");
        m_used = 1;
        /*<function call here>*/     
        
            for (int i = 0; i < m; ++i) {
                double *o1 = X, *o2 = X + 5;
        
                // unpack and normalize orientations
                vec_t m1 = make_vec(o1[0], o1[1], o1[2]); m1 = m1 / norm(m1);
                vec_t m2 = make_vec(o2[0], o2[1], o2[2]); m2 = m2 / norm(m2);
                double l11 = o1[3], l12 = o1[4];
                double l21 = o2[3], l22 = o2[4];
            
                // ensure: lambda >= L
                double L = 100;
                if (l11 < L)   l11 = L;
                if (l12 < L)   l12 = L;
                if (l21 < L)   l21 = L;
                if (l22 < L)   l22 = L;
        
                // write back
                vec2mem(m1, o1);  vec2mem(m2, o2);
                o1[3] = l11; o2[3] = l21;
                o1[4] = l12; o2[4] = l22;
        
                // Prepare for next.
                X += 10;
            }
        if(py_local_dict)                                  
        {                                                  
            py::dict local_dict = py::dict(py_local_dict); 
        }                                                  
    
    }                                
    catch(...)                       
    {                                
        return_val =  py::object();      
        exception_occured = 1;       
    }                                
    /*cleanup code*/                     
    if(X_used)
    {
        Py_XDECREF(py_X);
        #undef X1
        #undef X2
        #undef X3
        #undef X4
    }
    if(!(PyObject*)return_val && !exception_occured)
    {
                                  
        return_val = Py_None;            
    }
                                  
    return return_val.disown();           
}                                
static PyObject* c_model_2tensor_full_f(PyObject*self, PyObject* args, PyObject* kywds)
{
    py::object return_val;
    int exception_occured = 0;
    PyObject *py_local_dict = NULL;
    static char *kwlist[] = {"X","m","local_dict", NULL};
    PyObject *py_X, *py_m;
    int X_used, m_used;
    py_X = py_m = NULL;
    X_used= m_used = 0;
    
    if(!PyArg_ParseTupleAndKeywords(args,kywds,"OO|O:c_model_2tensor_full_f",kwlist,&py_X, &py_m, &py_local_dict))
       return NULL;
    try                              
    {                                
        py_X = py_X;
        PyArrayObject* X_array = convert_to_numpy(py_X,"X");
        conversion_numpy_check_type(X_array,PyArray_DOUBLE,"X");
        #define X1(i) (*((double*)(X_array->data + (i)*SX[0])))
        #define X2(i,j) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1])))
        #define X3(i,j,k) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1] + (k)*SX[2])))
        #define X4(i,j,k,l) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1] + (k)*SX[2] + (l)*SX[3])))
        npy_intp* NX = X_array->dimensions;
        npy_intp* SX = X_array->strides;
        int DX = X_array->nd;
        double* X = (double*) X_array->data;
        X_used = 1;
        py_m = py_m;
        int m = convert_to_int(py_m,"m");
        m_used = 1;
        /*<function call here>*/     
        
            for (int i = 0; i < m; ++i) {
                // Clamp lambdas.
                double *o = X;
                o[3] = max(o[3], 100);
                o[4] = max(o[4], 100);
                o[5] = max(o[5], 100);
        
                o[9] = max(o[9], 100);
                o[10] = max(o[10], 100);
                o[11] = max(o[11], 100);
        
                // Prepare for next.
                X += 12;
            }
        if(py_local_dict)                                  
        {                                                  
            py::dict local_dict = py::dict(py_local_dict); 
        }                                                  
    
    }                                
    catch(...)                       
    {                                
        return_val =  py::object();      
        exception_occured = 1;       
    }                                
    /*cleanup code*/                     
    if(X_used)
    {
        Py_XDECREF(py_X);
        #undef X1
        #undef X2
        #undef X3
        #undef X4
    }
    if(!(PyObject*)return_val && !exception_occured)
    {
                                  
        return_val = Py_None;            
    }
                                  
    return return_val.disown();           
}                                
static PyObject* c_model_2tensor_h(PyObject*self, PyObject* args, PyObject* kywds)
{
    py::object return_val;
    int exception_occured = 0;
    PyObject *py_local_dict = NULL;
    static char *kwlist[] = {"s","X","u","b","n","m","local_dict", NULL};
    PyObject *py_s, *py_X, *py_u, *py_b, *py_n, *py_m;
    int s_used, X_used, u_used, b_used, n_used, m_used;
    py_s = py_X = py_u = py_b = py_n = py_m = NULL;
    s_used= X_used= u_used= b_used= n_used= m_used = 0;
    
    if(!PyArg_ParseTupleAndKeywords(args,kywds,"OOOOOO|O:c_model_2tensor_h",kwlist,&py_s, &py_X, &py_u, &py_b, &py_n, &py_m, &py_local_dict))
       return NULL;
    try                              
    {                                
        py_s = py_s;
        PyArrayObject* s_array = convert_to_numpy(py_s,"s");
        conversion_numpy_check_type(s_array,PyArray_DOUBLE,"s");
        #define S1(i) (*((double*)(s_array->data + (i)*Ss[0])))
        #define S2(i,j) (*((double*)(s_array->data + (i)*Ss[0] + (j)*Ss[1])))
        #define S3(i,j,k) (*((double*)(s_array->data + (i)*Ss[0] + (j)*Ss[1] + (k)*Ss[2])))
        #define S4(i,j,k,l) (*((double*)(s_array->data + (i)*Ss[0] + (j)*Ss[1] + (k)*Ss[2] + (l)*Ss[3])))
        npy_intp* Ns = s_array->dimensions;
        npy_intp* Ss = s_array->strides;
        int Ds = s_array->nd;
        double* s = (double*) s_array->data;
        s_used = 1;
        py_X = py_X;
        PyArrayObject* X_array = convert_to_numpy(py_X,"X");
        conversion_numpy_check_type(X_array,PyArray_DOUBLE,"X");
        #define X1(i) (*((double*)(X_array->data + (i)*SX[0])))
        #define X2(i,j) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1])))
        #define X3(i,j,k) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1] + (k)*SX[2])))
        #define X4(i,j,k,l) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1] + (k)*SX[2] + (l)*SX[3])))
        npy_intp* NX = X_array->dimensions;
        npy_intp* SX = X_array->strides;
        int DX = X_array->nd;
        double* X = (double*) X_array->data;
        X_used = 1;
        py_u = py_u;
        PyArrayObject* u_array = convert_to_numpy(py_u,"u");
        conversion_numpy_check_type(u_array,PyArray_DOUBLE,"u");
        #define U1(i) (*((double*)(u_array->data + (i)*Su[0])))
        #define U2(i,j) (*((double*)(u_array->data + (i)*Su[0] + (j)*Su[1])))
        #define U3(i,j,k) (*((double*)(u_array->data + (i)*Su[0] + (j)*Su[1] + (k)*Su[2])))
        #define U4(i,j,k,l) (*((double*)(u_array->data + (i)*Su[0] + (j)*Su[1] + (k)*Su[2] + (l)*Su[3])))
        npy_intp* Nu = u_array->dimensions;
        npy_intp* Su = u_array->strides;
        int Du = u_array->nd;
        double* u = (double*) u_array->data;
        u_used = 1;
        py_b = py_b;
        double b = convert_to_float(py_b,"b");
        b_used = 1;
        py_n = py_n;
        int n = convert_to_int(py_n,"n");
        n_used = 1;
        py_m = py_m;
        int m = convert_to_int(py_m,"m");
        m_used = 1;
        /*<function call here>*/     
        
            for (int i = 0; i < m; ++i) {
                double *o1 = X, *o2 = X + 5;
                // unpack and normalize orientations
                vec_t m1 = make_vec(o1[0], o1[1], o1[2]); m1 /= norm(m1);
                double l11 = o1[3], l12 = o1[4];
                vec_t m2 = make_vec(o2[0], o2[1], o2[2]); m2 /= norm(m2);
                double l21 = o2[3], l22 = o2[4];
        
                // ensure: lambda >= L
                double L = 100;
                if (l11 < L)   l11 = L;
                if (l12 < L)   l12 = L;
                if (l21 < L)   l21 = L;
                if (l22 < L)   l22 = L;
        
                // flip if necessary
                if (m1._[0] < 0)   m1 = -m1;
                if (m2._[0] < 0)   m2 = -m2;
        
                // calculate diffusion matrix
                mat_t D1 = diffusion(m1, l11, l12);
                mat_t D2 = diffusion(m2, l21, l22);
        
                // reconstruct signal
                for (int i = 0; i < n; ++i, ++s) {
                    vec_t u_ = make_vec(u[3*i], u[3*i+1], u[3*i+2]);
                    *s = (exp(-b*dot(u_,D1*u_)) + exp(-b*dot(u_,D2*u_)))/2;
                }
        
                // prepare for next
                X += 10;
            }
        if(py_local_dict)                                  
        {                                                  
            py::dict local_dict = py::dict(py_local_dict); 
        }                                                  
    
    }                                
    catch(...)                       
    {                                
        return_val =  py::object();      
        exception_occured = 1;       
    }                                
    /*cleanup code*/                     
    if(s_used)
    {
        Py_XDECREF(py_s);
        #undef S1
        #undef S2
        #undef S3
        #undef S4
    }
    if(X_used)
    {
        Py_XDECREF(py_X);
        #undef X1
        #undef X2
        #undef X3
        #undef X4
    }
    if(u_used)
    {
        Py_XDECREF(py_u);
        #undef U1
        #undef U2
        #undef U3
        #undef U4
    }
    if(!(PyObject*)return_val && !exception_occured)
    {
                                  
        return_val = Py_None;            
    }
                                  
    return return_val.disown();           
}                                
static PyObject* c_model_2tensor_full_h(PyObject*self, PyObject* args, PyObject* kywds)
{
    py::object return_val;
    int exception_occured = 0;
    PyObject *py_local_dict = NULL;
    static char *kwlist[] = {"s","X","u","b","n","m","local_dict", NULL};
    PyObject *py_s, *py_X, *py_u, *py_b, *py_n, *py_m;
    int s_used, X_used, u_used, b_used, n_used, m_used;
    py_s = py_X = py_u = py_b = py_n = py_m = NULL;
    s_used= X_used= u_used= b_used= n_used= m_used = 0;
    
    if(!PyArg_ParseTupleAndKeywords(args,kywds,"OOOOOO|O:c_model_2tensor_full_h",kwlist,&py_s, &py_X, &py_u, &py_b, &py_n, &py_m, &py_local_dict))
       return NULL;
    try                              
    {                                
        py_s = py_s;
        PyArrayObject* s_array = convert_to_numpy(py_s,"s");
        conversion_numpy_check_type(s_array,PyArray_DOUBLE,"s");
        #define S1(i) (*((double*)(s_array->data + (i)*Ss[0])))
        #define S2(i,j) (*((double*)(s_array->data + (i)*Ss[0] + (j)*Ss[1])))
        #define S3(i,j,k) (*((double*)(s_array->data + (i)*Ss[0] + (j)*Ss[1] + (k)*Ss[2])))
        #define S4(i,j,k,l) (*((double*)(s_array->data + (i)*Ss[0] + (j)*Ss[1] + (k)*Ss[2] + (l)*Ss[3])))
        npy_intp* Ns = s_array->dimensions;
        npy_intp* Ss = s_array->strides;
        int Ds = s_array->nd;
        double* s = (double*) s_array->data;
        s_used = 1;
        py_X = py_X;
        PyArrayObject* X_array = convert_to_numpy(py_X,"X");
        conversion_numpy_check_type(X_array,PyArray_DOUBLE,"X");
        #define X1(i) (*((double*)(X_array->data + (i)*SX[0])))
        #define X2(i,j) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1])))
        #define X3(i,j,k) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1] + (k)*SX[2])))
        #define X4(i,j,k,l) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1] + (k)*SX[2] + (l)*SX[3])))
        npy_intp* NX = X_array->dimensions;
        npy_intp* SX = X_array->strides;
        int DX = X_array->nd;
        double* X = (double*) X_array->data;
        X_used = 1;
        py_u = py_u;
        PyArrayObject* u_array = convert_to_numpy(py_u,"u");
        conversion_numpy_check_type(u_array,PyArray_DOUBLE,"u");
        #define U1(i) (*((double*)(u_array->data + (i)*Su[0])))
        #define U2(i,j) (*((double*)(u_array->data + (i)*Su[0] + (j)*Su[1])))
        #define U3(i,j,k) (*((double*)(u_array->data + (i)*Su[0] + (j)*Su[1] + (k)*Su[2])))
        #define U4(i,j,k,l) (*((double*)(u_array->data + (i)*Su[0] + (j)*Su[1] + (k)*Su[2] + (l)*Su[3])))
        npy_intp* Nu = u_array->dimensions;
        npy_intp* Su = u_array->strides;
        int Du = u_array->nd;
        double* u = (double*) u_array->data;
        u_used = 1;
        py_b = py_b;
        double b = convert_to_float(py_b,"b");
        b_used = 1;
        py_n = py_n;
        int n = convert_to_int(py_n,"n");
        n_used = 1;
        py_m = py_m;
        int m = convert_to_int(py_m,"m");
        m_used = 1;
        /*<function call here>*/     
        
            for (int i = 0; i < m; ++i) {
                // Unpack the data.
                double *o1 = X, *o2 = X + 6;
                double theta1 = o1[0], phi1 = o1[1], psi1 = o1[2];
                double theta2 = o2[0], phi2 = o2[1], psi2 = o2[2];
                double l11 = o1[3], l12 = o1[4], l13 = o1[5];
                double l21 = o2[3], l22 = o2[4], l23 = o2[5];
        
                // Ensure: lambda >= L.
                double L = 100;
                if (l11 < L)   l11 = L;
                if (l12 < L)   l12 = L;
                if (l13 < L)   l13 = L;
        
                if (l21 < L)   l21 = L;
                if (l22 < L)   l22 = L;
                if (l23 < L)   l23 = L;
        
                // Calculate diffusion matrix.
                mat_t D1 = diffusion_euler(theta1, phi1, psi1, l11, l12, l13);
                mat_t D2 = diffusion_euler(theta2, phi2, psi2, l21, l22, l23);
        
                // Reconstruct signal.
                for (int i = 0; i < n; ++i, ++s) {
                    vec_t u_ = make_vec(u[3 * i], u[3 * i + 1], u[3 * i + 2]);
                    *s = (exp(-b * dot(u_, D1 * u_)) + exp(-b * dot(u_, D2 * u_))) / 2.0;
                }
        
                // Prepare for next.
                X += 12;
            }
        if(py_local_dict)                                  
        {                                                  
            py::dict local_dict = py::dict(py_local_dict); 
        }                                                  
    
    }                                
    catch(...)                       
    {                                
        return_val =  py::object();      
        exception_occured = 1;       
    }                                
    /*cleanup code*/                     
    if(s_used)
    {
        Py_XDECREF(py_s);
        #undef S1
        #undef S2
        #undef S3
        #undef S4
    }
    if(X_used)
    {
        Py_XDECREF(py_X);
        #undef X1
        #undef X2
        #undef X3
        #undef X4
    }
    if(u_used)
    {
        Py_XDECREF(py_u);
        #undef U1
        #undef U2
        #undef U3
        #undef U4
    }
    if(!(PyObject*)return_val && !exception_occured)
    {
                                  
        return_val = Py_None;            
    }
                                  
    return return_val.disown();           
}                                
static PyObject* c_state2tensor2(PyObject*self, PyObject* args, PyObject* kywds)
{
    py::object return_val;
    int exception_occured = 0;
    PyObject *py_local_dict = NULL;
    static char *kwlist[] = {"X","y","m1","l11","l12","m2","l21","l22","local_dict", NULL};
    PyObject *py_X, *py_y, *py_m1, *py_l11, *py_l12, *py_m2, *py_l21, *py_l22;
    int X_used, y_used, m1_used, l11_used, l12_used, m2_used, l21_used, l22_used;
    py_X = py_y = py_m1 = py_l11 = py_l12 = py_m2 = py_l21 = py_l22 = NULL;
    X_used= y_used= m1_used= l11_used= l12_used= m2_used= l21_used= l22_used = 0;
    
    if(!PyArg_ParseTupleAndKeywords(args,kywds,"OOOOOOOO|O:c_state2tensor2",kwlist,&py_X, &py_y, &py_m1, &py_l11, &py_l12, &py_m2, &py_l21, &py_l22, &py_local_dict))
       return NULL;
    try                              
    {                                
        py_X = py_X;
        PyArrayObject* X_array = convert_to_numpy(py_X,"X");
        conversion_numpy_check_type(X_array,PyArray_DOUBLE,"X");
        #define X1(i) (*((double*)(X_array->data + (i)*SX[0])))
        #define X2(i,j) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1])))
        #define X3(i,j,k) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1] + (k)*SX[2])))
        #define X4(i,j,k,l) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1] + (k)*SX[2] + (l)*SX[3])))
        npy_intp* NX = X_array->dimensions;
        npy_intp* SX = X_array->strides;
        int DX = X_array->nd;
        double* X = (double*) X_array->data;
        X_used = 1;
        py_y = py_y;
        PyArrayObject* y_array = convert_to_numpy(py_y,"y");
        conversion_numpy_check_type(y_array,PyArray_DOUBLE,"y");
        #define Y1(i) (*((double*)(y_array->data + (i)*Sy[0])))
        #define Y2(i,j) (*((double*)(y_array->data + (i)*Sy[0] + (j)*Sy[1])))
        #define Y3(i,j,k) (*((double*)(y_array->data + (i)*Sy[0] + (j)*Sy[1] + (k)*Sy[2])))
        #define Y4(i,j,k,l) (*((double*)(y_array->data + (i)*Sy[0] + (j)*Sy[1] + (k)*Sy[2] + (l)*Sy[3])))
        npy_intp* Ny = y_array->dimensions;
        npy_intp* Sy = y_array->strides;
        int Dy = y_array->nd;
        double* y = (double*) y_array->data;
        y_used = 1;
        py_m1 = py_m1;
        PyArrayObject* m1_array = convert_to_numpy(py_m1,"m1");
        conversion_numpy_check_type(m1_array,PyArray_DOUBLE,"m1");
        #define M11(i) (*((double*)(m1_array->data + (i)*Sm1[0])))
        #define M12(i,j) (*((double*)(m1_array->data + (i)*Sm1[0] + (j)*Sm1[1])))
        #define M13(i,j,k) (*((double*)(m1_array->data + (i)*Sm1[0] + (j)*Sm1[1] + (k)*Sm1[2])))
        #define M14(i,j,k,l) (*((double*)(m1_array->data + (i)*Sm1[0] + (j)*Sm1[1] + (k)*Sm1[2] + (l)*Sm1[3])))
        npy_intp* Nm1 = m1_array->dimensions;
        npy_intp* Sm1 = m1_array->strides;
        int Dm1 = m1_array->nd;
        double* m1 = (double*) m1_array->data;
        m1_used = 1;
        py_l11 = py_l11;
        PyArrayObject* l11_array = convert_to_numpy(py_l11,"l11");
        conversion_numpy_check_type(l11_array,PyArray_DOUBLE,"l11");
        #define L111(i) (*((double*)(l11_array->data + (i)*Sl11[0])))
        #define L112(i,j) (*((double*)(l11_array->data + (i)*Sl11[0] + (j)*Sl11[1])))
        #define L113(i,j,k) (*((double*)(l11_array->data + (i)*Sl11[0] + (j)*Sl11[1] + (k)*Sl11[2])))
        #define L114(i,j,k,l) (*((double*)(l11_array->data + (i)*Sl11[0] + (j)*Sl11[1] + (k)*Sl11[2] + (l)*Sl11[3])))
        npy_intp* Nl11 = l11_array->dimensions;
        npy_intp* Sl11 = l11_array->strides;
        int Dl11 = l11_array->nd;
        double* l11 = (double*) l11_array->data;
        l11_used = 1;
        py_l12 = py_l12;
        PyArrayObject* l12_array = convert_to_numpy(py_l12,"l12");
        conversion_numpy_check_type(l12_array,PyArray_DOUBLE,"l12");
        #define L121(i) (*((double*)(l12_array->data + (i)*Sl12[0])))
        #define L122(i,j) (*((double*)(l12_array->data + (i)*Sl12[0] + (j)*Sl12[1])))
        #define L123(i,j,k) (*((double*)(l12_array->data + (i)*Sl12[0] + (j)*Sl12[1] + (k)*Sl12[2])))
        #define L124(i,j,k,l) (*((double*)(l12_array->data + (i)*Sl12[0] + (j)*Sl12[1] + (k)*Sl12[2] + (l)*Sl12[3])))
        npy_intp* Nl12 = l12_array->dimensions;
        npy_intp* Sl12 = l12_array->strides;
        int Dl12 = l12_array->nd;
        double* l12 = (double*) l12_array->data;
        l12_used = 1;
        py_m2 = py_m2;
        PyArrayObject* m2_array = convert_to_numpy(py_m2,"m2");
        conversion_numpy_check_type(m2_array,PyArray_DOUBLE,"m2");
        #define M21(i) (*((double*)(m2_array->data + (i)*Sm2[0])))
        #define M22(i,j) (*((double*)(m2_array->data + (i)*Sm2[0] + (j)*Sm2[1])))
        #define M23(i,j,k) (*((double*)(m2_array->data + (i)*Sm2[0] + (j)*Sm2[1] + (k)*Sm2[2])))
        #define M24(i,j,k,l) (*((double*)(m2_array->data + (i)*Sm2[0] + (j)*Sm2[1] + (k)*Sm2[2] + (l)*Sm2[3])))
        npy_intp* Nm2 = m2_array->dimensions;
        npy_intp* Sm2 = m2_array->strides;
        int Dm2 = m2_array->nd;
        double* m2 = (double*) m2_array->data;
        m2_used = 1;
        py_l21 = py_l21;
        PyArrayObject* l21_array = convert_to_numpy(py_l21,"l21");
        conversion_numpy_check_type(l21_array,PyArray_DOUBLE,"l21");
        #define L211(i) (*((double*)(l21_array->data + (i)*Sl21[0])))
        #define L212(i,j) (*((double*)(l21_array->data + (i)*Sl21[0] + (j)*Sl21[1])))
        #define L213(i,j,k) (*((double*)(l21_array->data + (i)*Sl21[0] + (j)*Sl21[1] + (k)*Sl21[2])))
        #define L214(i,j,k,l) (*((double*)(l21_array->data + (i)*Sl21[0] + (j)*Sl21[1] + (k)*Sl21[2] + (l)*Sl21[3])))
        npy_intp* Nl21 = l21_array->dimensions;
        npy_intp* Sl21 = l21_array->strides;
        int Dl21 = l21_array->nd;
        double* l21 = (double*) l21_array->data;
        l21_used = 1;
        py_l22 = py_l22;
        PyArrayObject* l22_array = convert_to_numpy(py_l22,"l22");
        conversion_numpy_check_type(l22_array,PyArray_DOUBLE,"l22");
        #define L221(i) (*((double*)(l22_array->data + (i)*Sl22[0])))
        #define L222(i,j) (*((double*)(l22_array->data + (i)*Sl22[0] + (j)*Sl22[1])))
        #define L223(i,j,k) (*((double*)(l22_array->data + (i)*Sl22[0] + (j)*Sl22[1] + (k)*Sl22[2])))
        #define L224(i,j,k,l) (*((double*)(l22_array->data + (i)*Sl22[0] + (j)*Sl22[1] + (k)*Sl22[2] + (l)*Sl22[3])))
        npy_intp* Nl22 = l22_array->dimensions;
        npy_intp* Sl22 = l22_array->strides;
        int Dl22 = l22_array->nd;
        double* l22 = (double*) l22_array->data;
        l22_used = 1;
        /*<function call here>*/     
        
            #line 0 "state2tensor2"
            //-- first orientation
            // unpack
            double m1_norm = 1e-10;
            for (int i = 0; i < 3; ++i) {
              double x = m1[i] = X[i];
              m1_norm += x * x;
            }
            // normalize
            m1_norm = sqrt(m1_norm);
            for (int i = 0; i < 3; ++i)
              m1[i] /= m1_norm;
        
            // flip orientation?
            if (m1[0]*y[0] + m1[1]*y[1] + m1[2]*y[2] < 0) {
                for (int i = 0; i < 3; ++i)
                    m1[i] = -m1[i];
            }
        
            // lambda (clamped)
            *l11 = max(X[3], 100);
            *l12 = max(X[4], 100);
        
        
        
            //-- second orientation
            X += 5;
            // unpack
            double m2_norm = 1e-10;
            for (int i = 0; i < 3; ++i) {
              double x = m2[i] = X[i];
              m2_norm += x * x;
            }
            // normalize
            m2_norm = sqrt(m2_norm);
            for (int i = 0; i < 3; ++i)
              m2[i] /= m2_norm;
        
            // flip orientation?
            if (m2[0]*y[0] + m2[1]*y[1] + m2[2]*y[2] < 0) {
                for (int i = 0; i < 3; ++i)
                    m2[i] = -m2[i];
            }
        
            // lambda (clamped)
            *l21 = max(X[3], 100);
            *l22 = max(X[4], 100);
        if(py_local_dict)                                  
        {                                                  
            py::dict local_dict = py::dict(py_local_dict); 
        }                                                  
    
    }                                
    catch(...)                       
    {                                
        return_val =  py::object();      
        exception_occured = 1;       
    }                                
    /*cleanup code*/                     
    if(X_used)
    {
        Py_XDECREF(py_X);
        #undef X1
        #undef X2
        #undef X3
        #undef X4
    }
    if(y_used)
    {
        Py_XDECREF(py_y);
        #undef Y1
        #undef Y2
        #undef Y3
        #undef Y4
    }
    if(m1_used)
    {
        Py_XDECREF(py_m1);
        #undef M11
        #undef M12
        #undef M13
        #undef M14
    }
    if(l11_used)
    {
        Py_XDECREF(py_l11);
        #undef L111
        #undef L112
        #undef L113
        #undef L114
    }
    if(l12_used)
    {
        Py_XDECREF(py_l12);
        #undef L121
        #undef L122
        #undef L123
        #undef L124
    }
    if(m2_used)
    {
        Py_XDECREF(py_m2);
        #undef M21
        #undef M22
        #undef M23
        #undef M24
    }
    if(l21_used)
    {
        Py_XDECREF(py_l21);
        #undef L211
        #undef L212
        #undef L213
        #undef L214
    }
    if(l22_used)
    {
        Py_XDECREF(py_l22);
        #undef L221
        #undef L222
        #undef L223
        #undef L224
    }
    if(!(PyObject*)return_val && !exception_occured)
    {
                                  
        return_val = Py_None;            
    }
                                  
    return return_val.disown();           
}                                
static PyObject* c_state2tensor2_full(PyObject*self, PyObject* args, PyObject* kywds)
{
    py::object return_val;
    int exception_occured = 0;
    PyObject *py_local_dict = NULL;
    static char *kwlist[] = {"X","y","m1","l11","l12","l13","m2","l21","l22","l23","local_dict", NULL};
    PyObject *py_X, *py_y, *py_m1, *py_l11, *py_l12, *py_l13, *py_m2, *py_l21, *py_l22, *py_l23;
    int X_used, y_used, m1_used, l11_used, l12_used, l13_used, m2_used, l21_used, l22_used, l23_used;
    py_X = py_y = py_m1 = py_l11 = py_l12 = py_l13 = py_m2 = py_l21 = py_l22 = py_l23 = NULL;
    X_used= y_used= m1_used= l11_used= l12_used= l13_used= m2_used= l21_used= l22_used= l23_used = 0;
    
    if(!PyArg_ParseTupleAndKeywords(args,kywds,"OOOOOOOOOO|O:c_state2tensor2_full",kwlist,&py_X, &py_y, &py_m1, &py_l11, &py_l12, &py_l13, &py_m2, &py_l21, &py_l22, &py_l23, &py_local_dict))
       return NULL;
    try                              
    {                                
        py_X = py_X;
        PyArrayObject* X_array = convert_to_numpy(py_X,"X");
        conversion_numpy_check_type(X_array,PyArray_DOUBLE,"X");
        #define X1(i) (*((double*)(X_array->data + (i)*SX[0])))
        #define X2(i,j) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1])))
        #define X3(i,j,k) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1] + (k)*SX[2])))
        #define X4(i,j,k,l) (*((double*)(X_array->data + (i)*SX[0] + (j)*SX[1] + (k)*SX[2] + (l)*SX[3])))
        npy_intp* NX = X_array->dimensions;
        npy_intp* SX = X_array->strides;
        int DX = X_array->nd;
        double* X = (double*) X_array->data;
        X_used = 1;
        py_y = py_y;
        PyArrayObject* y_array = convert_to_numpy(py_y,"y");
        conversion_numpy_check_type(y_array,PyArray_DOUBLE,"y");
        #define Y1(i) (*((double*)(y_array->data + (i)*Sy[0])))
        #define Y2(i,j) (*((double*)(y_array->data + (i)*Sy[0] + (j)*Sy[1])))
        #define Y3(i,j,k) (*((double*)(y_array->data + (i)*Sy[0] + (j)*Sy[1] + (k)*Sy[2])))
        #define Y4(i,j,k,l) (*((double*)(y_array->data + (i)*Sy[0] + (j)*Sy[1] + (k)*Sy[2] + (l)*Sy[3])))
        npy_intp* Ny = y_array->dimensions;
        npy_intp* Sy = y_array->strides;
        int Dy = y_array->nd;
        double* y = (double*) y_array->data;
        y_used = 1;
        py_m1 = py_m1;
        PyArrayObject* m1_array = convert_to_numpy(py_m1,"m1");
        conversion_numpy_check_type(m1_array,PyArray_DOUBLE,"m1");
        #define M11(i) (*((double*)(m1_array->data + (i)*Sm1[0])))
        #define M12(i,j) (*((double*)(m1_array->data + (i)*Sm1[0] + (j)*Sm1[1])))
        #define M13(i,j,k) (*((double*)(m1_array->data + (i)*Sm1[0] + (j)*Sm1[1] + (k)*Sm1[2])))
        #define M14(i,j,k,l) (*((double*)(m1_array->data + (i)*Sm1[0] + (j)*Sm1[1] + (k)*Sm1[2] + (l)*Sm1[3])))
        npy_intp* Nm1 = m1_array->dimensions;
        npy_intp* Sm1 = m1_array->strides;
        int Dm1 = m1_array->nd;
        double* m1 = (double*) m1_array->data;
        m1_used = 1;
        py_l11 = py_l11;
        PyArrayObject* l11_array = convert_to_numpy(py_l11,"l11");
        conversion_numpy_check_type(l11_array,PyArray_DOUBLE,"l11");
        #define L111(i) (*((double*)(l11_array->data + (i)*Sl11[0])))
        #define L112(i,j) (*((double*)(l11_array->data + (i)*Sl11[0] + (j)*Sl11[1])))
        #define L113(i,j,k) (*((double*)(l11_array->data + (i)*Sl11[0] + (j)*Sl11[1] + (k)*Sl11[2])))
        #define L114(i,j,k,l) (*((double*)(l11_array->data + (i)*Sl11[0] + (j)*Sl11[1] + (k)*Sl11[2] + (l)*Sl11[3])))
        npy_intp* Nl11 = l11_array->dimensions;
        npy_intp* Sl11 = l11_array->strides;
        int Dl11 = l11_array->nd;
        double* l11 = (double*) l11_array->data;
        l11_used = 1;
        py_l12 = py_l12;
        PyArrayObject* l12_array = convert_to_numpy(py_l12,"l12");
        conversion_numpy_check_type(l12_array,PyArray_DOUBLE,"l12");
        #define L121(i) (*((double*)(l12_array->data + (i)*Sl12[0])))
        #define L122(i,j) (*((double*)(l12_array->data + (i)*Sl12[0] + (j)*Sl12[1])))
        #define L123(i,j,k) (*((double*)(l12_array->data + (i)*Sl12[0] + (j)*Sl12[1] + (k)*Sl12[2])))
        #define L124(i,j,k,l) (*((double*)(l12_array->data + (i)*Sl12[0] + (j)*Sl12[1] + (k)*Sl12[2] + (l)*Sl12[3])))
        npy_intp* Nl12 = l12_array->dimensions;
        npy_intp* Sl12 = l12_array->strides;
        int Dl12 = l12_array->nd;
        double* l12 = (double*) l12_array->data;
        l12_used = 1;
        py_l13 = py_l13;
        PyArrayObject* l13_array = convert_to_numpy(py_l13,"l13");
        conversion_numpy_check_type(l13_array,PyArray_DOUBLE,"l13");
        #define L131(i) (*((double*)(l13_array->data + (i)*Sl13[0])))
        #define L132(i,j) (*((double*)(l13_array->data + (i)*Sl13[0] + (j)*Sl13[1])))
        #define L133(i,j,k) (*((double*)(l13_array->data + (i)*Sl13[0] + (j)*Sl13[1] + (k)*Sl13[2])))
        #define L134(i,j,k,l) (*((double*)(l13_array->data + (i)*Sl13[0] + (j)*Sl13[1] + (k)*Sl13[2] + (l)*Sl13[3])))
        npy_intp* Nl13 = l13_array->dimensions;
        npy_intp* Sl13 = l13_array->strides;
        int Dl13 = l13_array->nd;
        double* l13 = (double*) l13_array->data;
        l13_used = 1;
        py_m2 = py_m2;
        PyArrayObject* m2_array = convert_to_numpy(py_m2,"m2");
        conversion_numpy_check_type(m2_array,PyArray_DOUBLE,"m2");
        #define M21(i) (*((double*)(m2_array->data + (i)*Sm2[0])))
        #define M22(i,j) (*((double*)(m2_array->data + (i)*Sm2[0] + (j)*Sm2[1])))
        #define M23(i,j,k) (*((double*)(m2_array->data + (i)*Sm2[0] + (j)*Sm2[1] + (k)*Sm2[2])))
        #define M24(i,j,k,l) (*((double*)(m2_array->data + (i)*Sm2[0] + (j)*Sm2[1] + (k)*Sm2[2] + (l)*Sm2[3])))
        npy_intp* Nm2 = m2_array->dimensions;
        npy_intp* Sm2 = m2_array->strides;
        int Dm2 = m2_array->nd;
        double* m2 = (double*) m2_array->data;
        m2_used = 1;
        py_l21 = py_l21;
        PyArrayObject* l21_array = convert_to_numpy(py_l21,"l21");
        conversion_numpy_check_type(l21_array,PyArray_DOUBLE,"l21");
        #define L211(i) (*((double*)(l21_array->data + (i)*Sl21[0])))
        #define L212(i,j) (*((double*)(l21_array->data + (i)*Sl21[0] + (j)*Sl21[1])))
        #define L213(i,j,k) (*((double*)(l21_array->data + (i)*Sl21[0] + (j)*Sl21[1] + (k)*Sl21[2])))
        #define L214(i,j,k,l) (*((double*)(l21_array->data + (i)*Sl21[0] + (j)*Sl21[1] + (k)*Sl21[2] + (l)*Sl21[3])))
        npy_intp* Nl21 = l21_array->dimensions;
        npy_intp* Sl21 = l21_array->strides;
        int Dl21 = l21_array->nd;
        double* l21 = (double*) l21_array->data;
        l21_used = 1;
        py_l22 = py_l22;
        PyArrayObject* l22_array = convert_to_numpy(py_l22,"l22");
        conversion_numpy_check_type(l22_array,PyArray_DOUBLE,"l22");
        #define L221(i) (*((double*)(l22_array->data + (i)*Sl22[0])))
        #define L222(i,j) (*((double*)(l22_array->data + (i)*Sl22[0] + (j)*Sl22[1])))
        #define L223(i,j,k) (*((double*)(l22_array->data + (i)*Sl22[0] + (j)*Sl22[1] + (k)*Sl22[2])))
        #define L224(i,j,k,l) (*((double*)(l22_array->data + (i)*Sl22[0] + (j)*Sl22[1] + (k)*Sl22[2] + (l)*Sl22[3])))
        npy_intp* Nl22 = l22_array->dimensions;
        npy_intp* Sl22 = l22_array->strides;
        int Dl22 = l22_array->nd;
        double* l22 = (double*) l22_array->data;
        l22_used = 1;
        py_l23 = py_l23;
        PyArrayObject* l23_array = convert_to_numpy(py_l23,"l23");
        conversion_numpy_check_type(l23_array,PyArray_DOUBLE,"l23");
        #define L231(i) (*((double*)(l23_array->data + (i)*Sl23[0])))
        #define L232(i,j) (*((double*)(l23_array->data + (i)*Sl23[0] + (j)*Sl23[1])))
        #define L233(i,j,k) (*((double*)(l23_array->data + (i)*Sl23[0] + (j)*Sl23[1] + (k)*Sl23[2])))
        #define L234(i,j,k,l) (*((double*)(l23_array->data + (i)*Sl23[0] + (j)*Sl23[1] + (k)*Sl23[2] + (l)*Sl23[3])))
        npy_intp* Nl23 = l23_array->dimensions;
        npy_intp* Sl23 = l23_array->strides;
        int Dl23 = l23_array->nd;
        double* l23 = (double*) l23_array->data;
        l23_used = 1;
        /*<function call here>*/     
        
            // First orientation.
            double theta = X[0], phi = X[1], psi = X[2];
            mat_t Q = rotation(theta, phi, psi);
        
            m1[0] = Q._[0];
            m1[1] = Q._[3];
            m1[2] = Q._[6];
        
            // Flip orientation if necessary.
            if (m1[0] * y[0] + m1[1] * y[1] + m1[2] * y[2] < 0) {
                for (int i = 0; i < 3; ++i)
                    m1[i] = -m1[i];
            }
        
            // Clamp lambdas.
            *l11 = max(X[3], 100);
            *l12 = max(X[4], 100);
            *l13 = max(X[5], 100);
        
            // Second Orientation.
            X += 6;
            theta = X[0];
            phi = X[1];
            psi = X[2];
            Q = rotation(theta, phi, psi);
        
            m2[0] = Q._[0];
            m2[1] = Q._[3];
            m2[2] = Q._[6];
        
            // Flip orientation if necessary.
            if (m2[0] * y[0] + m2[1] * y[1] + m2[2] * y[2] < 0) {
                for (int i = 0; i < 3; ++i)
                    m2[i] = -m2[i];
            }
        
            // Clamp lambdas.
            *l21 = max(X[3], 100);
            *l22 = max(X[4], 100);
            *l23 = max(X[5], 100);
        if(py_local_dict)                                  
        {                                                  
            py::dict local_dict = py::dict(py_local_dict); 
        }                                                  
    
    }                                
    catch(...)                       
    {                                
        return_val =  py::object();      
        exception_occured = 1;       
    }                                
    /*cleanup code*/                     
    if(X_used)
    {
        Py_XDECREF(py_X);
        #undef X1
        #undef X2
        #undef X3
        #undef X4
    }
    if(y_used)
    {
        Py_XDECREF(py_y);
        #undef Y1
        #undef Y2
        #undef Y3
        #undef Y4
    }
    if(m1_used)
    {
        Py_XDECREF(py_m1);
        #undef M11
        #undef M12
        #undef M13
        #undef M14
    }
    if(l11_used)
    {
        Py_XDECREF(py_l11);
        #undef L111
        #undef L112
        #undef L113
        #undef L114
    }
    if(l12_used)
    {
        Py_XDECREF(py_l12);
        #undef L121
        #undef L122
        #undef L123
        #undef L124
    }
    if(l13_used)
    {
        Py_XDECREF(py_l13);
        #undef L131
        #undef L132
        #undef L133
        #undef L134
    }
    if(m2_used)
    {
        Py_XDECREF(py_m2);
        #undef M21
        #undef M22
        #undef M23
        #undef M24
    }
    if(l21_used)
    {
        Py_XDECREF(py_l21);
        #undef L211
        #undef L212
        #undef L213
        #undef L214
    }
    if(l22_used)
    {
        Py_XDECREF(py_l22);
        #undef L221
        #undef L222
        #undef L223
        #undef L224
    }
    if(l23_used)
    {
        Py_XDECREF(py_l23);
        #undef L231
        #undef L232
        #undef L233
        #undef L234
    }
    if(!(PyObject*)return_val && !exception_occured)
    {
                                  
        return_val = Py_None;            
    }
                                  
    return return_val.disown();           
}                                
static PyObject* c_interp3signal(PyObject*self, PyObject* args, PyObject* kywds)
{
    py::object return_val;
    int exception_occured = 0;
    PyObject *py_local_dict = NULL;
    static char *kwlist[] = {"s","S","p","v","sigma","nx","ny","nz","n","local_dict", NULL};
    PyObject *py_s, *py_S, *py_p, *py_v, *py_sigma, *py_nx, *py_ny, *py_nz, *py_n;
    int s_used, S_used, p_used, v_used, sigma_used, nx_used, ny_used, nz_used, n_used;
    py_s = py_S = py_p = py_v = py_sigma = py_nx = py_ny = py_nz = py_n = NULL;
    s_used= S_used= p_used= v_used= sigma_used= nx_used= ny_used= nz_used= n_used = 0;
    
    if(!PyArg_ParseTupleAndKeywords(args,kywds,"OOOOOOOOO|O:c_interp3signal",kwlist,&py_s, &py_S, &py_p, &py_v, &py_sigma, &py_nx, &py_ny, &py_nz, &py_n, &py_local_dict))
       return NULL;
    try                              
    {                                
        py_s = py_s;
        PyArrayObject* s_array = convert_to_numpy(py_s,"s");
        conversion_numpy_check_type(s_array,PyArray_FLOAT,"s");
        #define S1(i) (*((float*)(s_array->data + (i)*Ss[0])))
        #define S2(i,j) (*((float*)(s_array->data + (i)*Ss[0] + (j)*Ss[1])))
        #define S3(i,j,k) (*((float*)(s_array->data + (i)*Ss[0] + (j)*Ss[1] + (k)*Ss[2])))
        #define S4(i,j,k,l) (*((float*)(s_array->data + (i)*Ss[0] + (j)*Ss[1] + (k)*Ss[2] + (l)*Ss[3])))
        npy_intp* Ns = s_array->dimensions;
        npy_intp* Ss = s_array->strides;
        int Ds = s_array->nd;
        float* s = (float*) s_array->data;
        s_used = 1;
        py_S = py_S;
        PyArrayObject* S_array = convert_to_numpy(py_S,"S");
        conversion_numpy_check_type(S_array,PyArray_FLOAT,"S");
        #define S1(i) (*((float*)(S_array->data + (i)*SS[0])))
        #define S2(i,j) (*((float*)(S_array->data + (i)*SS[0] + (j)*SS[1])))
        #define S3(i,j,k) (*((float*)(S_array->data + (i)*SS[0] + (j)*SS[1] + (k)*SS[2])))
        #define S4(i,j,k,l) (*((float*)(S_array->data + (i)*SS[0] + (j)*SS[1] + (k)*SS[2] + (l)*SS[3])))
        npy_intp* NS = S_array->dimensions;
        npy_intp* SS = S_array->strides;
        int DS = S_array->nd;
        float* S = (float*) S_array->data;
        S_used = 1;
        py_p = py_p;
        PyArrayObject* p_array = convert_to_numpy(py_p,"p");
        conversion_numpy_check_type(p_array,PyArray_DOUBLE,"p");
        #define P1(i) (*((double*)(p_array->data + (i)*Sp[0])))
        #define P2(i,j) (*((double*)(p_array->data + (i)*Sp[0] + (j)*Sp[1])))
        #define P3(i,j,k) (*((double*)(p_array->data + (i)*Sp[0] + (j)*Sp[1] + (k)*Sp[2])))
        #define P4(i,j,k,l) (*((double*)(p_array->data + (i)*Sp[0] + (j)*Sp[1] + (k)*Sp[2] + (l)*Sp[3])))
        npy_intp* Np = p_array->dimensions;
        npy_intp* Sp = p_array->strides;
        int Dp = p_array->nd;
        double* p = (double*) p_array->data;
        p_used = 1;
        py_v = py_v;
        PyArrayObject* v_array = convert_to_numpy(py_v,"v");
        conversion_numpy_check_type(v_array,PyArray_DOUBLE,"v");
        #define V1(i) (*((double*)(v_array->data + (i)*Sv[0])))
        #define V2(i,j) (*((double*)(v_array->data + (i)*Sv[0] + (j)*Sv[1])))
        #define V3(i,j,k) (*((double*)(v_array->data + (i)*Sv[0] + (j)*Sv[1] + (k)*Sv[2])))
        #define V4(i,j,k,l) (*((double*)(v_array->data + (i)*Sv[0] + (j)*Sv[1] + (k)*Sv[2] + (l)*Sv[3])))
        npy_intp* Nv = v_array->dimensions;
        npy_intp* Sv = v_array->strides;
        int Dv = v_array->nd;
        double* v = (double*) v_array->data;
        v_used = 1;
        py_sigma = py_sigma;
        double sigma = convert_to_float(py_sigma,"sigma");
        sigma_used = 1;
        py_nx = py_nx;
        int nx = convert_to_int(py_nx,"nx");
        nx_used = 1;
        py_ny = py_ny;
        int ny = convert_to_int(py_ny,"ny");
        ny_used = 1;
        py_nz = py_nz;
        int nz = convert_to_int(py_nz,"nz");
        nz_used = 1;
        py_n = py_n;
        int n = convert_to_int(py_n,"n");
        n_used = 1;
        /*<function call here>*/     
        
                   #line 0 "interp3signal"
                   double w_sum = 1e-16;
                   double px = p[0], py = p[1], pz = p[2];
                   double vx = v[0], vy = v[1], vz = v[2];
        
                   for (int i = 0; i < 2*n; ++i)
                       s[i] = 0;
        
                   for (int zz = -1; zz <= 1; ++zz) {
                       int z = round(pz) + zz;
                       if (z < 0 || nz <= z) continue;
                       double dz = (z - pz)*vz, dzz = dz*dz;
        
                       for (int yy = -1; yy <= 1; ++yy) {
                           int y = round(py) + yy;
                           if (y < 0 || ny <= y) continue;
                           double dy = (y - py)*vy, dyy = dy*dy;
        
                           for (int xx = -1; xx <= 1; ++xx) {
                               int x = round(px) + xx;
                               if (x < 0 || nx <= x) continue;
                               double dx = (x - px)*vx, dxx = dx*dx;
                               double w = exp( -(dxx + dyy + dzz)/sigma );
        
                               for (int i = 0; i < n; ++i)
                                   s[i] += w * S[(nz*ny*x + nz*y + z)*n + i];
        
                               w_sum += w;
                           }
                       }
                   }
        
                   w_sum *= 2; // double each occurance
                   for (int i = 0; i < n; ++i) {
                       s[i  ] /= w_sum;
                       s[i+n]  = s[i]; // push into second spot
                   }
        
        if(py_local_dict)                                  
        {                                                  
            py::dict local_dict = py::dict(py_local_dict); 
        }                                                  
    
    }                                
    catch(...)                       
    {                                
        return_val =  py::object();      
        exception_occured = 1;       
    }                                
    /*cleanup code*/                     
    if(s_used)
    {
        Py_XDECREF(py_s);
        #undef S1
        #undef S2
        #undef S3
        #undef S4
    }
    if(S_used)
    {
        Py_XDECREF(py_S);
        #undef S1
        #undef S2
        #undef S3
        #undef S4
    }
    if(p_used)
    {
        Py_XDECREF(py_p);
        #undef P1
        #undef P2
        #undef P3
        #undef P4
    }
    if(v_used)
    {
        Py_XDECREF(py_v);
        #undef V1
        #undef V2
        #undef V3
        #undef V4
    }
    if(!(PyObject*)return_val && !exception_occured)
    {
                                  
        return_val = Py_None;            
    }
                                  
    return return_val.disown();           
}                                
static PyObject* c_interp3scalar(PyObject*self, PyObject* args, PyObject* kywds)
{
    py::object return_val;
    int exception_occured = 0;
    PyObject *py_local_dict = NULL;
    static char *kwlist[] = {"M","p","v","sigma","nx","ny","nz","local_dict", NULL};
    PyObject *py_M, *py_p, *py_v, *py_sigma, *py_nx, *py_ny, *py_nz;
    int M_used, p_used, v_used, sigma_used, nx_used, ny_used, nz_used;
    py_M = py_p = py_v = py_sigma = py_nx = py_ny = py_nz = NULL;
    M_used= p_used= v_used= sigma_used= nx_used= ny_used= nz_used = 0;
    
    if(!PyArg_ParseTupleAndKeywords(args,kywds,"OOOOOOO|O:c_interp3scalar",kwlist,&py_M, &py_p, &py_v, &py_sigma, &py_nx, &py_ny, &py_nz, &py_local_dict))
       return NULL;
    try                              
    {                                
        py_M = py_M;
        PyArrayObject* M_array = convert_to_numpy(py_M,"M");
        conversion_numpy_check_type(M_array,PyArray_USHORT,"M");
        #define M1(i) (*((npy_ushort*)(M_array->data + (i)*SM[0])))
        #define M2(i,j) (*((npy_ushort*)(M_array->data + (i)*SM[0] + (j)*SM[1])))
        #define M3(i,j,k) (*((npy_ushort*)(M_array->data + (i)*SM[0] + (j)*SM[1] + (k)*SM[2])))
        #define M4(i,j,k,l) (*((npy_ushort*)(M_array->data + (i)*SM[0] + (j)*SM[1] + (k)*SM[2] + (l)*SM[3])))
        npy_intp* NM = M_array->dimensions;
        npy_intp* SM = M_array->strides;
        int DM = M_array->nd;
        npy_ushort* M = (npy_ushort*) M_array->data;
        M_used = 1;
        py_p = py_p;
        PyArrayObject* p_array = convert_to_numpy(py_p,"p");
        conversion_numpy_check_type(p_array,PyArray_DOUBLE,"p");
        #define P1(i) (*((double*)(p_array->data + (i)*Sp[0])))
        #define P2(i,j) (*((double*)(p_array->data + (i)*Sp[0] + (j)*Sp[1])))
        #define P3(i,j,k) (*((double*)(p_array->data + (i)*Sp[0] + (j)*Sp[1] + (k)*Sp[2])))
        #define P4(i,j,k,l) (*((double*)(p_array->data + (i)*Sp[0] + (j)*Sp[1] + (k)*Sp[2] + (l)*Sp[3])))
        npy_intp* Np = p_array->dimensions;
        npy_intp* Sp = p_array->strides;
        int Dp = p_array->nd;
        double* p = (double*) p_array->data;
        p_used = 1;
        py_v = py_v;
        PyArrayObject* v_array = convert_to_numpy(py_v,"v");
        conversion_numpy_check_type(v_array,PyArray_DOUBLE,"v");
        #define V1(i) (*((double*)(v_array->data + (i)*Sv[0])))
        #define V2(i,j) (*((double*)(v_array->data + (i)*Sv[0] + (j)*Sv[1])))
        #define V3(i,j,k) (*((double*)(v_array->data + (i)*Sv[0] + (j)*Sv[1] + (k)*Sv[2])))
        #define V4(i,j,k,l) (*((double*)(v_array->data + (i)*Sv[0] + (j)*Sv[1] + (k)*Sv[2] + (l)*Sv[3])))
        npy_intp* Nv = v_array->dimensions;
        npy_intp* Sv = v_array->strides;
        int Dv = v_array->nd;
        double* v = (double*) v_array->data;
        v_used = 1;
        py_sigma = py_sigma;
        double sigma = convert_to_float(py_sigma,"sigma");
        sigma_used = 1;
        py_nx = py_nx;
        int nx = convert_to_int(py_nx,"nx");
        nx_used = 1;
        py_ny = py_ny;
        int ny = convert_to_int(py_ny,"ny");
        ny_used = 1;
        py_nz = py_nz;
        int nz = convert_to_int(py_nz,"nz");
        nz_used = 1;
        /*<function call here>*/     
        
                   #line 0 "interp3scalar"
                   double s = 0, w_sum = 1e-16;
                   double px = p[0], py = p[1], pz = p[2];
                   double vx = v[0], vy = v[1], vz = v[2];
        
                   for (int zz = -1; zz <= 1; zz++) {
                       int z = round(pz) + zz;
                       if (z < 0 || nz <= z) continue;
                       double dz = (z - pz)*vz, dzz = dz*dz;
        
                       for (int yy = -1; yy <= 1; yy++) {
                           int y = round(py) + yy;
                           if (y < 0 || ny <= y) continue;
                           double dy = (y - py)*vy, dyy = dy*dy;
        
                           for (int xx = -1; xx <= 1; xx++) {
                               int x = round(px) + xx;
                               if (x < 0 || nx <= x) continue;
                               double dx = (x - px)*vx, dxx = dx*dx;
        
                               float w = exp( -(dxx + dyy + dzz)/sigma );
                               if (M[nz*ny*x + nz*y + z]) {
                                 s += w;
                               }
                               //float d = M[nz*ny*x + nz*y + z];
                               //s     += w * d;
                               w_sum += w;
                           }
                       }
                   }
                   return_val = s / w_sum;
        if(py_local_dict)                                  
        {                                                  
            py::dict local_dict = py::dict(py_local_dict); 
        }                                                  
    
    }                                
    catch(...)                       
    {                                
        return_val =  py::object();      
        exception_occured = 1;       
    }                                
    /*cleanup code*/                     
    if(M_used)
    {
        Py_XDECREF(py_M);
        #undef M1
        #undef M2
        #undef M3
        #undef M4
    }
    if(p_used)
    {
        Py_XDECREF(py_p);
        #undef P1
        #undef P2
        #undef P3
        #undef P4
    }
    if(v_used)
    {
        Py_XDECREF(py_v);
        #undef V1
        #undef V2
        #undef V3
        #undef V4
    }
    if(!(PyObject*)return_val && !exception_occured)
    {
                                  
        return_val = Py_None;            
    }
                                  
    return return_val.disown();           
}                                
static PyObject* c_s2ga(PyObject*self, PyObject* args, PyObject* kywds)
{
    py::object return_val;
    int exception_occured = 0;
    PyObject *py_local_dict = NULL;
    static char *kwlist[] = {"s","n","local_dict", NULL};
    PyObject *py_s, *py_n;
    int s_used, n_used;
    py_s = py_n = NULL;
    s_used= n_used = 0;
    
    if(!PyArg_ParseTupleAndKeywords(args,kywds,"OO|O:c_s2ga",kwlist,&py_s, &py_n, &py_local_dict))
       return NULL;
    try                              
    {                                
        py_s = py_s;
        PyArrayObject* s_array = convert_to_numpy(py_s,"s");
        conversion_numpy_check_type(s_array,PyArray_DOUBLE,"s");
        #define S1(i) (*((double*)(s_array->data + (i)*Ss[0])))
        #define S2(i,j) (*((double*)(s_array->data + (i)*Ss[0] + (j)*Ss[1])))
        #define S3(i,j,k) (*((double*)(s_array->data + (i)*Ss[0] + (j)*Ss[1] + (k)*Ss[2])))
        #define S4(i,j,k,l) (*((double*)(s_array->data + (i)*Ss[0] + (j)*Ss[1] + (k)*Ss[2] + (l)*Ss[3])))
        npy_intp* Ns = s_array->dimensions;
        npy_intp* Ss = s_array->strides;
        int Ds = s_array->nd;
        double* s = (double*) s_array->data;
        s_used = 1;
        py_n = py_n;
        int n = convert_to_int(py_n,"n");
        n_used = 1;
        /*<function call here>*/     
        
                   #line 0 "s2ga"
                   double mu = 0, mu_sq = 0;
                   for (int i = 0; i < n; ++i, ++s) {
                       mu    += *s;
                       mu_sq += *s * *s;
                   }
                   mu    /= n;
                   mu_sq /= n;
                   double ga = sqrt(mu_sq - mu*mu) / sqrt(mu_sq);
                   return_val = ga;
        if(py_local_dict)                                  
        {                                                  
            py::dict local_dict = py::dict(py_local_dict); 
        }                                                  
    
    }                                
    catch(...)                       
    {                                
        return_val =  py::object();      
        exception_occured = 1;       
    }                                
    /*cleanup code*/                     
    if(s_used)
    {
        Py_XDECREF(py_s);
        #undef S1
        #undef S2
        #undef S3
        #undef S4
    }
    if(!(PyObject*)return_val && !exception_occured)
    {
                                  
        return_val = Py_None;            
    }
                                  
    return return_val.disown();           
}                                


static PyMethodDef compiled_methods[] = 
{
    {"c_model_1tensor_f",(PyCFunction)c_model_1tensor_f , METH_VARARGS|METH_KEYWORDS},
    {"c_model_1tensor_full_f",(PyCFunction)c_model_1tensor_full_f , METH_VARARGS|METH_KEYWORDS},
    {"c_model_1tensor_h",(PyCFunction)c_model_1tensor_h , METH_VARARGS|METH_KEYWORDS},
    {"c_model_1tensor_full_h",(PyCFunction)c_model_1tensor_full_h , METH_VARARGS|METH_KEYWORDS},
    {"c_state2tensor1",(PyCFunction)c_state2tensor1 , METH_VARARGS|METH_KEYWORDS},
    {"c_state2tensor1_full",(PyCFunction)c_state2tensor1_full , METH_VARARGS|METH_KEYWORDS},
    {"c_model_2tensor_f",(PyCFunction)c_model_2tensor_f , METH_VARARGS|METH_KEYWORDS},
    {"c_model_2tensor_full_f",(PyCFunction)c_model_2tensor_full_f , METH_VARARGS|METH_KEYWORDS},
    {"c_model_2tensor_h",(PyCFunction)c_model_2tensor_h , METH_VARARGS|METH_KEYWORDS},
    {"c_model_2tensor_full_h",(PyCFunction)c_model_2tensor_full_h , METH_VARARGS|METH_KEYWORDS},
    {"c_state2tensor2",(PyCFunction)c_state2tensor2 , METH_VARARGS|METH_KEYWORDS},
    {"c_state2tensor2_full",(PyCFunction)c_state2tensor2_full , METH_VARARGS|METH_KEYWORDS},
    {"c_interp3signal",(PyCFunction)c_interp3signal , METH_VARARGS|METH_KEYWORDS},
    {"c_interp3scalar",(PyCFunction)c_interp3scalar , METH_VARARGS|METH_KEYWORDS},
    {"c_s2ga",(PyCFunction)c_s2ga , METH_VARARGS|METH_KEYWORDS},
    {NULL,      NULL}        /* Sentinel */
};

PyMODINIT_FUNC initfiltered_ext(void)
{
    
    Py_Initialize();
    import_array();
    PyImport_ImportModule("numpy");
    (void) Py_InitModule("filtered_ext", compiled_methods);
}

#ifdef __CPLUSCPLUS__
}
#endif
