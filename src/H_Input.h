

#ifndef __H_INPUT_H_
#define __H_INPUT_H_

namespace std {}
using namespace std;

#include "H_Main.h"
#include "H_Standard.h"

class KeyJoystick : public HJoystick
{
public:
	KeyJoystick(HWindow *w);
	virtual ~KeyJoystick();
	virtual bool IsValid();
	virtual const char *GetDescription();
	virtual int GetNumAxis();
	virtual int GetNumButtons();
	virtual void UpdateState();
	virtual REAL GetAxisPos(int i);
	virtual bool GetButtonPos(int i);
	virtual bool SetForceFeedback(REAL x, REAL y);
public:
	SDLKey up_key;
	SDLKey down_key;
	SDLKey left_key;
	SDLKey right_key;
	SDLKey button_key;
	REAL speed;
private:
	HWindow *m_HWindow;
	REAL Yaxis; //from -1(down) to 1(up)
	REAL Xaxis; //from -1(left) to 1(right)
	bool button;
};

#endif //__H_INPUT_H_
