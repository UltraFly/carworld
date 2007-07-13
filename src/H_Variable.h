

#ifndef _H_VARIABLE_H_
#define _H_VARIABLE_H_

namespace std {}
using namespace std;

#include "H_Standard.h"
#include <typeinfo>
#include <map>

class HVariable
{
public:
	HVariable(const char *name);
	template <class InputIterator>
		void add(InputIterator first, InputIterator last)
			{for (InputIterator I=first ; I!=last ; I++) add(*I);}

	virtual const char *name() const = 0;
	virtual const char *type() const = 0;
	virtual void set_str(const string &) = 0;
	virtual string get_str() = 0;
	virtual ~HVariable();
public:
	string m_name;
};
ostream &operator<< (ostream &out, HVariable &A);

istream &operator>>(istringstream &in, bool &x);

template <class T> class HVar : public HVariable
{
	typedef void (*SetFunction)(T value); 
	typedef T (*GetFunction)();
private:
	T *m_value;
	SetFunction m_SetFunction;
	GetFunction m_GetFunction;
public:
	HVar(const char *name, SetFunction set, GetFunction get) :
		HVariable(name), m_value(NULL), m_SetFunction(set), m_GetFunction(get)
	{}
	HVar(const char *name, T *value, SetFunction set = NULL, GetFunction get = NULL) :
		HVariable(name), m_value(value), m_SetFunction(set), m_GetFunction(get)
	{}
	const char *name() const
	{
		return m_name.c_str();
	}
	const char *type() const
	{
		return typeid(T).name();
	}

	void set(T value)
	{
		if (m_SetFunction!=NULL)
			m_SetFunction(value);
		if (m_value!=NULL)
			*m_value = value;
	}

	T get()
	{
		if (m_GetFunction!=NULL)
			return m_GetFunction();
		else
			return *m_value;
	}

	void set_str(const string &value)
	{
		istringstream in(value.c_str());
		T Tvalue;
		in >> Tvalue;
		set(Tvalue);
	}
	string get_str()
	{
		ostringstream out;
		out << get();
		return out.str();
	}
};

template <class Obj, class T> class HVarObj : public HVariable
{
	typedef void (Obj:: *SetFunction)(T value); 
	typedef T (Obj:: *GetFunction)();
private:
	Obj *m_obj;
	T Obj:: *m_value;
	SetFunction m_SetFunction;
	GetFunction m_GetFunction;
public:
	HVarObj(const char *name, Obj *obj, SetFunction set, GetFunction get) :
		HVariable(name), m_obj(obj), m_value(NULL), m_SetFunction(set), m_GetFunction(get)
	{}
	HVarObj(const char *name, Obj *obj, T Obj:: *value, SetFunction set = NULL, GetFunction get = NULL) :
		HVariable(name), m_obj(obj), m_value(value), m_SetFunction(set), m_GetFunction(get)
	{}
	const char *name() const
	{
		return m_name.c_str();
	}
	const char *type() const
	{
		return typeid(T).name();
	}

	void set(T value)
	{
		if (m_SetFunction!=0)
			(m_obj->*m_SetFunction)(value);
		if (m_value!=0)
			m_obj->*m_value = value;
	}

	T get()
	{
		if (m_GetFunction!=0)
			return (m_obj->*m_GetFunction)();
		else
			return m_obj->*m_value;
	}

	void set_str(const string &value)
	{
		istringstream in(value.c_str());
		T Tvalue;
		in >> Tvalue;
		set(Tvalue);
	}
	string get_str()
	{
		ostringstream out;
		out << get();
		return out.str();
	}
};


class HExecutable
{
public:
	virtual void exec(const Command &c);
	virtual void serialize(ostream &out);
	virtual ~HExecutable();
};

template <class T> class MethodCall : public HExecutable
{
private:
	T *m_object;
	void(T:: *m_method)();
public:
	MethodCall(T *object,void(T:: *method)()) : m_object(object), m_method(method) {}
	virtual void exec(const Command &c)
	{
		(m_object->*m_method)();
	}
	virtual ~MethodCall() {}
};

/*template <class T, class Param> class MethodCall1 : public HExecutable
{
private:
	T *m_object;
	void(T:: *m_method)();
	TParam m_Param;
public:
	MethodCall1(T *object, void(T:: *method)(), TParam Param) :
		m_object(object), m_method(method), m_Param(Param) {}
	virtual void exec(const Command &c)
	{
		(m_object->*m_method)(m_Param);
	}
	virtual ~MethodCall1() {}
};*/

class HExecutableSet : public HExecutable
{
public:
	virtual ~HExecutableSet();
	void serialize(ostream &out);
	virtual void exec(const Command &c);
	HVariable &get(const string &variable);
	void add(HVariable *variable);
private:
	map<string,HVariable *> Map;
};

#endif //_H_VARIABLE_H_
