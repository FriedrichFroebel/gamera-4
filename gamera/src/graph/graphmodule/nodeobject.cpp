/*
 *
 * Copyright (C) 2011 Christian Brandt
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


#include "nodeobject.hpp"
#include "iteratorobject.hpp"
#include "graph.hpp"
#ifdef __DEBUG_GAPI__
#include <iostream>
#endif

// -----------------------------------------------------------------------------
/* Python Type Definition                                                     */
// -----------------------------------------------------------------------------
static PyTypeObject NodeType = {
   PyVarObject_HEAD_INIT(nullptr, 0)
};



// -----------------------------------------------------------------------------
PyMethodDef node_methods[] = {
};



// -----------------------------------------------------------------------------
/// node_new creates a new Python NodeObject by copying the given note.
PyObject* node_new(Node* n) {
#ifdef __DEBUG_GAPI__
   std::cout<<"node_new(Node* n)\n";
#endif

   if (n == nullptr) {
      RETURN_VOID()
   }

   NodeObject* so;
   so = (NodeObject*)(NodeType.tp_alloc(&NodeType, 0));
   so->_node=n;
   so->_graph = nullptr;
   return (PyObject*)so;
}



// -----------------------------------------------------------------------------
PyObject* node_deliver(Node* n, GraphObject* go) {
   if(n == nullptr || go == nullptr)
      return nullptr;

   GraphDataPyObject* nodedata = dynamic_cast<GraphDataPyObject*>(n->_value);
   if(nodedata->_node == nullptr) {
      nodedata->_node = node_new(n);
      ((NodeObject*)nodedata->_node)->_graph = go;
      Py_XINCREF(go);
//      Py_XINCREF(nodedata->_node);
   }
   else {
      Py_XINCREF(nodedata->_node);
   }

   return nodedata->_node;
}



// -----------------------------------------------------------------------------
bool is_NodeObject(PyObject* self) {
   return PyObject_TypeCheck(self, &NodeType);
}



// -----------------------------------------------------------------------------
/* Wrapper Methods                                                            */
// -----------------------------------------------------------------------------



// -----------------------------------------------------------------------------
static void node_dealloc(PyObject* self) {
#ifdef __DEBUG_GAPI__
   std::cout<<"node_dealloc(PyObjecT* self)\n";
#endif

   INIT_SELF_NODE();
   if(so->_node) {
      GraphDataPyObject* nodedata = dynamic_cast<GraphDataPyObject*>(so->_node->_value);
      nodedata->_node = nullptr;
   }
   
   if(so->_graph != nullptr && is_GraphObject((PyObject*)so->_graph)) {
      Py_XDECREF(so->_graph);
   }

   self->ob_type->tp_free(self);
}



// -----------------------------------------------------------------------------
static PyObject* node_get_edges(PyObject* self) {
   INIT_SELF_NODE();
   Node* n = so->_node;
   EdgePtrIterator* it = n->get_edges();

   ETIteratorObject<EdgePtrIterator>* nti = 
      iterator_new<ETIteratorObject<EdgePtrIterator> >();
   nti->init(it, so->_graph);

   return (PyObject*)nti;
}



// -----------------------------------------------------------------------------
static PyObject* node_get_nodes(PyObject* self) {
   INIT_SELF_NODE();
   Node* n = so->_node;
   NodePtrEdgeIterator* it = n->get_nodes();

   NTIteratorObject<NodePtrEdgeIterator>* nti = 
      iterator_new<NTIteratorObject<NodePtrEdgeIterator> >();
   nti->init(it, so->_graph);

   return (PyObject*)nti;
}



// -----------------------------------------------------------------------------
static PyObject* node_get_nedges(PyObject* self) {
   INIT_SELF_NODE();
   size_t nedges = so->_node->get_nedges();
   RETURN_INT(nedges)
}



// -----------------------------------------------------------------------------
static PyObject* node_get_data(PyObject* self) {
   INIT_SELF_NODE();
   PyObject *data = dynamic_cast<GraphDataPyObject*>(so->_node->_value)->data;
   Py_XINCREF(data);
   return data;
}



// -----------------------------------------------------------------------------
static PyObject* node___call__(PyObject* self, PyObject* args, PyObject* kwds) {
   return node_get_data(self);
}



// -----------------------------------------------------------------------------
static PyObject* node___repr__(PyObject* self) {
   PyObject* data = node_get_data(self);
   PyObject* repr = PyObject_Repr(data);
   Py_XINCREF(repr);
   PyObject* ret = PyUnicode_FromFormat("<Node of %s>", PyUnicode_AsUTF8(repr));
   Py_XDECREF(repr);
   Py_XDECREF(data);
   return ret;
}


// -----------------------------------------------------------------------------
PyGetSetDef node_getset[] = {
        {  "data", (getter)node_get_data, nullptr,
                 "The value the identified with this node. (get/set)", nullptr },
        {  "edges", (getter)node_get_edges, nullptr,
                 "An iterator over edges pointing in/out from node (get)", nullptr },
        {  "nodes", (getter)node_get_nodes, nullptr,
                 "An iterator over nodes that can be reached directly "
                              "(through a single edge) from this node (get)", nullptr },
        {  "nedges", (getter)node_get_nedges, nullptr,
                 "The number of edges pointing in/out of this node (get)", nullptr },
        { nullptr }
};



// -----------------------------------------------------------------------------
void init_NodeType() {
    #ifdef Py_SET_TYPE
      Py_SET_TYPE(&NodeType, &PyType_Type);
    #else
      Py_TYPE(&NodeType) = &PyType_Type;
    #endif
    NodeType.tp_name =  "gamera.graph.Node";
    NodeType.tp_basicsize = sizeof(NodeObject);
    NodeType.tp_dealloc = node_dealloc;
    NodeType.tp_repr = node___repr__;
    NodeType.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
    NodeType.tp_getattro = PyObject_GenericGetAttr;
    NodeType.tp_alloc = nullptr; // PyType_GenericAlloc;
    NodeType.tp_free = nullptr; // _PyObject_Del;
    NodeType.tp_methods = node_methods;
    NodeType.tp_getset = node_getset;
    NodeType.tp_call = node___call__;
    NodeType.tp_weaklistoffset = 0;
    PyType_Ready(&NodeType);
}
