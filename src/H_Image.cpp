
#include <math.h>
#include <fstream>
#include <stdio.h>

#include "H_Standard.h"
#include "H_Image.h"

static void SkipComments(std::ifstream &infile)
{
	char TmpText[1024];
	while (infile.peek() == '#')
		infile.getline(TmpText,1024);
}

void H_ReadImage(const char *FileName, unsigned char * &buffer, int &width, int &height)
{
	char TmpText[1024];

	ifstream infile;
	OpenDataFile(infile, FileName);
	if (!infile)
		throw HException(string("file : \"")+FileName+"\" failed to open.");

	SkipComments(infile);
	infile.getline(TmpText,1024);
	TmpText[2] = '\0';
	if (strcmp(TmpText,"P6") == 0)
	{
		SkipComments(infile);
		infile >> width >> height;
		infile.getline(TmpText,1024);
		SkipComments(infile);
		int ColorDeapth;
		infile.getline(TmpText,1024);
		ColorDeapth = atoi(TmpText);

		int DataSize = width*height*3;
		buffer = new unsigned char[DataSize+15];
		if (buffer == NULL)
			throw HException(string("picture file : \"")+FileName+("\" invalid format?"));
		infile.read((char*)buffer, DataSize+15);
		//if(infile.gcount()!=DataSize)
		//	throw HException(string("picture file : \"")+FileName+("\" invalid format?"));
	}
/*	else if (strcmp(TmpText,"P3") == 0)
	{
		SkipComments(infile);
		infile >> width >> height;
		infile.getline(TmpText,1024);
		SkipComments(infile);
		//get the colordeapth line...
		infile.getline(TmpText,1024);

		int DataSize = width*height*3;
		buffer = new unsigned char[DataSize];
		if (buffer == NULL)
			throw HException(string("picture file : \"")+FileName+("\" invalid format?"));
		int tmp;
		for (int i=0 ; i<DataSize ; i++)
		{
			infile >> tmp;
			buffer[i] = (unsigned char)tmp;
		}
	}*/
	else
		throw HException(string("unsupported or corrupted picture file : \"")+FileName+("\"."));
}
