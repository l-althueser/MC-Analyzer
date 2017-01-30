#!/bin/bash
# Generate a batch job for every mac file in folder
# kill all jobs: qdel -u <user>

# Which MC version is used?
MCTAG="GitHub"
# work in a specific path
WDIR="/archive_lngs100TB/mc/xe1tsim/althueser/testing_${MCTAG}"
# simulation path
MCPATH="/home/althueser/geant4_workdir/xenon1t_GitHub/"
#simulation binary name
MCNAME="xenon1t_G4_GitHub"

# Get mac files of
MACNEW="mac_new"
# move processed files to (if root file exists)
MACPROCESSED="mac_processed"
# generated rootfiles
ROOTFILES="rootfiles"
# Cluster type
CLUSTER="ULITE" #xecluster

FILESSUBMITTED=0
verbose=0

if [ ! -d "${WDIR}" ]; 
then
	echo "Cn not find: ${WDIR}"
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

for file in ${WDIR}/${MACNEW}/*.mac
do
	if [ -f "${WDIR}/${ROOTFILES}/$(basename "$file" .mac)_01.root" ];
	then
		mv $file ${WDIR}/${MACPROCESSED}/
		if ((verbose)); then echo echo "$file moved!"; fi
	else
		if ((verbose)); then echo echo "$file"; fi
		cp -f ./template/${CLUSTER}.sh ${WDIR}/run_mac_tmp.sh
		sed -i -- "s#\[MACRO\]#${WDIR}/${MACNEW}/$(basename "$file" .mac).mac#g" ${WDIR}/run_mac_tmp.sh
		sed -i -- "s#\[OUTPUT\]#${WDIR}/${ROOTFILES}/$(basename "$file" .mac)\_01.root#g" ${WDIR}/run_mac_tmp.sh
		sed -i -- "s#\[MCPATH\]#${MCPATH}#g" ${WDIR}/run_mac_tmp.sh
		sed -i -- "s#\[MCNAME\]#${MCNAME}#g" ${WDIR}/run_mac_tmp.sh
		
		if [[ $file == *"S1"* ]];
		then
			if ((verbose)); then echo echo "S1 simulation"; fi
			sed -i -- "s#\[EVENTS\]#100#g" ${WDIR}/run_mac_tmp.sh
			#sed -i -- "s#\[EVENTS\]#1000000#g" ${WDIR}/run_mac_tmp.sh
			sed -i -- "s#\[OPSETUP\]#setup\_optical\_S1.mac#g" ${WDIR}/run_mac_tmp.sh
		else
			if ((verbose)); then echo echo "S2 simulation"; fi
			sed -i -- "s#\[EVENTS\]#100#g" ${WDIR}/run_mac_tmp.sh
			#sed -i -- "s#\[EVENTS\]#100000#g" ${WDIR}/run_mac_tmp.sh
			sed -i -- "s#\[OPSETUP\]#setup\_optical\_S2.mac#g" ${WDIR}/run_mac_tmp.sh
		fi
		
		if [[ $CLUSTER == *"ULITE"* ]];
		then
			qsub ${WDIR}/run_mac_tmp.sh
			#rm ${WDIR}/run_mac_tmp.sh
			((FILESSUBMITTED++))
		else
			rm ${WDIR}/run_mac_tmp.sh
			echo "Cluster not found: ${CLUSTER}"
			exit
		fi
		
 	fi 
done
echo "------------------------------------------------------"
echo "$FILESSUBMITTED files submitted to ${CLUSTER}."
echo "------------------------------------------------------"