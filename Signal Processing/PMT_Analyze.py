import numpy as np
import matplotlib.pyplot as plt

NTraces = 1000
NPoints = 1024

def AnalyzeFile(filename):
    print('Analyzing ' + filename)
    data = ReadData(filename)
    sums = np.zeros(NTraces)
    for i in range(NTraces):
        data[i,:] = data[i,:] - Offset(data[i,0:75])             
        sums[i] = data[i,:].sum()  
        #plotEvent(data[i,:])
    return sums
        
    
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
    hist, bin = np.histogram(data, bins = np.arange(-10,11))
    bin = np.delete(bin,-1)
    offset = (hist * bin).sum() / hist.sum()
    return offset

def plotEvent(data):
        plt.plot(data)
        plt.show()
        plt.waitforbuttonpress()
        plt.clf()