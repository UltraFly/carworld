


#include "H_Variable.h"


//CLASS HExecutable
HExecutable::~HExecutable() {}
void HExecutable::serialize(ostream &out) {}
void HExecutable::exec(const Command &c) {}

//CLASS HExecutableSet
HExecutableSet::~HExecutableSet()
{
	for (map<string,HVariable *>::iterator I = Map.begin(); I != Map.end() ; I++)
		delete (*I).second;
}

void HExecutableSet::serialize(ostream &out)
{
	for (map<string,HVariable *>::iterator I = Map.begin(); I != Map.end() ; I++)
	{
		out << "set " << (*I).second->name() << " \"";
		out << (*I).second->get_str() << "\" //" << (*I).second->type() << endl;
	}
}

void HExecutableSet::exec(const Command &c)
{
	//must have 3 arguments
	if (c.size() == 3)
	{
		get(c[1]).set_str(c[2]);
		cout << get(c[1]) << endl;
	}
	else
		cout << "usage: set <variable> <value>\n";
}

HVariable &HExecutableSet::get(const string &variable)
{
	map<string,HVariable *>::iterator I = Map.find(variable);
	if (I == Map.end())
		throw HException(string(variable) + " is not a variable");
	else
		return *(Map[variable]);
}

void HExecutableSet::add(HVariable *variable)
{
	Map[variable->m_name] = variable;
}


//CLASS HVariable
HVariable::HVariable(const char *name) : m_name(name)
{}

istream& operator>>(istringstream& in, bool& x)
{
	int tmp;
	in >> tmp;
	x=tmp!=0;
	return in;
}

ostream& operator<< (ostream& out, HVariable& A)
{
	out << A.type() << " " << A.name() << " == " << A.get_str();
	return out;
}

HVariable::~HVariable() {}
