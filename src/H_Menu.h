
#ifndef _H_MENU_H_
#define _H_MENU_H_

/*class HMenuItem
{
public:
	virtual ~HMenuItem();
	virtual void Draw(HRect &DrawArea) = 0;
	virtual void OnSelect();
	virtual void OnAction1();
	virtual void OnAction2();
	virtual void OnApply();
};

class HMenu : public HMenuItem
{
public:
	HMenu(HMenu *parent);
	virtual ~HMenu();
	

//draw as a menu item
	virtual void Draw(HRect &DrawArea);
	virtual void OnSelect();

//draw as a menu...
	void DrawMenu(HRect &DrawArea);
	void Add(HMenuItem *child);
	void SetCurrentSubMenu(HMenu *SubMenu);
	void OnKeyDown(H_KEY k);
private:
	HMenu *m_parent;
	list<HMenuItem*> m_children;
	HMenu *m_CurrentItem;
	HMenu *m_CurrentSubMenu;
};*/


#endif //_H_MENU_H_
