#!/bin/bash
echo hello
HOME=/home/qftsm/Work/GeantDevExamples
SRC=$HOME/milliq/geant4/src
BUILD=$HOME/BuildExamples/MilliQBuild
ROOT=$HOME/milliq/RootAnalysis
DATA=$HOME/../../Dropbox/MilliQ/PropagatingData
RESULTS=$HOME/rootAnalysis/DataResults

masses=(0.105) #1.0 10.0 100.0)
charges=(0.01) # 0.01 0.1 1.0)
process=(DY)
for mass in ${masses[*]}
do
for charge in ${charges[*]}
do
for proc in ${process[*]}
do
	echo Mass:$j GeV Charge:$i
	name="$proc"."$mass"GeV."$charge"Q
	nEv=$(cat $DATA/"$name".txt | wc -l)
        sed -i '/  fElCharge = /c\  fElCharge = '"$charge"';' $SRC/MilliQMonopolePhysics.cc
	sed -i '/  fMonopoleMass = /c\  fMonopoleMass = '"$mass"'*GeV;' $SRC/MilliQMonopolePhysics.cc
	sed -i '/    std::string filename=/c\    std::string filename="'"$name"'.txt";' $SRC/MilliQPrimaryGeneratorAction.cc
#	sed -i '/\/run\/beamOn /c\/run\/beamOn '"$nEv"'' $ROOT/run.exclusions.mac	
	sed -i '/\/run\/beamOn /c\/run\/beamOn 100' $ROOT/run.exclusions.mac
	cd $BUILD
	make -w -s MilliQ
	./MilliQ $ROOT/run.exclusions.mac
	cd $ROOT
	root -b -q $ROOT/histogram.C
	mv $ROOT/mcpall.dat $RESULTS/mcpall."$name".dat 
	mv $ROOT/sedep.dat $RESULTS/sedep."$name".dat
done
done
done
