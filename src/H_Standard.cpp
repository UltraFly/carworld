
#include <string>

#include "H_Standard.h"

//CLASS HException:
HException::HException(const string &s) throw() :
	s(s)
{}

HException::~HException() throw()
{}

const char *HException::what() const throw()
{
	return s.c_str();
}

string PathOf(const string &AString)
{
	string tmp(AString);
	string::reverse_iterator I;
	for (I = tmp.rbegin() ; (I!=tmp.rend() && ((*I) != '\\') && ((*I) != '/')) ; I++);
	tmp.erase(&(*I),&(*tmp.end()));
	//cout << "PathOf \"" << AString << "\" is \"" << tmp << "\"\n";
	return tmp;
}

string TitleOf(const string &AString)
{
	string tmp(AString);
	string::iterator I;
	for (I = tmp.end() ; (I!=tmp.begin() && (*I != '\\') && (*I != '/')) ; I--);
	tmp.erase(tmp.begin(),I);
	if (!tmp.empty() && ((tmp[0] == '\\')  || (tmp[0] == '/')))
		tmp.erase(tmp.begin());
	return tmp;
}

istream &EatWhite(istream &in)
{
	unsigned char TmpChar;
	bool Finnished = false;
	while (!Finnished)
	{
		in >> TmpChar;
		if (TmpChar != ' ' &&
			TmpChar != '\t' &&
			TmpChar != '\n' &&
			TmpChar != '\r')
		{
			in.putback(TmpChar);
			Finnished = true;
		}
	}
	return in;
}

//CLASS Command:
Command::Command() {}

Command::Command(const string &AString)
{
	string tmp(AString);
	string::iterator ptr = tmp.begin();

	while (ptr!=tmp.end())
	{
		string CurrentArg;
		//skip the white...
		while (ptr!=tmp.end() && isspace(*ptr))
			ptr++;
		if (ptr==tmp.end())
			return;
		//read the argument into a vector
		if ((*ptr)=='"') //case where the parameter is in brackets
		{
			ptr++;
			while (ptr!=tmp.end() && (*ptr)!='"')
			{
				CurrentArg.append(&(*ptr),1);
				ptr++;
			}
			if (ptr==tmp.end())
				return;
			if ((*ptr)=='"')
				ptr++;
		}
		else //case where the parameter is not in brackets
		{
			int comment = 0;
			while (ptr!=tmp.end() && !isspace(*ptr))
			{
				if (*ptr=='/')
					comment++;
				else
					comment=0;
				if (comment==2) return;
				CurrentArg.append(&(*ptr),1);
				ptr++;
			}
		}
		if (!CurrentArg.empty())
			data.push_back(CurrentArg);
	}
}

bool Command::IsIn(const char *AArgument) const
{
	return (FindArg(AArgument) >= 0);
}
int Command::FindArg(const char *AArgument) const
{
	for (unsigned int i=0 ; i<data.size() ; i++)
		if (data[i]==AArgument)
			return int(i);
	return -1;
}
const string &Command::operator[] (unsigned int i) const
{
	if (i>=data.size())
		throw HException("in Command, tried to access invalid argument");
	return data[i];
}
unsigned int Command::size() const
{
	return data.size();
}

#include <stdio.h>
#include <stdlib.h>

void OpenDataFile(ifstream &infile, const char *FileName)
{
	char tmp[FILENAME_MAX];
//try as is
	{
		sprintf(tmp,"%s",FileName);
		infile.open(tmp, ios::in | ios::binary);
	}
//try in the home directory
	if (!infile)
	{
		sprintf(tmp,"%s/.carworld/%s",getenv("HOME"),FileName);
		infile.open(tmp, ios::in | ios::binary);
	}
//try in the installation directory
	if (!infile)
	{
		sprintf(tmp,"/usr/share/carworld/%s",FileName);
		infile.open(tmp, ios::in | ios::binary);
	}
	if (!infile)
		throw HException(string("file : \"")+FileName+("\" failed to open."));
	cout << "opened: " << tmp << endl;
}
