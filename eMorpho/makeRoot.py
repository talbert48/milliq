from ROOT import TNtuple, TFile, TH1F
import os

dir = 'D:\\PMT_Analysis\\Data\\CERNdata\\CMS\\July8\\AM241'
timestamp = 1436372044662
r = TFile(dir + '\\' + (str)(timestamp) + ".root", "recreate")
    
def main():
	for file in os.listdir(dir): # Look at every file in directory
		if file.endswith((str)(timestamp) + ".dat"): # Only care about files with correct time stamp
			sn = file[0:7]
			filename = dir + '\\' + file 
			print('Analyzing ' + filename)
			makeRoot(filename,sn)
    
	r.Close()
    
def makeRoot(filename,sn):
    bufferLength = 340
    f = open(filename)
    he = TH1F("he;" + sn, sn + " energy distribution", 4096, 0, 4096)
    htcor = TH1F("htcor;" + sn, sn +  " time distribution",   10000, 0, 96000000000)
    ntuple = TNtuple("ntuple;" + sn, sn + " eMorpho data","energy:time:rollover:correctedTime")
    
    rollover = 0
    oldTime = 0
    while True:
        temp = f.readline().split(" ")
        if temp == ['']: #if the end of file is reached, break
            break
        for j in range(bufferLength):
            energy = float(temp[2*j + 6])/16.0 # get energy
            time = float(temp[2*j + 7]) # get time stamp
            if time < oldTime:
                rollover = rollover + 1
            oldTime = time
            timeCorrection = rollover * 2**32
            he.Fill(energy)
            htcor.Fill(time + timeCorrection)
            ntuple.Fill(energy, time, rollover,time + timeCorrection)
            
    f.close()
    ntuple.Write()
    he.Write()
    htcor.Write()

main()