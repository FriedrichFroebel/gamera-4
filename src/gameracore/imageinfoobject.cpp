/*
 *
 * Copyright (C) 2001-2005 Ichiro Fujinaga, Michael Droettboom, and Karl MacMillan
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef GAMERACORE_INTERNAL
   #define GAMERACORE_INTERNAL
#endif
#include "gameramodule.hpp"

using namespace Gamera;

static PyTypeObject ImageInfoType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

PyTypeObject* get_ImageInfoType() {
  return &ImageInfoType;
}

static PyObject* imageinfo_new(PyTypeObject* pytype, PyObject* args, PyObject* kwds) {
  int num_args = PyTuple_GET_SIZE(args);
  if (num_args != 0) {
    PyErr_SetString(PyExc_TypeError, "Invalid arguments to ImageInfo constructor.");
    return 0;
  }
  ImageInfoObject* o;
  o = (ImageInfoObject*)pytype->tp_alloc(pytype, 0);
  o->m_x = new ImageInfo();
  return (PyObject*)o;
}

static void imageinfo_dealloc(PyObject* self) {
  ImageInfoObject* x = (ImageInfoObject*)self;
  delete x->m_x;
  self->ob_type->tp_free(self);
}

#define CREATE_GET_FUNC(name) static PyObject* imageinfo_get_##name(PyObject* self) {\
  ImageInfo* x = ((ImageInfoObject*)self)->m_x; \
  return PyLong_FromLong((int)x->name()); \
}

#define CREATE_SET_FUNC(name) static int imageinfo_set_##name(PyObject* self, PyObject* value) {\
  ImageInfo* x = ((ImageInfoObject*)self)->m_x; \
  x->name((size_t)PyLong_AS_LONG(value)); \
  return 0; \
}

#define CREATE_GET_FLOAT_FUNC(name) static PyObject* imageinfo_get_##name(PyObject* self) {\
  ImageInfo* x = ((ImageInfoObject*)self)->m_x; \
  return Py_BuildValue( "f", x->name()); \
}

#define CREATE_SET_FLOAT_FUNC(name) static int imageinfo_set_##name(PyObject* self, PyObject* value) {\
  ImageInfo* x = ((ImageInfoObject*)self)->m_x; \
  x->name(PyFloat_AS_DOUBLE(value)); \
  return 0; \
}

CREATE_GET_FLOAT_FUNC(x_resolution)
CREATE_SET_FLOAT_FUNC(x_resolution)
CREATE_GET_FLOAT_FUNC(y_resolution)
CREATE_SET_FLOAT_FUNC(y_resolution)
CREATE_GET_FUNC(nrows)
CREATE_SET_FUNC(nrows)
CREATE_GET_FUNC(ncols)
CREATE_SET_FUNC(ncols)
CREATE_GET_FUNC(depth)
CREATE_SET_FUNC(depth)
CREATE_GET_FUNC(ncolors)
CREATE_SET_FUNC(ncolors)

static PyGetSetDef imageinfo_getset[] = {
        { (char *)"x_resolution", (getter)imageinfo_get_x_resolution,
                (setter)imageinfo_set_x_resolution, (char *)"The x resolution of the image." },
        { (char *)"y_resolution", (getter)imageinfo_get_y_resolution,
                (setter)imageinfo_set_y_resolution, (char *)"The y resolution of the image." },
        { (char *)"ncols", (getter)imageinfo_get_ncols,
                (setter)imageinfo_set_ncols, (char *)"The number of columns of the image." },
        { (char *)"nrows", (getter)imageinfo_get_nrows,
                (setter)imageinfo_set_nrows, (char *)"The number of rows of the image." },
        { (char *)"depth", (getter)imageinfo_get_depth,
                (setter)imageinfo_set_depth, (char *)"The bit depth of the image (in bits)." },
        { (char *)"ncolors", (getter)imageinfo_get_ncolors,
                (setter)imageinfo_set_ncolors, (char *)"The number of colors in the image." },
        { nullptr }
};

void init_ImageInfoType(PyObject* module_dict) {
  #ifdef Py_SET_TYPE
    Py_SET_TYPE(&ImageInfoType, &PyType_Type);
  #else
    Py_TYPE(&ImageInfoType) = &PyType_Type;
  #endif
  ImageInfoType.tp_name =  "gameracore.ImageInfo";
  ImageInfoType.tp_basicsize = sizeof(ImageInfoObject);
  ImageInfoType.tp_dealloc = imageinfo_dealloc;
  ImageInfoType.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
  ImageInfoType.tp_new = imageinfo_new;
  ImageInfoType.tp_getattro = PyObject_GenericGetAttr;
  ImageInfoType.tp_alloc = nullptr; // PyType_GenericAlloc;
  ImageInfoType.tp_getset = imageinfo_getset;
  ImageInfoType.tp_free = nullptr; // _PyObject_Del;
  ImageInfoType.tp_doc =
"__init__()\n\n"
"The ImageInfo class allows the properties of a disk-based image file "
"to be examined without loading it.\n\n"
"It is rare to instantiate this class directly.\n\n"
"To get image info, use the image_info(*filename*) function in the "
"module ``gamera.core``.";
  PyType_Ready(&ImageInfoType);
  PyDict_SetItemString(module_dict, "ImageInfo", (PyObject*)&ImageInfoType);
}
