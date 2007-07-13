
#include "H_XML.h"
#include "automat.h"

using namespace std;

#define space_chars " \t\n\r"
#define alphanum_chars "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_"
#define punctuation_chars ".,-"

XmlTag::XmlTag()
{
}

void XmlTag::read_from(istream &in)
{
	//   ___<___tag___name__=___"value"____>
	automat<XmlTag> parser(this);
	parser.add_transition("begin",			"begin",		space_chars);
	parser.add_transition("begin",			"start tag",	"<");
	parser.add_transition("start tag",		"closing tag",	"/");
	parser.add_transition("start tag",		"tag name",		alphanum_chars, &XmlTag::write_name);
	parser.add_transition("tag name",		"tag name",		alphanum_chars, &XmlTag::write_name);
	parser.add_transition("closing tag",	"closing name",	alphanum_chars);
	parser.add_transition("closing name",	"closing name",	alphanum_chars);
	parser.add_transition("closing name",	"end",			">");
	parser.add_transition("tag name",		"start var",	space_chars);
	parser.add_transition("tag name",		"close tag",	"/");
	parser.add_transition("close tag",		"end",			">");
	parser.add_transition("start var",		"start var",	space_chars);
	parser.add_transition("start var",		"var name",		alphanum_chars, &XmlTag::add_var);
	parser.add_transition("start var",		"end",			">");
	parser.add_transition("start var",		"close tag",	"/");
	parser.add_transition("var name",		"var name",		alphanum_chars, &XmlTag::write_var_name);
	parser.add_transition("var name",		"=",			space_chars);
	parser.add_transition("var name",		"start value",	"=");
	parser.add_transition("=",				"=",			space_chars);
	parser.add_transition("=",				"start value",	space_chars);
	parser.add_transition("start value",	"start value",	space_chars);
	parser.add_transition("start value",	"var value",	"\"");
	parser.add_transition("var value",		"var value",	alphanum_chars space_chars punctuation_chars "/\\", &XmlTag::write_var_value);
	parser.add_transition("var value",		"start var",	"\"");

	assignments.clear();
	m_name = "";

	m_type = opening;
	parser.read_from(in);
	if (parser.is_error())
		m_type = broken;
	if (parser.is_eof())
		m_type = empty;
}

XmlTag::type XmlTag::get_type()
{
	return m_type;
}

string &XmlTag::name()
{
	return m_name;
}

void XmlTag::write_to(map<string,HVariable*> &Map)
{
	for (list<pair<string,string> >::iterator I=assignments.begin() ; I!=assignments.end() ; I++)
	{
		//string bobo = (*I).first;
		map<string,HVariable*>::iterator J = Map.find((*I).first);
		if (J==Map.end())
			throw HException("invalid variable in TAG");
		(*J).second->set_str((*I).second);
	}
}

void XmlTag::write_name(char c)
{
	m_name.append(1,c);
}

void XmlTag::add_var(char c)
{
	assignments.push_back(pair<string,string>());
	write_var_name(c);
}

void XmlTag::write_var_name(char c)
{
	assignments.back().first.append(1,c);
}

void XmlTag::write_var_value(char c)
{
	assignments.back().second.append(1,c);
}

void XmlTag::is_closing(char c)
{
	m_type = closing;
}
