#!/bin/bash
# Generate a batch job for every mac file in folder
# kill all jobs: qdel -u <user>

# Which MC version is used?
MCTAG="0.1.7"
# work in a specific path
WDIR="/archive_lngs15TB/mc/althueser/optPhot/${MCTAG}_test"
# simulation path
MCPATH="/home/althueser/geant4_workdir/xenon1t_GitHub/"
#simulation binary name
MCNAME="xenon1t_G4_GitHub"

# root file number
RNUMBER="01"
# Get mac files of
MACNEW="mac_new"
# move processed files to (if root file exists)
MACPROCESSED="mac_processed"
# generated rootfiles dir
ROOTFILES="rootfiles"
# number of events to simulate
EVENTS="10000"
# Cluster type
CLUSTER="ULITE" #xecluster
CLUSTERMAIL="#PBS -M coding@l-althueser.de"
CLUSTERJOBNAME="Xe1T-${MCTAG}-optPhot"

FILESSUBMITTED=0
verbose=0

if [ ! -d "${WDIR}" ]; 
then
	echo "Can not find: ${WDIR}"
	exit
fi

echo "------------------------------------------------------"
echo "Generate batch jobs for:"
echo "------------------------------------------------------"
echo "MCTAG: ${MCTAG}"
echo "MCPATH: ${MCPATH}"
echo "MCNAME: ${MCNAME}"
echo "Cluster: ${CLUSTER}"
echo "------------------------------------------------------"

if [ ! -d "${WDIR}/${MACPROCESSED}" ]; then mkdir -p ${WDIR}/${MACPROCESSED}; fi
if [ ! -d "${WDIR}/${ROOTFILES}" ]; then mkdir -p ${WDIR}/${ROOTFILES}; fi

for file in ${WDIR}/${MACNEW}/*.mac
do
	if [ -f $file ];
	then
		if [ -f "${WDIR}/${ROOTFILES}/$(basename "$file" .mac)_${RNUMBER}.root" ];
		then
			mv $file ${WDIR}/${MACPROCESSED}/
			if ((verbose)); then echo echo "$file moved!"; fi
		else
			if ((verbose)); then echo echo "$file"; fi
			cp -f ./template/${CLUSTER}.sh ${WDIR}/run_mac_tmp.sh
			sed -i -- "s#\[MACRO\]#${WDIR}/${MACNEW}/$(basename "$file" .mac).mac#g" ${WDIR}/run_mac_tmp.sh
			sed -i -- "s#\[OUTPUT\]#${WDIR}/${ROOTFILES}/$(basename "$file" .mac)\_${RNUMBER}.root#g" ${WDIR}/run_mac_tmp.sh
			sed -i -- "s#\[MCPATH\]#${MCPATH}#g" ${WDIR}/run_mac_tmp.sh
			sed -i -- "s#\[MCNAME\]#${MCNAME}#g" ${WDIR}/run_mac_tmp.sh
			sed -i -- "s#\[MAIL\]#${CLUSTERMAIL}#g" ${WDIR}/run_mac_tmp.sh
			sed -i -- "s#\[JOBNAME\]#${CLUSTERJOBNAME}#g" ${WDIR}/run_mac_tmp.sh
			
			if [[ $file == *"S1"* ]];
			then
				if ((verbose)); then echo echo "S1 simulation"; fi
				sed -i -- "s#\[EVENTS\]#${EVENTS}#g" ${WDIR}/run_mac_tmp.sh
				sed -i -- "s#\[OPSETUP\]#macros/setup\_optical\_S1.mac#g" ${WDIR}/run_mac_tmp.sh
			else
				if ((verbose)); then echo echo "S2 simulation"; fi
				sed -i -- "s#\[EVENTS\]#${EVENTS}#g" ${WDIR}/run_mac_tmp.sh
				sed -i -- "s#\[OPSETUP\]#macros/setup\_optical\_S2.mac#g" ${WDIR}/run_mac_tmp.sh
			fi
			
			if [[ $CLUSTER == *"ULITE"* ]];
			then
				qsub ${WDIR}/run_mac_tmp.sh
				rm ${WDIR}/run_mac_tmp.sh
				((FILESSUBMITTED++))
			else
				rm ${WDIR}/run_mac_tmp.sh
				echo "Cluster not found: ${CLUSTER}"
				exit
			fi
		fi
 	fi 
done
echo "------------------------------------------------------"
echo "$FILESSUBMITTED files submitted to ${CLUSTER}."
echo "You can check them with: qstat -n1"
echo "------------------------------------------------------"