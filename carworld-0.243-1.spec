 
Summary: car simulation useing OpenGL for rendering 
Name: carworld 
Version: 0.242 
Release: 1 
Copyright: GPL 
Group: Amusements/Games 
Source: carworld.sourceforge.net/carworld-0.242.tar.gz 
BuildRoot: /var/tmp/%{name}-buildroot 
 
%description 
Car simulaton with an emphasis on dynamics useing OpenGL for rendering, 
with support for networked mode (up to 8 drivers). 
 
%prep 
%setup 
 
%build 
make release 
 
%install 
make install 
 
%files 
%doc readme.txt 
/usr/bin/carworld 
/usr/share/carworld
