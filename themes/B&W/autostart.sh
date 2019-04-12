#! /bin/rcsh

# BG="#f2f0ec"
# FG="#393939"
# -bg $BG -bc $FG -fg $FG -hlfg $BG -hlbg $FG6 
# exec $HOME/.dwm/fonts.sh &
sudo ifconfig wlan0 down &
# exec export NO_AT_BRIDGE=1 &

# rofi -rkey mod1+grave -loc 0 -padding 10 &
dunst &
# if (!{pgrep udevil}) {exec devmon} &
if (!{pgrep urxvtd}) {exec urxvtd -q -f} &
# nitrogen --restore &
# xcompmgr -c &
# sxhkd -c $HOME/.dwm/sxhkd.rc &
deadbeef &
subl &
doublecmd &
$HOME/bin/pathadd/remount &
# sudo openvpn ~/.config/antizapret.ovpn &

exit 0
