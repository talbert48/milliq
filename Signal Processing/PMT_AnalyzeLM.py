import numpy as np
import matplotlib.pyplot as plt

NPoints = 250000 # initial guess to size of data
bufferLength = 512

def AnalyzeFile(filename):
    print('Analyzing ' + filename)
    outData = ReadLMData(filename)
    return outData
        
def ReadLMData(filename):
    data = np.zeros((1,NPoints),dtype=float)
    f = open(filename)
    ind = 0
    while True:
        temp = f.readline().split(" ")
        if temp == ['']: #if the end of file is reached, break
            break
        for j in range(bufferLength):
            data[0,ind] = float(temp[2*j + 6]) + 290 # only want every other value from file
            ind = ind + 1
            if ind > NPoints:
                data = np.hstack((data,0))
    f.close()
    return data