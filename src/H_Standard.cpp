
//to avoid problems with non compliant microsoft compiler
#define _CRT_SECURE_NO_DEPRECATE

#include "H_Standard.h"


#include <string.h>

#include <string>

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
	/*string tmp(AString);
	string::reverse_iterator I;
	for (I = tmp.rbegin() ; (I!=tmp.rend() && ((*I) != '\\') && ((*I) != '/')) ; I++);
	tmp.erase(&(*I),&(*tmp.end()));
	//cout << "PathOf \"" << AString << "\" is \"" << tmp << "\"\n";
	return tmp;*/
	
	int i;
	char tmp[FILENAME_MAX];
	//strcpy_s(tmp, ARRAY_SIZE(tmp), AString.c_str());
	strcpy(tmp, AString.c_str());
	for (i=int(AString.size())-1 ; i>=0 && AString[i]!='\\' && AString[i]!='/' ; i--);
	/*if (i<0)
		tmp = AString;
	else
		tmp = &AString[i];*/
	tmp[i] = 0;

	//cout << "PathOf \"" << AString << "\" is \"" << tmp << "\"\n";
	return tmp;
}

/*string TitleOf(const string &AString)
{
	string tmp(AString);
	string::iterator I;
	for (I = tmp.end() ; (I!=tmp.begin() && (*I != '\\') && (*I != '/')) ; I--);
	tmp.erase(tmp.begin(),I);
	if (!tmp.empty() && ((tmp[0] == '\\')  || (tmp[0] == '/')))
		tmp.erase(tmp.begin());
	return tmp;
}*/

string TitleOf(const string &AString)
{
	return AString;
	/*string tmp(AString);
	string::iterator I;
	for (I = tmp.end() ; (I!=tmp.begin() && (*I != '\\') && (*I != '/')) ; I--);
	tmp.erase(tmp.begin(),I);
	if (!tmp.empty() && ((tmp[0] == '\\')  || (tmp[0] == '/')))
		tmp.erase(tmp.begin());
	return tmp;*/
	/*string tmp;
	string::const_reverse_iterator I;
	for (I=AString.rbegin() ; (I!=tmp.rend() && (*I != '\\') && (*I != '/')) ; I++)
		tmp.push_back(*I);
	return tmp;*/

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
size_t Command::size() const
{
	return data.size();
}

istream &ReadTextLine(istream &in, string &line)
{
	getline(in, line);
	if (!line.empty() && line[line.size()-1] == '\r')
		line.erase(line.size()-1);
	return in;
}

#include <stdio.h>
#include <stdlib.h>

void OpenDataFile(ifstream &infile, const char *FileName)
{
	//cout << "opening \"" << FileName << "\"" << endl;

	string FullPath;
//try as is
	{
		FullPath = FileName;
		//cout << "trying \"" << FullPath << "\"..." << endl;
		infile.open(FullPath.c_str(), ios::in);
	}
//try in the home directory
	const char* HomeDirectory = getenv("HOME");
	if (!infile && HomeDirectory!=NULL)
	{
		infile.clear();

		FullPath = HomeDirectory;
		FullPath += "/.carworld/";
		FullPath += FileName;
		//cout << "trying \"" << FullPath << "\"..." << endl;
		infile.open(FullPath.c_str(), ios::in);
	}
//try in the installation directory
	if (!infile)
	{
		infile.clear();
		FullPath = "/usr/share/carworld/";
		FullPath += FileName;
		//cout << "trying \"" << FullPath << "\"..." << endl;
		infile.open(FullPath.c_str(), ios::in);
	}

	if (!infile)
		throw HException(string("file : \"")+FileName+("\" failed to open."));
	cout << "opened: \"" << FullPath << "\"" << endl;
}
