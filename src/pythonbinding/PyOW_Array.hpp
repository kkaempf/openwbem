#ifndef PYOW_ARRAY_HPP_
#define PYOW_ARRAY_HPP_

#include <OW_String.hpp>
#include <OW_Array.hpp>

// note this comes *after* the OpenWBEM headers, because it has a
// #define ANY void
// which really screws up OpenWBEM
#include <boost/python.hpp>
#ifdef ANY
#undef ANY
#endif

namespace {

template <typename T>
T T_getslice_(const T& a, int i, int j)
{
    int len = a.size();
    if (i < 0)
    {
        i = 0;
    }
    if (j < 0)
    {
        j = 0;
    }
    if (i > j)
        return T();
    T rval(a);
    if (j < len)
    {
        rval.remove(j, len);
    }
    if (i > 0)
    {
        rval.remove(0, i);
    }
    return rval;
}

template <typename T>
typename T::value_type T_getitem_(const T& t, int i)
{
    if (i < 0 || i >= t.size())
    {
        // raise IndexError
        PyErr_SetString(PyExc_IndexError,
                "index out of range");
        boost::python::throw_error_already_set();
    }
    return t[i];
}

template <typename T>
void T_setitem_(T& t, int i, const typename T::value_type& x)
{
    if (i < 0 || i >= t.size())
    {
        // raise IndexError
        PyErr_SetString(PyExc_IndexError,
                "index out of range");
        boost::python::throw_error_already_set();
    }
    t[i] = x;
}

template <typename T>
void T_delitem_(T& t, int i)
{
    if (i < 0 || i >= t.size())
    {
        // raise IndexError
        PyErr_SetString(PyExc_IndexError,
                "index out of range");
        boost::python::throw_error_already_set();
    }
    t.erase(t.begin() + i);
}

template <typename T>
int T_count(const T& s, typename T::value_type const& c)
{
    int rval = 0;
    for (typename T::const_iterator i = s.begin(); i != s.end(); ++i)
    {
        if (*i == c)
            ++rval;
    }
    return rval;
}

template <typename T>
void T_append(T& s, typename T::value_type const& c)
{
    s.push_back(c);
}

template <typename T>
int T_index(const T& s, typename T::value_type const& c)
{
    for (typename T::const_iterator i = s.begin(); i != s.end(); ++i)
    {
        if (*i == c)
            return std::distance(s.begin(), i);
    }

    // Raise ValueError
    PyErr_SetString(PyExc_ValueError,
            "index(x): x not in list");
    boost::python::throw_error_already_set();

}

template <typename T>
void T_insert(T& s, int i, typename T::value_type const& x)
{
    typename T::size_type length = s.size();
    if (i < 0)
    {
        i = 0;
    }
    if (i >= length)
    {
        s.insert(s.end(), x);
    }
    else
    {
        s.insert(s.begin() + i, x);
    }
}

template <typename T>
typename T::value_type T_pop(T& s, int i = -1)
{
    typename T::size_type length = s.size();
    while (true)
    {
        if (i < 0)
        {
            i += length;
        }
        else if (i >= length)
        {
            i -= length;
        }
        else
        {
            break;
        }
    }
    typename T::value_type rval = s[i];
    s.erase(s.begin() + i);

    return rval;
}

BOOST_PYTHON_FUNCTION_OVERLOADS(T_pop_overloads, T_pop, 1, 2)

template <typename T>
void T_remove(T& s, typename T::value_type const& x)
{
    int i = T_index(s, x);
    s.erase(s.begin() + i);
}

template <typename T>
void T_reverse(T& s)
{
    std::reverse(s.begin(), s.end());
}

template <typename T>
void T_sort(T& s)
{
    std::sort(s.begin(), s.end());
}

template <typename T>
T T_add_(T const& x, T const& y)
{
    T rval(x);
    rval.insert(rval.end(), y.begin(), y.end());
    return rval;
}

template <typename T>
T& T_iadd_(T& x, T const& y)
{
    x.insert(x.end(), y.begin(), y.end());
    return x;
}

template <typename T>
T T_mul_(T const& x, int y)
{
    T rval;
    for (int i = 0; i < y; ++i)
    {
        rval.insert(rval.end(), x.begin(), x.end());
    }
    return rval;
}

template <typename T>
T T_rmul_(int y, T const& x)
{
    return T_mul_(x, y);
}

template <typename T>
T& T_imul_(T & x, int y)
{
    if (y < 1)
    {
        x.clear();
        return x;
    }

    typename T::size_type l = x.size();
    x.reserve(y * l);
    for (int i = 1; i < y; ++i)
    {
        typename T::iterator end = x.begin();
        std::advance(end, l);
        x.insert(x.end(), x.begin(), end);
    }
    return x;
}

template <typename T>
bool T_contains_(T const& self, typename T::value_type const& item)
{
    return std::find(self.begin(), self.end(), item) != self.end();
}

template <typename T>
PyObject* T_repr(const T& t)
{
    OW_String rval("owclient.OW_Array<T>(");
    for (typename T::const_iterator i = t.begin(); i != t.end(); ++i)
    {
        boost::python::object o(*i);
        boost::python::str tmpstr(o.attr("__repr__")());
        rval += OW_String(boost::python::extract<const char*>(tmpstr));
        if (i + 1 != t.end())
            rval += ", ";
    }
    rval += ")";
    return ::Py_BuildValue("s#", rval.c_str(), rval.length());
}

template <typename T>
PyObject* T_str(const T& t)
{
    OW_String rval;
    for (typename T::const_iterator i = t.begin(); i != t.end(); ++i)
    {
        boost::python::object o(*i);
        boost::python::str tmpstr(o.attr("__str__")());
        rval += OW_String(boost::python::extract<const char*>(tmpstr));
        if (i + 1 != t.end())
            rval += ", ";
    }
    return ::Py_BuildValue("s#", rval.c_str(), rval.length());
}

template <typename T>
void registerOW_ArrayImpl(const char* className)
{
    using namespace boost::python;

    typedef typename OW_Array<T>::iterator iter_t;
    typedef typename OW_Array<T>::const_iterator const_iter_t;
    typedef typename OW_Array<T>::reverse_iterator riter_t;
    typedef typename OW_Array<T>::size_type size_type;
    typedef typename OW_Array<T>::reference reference;
    class_<OW_Array<T> >(className)
        .def(init<size_type, const T&>())
        .def(init<int, const T&>())
        .def(init<long, const T&>())
        .def(init<size_type>())
        // these call the template constructor.  Since we have to instantiate
        // each type we want to expose, let's not go overboard!
        .def(init<iter_t, iter_t>())
        .def(init<const_iter_t, const_iter_t>())
        .def("begin", (iter_t (OW_Array<T>::*)())(&OW_Array<T>::begin))
        .def("end", (iter_t (OW_Array<T>::*)())(&OW_Array<T>::end))
        .def("__iter__", iterator<OW_Array<T> >())
        .def("rbegin", (riter_t (OW_Array<T>::*)())(&OW_Array<T>::rbegin))
        .def("rend", (riter_t (OW_Array<T>::*)())(&OW_Array<T>::rend))
        .def("size", &OW_Array<T>::size)
        .def("max_size", &OW_Array<T>::max_size)
        .def("capacity", &OW_Array<T>::capacity)
        .def("empty", &OW_Array<T>::empty)
        .def(self += T())
        .def("reserve", &OW_Array<T>::reserve)
        // These won't compile with T=native type
        //.def("front", (reference (OW_Array<T>::*)())&OW_Array<T>::front, return_internal_reference<>())
        //.def("back", (reference (OW_Array<T>::*)())&OW_Array<T>::back, return_internal_reference<>())
        .def("push_back", &OW_Array<T>::push_back)
        .def("append", &OW_Array<T>::append)
        .def("swap", &OW_Array<T>::swap)
        .def("insert", (iter_t (OW_Array<T>::*)(iter_t, const T&))&OW_Array<T>::insert)
        .def("insert", (void (OW_Array<T>::*)(size_type, const T&))&OW_Array<T>::insert)
        .def("insert", (void (OW_Array<T>::*)(iter_t, iter_t, iter_t))&OW_Array<T>::insert)
        .def("remove", (void (OW_Array<T>::*)(size_type))(&OW_Array<T>::remove))
        .def("remove", (void (OW_Array<T>::*)(size_type, size_type))(&OW_Array<T>::remove))
        .def("appendArray", &OW_Array<T>::appendArray)
        .def("pop_back", &OW_Array<T>::pop_back)
        .def("erase", (iter_t (OW_Array<T>::*)(iter_t))(&OW_Array<T>::erase))
        .def("erase", (iter_t (OW_Array<T>::*)(iter_t, iter_t))(&OW_Array<T>::erase))
        .def("resize", (void (OW_Array<T>::*)(size_type))(&OW_Array<T>::resize))
        .def("resize", (void (OW_Array<T>::*)(size_type, const T&))(&OW_Array<T>::resize))
        .def("clear", &OW_Array<T>::clear)
//        .def("readObject", &OW_Array<T>::readObject)
//        .def("writeObject", &OW_Array<T>::writeObject)
        .def(self == self)
        .def(self < self)

        // python container functions
        .def("__len__", &OW_Array<T>::size)
        .def("__getslice__", &T_getslice_<OW_Array<T> >)
        .def("__getitem__", &T_getitem_<OW_Array<T> >)
        .def("__setitem__", &T_setitem_<OW_Array<T> >)
        .def("__delitem__", &T_delitem_<OW_Array<T> >)
        .def("append", &T_append<OW_Array<T> >)
        .def("count", &T_count<OW_Array<T> >)
        .def("index", &T_index<OW_Array<T> >)
        .def("insert", &T_insert<OW_Array<T> >)
        .def("pop", &T_pop<OW_Array<T> >, T_pop_overloads(args("i")))
        .def("remove", &T_remove<OW_Array<T> >)
        .def("reverse", &T_reverse<OW_Array<T> >)
        .def("sort", &T_sort<OW_Array<T> >)
        .def("__add__", &T_add_<OW_Array<T> >)
        .def("__radd__", &T_add_<OW_Array<T> >)
        .def("__iadd__", &T_iadd_<OW_Array<T> >, return_internal_reference<1>())
        .def("__mul__", &T_mul_<OW_Array<T> >)
        .def("__rmul__", &T_rmul_<OW_Array<T> >)
        .def("__imul__", &T_imul_<OW_Array<T> >, return_internal_reference<1>())
        .def("__contains__", &T_contains_<OW_Array<T> >)

        .def("__repr__", &T_repr<OW_Array<T> >)
        .def("__str__", &T_str<OW_Array<T> >)
    ;
}

}

#endif // #ifndef PYOW_ARRAY_HPP_
