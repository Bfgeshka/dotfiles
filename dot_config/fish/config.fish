set -x GREP_COLOR "1;37;45"
# set -x JRUBYOPT "-Xcext.enabled=true"
set -x RBXOPT -X19
set -x BROWSER "/usr/bin/palemoon"
set -x PAGER "/usr/bin/less"
set -x SDL_AUDIODRIVER "alsa"
set -x SDL_JOYSTICK_DEVICE "/dev/input/js0"
set -x TMPFSPATH "/mnt/tmp"

set caveresume "/mnt/tmp/cave_resume"
set default_user bfg

# Paths
test -d /usr/local/sbin          ; and set PATH /usr/local/sbin $PATH
test -d /usr/local/bin           ; and set PATH /usr/local/bin $PATH
test -d /home/bfg/bin/pathadd    ; and set PATH /home/bfg/bin/pathadd $PATH
test -d /home/bfg/.local/bin     ; and set PATH /home/bfg/.local/bin $PATH

# Navigation
function ..   ; cd .. ; end
function ...  ; cd ../.. ; end
function .... ; cd ../../.. ; end
function \~   ; cd ~ ; end

# Utilities
function grep		; command grep --color=auto $argv ; end
function trace		; command lft $argv ; end

function emsync		; sudo emerge --sync ; end
function emupd		; sudo emerge -avuDN --color y world; end
function em			; sudo emerge $argv ; end
function un			; sudo emerge --unmerge $argv ; end

function cem		; sudo cave resolve --resume-file $caveresume $argv; end
function cun		; sudo cave uninstall --resume-file $caveresume $argv ; end
function csync		; sudo cave sync; notify-send "Sync done!" ; end
function cupd		; sudo cave resolve --resume-file $caveresume $argv -c --reinstall-scm weekly --follow-installed-build-dependencies --suggestions display --recommendations display --continue-on-failure if-satisfied world ; end
function cupds		; sudo cave resolve $argv -c --reinstall-scm weekly --follow-installed-build-dependencies --suggestions display --recommendations display --continue-on-failure if-satisfied system ; end
function cres		; sudo cave resume --resume-file $caveresume $argv; end

function sss		; sudo geany ; end
function wfallout	; cd /home/bfg/bin/wine/prefixes/fallout/drive_c/Fallout2; env WINEPREFIX=/home/bfg/bin/wine/prefixes/fallout WINEARCH=win32 WINEDLLOVERRIDES="ddraw.dll=n" wine FALLOUT2.EXE; cd ~; end
function frecord	; ffmpeg -f x11grab -s 1366x768 -i :0.0 -r 25 -vcodec libx264 -vsync 0 ~/output.mkv; end

function steamenv   ; sh ~/.local/share/Steam/ubuntu12_32/steam-runtime/run.sh $argv; end


function make_completion --argument-names alias command
    echo "
    function __alias_completion_$alias
        set -l cmd (commandline -o)
        set -e cmd[1]
        complete -C\"$command \$cmd\"
    end
    " | .
    complete -c $alias -a "(__alias_completion_$alias)"
end

function fish_greeting
	clear
	echo "	><(((\">"
	set_color normal
end

