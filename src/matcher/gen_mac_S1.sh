#!/bin/bash
MCTAG="rev338"
# Define all possible values for matching simulations
LXeR=(56 60 63 69) # "1." is added automatically
# TR is defined for GXeTR and LXeTR
TR=(99 95 92 90 87) # "0." is added automatically
LXeAbsL=(5000)
GXeAbsL=(10000 100 50 40 30)
LXeRSL=(30 45 60 100)

FILEGENERATED=0

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
					if [ -f "./mac_processed/${MCTAG}_S1_${j}_${j}_${k}_${l}_${m}_1${i}.mac" ];
					then
						echo "Skip: ${MCTAG}_S1_${j}_${j}_${k}_${l}_${m}_1${i}.mac"
					else
						echo LXeR $i, LXeTR $j, GXeTR $j, LXeAbsL $k, GXeAbsL $l, LXeRSL $m
						# copy from template
						cp -f ./gen_mac_template_S1.mac ./mac_new/${MCTAG}_S1_${j}_${j}_${k}_${l}_${m}_1${i}.mac
						# change values
						sed -i -- "s/\[LXeR\]/1.${i}/g" *mac_new/${MCTAG}_S1_${j}_${j}_${k}_${l}_${m}_1${i}.mac*
						sed -i -- "s/\[LXeTR\]/0.${j}/g" *mac_new/${MCTAG}_S1_${j}_${j}_${k}_${l}_${m}_1${i}.mac*
						sed -i -- "s/\[GXeTR\]/0.${j}/g" *mac_new/${MCTAG}_S1_${j}_${j}_${k}_${l}_${m}_1${i}.mac*
						sed -i -- "s/\[LXeAbsL\]/${k} cm/g" *mac_new/${MCTAG}_S1_${j}_${j}_${k}_${l}_${m}_1${i}.mac*
						sed -i -- "s/\[GXeAbsL\]/${l} cm/g" *mac_new/${MCTAG}_S1_${j}_${j}_${k}_${l}_${m}_1${i}.mac*
						sed -i -- "s/\[LXeRSL\]/${m} cm/g" *mac_new/${MCTAG}_S1_${j}_${j}_${k}_${l}_${m}_1${i}.mac*
						((FILEGENERATED++))
					fi
				done
			done
		done
	done
done

echo "$FILEGENERATED files generated."

