import numpy as np
import matplotlib.pyplot as plt

NPoints = 1000000 # initial guess to size of data
bufferLength = 340

def AnalyzeFile(filename):
    print('Analyzing ' + filename)
    outData = ReadLMRData(filename)
    return outData
        
def ReadLMRData(filename):
    data = np.zeros((1,NPoints),dtype=float)
    f = open(filename)
    ind = 0
    while True:
        temp = f.readline().split(" ")
        if temp == ['']: #if the end of file is reached, break
            break
        for j in range(bufferLength):
            data[0,ind] = float(temp[3*j + 6]) # only want every other value from file
            ind = ind + 1
            if ind > NPoints-1:
                data = np.hstack((data,0))
    f.close()
    return data