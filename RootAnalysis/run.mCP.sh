#!/bin/bash
echo hello

proc="test"
#proc="mCP_UFO"
#proc="JPsi"
#proc="Y1S"
#proc="Y2S"
#proc="Y3S"

if [ "$proc" = "mCP_UFO" ]; then
	masses=(0.1 0.43 0.78 10.0 11.2 12.5 1.52 17.2 19.1 2.2 25.6 28.2 30.9 3.54 40.5 44.2 48.2 5.4 6.15 67.4 73.0 79.1 8.9 0.28 0.6 1.0 99.9 1.25 14.0 15.5 1.84 21.1 23.3 2.6 3.04 33.9 37.1 4.1 4.71 52.5 57.1 62.1 6.98 7.9 85.6 92.6)
elif [ "$proc" = "JPsi" ]; then
	masses=(0.1 0.28 0.43 0.6 0.78 1.0 1.25)
elif [ "$proc" = "Y1S" ] || [ "$proc" = "Y2S" ] || [ "$proc" = "Y3S" ]; then
	masses=(0.1 0.28 0.43 0.6 0.78 1.0 1.25 1.52 1.84 2.2 2.6 3.04 3.54 4.1)
elif [ "$proc" = "test" ]; then
	masses=(0.1)
fi




#risetime=(0.00 0.08 0.16 0.24 0.32 0.40 0.48 0.56 0.64 0.72 0.80 0.88 0.96 1.04 1.12 1.20 1.28 1.36 1.44 1.52 1.60 1.68 1.76 1.84 1.92 2.00)

for mass in ${masses[*]}
do

	if [ "$mass" = "0.1" ]; then
		charges=(0.0010 0.0011 0.0012 0.0013 0.0014 0.0015 0.0016 0.0017 0.0018 0.0020 0.0021 0.0023 0.0025 0.0027 0.0029 0.0031 0.0034 0.0036 0.0039 0.0042)
	fi
	if [ "$mass" = "1.0" ]; then
		charges=(0.0010 0.0011 0.0013 0.0014 0.0016 0.0018 0.0021 0.0023 0.0026 0.0029 0.0033 0.0037 0.0042 0.0047 0.0053 0.0060 0.0068 0.0077 0.0086 0.0097)
	fi
	if [ "$mass" = "3.04" ]; then
		charges=(0.0010 0.0012 0.0015 0.0018 0.0022 0.0026 0.0032 0.0039 0.0047 0.0057 0.0069 0.0084 0.0102 0.0124 0.0150 0.0182 0.0221 0.0268 0.0325 0.0394)
	fi
	if [ "$mass" = "21.1" ]; then
		charges=(0.0014 0.0020 0.0028 0.0039 0.0056 0.0079 0.0111 0.0157 0.0222 0.0313 0.0443 0.0625 0.0883 0.1250 0.1760 0.2490 0.3520 0.4970 0.7020 0.9910)
	fi
	if [ "$proc" = "test" ]; then
		proc="mCP_UFO"
		charges=(0.004)
	fi


	for charge in ${charges[*]}
	do
		cnew=$(echo $charge | sed 's/0.//')
		if [ "$cnew" -le "0060" ]; then
			nEv=10000
		elif [ "$cnew" -le "0200" ]; then
	                nEv=400
		elif [ "$cnew" -le "0500" ]; then
	                nEv=100
		elif [ "$cnew" -gt "0500" ]; then
	                nEv=50
		fi
	
		echo Now submitting charge $charge mass $mass
	        sbatch -p cerberus --ntasks=1 ./submit.mcp.exclusions.sh $charge $mass $nEv $proc
	done
done
