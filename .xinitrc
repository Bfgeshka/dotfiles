#! /bin/mksh

xrdb ~/.Xresources

compton &
sudo xboxdrv --detach-kernel-driver -d --silent --mimic-xpad &
dwmstatus &
spacefm &
geany &
nachalnick -d &
sh ~/bin/pathadd/wxrandr.sh &

exec dwm
