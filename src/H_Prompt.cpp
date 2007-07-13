
#include <stdio.h>
#include <ctype.h>
#include <fstream>

#include "H_Standard.h"
#include "H_Prompt.h"
#include "H_Graphics.h"

ofstream herr("log.txt", ios::out);

//CLASS Hgl_streambuf
Hgl_streambuf::Hgl_streambuf()
{
	history.push_back(string());
	CommandLine = ">";
}

Hgl_streambuf::~Hgl_streambuf() {}

int Hgl_streambuf::overflow(int c)
{
	herr << (char)c;
	if (c=='\n')
	{
		history.push_back(string());
	}
	else
	{
		history.back() += char(c);
	}
	return c;
}

int Hgl_streambuf::underflow()
{
	return 0;
}

string Hgl_streambuf::HitKey(SDLKey AKey, char c)
{
	switch (AKey)
	{
	case  SDLK_BACKSPACE:
		if (CommandLine.size()>1)
		{
			CommandLine.erase(CommandLine.size()-1, 1);
		}
		break;
	case  SDLK_RETURN:
		{
			string ReturnedCommand = CommandLine;
			ReturnedCommand.erase(0,1);
			sputn(CommandLine.c_str(), CommandLine.size());
			sputn("\n", 1);
			CommandLine = ">";
			return ReturnedCommand;
		}
	default:
		if (isprint(c))
		{
			CommandLine += char(c);
		}
	}
	return string();
}

Point2D Hgl_streambuf::charCoord(int x, int y)
{
	return Point2D(x*REAL(FONT_WIDTH+2),y*REAL(FONT_HEIGHT+2));
}

void Hgl_streambuf::drawLine(const char* LineText, int &StartLine, int CharWidth, int CharHeight)
{
	Hgl::WriteText(LineText, charCoord(0,StartLine));
}

void Hgl_streambuf::draw()
{
	HRect ViewPort = Hgl::GetViewPort();

    glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0,ViewPort.GetWidth(),0,ViewPort.GetHeight(), -1.0, 1.0 );

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	Hgl::SetColor(White);

	unsigned int CharWidth = (unsigned int)(ViewPort.GetWidth()/(FONT_WIDTH+2));
	unsigned int CharHeight = (unsigned int)(ViewPort.GetHeight()/(FONT_HEIGHT+2));
	int CurrentLine= 0;

	drawLine(CommandLine.c_str(), CurrentLine, CharWidth, CharHeight);
	for (list<string >::reverse_iterator I=history.rbegin() ; I!=history.rend() ; I++)
	{
		CurrentLine++;
		drawLine((*I).c_str(), CurrentLine, CharWidth, CharHeight);
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}
