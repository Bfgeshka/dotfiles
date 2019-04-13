#! /bin/mksh

sh ~/bin/pathadd/wxrandr.sh &

sudo chmod 666 /dev/dri/renderD128
# echo bfq | sudo tee /sys/block/sda/queue/scheduler
# echo bfq | sudo tee /sys/block/sdb/queue/scheduler

mkdir /mnt/tmp/sxiv_cache

# xcompmgr -c &
# compton &
# sudo xboxdrv --detach-kernel-driver -d --silent --mimic-xpad &
dwmstatus &
# spacefm &
geany &
nachalnick -d &

mpv --volume=80 --no-config ./Win98Startup.opus > /dev/null 2>&1 &

## exec dwm
exec /home/bfg/bin/git/dwm_new/dwm/dwm
