import numpy as np
import matplotlib.pyplot as plt

NPoints = 4096 # size of data

def AnalyzeFile(filename):
    print('Analyzing ' + filename)
    outData = ReadMCAData(filename)
    return outData
        
def ReadMCAData(filename):
    f = open(filename)
    temp = f.readline().split(" ")
    data = temp[79:]
    f.close()
    return data