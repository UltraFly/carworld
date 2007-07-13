
#ifndef __H_PROMPT_H_
#define __H_PROMPT_H_

#include <iostream>
#include <string>
#include <list>
using namespace std;

#include "H_Keys.h"
#include "H_Geometry.h"

class Hgl_streambuf : public streambuf
{
public:
	Hgl_streambuf();
	virtual ~Hgl_streambuf();
	string HitKey(SDLKey AKey, char c);
	void draw();
protected:
    virtual int overflow(int c = EOF);
    virtual int underflow();
private:
	Point2D charCoord(int x, int y);
	void drawLine(const char* LineText, int &StartLine, int CharWidth, int CharHeight);
private:
	list<string > history;
	string CommandLine;
};

#endif //__H_PROMPT_H_
