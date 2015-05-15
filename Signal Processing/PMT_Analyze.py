import numpy as np
import matplotlib.pyplot as plt

NTraces = 1000
NPoints = 1024

def AnalyzeFile(filename,type):
    print('Analyzing ' + filename)
    data = ReadData(filename)
    sums = np.zeros(NTraces)
    maxs = np.zeros(NTraces)
    Tail = np.zeros(NTraces)
    outData = np.zeros((3,NTraces))
    for i in range(NTraces):
        #data[i,:] = data[i,:] - Offset(data[i,0:75])
        data[i,:] = data[i,:] - np.amin(data[i,200:])
        data[i,:] = data[i,:] - Offset(data[i,200:])
        if np.amax(data[i,:]) < 7: # zero out garbage events
            data[i,np.arange(0,NPoints)]=0
            continue
        if type == 0:
            sums[i] = data[i,:].sum()
        else:
            sums[i] = data[i,90:250].sum()
        # if sums[i] < 0:
        plotEvent(data[i,:])            
        maxs[i] = np.amax(data[i,:])
        Tail[i] = data[i,110:250].sum();
        outData[0,i] = sums[i]
        outData[1,i] = maxs[i]
        outData[2,i] = Tail[i]/sums[i]
        #outData[2,i] = np.amin(data[i,:])
    return outData
        
    
def ReadData(filename):
    data = np.zeros((NTraces,NPoints),dtype=float)
    f = open(filename)
    for i in range(NTraces):
        temp = f.readline().split(" ")
        if temp == ['']:
            break      
        for j in range(NPoints):
            data[i,j] = float(temp[j+4]) - 115
    f.close()
    return data
    
def Offset(data):
    hist, bin = np.histogram(data, bins = np.arange(-10,10))
    bin = np.delete(bin,-1)
    offset = (hist * bin).sum() / hist.sum()
    return offset

def plotEvent(data):
        x = np.linspace(0.0,12.8, num = NPoints)
        plt.plot(x,data)
        plt.xlabel('$\mu$s')
        plt.ylabel('mV')
        plt.title('PMT Pulse')
        plt.show()
        plt.waitforbuttonpress()
        plt.clf()