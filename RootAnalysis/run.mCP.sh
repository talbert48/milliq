#!/bin/bash
echo hello

function contains() {
    local n=$#
    local value=${!n}
    for ((i=1;i < $#;i++)) {
        if [ "${!i}" = "${value}" ]; then
            echo "y"
            return 0
        fi
    }
    echo "n"
    return 1
}

#Ran JPSI, Y1S

#proc="test"
proc="mCP_UFO"
#proc="JPsi"
#proc="Y1S"
#proc="Y2S"
#proc="Y3S"

if [ "$proc" = "mCP_UFO" ]; then
#	masses=(0.1 0.28 0.43 0.6 0.78 1.0 1.25 1.52 1.84 2.2 2.6 3.04 3.54 4.1 4.71 5.4 6.15 6.98 7.9 8.9 10.0 11.2 12.5 14.0 15.5 17.2 19.1 21.1 23.3 25.6 28.2 30.9 33.9 37.1 40.5 44.2 48.2 52.5 57.1 62.1 67.4 73.0 79.1 85.6 92.6 99.9)
	masses=(99.9)
elif [ "$proc" = "JPsi" ]; then
	masses=(0.1 0.28 0.43 0.6 0.78 1.0 1.25)
elif [ "$proc" = "Y1S" ] || [ "$proc" = "Y2S" ] || [ "$proc" = "Y3S" ]; then
	masses=(0.1 0.28 0.43 0.6 0.78 1.0 1.25 1.52 1.84 2.2 2.6 3.04 3.54 4.1)
#	masses=(0.1 1.0)
elif [ "$proc" = "test" ]; then
	masses=(0.1)
fi




#risetime=(0.00 0.08 0.16 0.24 0.32 0.40 0.48 0.56 0.64 0.72 0.80 0.88 0.96 1.04 1.12 1.20 1.28 1.36 1.44 1.52 1.60 1.68 1.76 1.84 1.92 2.00)

masssmall=(0.1 0.28 0.43 0.6 0.78)
massmedium=(1.0 1.25 1.52 1.84 2.2 2.6)
masslarge=(3.04 3.54 4.1 4.71 5.4 6.15 6.98 7.9 8.9 10.0 11.2 12.5 14.0 15.5 17.2 19.1)
massverylarge=(21.1 23.3 25.6 28.2 30.9 33.9 37.1 40.5 44.2 48.2 52.5 57.1 62.1 67.4 73.0 79.1 85.6 92.6 99.9)

for mass in ${masses[*]}
do

	if [ $(contains "${masssmall[@]}" "$mass") == "y" ]; then
		charges=(0.0010)
	fi
	if [ $(contains "${massmedium[@]}" "$mass") == "y" ]; then
		charges=(0.0010)
	fi
	if [ $(contains "${masslarge[@]}" "$mass") == "y" ]; then
		charges=(0.0010)
	fi
	if [ $(contains "${massverylarge[@]}" "$mass") == "y" ]; then
		charges=(0.0014)
	fi
	if [ "$proc" = "test" ]; then
		charges=(0.0040)
	fi
	#Full Eff
#	charges=(0.0010 0.0015 0.0022 0.0034 0.0051 0.0076 0.0114 0.0171 0.0256 0.0384 0.0575 0.1000)
	#Block Eff
#	charges=(0.0010 0.0020 0.0030 0.0040 0.0050 0.0060 0.0070 0.0080 0.0090 0.0100 0.0120 0.0140)

	#Large Charge
	charges=(0.0010 0.0050 0.1000 0.2000 0.3000 0.4000 0.5000 0.6000 0.7000 0.8000 0.9000 0.9900)

	for charge in ${charges[*]}
	do
		cnew=$(echo $charge | sed 's/0.//')
                if [ "$cnew" -le "0030" ]; then
			nEv=25000
		elif [ "$cnew" -le "0050" ]; then
			nEv=10000
                elif [ "$cnew" -le "0090" ]; then
                        nEv=1000
		elif [ "$cnew" -le "0200" ]; then
	                nEv=400
		elif [ "$cnew" -le "0500" ]; then
	                nEv=100
		elif [ "$cnew" -le "2000" ]; then
	                nEv=50
		elif [ "$cnew" -gt "2000" ]; then
			nEv=20
		fi
	        if [ "$proc" = "test" ]; then
                	proc="mCP_UFO"
                	nEv=200;
        	fi
	
	
		echo Now submitting charge $charge mass $mass
	        sbatch -p cerberus --ntasks=1 ./submit.mcp.exclusions.sh $charge $mass $nEv $proc
	done
done
