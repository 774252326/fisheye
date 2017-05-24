cd /home/pi/fisheye/ocamMeasure
lxterminal -e "../build-agvred-Desktop-Release/agvred"

gnome-terminal -e "/home/u/fisheye//build-ocamMeasure-Desktop-Release/ocamMeasure /home/u/fisheye/ocamMeasure/2.1mm/calib_results1920x1080.txt 120 /dev/ttyACM0"
