#!/bin/bash
echo hello
#proc="MilliQ.UFO.14TeV"

#masses=(0.10 0.28 0.43 0.60 0.78 1.00 1.25 1.52 1.84 2.20 2.60 3.04 3.54 4.10 4.71 5.40 6.15 6.98 7.90 8.90 10.00 11.20 12.50 14.00 15.50 17.20 19.10 21.10 23.30 25.60 28.20 30.90 33.90 37.10 40.50 44.20 48.20 52.50 57.10 62.10 67.40 73.00 79.10 85.60 92.60 100.00)
#masses=(0.13 67.40)
charges=(0.01 0.02) #(0.001 0.002 0.003 0.004 0.006 0.010 0.016 0.025 0.040 0.063 0.100 0.158 0.251 0.398 0.631 1.000)

for charge in ${charges[*]}
#for mass in ${masses[*]}
do
	echo Now submitting charge $charge
        sbatch -p cerberus --ntasks=1 ./submit.mcp.exclusions.sh $charge
	sleep 2m
done
