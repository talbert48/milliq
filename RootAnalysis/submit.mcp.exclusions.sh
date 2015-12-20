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
sourcemass=0.105
charge=0.01
process=(DY)
rise=$1

mkdir $JOB

for mass in ${masses[*]}
do
#for charge in ${charges[*]}
#do
for proc in ${process[*]}
do
	echo Mass:$j GeV Charge:$i
	outputname="$proc"."$mass"GeV."$charge"Q."$rise"ns
	sourcename="$proc"."$sourcemass"GeV."$sourcecharge"Q
#	nEv=$(cat $DATA/"$sourcename".txt | wc -l)
	nEv=300
	cp $ROOT/mcp.mac $JOB
	cp $ROOT/histogram.C $JOB
	cp -r $HOME/milliq/geant4 $JOB
	cd $JOB
	sed -i '/  fElCharge = /c\  fElCharge = '"$charge"';' $JOB/$SRC/MilliQMonopolePhysics.cc
        sed -i '/  fMonopoleMass = /c\  fMonopoleMass = '"$mass"'*GeV;' $JOB/$SRC/MilliQMonopolePhysics.cc
        sed -i '/    std::string filename=/c\    std::string filename="'"$sourcename"'.txt";' $JOB/$SRC/MilliQPrimaryGeneratorAction.cc
        sed -i '/    std::string pathname=/c\    std::string pathname="'"$DATA"'";' $JOB/$SRC/MilliQPrimaryGeneratorAction.cc
        sed -i '/\/run\/beamOn /c\/run\/beamOn '"$nEv"'' mcp.mac

	sed -i '/        fScintillator_mt->AddConstProperty("FASTSCINTILLATIONRISE/c\        fScintillator_mt->AddConstProperty("FASTSCINTILLATIONRISETIME", '"$rise"' * ns);' $JOB/$SRC/MilliQDetectorConstruction.cc

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
