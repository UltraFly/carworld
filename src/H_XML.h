
#ifndef _H_XML_H_
#define _H_XML_H_

#include "H_Variable.h"
#include <list>
#include <map>
#include <string>

class XmlTag
{
public:
	enum type {broken, empty, opening, closing};

	XmlTag();
	void read_from(istream &in);
	type get_type();
	string &name();
	void write_to(map<string,HVariable*> &Map);

//used for parsing:
	void write_name(char c);
	void add_var(char c);
	void write_var_name(char c);
	void write_var_value(char c);
	void is_closing(char c);
private:
	type m_type;
	string m_name;
	list<pair<string,string> > assignments;
};

#endif //_H_XML_H_
