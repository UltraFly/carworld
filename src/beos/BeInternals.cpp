
#include <stdlib.h>

#include "BeJoystick.h"
#include "BeInternals.h"

//CLASS HBGLView
HBGLView::HBGLView(HglApplication *app, BRect r, char *name, ulong resizingMode, ulong options)
	: BGLView(r,name,resizingMode,0,options),
	m_app(app)
{}

HBGLView::~HBGLView()
{}

//BGLView METHODS
void HBGLView::AttachedToWindow()
{

	BGLView::AttachedToWindow();
	Window()->SetPulseRate(100000);
	
	LockGL();

	m_app->draw_init();
	BRect r = Bounds();
	m_app->resize(r.IntegerWidth()+1, r.IntegerHeight()+1);

	DrawFrame(false);

	UnlockGL();
}

void HBGLView::DetachedFromWindow()
{
	BGLView::DetachedFromWindow();

	long locks=0;
	
	//whats this?
	while (Window()->IsLocked()) {
		locks++;
		Window()->Unlock();
	};

	m_app->draw_shutdown(); //?

	while (locks--) Window()->Lock();
}

void HBGLView::Pulse()
{
//is this necessary?
	Window()->Lock();
	BRect p = Parent()->Bounds();
	BRect b = Bounds();
	p.OffsetTo(0,0);
	b.OffsetTo(0,0);
	if (b != p)
	{
		ResizeTo(p.right-p.left,p.bottom-p.top);
	};
	Window()->Unlock();

	LockGL();
	m_app->on_idle(100); //lock?
	//SwapBuffers();
	UnlockGL();
}

void HBGLView::FrameResized(float w, float h)
{
	LockGL();

	BGLView::FrameResized(w,h);

	BRect b = Bounds();
	w = b.Width();
	h = b.Height();
	
	m_app->resize(w,h);

	//DrawFrame(false);

	UnlockGL();
}

void HBGLView::DrawFrame(bool noPause)
{
	LockGL();

	cerr << "about to draw...\n";
	m_app->draw();

	//SwapBuffers(); //?
	UnlockGL();
}


//CLASS QuitWindow
QuitWindow::QuitWindow(BRect r, char *name, window_type wt, ulong something)
	: BDirectWindow(r,name,wt,something)
{}

QuitWindow::~QuitWindow()
{}

bool QuitWindow::QuitRequested()
{
	bgl->EnableDirectMode( false );
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
};

void QuitWindow::DirectConnected( direct_buffer_info *info )
{
	if( bgl )
		bgl->DirectConnected( info );	
	bgl->EnableDirectMode( true );
}

//CLASS HBeWindow
HBeWindow::HBeWindow() : win(NULL)
{}

HBeWindow::~HBeWindow()
{
//delete win? maybe not...
}

const char *HBeWindow::GetKeyboardDescription()
{
	return "not available";
}

bool HBeWindow::IsPressed(H_KEY k)
{
	return false;
}

HJoystick *HBeWindow::GetJoystick()
{
	return new BeJoystick();
}

void HBeWindow::MakeCurrent()
{}

void HBeWindow::SwapBuffers()
{
	win->bgl->SwapBuffers();
}

void HBeWindow::SetAttrib(int width, int height, bool fullscreen)
{
	m_width = width;
	m_height = height;
	m_fullscreen = fullscreen;
}

void HBeWindow::CreateWindow(HglApplication *app)
{
	BRect r(100,100,300,315);
	win = new QuitWindow(r,"GLTeapot",B_TITLED_WINDOW,0);
	win->Lock();

	r = win->Bounds();
	BView *sv = new BView(r,"subview",B_FOLLOW_ALL,0);
	win->AddChild(sv);

	r = sv->Bounds();
	win->bgl = new HBGLView(app,r,"HBGLView",B_FOLLOW_NONE,BGL_RGB | BGL_DEPTH | BGL_DOUBLE);
	sv->AddChild(win->bgl);

	win->Unlock();
	win->Show();
}
