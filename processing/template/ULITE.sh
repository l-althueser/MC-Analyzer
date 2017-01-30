#!/bin/bash

#PBS -q xe-long
#PBS -N MC-Xe1T-optPhot
#PBS -S /bin/bash
#PBS -o localhost:~/ULITE_output/
#PBS -e localhost:~/ULITE_error/
#PBS -W umask=0133 
#PBS -v variable=mypreferredvar
#PBS -m ae 
#PBS -M l.althueser@uni-muenster.de

sleep 60
echo "Welcome user: $USER"
echo "Today is "`date`
echo "You run on machine "`hostname`
echo "And my variable is $variable"
echo ""

echo "Start simulation ([OUTPUT];[EVENTS]) ..."
echo "Use macro: [MACRO] ..."

. /usr/local/Geant495p01/share/Geant4-9.5.1/geant4make/geant4make.sh
cd [MCPATH]

[MCNAME] -p macros/preinit.mac -s [OPSETUP] -f [MACRO] -o [OUTPUT] -n [EVENTS]
