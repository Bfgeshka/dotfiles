background        no
out_to_console    yes
update_interval   1
total_run_times   0
short_units       yes
uppercase         no
use_spacer        left

#	special			▲▼ ⮀⮁  ⮃⮂ 

#	red				#9A2601
#	yellow			#9DA214
#	green			#318E47
#	blue			#488B84

#	black			#050505
#	white			#ffffff

TEXT
%{B\#ff050505} \
${if_up eth0}%{B\#ff9A2601} %{F\#ffffffff}${upspeed eth0} %{F\#ff00000}⇅ %{F\#ffffffff}${downspeed eth0}%{F\#ff00000} %{B\#ff050505}${endif} \
${if_match ${battery_percent} < 95}%{B\#ff488B84} ⭫ $battery_percent%%, $battery_time %{B\#ff050505} ${endif}\
%{B\#ff9DA214} $swap %{B\#ff050505} \
%{A4:rcsh $HOME/.config/dwm/bar_volup.sh:}%{A5:rcsh $HOME/.config/dwm/bar_voldown.sh:}%{B\#ff318E47} ♪ ${exec amixer get Master | grep -o "[1]*[0-9]*%"} %{B\#ff050505} %{A}%{A}\
%{c}\
${if_running deadbeef-gtkui}%{B\#ff488B84} \
%{A:deadbeef --prev:}%{B\#ff050505}%{F\#ff488B84}  ⏪  %{F\#ff00000}%{B\#ff050505}%{A}\
\
%{A3:deadbeef --stop:}%{A1:deadbeef --toggle-pause:}%{A4:rcsh $HOME/.config/dwm/bar_volup.sh:}%{A5:rcsh $HOME/.config/dwm/bar_voldown.sh:}%{A2:rcsh $HOME/.dwm/bar_mute.sh:}\
%{B\#ff488B84}  ${exec deadbeef --nowplaying "%a <~*~> %t  %e/%l" 2>/dev/null}  \
\
%{A}%{A}%{A}%{A}%{A}\
%{A:deadbeef --next:}%{B\#ff050505}%{F\#ff488B84}  ⏩  %{F\#ff00000}%{B\#ff050505}%{A}\
%{B\#ff488B84} %{B\#ff050505} \
${endif}\
%{r}\
%{B\#ff318E47} $mem %{B\#ff050505} \
%{B\#ff9DA214} ${execi 30 $HOME/.config/dwm/weather.sh} %{B\#ff050505} \
%{B\#ff9A2601} %{F\#ffffffff}$cpu%  $loadavg %{F\#ff00000}%{B\#ff050505} \
