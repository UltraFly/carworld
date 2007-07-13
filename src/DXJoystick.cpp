
/*#pragma warning( disable : 4786 ) //disable "identifier name too long" warning

#define INITGUID
#include <math.h>
#include "DXJoystick.h"


DXJoystick::DXJoystick(HINSTANCE hInst, HWND hWnd) :
	valid(false),
	JoyName("no joystick found"),
	g_pDI(NULL),
	g_pJoystick(NULL),
	g_pEffect(NULL),
	g_hInst(hInst)
{
    HRESULT hr;

    //Register with the DirectInput subsystem and get a pointer
    //to a IDirectInput interface we can use.
    hr = DirectInputCreate(g_hInst, DIRECTINPUT_VERSION, &g_pDI, NULL);
    if (FAILED(hr)) 
        return;

    //find the first joystick
    hr = g_pDI->EnumDevices(
		DIDEVTYPE_JOYSTICK,
		EnumFFJoysticksCallback,
		(void*)this,
		DIEDFL_ATTACHEDONLY
	);
    if (FAILED(hr)) 
        return;

    if (g_pJoystick == NULL)
        return;

    //Set the data format to "simple joystick" - a predefined data format
    //This tells DirectInput that we will be passing a
    //DIJOYSTATE structure to IDirectInputDevice2::GetDeviceState.
    hr = g_pJoystick->SetDataFormat(&c_dfDIJoystick);
    if (FAILED(hr)) 
        return;

    //Exclusive access is required in order to perform force feedback.
    hr = g_pJoystick->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
    if (FAILED(hr)) 
        return;
	
    didc.dwSize = sizeof(DIDEVCAPS);
    hr = g_pJoystick->GetCapabilities(&didc);
    if (FAILED(hr))
        return;

	//get the ranges of the axes (I suppose the are all the same as the X axis range...)
	{
		DIPROPRANGE   dipr;
		dipr.diph.dwSize        = sizeof(DIPROPRANGE);
		dipr.diph.dwHeaderSize  = sizeof(dipr.diph);
		dipr.diph.dwHow         = DIPH_BYOFFSET;
		//get the x-axis range property
		dipr.diph.dwObj         = DIJOFS_X;
		hr = g_pJoystick->GetProperty(DIPROP_RANGE, &dipr.diph);
		lMin = dipr.lMin;
		lMax = dipr.lMax;
	}

	//this is a force feedback joystick
	if (didc.dwFlags & DIDC_FORCEFEEDBACK)
	{
		cout << "force feedback detected...\n";
		//Since we will be playing force feedback effects,
		//we should disable the auto-centering spring.
		DIPROPDWORD dipdw;
		dipdw.diph.dwSize = sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		dipdw.diph.dwObj = 0;
		dipdw.diph.dwHow = DIPH_DEVICE;
		dipdw.dwData = FALSE;

		hr = g_pJoystick->SetProperty(DIPROP_AUTOCENTER, &dipdw.diph);
		if ( FAILED(hr) ) 
			return;

		//This application needs only one effect:  Applying raw forces.
		DIEFFECT eff;
		DWORD rgdwAxes[2] = { DIJOFS_X, DIJOFS_Y };
		LONG rglDirection[2] = { 0, 0 };
		DICONSTANTFORCE cf = { 0 };

		eff.dwSize = sizeof(DIEFFECT);
		eff.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
		eff.dwDuration = INFINITE;
		eff.dwSamplePeriod = 0;
		eff.dwGain = DI_FFNOMINALMAX;
		eff.dwTriggerButton = DIEB_NOTRIGGER;
		eff.dwTriggerRepeatInterval = 0;
		eff.cAxes = 2;
		eff.rgdwAxes = rgdwAxes;
		eff.rglDirection = rglDirection;
		eff.lpEnvelope = 0;
		eff.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
		eff.lpvTypeSpecificParams = &cf;

		// create the prepared effect
		hr = g_pJoystick->CreateEffect(GUID_ConstantForce, &eff, &g_pEffect, NULL);
		cout << "force feedback engaged!\n";
	}

	valid = SUCCEEDED(hr);
}


//Initialize the DirectInput variables.
DXJoystick::~DXJoystick()
{
    // Release any DirectInputEffect objects.
    if (g_pEffect != NULL) 
    {
        g_pEffect->Release();
        g_pEffect = NULL;
    }

    // Unacquire and release any DirectInputDevice objects.
    if (g_pJoystick != NULL) 
    {
        // Unacquire the device one last time just in case 
        // the app tried to exit while the device is still acquired.
        g_pJoystick->Unacquire();

        g_pJoystick->Release();
        g_pJoystick = NULL;
    }

    // Release any DirectInput objects.
    if (g_pDI != NULL) 
    {
        g_pDI->Release();
        g_pDI = NULL;
    }
}

bool DXJoystick::IsValid()
{
	return valid;
}

const char *DXJoystick::GetDescription()
{
	return JoyName.c_str();
}

int DXJoystick::GetNumAxis()
{
	return didc.dwAxes;
}

int DXJoystick::GetNumButtons()
{
	return didc.dwButtons;
}

void DXJoystick::UpdateState()
{
	if (!valid) return;

    HRESULT hr;

	//gpdiJoystick->Acquire();
    //poll the joystick to read the current state
    hr = g_pJoystick->Poll();
	
    // read the device state
    hr = g_pJoystick->GetDeviceState(sizeof(DIJOYSTATE), &dijs);
	
    if(FAILED(hr))
    {
        if((hr == DIERR_INPUTLOST))
           SetAcquire(true);
        return;
    }
}

REAL DXJoystick::GetAxisPos(int i)
{
	UpdateState();
	switch (i)
	{
	case 0: return 1 - (2.*dijs.lX-lMin)/lMax;
	case 1: return 1 - (2.*dijs.lY-lMin)/lMax;
	case 2: return 1 - (2.*dijs.lZ-lMin)/lMax;
	case 3: return 1 - (2.*dijs.lRx-lMin)/lMax;
	case 4: return 1 - (2.*dijs.lRy-lMin)/lMax;
	case 5: return 1 - (2.*dijs.lRz-lMin)/lMax;
	case 6: return 1 - (2.*dijs.rglSlider[0]-lMin)/lMax;
	case 7: return 1 - (2.*dijs.rglSlider[1]-lMin)/lMax;
	default: return 0;
	}
}

bool DXJoystick::GetButtonPos(int i)
{
	UpdateState();
	if ((i>=0) && (i<32))
		return (dijs.rgbButtons[i] & 0x80) != 0;
	else
		return false;
}

//Acquire or unacquire the mouse, depending on if the app is active
//Input device must be acquired before the GetDeviceState is called
bool DXJoystick::SetAcquire(bool Active)
{
    // nothing to do if g_pJoystick is NULL
    if (g_pJoystick == NULL)
        return false;

    if (Active) 
    {
        // acquire the input device 
        g_pJoystick->Acquire();

        if (g_pEffect != NULL) 
        {       
            g_pEffect->Start( 1, 0 ); // start the effect
        }

    } 
    else 
    {
        // unacquire the input device 
        g_pJoystick->Unacquire();
    }

    return true;
}


//Apply the X and Y forces to the effect we prepared.
bool DXJoystick::SetForceFeedback(REAL x, REAL y)
{
	//don't do anythin if there is no force feedback
	if (!(didc.dwFlags & DIDC_FORCEFEEDBACK))
		return false;

	// Modifying an effect is basically the same as creating
	// a new one, except you need only specify the parameters
	// you are modifying; the rest are left alone.
	LONG rglDirection[2] =
	{
		(LONG)max(min(x*DI_FFNOMINALMAX,DI_FFNOMINALMAX),-DI_FFNOMINALMAX),
		(LONG)max(min(y*DI_FFNOMINALMAX,DI_FFNOMINALMAX),-DI_FFNOMINALMAX)
	};

	DICONSTANTFORCE cf;
	DIEFFECT eff;
	cf.lMagnitude = (DWORD)sqrt( double(rglDirection[0]*rglDirection[0] +
								 rglDirection[1]*rglDirection[1]) );
	eff.dwSize = sizeof(DIEFFECT);
	eff.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
	eff.cAxes = 2;
	eff.rglDirection = rglDirection;
	eff.lpEnvelope = 0;
	eff.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
	eff.lpvTypeSpecificParams = &cf;

	// now set the new parameters and start the effect immediately.
	HRESULT hr = g_pEffect->SetParameters( &eff, 
							DIEP_DIRECTION |
							DIEP_TYPESPECIFICPARAMS |
							DIEP_START );

	return SUCCEEDED(hr);
}


//Called once for each enumerated force feedback joystick.
//If we find one, create a device interface on it so we can
//play with it.
BOOL CALLBACK DXJoystick::EnumFFJoysticksCallback(const DIDEVICEINSTANCE * pInst, VOID* lpvContext)
{
    HRESULT                 hr;
    LPDIRECTINPUTDEVICE     pDevice;
    LPDIRECTINPUTDEVICE2    pDevice2;
	DXJoystick* This = (DXJoystick*)lpvContext;

    // obtain an interface to the enumerated force feedback joystick.
    hr = This->g_pDI->CreateDevice( pInst->guidInstance, &pDevice, NULL );

    // if it failed, then we can't use this joystick for some
    // bizarre reason.  (Maybe the user unplugged it while we
    // were in the middle of enumerating it.)  So continue enumerating
    if (FAILED(hr)) 
        return DIENUM_CONTINUE;

    // query for IDirectInputDevice2
    hr = pDevice->QueryInterface( IID_IDirectInputDevice2, (LPVOID *)&pDevice2);
    pDevice->Release(); // done with old interface now

    if (FAILED(hr)) 
    {
        return DIENUM_CONTINUE;  // try again
    }

    // we successfully created an IDirectInputDevice2.  So stop looking 
    // for another one.
    This->g_pJoystick = pDevice2;
	This->JoyName = pInst->tszProductName;

    return DIENUM_STOP;
}*/

