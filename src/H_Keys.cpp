
#include "H_Keys.h"

static pair<H_KEY,string> KeyMapValues[] =
{
	pair<H_KEY,string>(HK_INVALID,"INVALID"),
	pair<H_KEY,string>(HK_LBUTTON,"MOUSE1"),
	pair<H_KEY,string>(HK_RBUTTON,"MOUSE2"),
	pair<H_KEY,string>(HK_CANCEL ,"CANCEL"),
	pair<H_KEY,string>(HK_MBUTTON,"MOUSE3"),
	pair<H_KEY,string>(HK_BACK   ,"BACK"),
	pair<H_KEY,string>(HK_TAB    ,"TAB"),
	pair<H_KEY,string>(HK_CLEAR  ,"CLR"),
	pair<H_KEY,string>(HK_RETURN ,"RETURN"),
	pair<H_KEY,string>(HK_SHIFT  ,"SHIFT"),
	pair<H_KEY,string>(HK_CONTROL,"CTRL"),
	pair<H_KEY,string>(HK_MENU   ,"MENU"),
	pair<H_KEY,string>(HK_PAUSE  ,"PAUSE"),
	pair<H_KEY,string>(HK_ESCAPE ,"ESC"),
	pair<H_KEY,string>(HK_SPACE  ,"SPACE"),
	pair<H_KEY,string>(HK_PRIOR  ,"PRIOR"),
	pair<H_KEY,string>(HK_NEXT   ,"NEXT"),
	pair<H_KEY,string>(HK_END    ,"END"),
	pair<H_KEY,string>(HK_HOME   ,"HOME"),
	pair<H_KEY,string>(HK_LEFT   ,"LEFTARROW"),
	pair<H_KEY,string>(HK_UP     ,"UPARROW"),
	pair<H_KEY,string>(HK_RIGHT  ,"RIGHTARROW"),
	pair<H_KEY,string>(HK_DOWN   ,"DOWNARROW"),
	pair<H_KEY,string>(HK_SELECT ,"SELECT"),
	pair<H_KEY,string>(HK_PRINT  ,"PRINT"),
	pair<H_KEY,string>(HK_EXECUTE,"EXECUTE"),
	pair<H_KEY,string>(HK_INSERT ,"INSERT"),
	pair<H_KEY,string>(HK_DELETE ,"DEL"),
	pair<H_KEY,string>(HK_HELP   ,"HELP"),
	pair<H_KEY,string>(HK_0      ,"0"),
	pair<H_KEY,string>(HK_1      ,"1"),
	pair<H_KEY,string>(HK_2      ,"2"),
	pair<H_KEY,string>(HK_3      ,"3"),
	pair<H_KEY,string>(HK_4      ,"4"),
	pair<H_KEY,string>(HK_5      ,"5"),
	pair<H_KEY,string>(HK_6      ,"6"),
	pair<H_KEY,string>(HK_7      ,"7"),
	pair<H_KEY,string>(HK_8      ,"8"),
	pair<H_KEY,string>(HK_9      ,"9"),
	pair<H_KEY,string>(HK_A      ,"a"),
	pair<H_KEY,string>(HK_B      ,"b"),
	pair<H_KEY,string>(HK_C      ,"c"),
	pair<H_KEY,string>(HK_D      ,"d"),
	pair<H_KEY,string>(HK_E      ,"e"),
	pair<H_KEY,string>(HK_F      ,"f"),
	pair<H_KEY,string>(HK_G      ,"g"),
	pair<H_KEY,string>(HK_H      ,"h"),
	pair<H_KEY,string>(HK_I      ,"i"),
	pair<H_KEY,string>(HK_J      ,"j"),
	pair<H_KEY,string>(HK_K      ,"k"),
	pair<H_KEY,string>(HK_L      ,"l"),
	pair<H_KEY,string>(HK_M      ,"m"),
	pair<H_KEY,string>(HK_N      ,"n"),
	pair<H_KEY,string>(HK_O      ,"o"),
	pair<H_KEY,string>(HK_P      ,"p"),
	pair<H_KEY,string>(HK_Q      ,"q"),
	pair<H_KEY,string>(HK_R      ,"r"),
	pair<H_KEY,string>(HK_S      ,"s"),
	pair<H_KEY,string>(HK_T      ,"t"),
	pair<H_KEY,string>(HK_U      ,"u"),
	pair<H_KEY,string>(HK_V      ,"v"),
	pair<H_KEY,string>(HK_W      ,"w"),
	pair<H_KEY,string>(HK_X      ,"x"),
	pair<H_KEY,string>(HK_Y      ,"y"),
	pair<H_KEY,string>(HK_Z      ,"z"),
	pair<H_KEY,string>(HK_NUMPAD0,"NUMPAD0"),
	pair<H_KEY,string>(HK_NUMPAD1,"NUMPAD1"),
	pair<H_KEY,string>(HK_NUMPAD2,"NUMPAD2"),
	pair<H_KEY,string>(HK_NUMPAD3,"NUMPAD3"),
	pair<H_KEY,string>(HK_NUMPAD4,"NUMPAD4"),
	pair<H_KEY,string>(HK_NUMPAD5,"NUMPAD5"),
	pair<H_KEY,string>(HK_NUMPAD6,"NUMPAD6"),
	pair<H_KEY,string>(HK_NUMPAD7,"NUMPAD7"),
	pair<H_KEY,string>(HK_NUMPAD8,"NUMPAD8"),
	pair<H_KEY,string>(HK_NUMPAD9,"NUMPAD9"),
	pair<H_KEY,string>(HK_MULTIPLY,"*"),
	pair<H_KEY,string>(HK_ADD    ,"+"),
	pair<H_KEY,string>(HK_SEPARATOR,"HK_SEPARATOR"), //?
	pair<H_KEY,string>(HK_SUBTRACT,"-"),
	pair<H_KEY,string>(HK_DECIMAL,"."),
	pair<H_KEY,string>(HK_DIVIDE ,"/"),
	pair<H_KEY,string>(HK_F1     ,"F1"),
	pair<H_KEY,string>(HK_F2     ,"F2"),
	pair<H_KEY,string>(HK_F3     ,"F3"),
	pair<H_KEY,string>(HK_F4     ,"F4"),
	pair<H_KEY,string>(HK_F5     ,"F5"),
	pair<H_KEY,string>(HK_F6     ,"F6"),
	pair<H_KEY,string>(HK_F7     ,"F7"),
	pair<H_KEY,string>(HK_F8     ,"F8"),
	pair<H_KEY,string>(HK_F9     ,"F9"),
	pair<H_KEY,string>(HK_F10    ,"F10"),
	pair<H_KEY,string>(HK_F11    ,"F11"),
	pair<H_KEY,string>(HK_F12    ,"F12"),
	pair<H_KEY,string>(HK_NUMLOCK,"NUM"),
	pair<H_KEY,string>(HK_SCROLL ,"HK_SCROLL")
};

HMapper<H_KEY,string> KeyMap(
	KeyMapValues,
	KeyMapValues+(sizeof(KeyMapValues)/sizeof(pair<H_KEY,string>))
);

ostream &operator<<(ostream &out, const H_KEY key)
{
	return out << KeyMap.find(key);
}

istream &operator>>(istream &in, H_KEY &key)
{
	string tmp;
	getline(in,tmp,'\0');
	key = KeyMap.find(tmp);
	return in;
}


