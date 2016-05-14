#!/bin/bash
# get workingdirectory
WD=$PWD
# Absolute path to this script, e.g. /home/user/bin/foo.sh
SCRIPT=$(readlink -f "$0")
# Absolute path this script is in, thus /home/user/bin
SCRIPTPATH=$(dirname "$SCRIPT")
# change directory to analysis
cd $SCRIPTPATH

if [ -z ${ROOTSYS+x} ]; 
then 
 exit; 
fi

echo "==========================================="
# execute the analysis scripts with optional arguments
echo "creating konsole/screen: ROOT_analysis"
if [ $(dpkg-query -W -f='${Status}' xfce4-terminal 2>/dev/null | grep -c "ok installed") -eq 1 ];
then
 if [ $(dpkg-query -W -f='${Status}' screen 2>/dev/null | grep -c "ok installed") -eq 0 ];
 then
	#-H or --no-close is also possible ...
 	xfce4-terminal --tab -T "$(date +"20%y%m%d %T")" -x bash -c ". $ROOTSYS/bin/thisroot.sh;root -l 'TPCsim_analysis.C(\"$@\")'"
 	echo "-------------------------------------------"
 	echo "starting new konsole window/tab .."		
 	echo "==========================================="
 else
 	xfce4-terminal --tab -T "$(date +"20%y%m%d %T")" -x bash -c ". $ROOTSYS/bin/thisroot.sh;root -l 'TPCsim_analysis.C(\"$@\")'"
 	echo "-------------------------------------------"
 	echo "starting new konsole window/tab .."		
 	echo "===========================================" 	
	#screen -S "ROOT_analysis" bash -c ". $ROOTSYS/bin/thisroot.sh;root -l 'TPCsim_analysis.C(\"$@\")'"
 	#echo "-------------------------------------------"
	#echo "information about the screen command:"
	#echo "  -> ctrl+A followed by C     : open new window"
	#echo "  -> ctrl+A followed by SPACE : switch window in session"
	#echo "  -> ctrl+A followed by D     : detach screen"
	#echo "  -> ctrl+A followed by ?     : get all shortcuts"
	#echo "  -> ctrl+D or exit           : close current session/windows"
	#echo ""
	#echo "  -> list all screens : screen -r"
	#echo "  -> resume a screen  : screen -r SCREENNAME"
	#echo "==========================================="
 fi
else
 echo "-------------------------------------------"
 echo "xfce4-terminal is not supported by your machine .."		
 echo "==========================================="
 
 if [ $(dpkg-query -W -f='${Status}' konsole 2>/dev/null | grep -c "ok installed") -eq 1 ];
 then
  if [ $(dpkg-query -W -f='${Status}' screen 2>/dev/null | grep -c "ok installed") -eq 0 ];
  then
 	 konsole --new-tab -p tabtitle="$(date +"20%y%m%d %T")" -e bash -c ". $ROOTSYS/bin/thisroot.sh;root -l 'TPCsim_analysis.C(\"$@\")'"
 	 echo "-------------------------------------------"
 	 echo "starting new konsole window/tab .."		
 	 echo "==========================================="
  else
 	 konsole --new-tab -p tabtitle="$(date +"20%y%m%d %T")" -e bash -c ". $ROOTSYS/bin/thisroot.sh;root -l 'TPCsim_analysis.C(\"$@\")'"
 	 #konsole -p tabtitle="$(date +"20%y%m%d %T")" -e screen -S "ROOT_analysis" bash -c ". $ROOTSYS/bin/thisroot.sh;root -l 'TPCsim_analysis.C(\"$@\")'"
 	 echo "-------------------------------------------"
	 echo "information about the screen command:"
	 echo "  -> ctrl+A followed by C     : open new window"
	 echo "  -> ctrl+A followed by SPACE : switch window in session"
	 echo "  -> ctrl+A followed by D     : detach screen"
	 echo "  -> ctrl+A followed by ?     : get all shortcuts"
	 echo "  -> ctrl+D or exit           : close current session/windows"
	 echo ""
	 echo "  -> list all screens : screen -r"
	 echo "  -> resume a screen  : screen -r SCREENNAME"
	 echo "==========================================="
  fi
 else
  echo "-------------------------------------------"
  echo "konsole is not supported by your machine .."		
  echo "==========================================="
 fi
fi





