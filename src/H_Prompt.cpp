
#include <stdio.h>
#include <ctype.h>

#include "H_Prompt.h"
#include "H_Graphics.h"

ofstream herr("log.txt", ios::out);

//CLASS Hgl_ostream
Hgl_ostream::Hgl_ostream(unsigned int ABufferSize) :
	ostream(new Hgl_streambuf(ABufferSize))
{}

Hgl_ostream::~Hgl_ostream()
{
	//delete rdbuf();
}

string Hgl_ostream::HitKey(H_KEY AKey, char c)
{
	return rdHgl_streambuf()->HitKey(*this,AKey,c);
}

void Hgl_ostream::draw()
{
	rdHgl_streambuf()->draw();
}

Hgl_streambuf *Hgl_ostream::rdHgl_streambuf() const
{
	return dynamic_cast<Hgl_streambuf*>(rdbuf());
}

//CLASS Hgl_streambuf
Hgl_streambuf::Hgl_streambuf(unsigned int ABufferSize)
{
	Alloc(ABufferSize);
}

Hgl_streambuf::~Hgl_streambuf() {}

void Hgl_streambuf::Alloc(unsigned int ABufferSize)
{
	//create the buffer
	bufferHead = 1;
	charsInBuffer = 1;
	buffer.resize(ABufferSize);
	buffer[0] = '\n';

	//create the command line
	CursorPos = 1;
	CommandLine.resize(H_MAX(ABufferSize/10,100));
	strcpy(CommandLine,">");
}

unsigned int Hgl_streambuf::nextIndex(int i)
{
	return (++i)%buffer.size();
}
unsigned int Hgl_streambuf::previousIndex(int i)
{
	return (--i)%buffer.size();
}

int Hgl_streambuf::pushChar(int c)
{
	cerr << (char)c;
	buffer[bufferHead] = (char)c;
	bufferHead = nextIndex(bufferHead);
	charsInBuffer = LIMIT(0,charsInBuffer+1,(int)buffer.size());
	return c;
}

string Hgl_streambuf::HitKey(ostream &out, H_KEY AKey, char c)
{
	switch (AKey)
	{
	case  HK_BACK:
		if (CursorPos>1)
		{
			CursorPos--;
			CommandLine[CursorPos] = '\0';
		}
		break;
	case  HK_RETURN:
		{
			CommandLine[CursorPos] = '\0';
			string ReturnedCommand(&(CommandLine[1]));
			out << &(CommandLine[0]) << endl;
			strcpy(CommandLine,">");
			CursorPos = 1;
			return ReturnedCommand;
		}
	default:
		if (CursorPos+2<CommandLine.size() && isprint(c))
		{
			CommandLine[CursorPos] = c;
			CommandLine[CursorPos+1] = '\0';
			CursorPos++;
		}
	}
	return string();
}

Point2D Hgl_streambuf::charCoord(int x, int y)
{
	return Point2D(x*REAL(FONT_WIDTH+2),y*REAL(FONT_HEIGHT+2));
}

void Hgl_streambuf::drawLine(char *LineText, int &StartLine, int CharWidth, int CharHeight)
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

	drawLine(CommandLine, CurrentLine, CharWidth, CharHeight);

	//draw the buffer to the screen
	bool Finnished = false;
	char nextLine[200];
	unsigned int tmp = previousIndex(bufferHead);
	int charsLeft = charsInBuffer;
	while (!Finnished)
	{
		//condition d'arret:
		if ((charsLeft<=1)||(CurrentLine>=(int)CharHeight))
			Finnished=true;
		
		//nouvelle ligne:
		else if (buffer[tmp]=='\n')
		{
			tmp = previousIndex(tmp);
			charsLeft--;
			CurrentLine++;
		}

		//ligne a ecrir:
		else
		{
			unsigned int LineSize = 0;
			while ((charsLeft>0) && buffer[tmp]!='\n')
			{
				if (LineSize<200-1) LineSize++;
				tmp = previousIndex(tmp);
				charsLeft--;
			}
			if (LineSize>0)
			{
				if ((tmp+LineSize)<buffer.size())
					memcpy(nextLine,&(buffer[nextIndex(tmp)]),LineSize*sizeof(char));
				else
				{
					int FirstHalf = buffer.size()-tmp;
					memcpy(nextLine,&(buffer[nextIndex(tmp)]),FirstHalf*sizeof(char));
					memcpy(&(nextLine[FirstHalf]),buffer,(LineSize-FirstHalf)*sizeof(char));
				}	
				nextLine[LineSize]='\0';
				drawLine(nextLine, CurrentLine, CharWidth, CharHeight);
			}
		}
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}
