

#ifndef __H_STANDARD_H_
#define __H_STANDARD_H_

#ifdef WIN32

#pragma warning( disable : 4786 ) //disable "identifier name too long" warning
#include <strstream> //should later be replaced by stringstream

#else

#include <strstream.h>

#endif //WIN32


#include <string>
#include <iostream>
#include <fstream>

#include <vector>
#include <list>
#include <map>

using namespace std;

#define H_DUMP(filename,text) {ofstream out_err(filename,ios::out); out_err<<text; out_err.close();}
extern ofstream log_file;

//My defines
#ifndef PI
#define PI 3.14159265358979323846f
#endif

typedef float REAL;
#define H_EPSILON 0.000001f

//clamp b to a<b<c
#define LIMIT(a,b,c) (((a)>(b)) ? (a) : (((b)>(c)) ? (c) : (b)))
#define ABS(x) (((x)>=0) ? (x) : -(x))
#define H_MIN(a,b) (((a)>(b)) ? (b) : (a))
#define H_MAX(a,b) (((a)>(b)) ? (a) : (b))

using namespace std;

class HException : public exception
{
public:
	HException(const string &s) throw();
    virtual ~HException() throw();
	virtual const char *what() const throw();
private:
	string s;
};

string PathOf(const string &AString);
string TitleOf(const string &AString);
void OpenDataFile(ifstream &infile, const char *FileName);
istream &EatWhite(istream &in);

template <class T> string to_string(T x)
{
	strstream tmp;
	tmp << x;
	return string(tmp.str());
}

//class used to map an enum onto a string
//usually for human readable IO
//usually declared as static in a class
template <class T1, class T2> class HMapper
{
public:
	//the first pair is used for invalid returns...
	//should be template on iterator but is not supported by VC6
	HMapper(pair<T1,T2> *data_begin, pair<T1,T2> *data_end)
	{
		invalidT1 = data_begin->first;
		invalidT2 = data_begin->second;
		for (pair<T1,T2> *I=data_begin ; I!=data_end ; I++)
			add(I->first,I->second);
	}
	void add(const T1 &value, const T2 &name)
	{
		T1_map[value] = name;
		T2_map[name] = value;
	}
	T2 &find(const T1 &A)
	{
		map<T1,T2>::iterator I = T1_map.find(A);
		return (I==T1_map.end()) ? invalidT2 : (*I).second;
	}
	T1 &find(const T2 &A)
	{
		map<T2,T1>::iterator I = T2_map.find(A);
		return (I==T2_map.end()) ? invalidT1 : (*I).second;
	}
private:
	map<T1,T2> T1_map;
	map<T2,T1> T2_map;
	T1 invalidT1;
	T2 invalidT2;
};

template <class T> class HPointer
{
private:
	class Ref
	{
	public:
		Ref() : ptr(NULL) {}
		void reset(T* Aptr) {ptr=Aptr;count=1;}
		~Ref() {delete ptr;}
	public:
		T* ptr;
		unsigned int count;
	};
	static list<Ref> RefList;
	list<Ref>::iterator pos;
public:
	HPointer() {AddRef(NULL);}
	HPointer(T *ptr) {AddRef(ptr);}
	HPointer(const HPointer &Hptr) {pos=Hptr.pos; (*pos).count++;}
	T* c_ptr() const {return (*pos).ptr;}
	operator T *() const {return c_ptr();}
	T *operator = (T *ptr) {UnRef(); AddRef(ptr); return c_ptr();}
	HPointer &operator = (const HPointer &Hptr)
		{UnRef(); pos=Hptr.pos; (*pos).count++; return *this;}
	~HPointer() {UnRef();}
	//T &operator *() const {return *((*pos).ptr);}
private:
	void AddRef(T* ptr)
		{RefList.push_front(Ref()); pos = RefList.begin(); (*pos).reset(ptr);}
	void UnRef() {if ((--(*pos).count)==0) RefList.erase(pos);}
};

//this template is to be used when a STL vector is needed but
//we also need direct access to the pointer onto the elements
//for optimisation reasons...
template <class T> class HVector
{
public:
	typedef T* iterator;
	HVector() : m_data(NULL), m_size(0) {}
	HVector(unsigned int n) : m_data(NULL), m_size(0) {resize(n);}
	HVector(const HVector<T> &V) : m_data(NULL), m_size(0) {*this = V;}
	//data is not conserved...
	void resize(unsigned int n)
	{
		delete [] m_data;
		if ((m_size=n)==0)
			m_data=NULL;
		else
		{
			m_data = new T[m_size];
			if(m_data==NULL)
			{
				//string tmp;
				//throw HException(tmp+"failed to allocate "+(m_size*sizeof(T))+" bytes");
			}
		}
	}
	HVector<T> &operator= (const HVector<T> &V)
	{
		resize(V.m_size);
		for (unsigned int i=0; i<m_size; i++) m_data[i] = V.m_data[i];
		return *this;
	}
	~HVector() {delete [] m_data;}
	//T &operator[] (unsigned int i) const {return m_data[i];}
	inline T *ptr() const {return m_data;}
	inline operator T* () const {return m_data;};
	inline unsigned int size() const {return m_size;}
	inline iterator begin() const {return m_data;}
	inline iterator end() const {return m_data+m_size;}
private:
	T *m_data;
	unsigned int m_size;
};


class Command
{
public:
	Command();
	Command(const string &AString);
	bool IsIn(const char *AArgument) const;
	int FindArg(const char *AArgument) const;
	const string &operator[] (unsigned int i) const;
	unsigned int size() const;

	friend istream &operator>> (istream &in, Command &A);
private:
	vector<string> data;
};

#endif //__H_STANDARD_H_
