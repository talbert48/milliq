import numpy as np
import matplotlib.pyplot as plt

NTraces = 1000
NPoints = 1024

def AnalyzeFile(filename,type):
    print('Analyzing ' + filename)
    data = ReadData(filename)
    sums = 0
    maxs = 0
    Tail = 0
    outData = np.zeros((3,NTraces))
    for i in range(NTraces):
        data[i,:] = data[i,:] - np.amin(data[i,300:])
        data[i,:] = data[i,:] - Offset(data[i,300:])
        if type == 0:
           sums[i] = data[i,100:260].sum() * 16807 / (2**13)
        else:
            sums = data[i,100:260].sum()
            data[i,data[i,:] < 0] = 0
            Tail = data[i,100:260].sum()
        maxs = np.amax(data[i,:])
        outData[0,i] = sums
        outData[1,i] = maxs
        outData[2,i] = Tail
    return outData
        
    
def ReadData(filename):
    data = np.zeros((NTraces,NPoints),dtype=float)
    f = open(filename)
    for i in range(NTraces):
        temp = f.readline().split(" ")
        if temp == ['']:
            break      
        for j in range(NPoints):
            data[i,j] = (float(temp[j+4]) - 67)             
    f.close()
    return data
    
def Offset(data):
    hist, bin = np.histogram(data, bins = np.arange(-10,10))
    bin = np.delete(bin,-1)
    offset = (hist * bin).sum() / hist.sum()
    return offset

def plotEvent(filename):
    print('Plotting ' + filename)
    data = ReadData(filename)
    plt.ion()
    for i in range(NTraces):
        data[i,:] = data[i,:] - np.amin(data[i,300:])
        data[i,:] = data[i,:] - Offset(data[i,300:])
        print(data[i,100:260].sum())
        x = np.linspace(0.0,12.8, num = NPoints)
        plt.figure()
        plt.plot(x,data[i,:])
        plt.xlabel('$\mu$s')
        plt.ylabel('mV')
        plt.title('PMT Pulse')
        plt.draw()
        if input('Press 0 to exit or enter to continue... ') == '0':
            return True #exit
        plt.close()
    return False
    

    
    
    