
#ifndef __H_PROMPT_H_
#define __H_PROMPT_H_

namespace std {}
using namespace std;

#include "H_Keys.h"
#include "H_Graphics.h"
#include <iostream>
#include <fstream>
#include <string>

class Hgl_streambuf : public streambuf
{
public:
	//create a new prompt that draws on this camera
	Hgl_streambuf(unsigned int ABufferSize = 10000);
	string HitKey(ostream &out, H_KEY AKey, char c);
	void draw();
	virtual ~Hgl_streambuf();
	int pushChar(int c);
protected:
    virtual int overflow(int c = EOF) {return pushChar(c);}
    virtual int underflow() {return 0;}
private:
	unsigned int nextIndex(int i);
	unsigned int previousIndex(int i);
	Point2D charCoord(int x, int y);
	void drawLine(char *LineText, int &StartLine, int CharWidth, int CharHeight);
	void Alloc(unsigned int ABufferSize);
private:
	//buffer
	unsigned int bufferHead;
	int charsInBuffer;
	HVector<char> buffer;

	//CommandLine
	unsigned int CursorPos;
	HVector<char> CommandLine;
};

class Hgl_ostream : public ostream
{
public:
	Hgl_ostream(unsigned int ABufferSize = 10000);
	virtual ~Hgl_ostream();
	string HitKey(H_KEY AKey, char c);
	void draw();
private:
	Hgl_streambuf *rdHgl_streambuf() const;
};

extern ofstream herr;

#endif //__H_PROMPT_H_
