#!/bin/bash
# Generate mac files for optical simulations (S1 and S2).
# New mac files will only be generated in "./<mac folder>_new/*" if the file 
# is not already present in "./<mac folder>_processed/*".

# Which MC version is used?
MCTAG="GitHub"
# work in a specific path
WDIR="/archive_lngs100TB/mc/xe1tsim/althueser/testing_${MCTAG}"

# Save generated files to
MACNEW="mac_new"
# ... with respect to already used macs in
MACPREVGEN="mac_processed"
# Generate mac files for which optical simulation case?
# .. use ("S1" "S2") for all available
SIMULATION=("S1" "S2")
# Define all possible values for matching simulations
LXeR=(63) # "1." is added automatically
TR=(99) # % | "0." is added automatically | for GXeTR and LXeTR
LXeAbsL=(5000) # cm
GXeAbsL=(10000) # cm
LXeRSL=(30) # cm

FILESGENERATED=0
verbose=1

if [ ! -d "${WDIR}" ]; 
then
	echo "You have to create: ${WDIR}"
	exit
fi
	
echo "------------------------------------------------------"
echo "Generate mac files for the following parameter space:"
echo "------------------------------------------------------"
echo "MCTAG: ${MCTAG}"
echo "SIMULATION: ${SIMULATION[@]}"
echo "LXeR: ${LXeR[@]}"
echo "TR: ${TR[@]}"
echo "LXeAbsL: ${LXeAbsL[@]}"
echo "GXeAbsL: ${GXeAbsL[@]}"
echo "LXeRSL: ${LXeRSL[@]}"
echo ".. save in: ${WDIR}/${MACNEW}/*"
echo "------------------------------------------------------"

# prepare output folder
if [ ! -d "${WDIR}/${MACNEW}" ]; then mkdir -p ${WDIR}/${MACNEW}; fi

if [ -d "./template" ]; 
then
	for S in "${SIMULATION[@]}"
	do
		for i in "${LXeR[@]}"
		do
		   for j in "${TR[@]}"
			do
			   for k in "${LXeAbsL[@]}"
				do
				   for l in "${GXeAbsL[@]}"
					do
					   for m in "${LXeRSL[@]}"
						do
							if [ -f "${WDIR}/${MACPREVGEN}/${MCTAG}_${S}_${j}_${j}_${k}_${l}_${m}_1${i}.mac" ];
							then
								if ((verbose)); then echo "Skip: ${MCTAG}_${S}_${j}_${j}_${k}_${l}_${m}_1${i}.mac"; fi
							else
								if ((verbose)); then echo "${S}: LXeR $i, LXeTR $j, GXeTR $j, LXeAbsL $k, GXeAbsL $l, LXeRSL $m"; fi
								# copy from template and generate folder (if needed)
								cp -f ./template/${S}.mac ${WDIR}/${MACNEW}/${MCTAG}_${S}_${j}_${j}_${k}_${l}_${m}_1${i}.mac
								# change values
								sed -i -- "s/\[LXeR\]/1.${i}/g" ${WDIR}/${MACNEW}/${MCTAG}_${S}_${j}_${j}_${k}_${l}_${m}_1${i}.mac
								sed -i -- "s/\[LXeTR\]/0.${j}/g" ${WDIR}/${MACNEW}/${MCTAG}_${S}_${j}_${j}_${k}_${l}_${m}_1${i}.mac
								sed -i -- "s/\[GXeTR\]/0.${j}/g" ${WDIR}/${MACNEW}/${MCTAG}_${S}_${j}_${j}_${k}_${l}_${m}_1${i}.mac
								sed -i -- "s/\[LXeAbsL\]/${k} cm/g" ${WDIR}/${MACNEW}/${MCTAG}_${S}_${j}_${j}_${k}_${l}_${m}_1${i}.mac
								sed -i -- "s/\[GXeAbsL\]/${l} cm/g" ${WDIR}/${MACNEW}/${MCTAG}_${S}_${j}_${j}_${k}_${l}_${m}_1${i}.mac
								sed -i -- "s/\[LXeRSL\]/${m} cm/g" ${WDIR}/${MACNEW}/${MCTAG}_${S}_${j}_${j}_${k}_${l}_${m}_1${i}.mac
								((FILESGENERATED++))
							fi
						done
					done
				done
			done
		done
	done
else
	echo "No templates found!"
fi

echo "------------------------------------------------------"
echo "$FILESGENERATED files generated."
echo "------------------------------------------------------"