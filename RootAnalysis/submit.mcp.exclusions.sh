#!/bin/bash
echo hello
HOME=/xfs1/gmagill/GeantDevExamples
SRC=$HOME/milliq/geant4/src
BUILD=$HOME/BuildExamples/MilliQBuild
ROOT=$HOME/milliq/RootAnalysis
DATA=/xfs1/gmagill/Repository_MilliCharged/Geant4/SourceFiles/
RESULTS=/xfs1/gmagill/Repository_MilliCharged/Geant4/MCPRepo

masses=(0.105) #1.0 10.0 100.0)
charges=(0.01)
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
	sed -i '/    std::string pathname=/c\    std::string pathname="'"$DATA"'";' $SRC/MilliQPrimaryGeneratorAction.cc
	sed -i '/\/run\/beamOn /c\/run\/beamOn '"$nEv"'' $ROOT/mcp.mac	
#	sed -i '/\/run\/beamOn /c\/run\/beamOn 300' $ROOT/mcp.mac
	cd $BUILD
	make MilliQ
	./MilliQ $ROOT/mcp.mac
	cd $ROOT
	root -b -q $ROOT/histogram.C
	mv $ROOT/mcpall.dat $RESULTS/mcpall."$name".dat 
	mv $ROOT/sedep.dat $RESULTS/sedep."$name".dat
done
done
done
