

#ifndef __H_STANDARD_H_
#define __H_STANDARD_H_

#include <sstream>

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

#define ARRAY_SIZE(T) (sizeof(T)/sizeof(T[0]))

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
	stringstream tmp;
	tmp << x;
	return tmp.str();
}

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
	typename list<Ref>::iterator pos;
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
