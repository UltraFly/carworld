
#ifndef _BE_INTERNALS_
#define _BE_INTERNALS_

#include <GL/glu.h>
#include <GLView.h>
#include "../H_Main.h"

class HBGLView : public BGLView
{
public:
	HBGLView(HglApplication *app, BRect r, char *name, ulong resizingMode, ulong options);
	virtual ~HBGLView();
//BGLView members
	virtual void AttachedToWindow();
	virtual void DetachedFromWindow();
	virtual void FrameResized(float width, float height);
	virtual void DrawFrame(bool noPause);
	virtual void Pulse();
private:
	HglApplication *m_app;
};

class QuitWindow : public BDirectWindow
{
public:
	QuitWindow(BRect r, char *name, window_type wt, ulong something);
	virtual ~QuitWindow();
//BDirectWindow members
	virtual bool QuitRequested();
	virtual void DirectConnected( direct_buffer_info *info );
	HBGLView *bgl;
};


class HBeWindow : public HWindow
{
public:
	HBeWindow();
	virtual ~HBeWindow();
//HWindow members
	virtual const char *GetKeyboardDescription();
	virtual bool IsPressed(H_KEY k);
	virtual HJoystick *GetJoystick();
	virtual void MakeCurrent();
	virtual void SwapBuffers();
	virtual void SetAttrib(int width, int height, bool fullscreen);
//HBeWindow specific:
	void CreateWindow(HglApplication *app);
private:
	QuitWindow *win;
};

#endif //_BE_INTERNALS_
