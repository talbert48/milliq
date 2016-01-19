#!/bin/bash
echo hello
HOME=/xfs1/gmagill/GeantDevExamples
SRC=/geant4/src
BUILD=$HOME/BuildExamples/MilliQBuild
ROOT=$HOME/milliq/RootAnalysis
DATA=/xfs1/gmagill/Repository_MilliCharged/Geant4/SourceFiles/
rseed=$SLURM_JOB_ID
JOB=$BUILD/Job."$rseed"

charge=$1
mass=$2
nEv=$3
process=$4
RESULTS=/xfs1/gmagill/Repository_MilliCharged/Geant4/MCPRepo/FullRun

cnew="$(echo $charge | sed 's/0.//')"
if [ "$cnew" -lt "0100" ]; then
        sourcecharge=0.001
elif [ "$cnew" -lt "1000" ]; then
        sourcecharge=0.01
elif [ "$cnew" -ge "1000" ]; then
        sourcecharge=0.1
fi

echo "$charge" "$mass" "$nEv" "$process" "$sourcecharge"

mkdir $JOB

for proc in ${process[*]}
do
	echo Mass:$j GeV Charge:$i
	outputname="$proc"."$mass"GeV."$charge"Q
	sourcename="$proc"/"$mass"/"$sourcecharge"/hit_4_vecs.txt
	cp $ROOT/mcp.mac $JOB
	cp $ROOT/histogram.C $JOB
	cp -r $HOME/milliq/geant4 $JOB
	cd $JOB
	sed -i '/  fElCharge = /c\  fElCharge = '"$charge"';' $JOB/$SRC/MilliQMonopolePhysics.cc
        sed -i '/  fMonopoleMass = /c\  fMonopoleMass = '"$mass"'*GeV;' $JOB/$SRC/MilliQMonopolePhysics.cc
        sed -i '/    std::string filename=/c\    std::string filename="'"$sourcename"'";' $JOB/$SRC/MilliQPrimaryGeneratorAction.cc
        sed -i '/    std::string pathname=/c\    std::string pathname="'"$DATA"'";' $JOB/$SRC/MilliQPrimaryGeneratorAction.cc
        sed -i '/\/run\/beamOn /c\/run\/beamOn '"$nEv"'' mcp.mac

#	sed -i '/        fScintillator_mt->AddConstProperty("FASTSCINTILLATIONRISE/c\        fScintillator_mt->AddConstProperty("FASTSCINTILLATIONRISETIME", '"$rise"' * ns);' $JOB/$SRC/MilliQDetectorConstruction.cc

	cmake -DGeant4_DIR=/xfs1/gmagill/geant4.10.02-build/lib/Geant4-10.2.0/ $JOB/geant4/
	make MilliQ
	./MilliQ mcp.mac
	root -b -q histogram.C
#	cp mcpall.dat $RESULTS/mcpall."$outputname".dat 
#	mv sedep.dat $RESULTS/sedep."$outputname".dat
#	mv pmt0time.dat $RESULTS/pmt0time."$outputname".dat
#	mv pmt1time.dat $RESULTS/pmt1time."$outputname".dat
#	mv pmt2time.dat $RESULTS/pmt2time."$outputname".dat
	nPass=$(cat mcpall.dat | wc -l)
	echo $mass $charge $nEv $nPass >> $RESULTS/Acceptances."$proc".SingleLayer.15ns.OffSet.Shield.RegGun.Refl0.98.NoLG.10b10.PMTRad5.LayerSpacing.ScintThick.dat
	cd ../
	rm -r $JOB
	rm $ROOT/slurm-"$rseed".out
done
