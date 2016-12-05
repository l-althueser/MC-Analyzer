#!/bin/bash
# Generate a ULITE job for every mac file in mac_new
# /archive_lngs100TB/mc/xe1tsim/althueser/rev338_matching/

for file in ./mac_new/*.mac
do
	if [ -f "./rootfiles/$(basename "$file" .mac)_01.root" ];
	then
		mv $file ./mac_processed/
		echo "$file moved!"
	else
		echo "$file"
		cp -f ./run_mac_template.sh ./run_mac_tmp.sh
		sed -i -- "s/\[MACRO\]/\/archive\_lngs100TB\/mc\/xe1tsim\/althueser\/rev338\_matching\/mac\_new\/$(basename "$file" .mac).mac/g" *run_mac_tmp.sh*
		sed -i -- "s/\[OUTPUT\]/\/archive\_lngs100TB\/mc\/xe1tsim\/althueser\/rev338\_matching\/rootfiles\/$(basename "$file" .mac)\_01.root/g" *run_mac_tmp.sh*
		sed -i -- "s/\[EVENTS\]/1000000/g" *run_mac_tmp.sh*
 
		qsub run_mac_tmp.sh
		rm ./run_mac_tmp.sh
 	fi 
done
