#!/bin/bash
echo hello
HOME=/xfs1/gmagill/GeantDevExamples
SRC=/geant4/src
BUILD=$HOME/BuildExamples/MilliQBuild
ROOT=$HOME/milliq/RootAnalysis
DATA=/xfs1/gmagill/Repository_MilliCharged/Geant4/SourceFiles/
RESULTS=/xfs1/gmagill/Repository_MilliCharged/Geant4/MCPRepo
rseed=$SLURM_JOB_ID
JOB=$BUILD/Job."$rseed"

masses=(0.105) #1.0 10.0 100.0)
sourcecharge=0.01
charge=$1
process=(DY)

mkdir $JOB

for mass in ${masses[*]}
do
#for charge in ${charges[*]}
#do
for proc in ${process[*]}
do
	echo Mass:$j GeV Charge:$i
	outputname="$proc"."$mass"GeV."$charge"Q
	sourcename="$proc"."$mass"GeV."$sourcecharge"Q
	nEv=$(cat $DATA/"$sourcename".txt | wc -l)
#	nEv=500
	cp $ROOT/mcp.mac $JOB
	cp $ROOT/histogram.C $JOB
	cp -r $HOME/milliq/geant4 $JOB
	cd $JOB
	sed -i '/  fElCharge = /c\  fElCharge = '"$charge"';' $JOB/$SRC/MilliQMonopolePhysics.cc
        sed -i '/  fMonopoleMass = /c\  fMonopoleMass = '"$mass"'*GeV;' $JOB/$SRC/MilliQMonopolePhysics.cc
        sed -i '/    std::string filename=/c\    std::string filename="'"$sourcename"'.txt";' $JOB/$SRC/MilliQPrimaryGeneratorAction.cc
        sed -i '/    std::string pathname=/c\    std::string pathname="'"$DATA"'";' $JOB/$SRC/MilliQPrimaryGeneratorAction.cc
        sed -i '/\/run\/beamOn /c\/run\/beamOn '"$nEv"'' mcp.mac

	cmake -DGeant4_DIR=/xfs1/gmagill/geant4.10.02-build/lib/Geant4-10.2.0/ $JOB/geant4/
	make MilliQ
	./MilliQ mcp.mac
	root -b -q histogram.C
	mv mcpall.dat $RESULTS/mcpall."$outputname".dat 
	mv sedep.dat $RESULTS/sedep."$outputname".dat
	cd ../
	rm -r $JOB
done
#done
done
