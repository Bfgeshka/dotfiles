#! /bin/rcsh

BG='#15161a'
FG='#cb2322'

# exec $HOME/.dwm/fonts.sh &
# exec sudo killall openvpn &
sudo ifconfig wlan0 down &
# exec export NO_AT_BRIDGE=1 &
rofi -rkey mod1+grave -bg $BG -bc $FG -bw 0 -fg $FG -hlfg $BG -hlbg $FG -font Ohsnap.Icons-9 -width 50 -opacity 80 -loc 0 -padding 10 -columns 3 &
# rofi -rkey mod1+grave -loc 0 -padding 10 &
dunst &
if (!{pgrep udevil}) {exec devmon} &
if (!{pgrep urxvtd}) {exec urxvtd -q -f} &
# {pgrep -v devmon && devmon -s --ignore-device /dev/sdb1} &
nitrogen --restore &
# xcompmgr -c &
# sxhkd -c $HOME/.dwm/sxhkd.rc &
deadbeef &
subl &
# exec stalonetray -i 12 --geometry 1x1-700-12 -bg $BG &
# exec sudo openvpn $HOME/.config/antizapret.ovpn &
doublecmd &
$HOME/bin/pathadd/remount &
sudo /etc/init.d/transmission-daemon restart &
# {sh -s conky -c $HOME/.dwm/conkyrcbar | sh -cbar -b -g 1366x12 -d -f "-lucy-tewi-medium-r-normal--11-90-75-75-p-58-iso10646-1" -p | rcsh} &
# bash -c "conky -c $HOME/.dwm/conkyrcbar | lemonbar -b -g 1366x12 -d -f "-lucy-tewi-medium-r-normal--11-90-75-75-p-58-iso10646-1" -p | rcsh" &

conky -c $HOME/.dwm/conkyrcbar | lemonbar -b -g 1366x12 -d -f "-lucy-tewi-medium-r-normal--11-90-75-75-p-58-iso10646-1" -p | rcsh &
sudo openvpn ~/.config/antizapret.ovpn &
