import os
import PMT_Analyze
import numpy as np
import matplotlib.pyplot as plt

#set items for binning and plotting integration
x = np.arange(0,30000,5)
bincenters = 0.5*(x[1:]+x[:-1]) * 12.8 / 1024 # convert from ADC to micro seconds

#set items for binning and plotting maximums
x2 = np.arange(0,1100,1)
bincentersMaxs = 0.5*(x2[1:]+x2[:-1])

colors = ['blue', 'green', 'magenta', 'cyan']

dir = ['D:\\PMT_Analysis\\Data\\May14\\PulserHigh','D:\\PMT_Analysis\\Data\\May13\\LEDonTriggerOn'] # Set directory for data analysis
for i in range(len(dir)):
    dataTemp = np.zeros((3,1))
    for file in os.listdir(dir[i]): # Look at every file in directory
        if file.endswith(".dat"): # Only care about certain extension types
            filename = dir[i] + '\\' + file
            dataTemp = np.hstack((dataTemp,PMT_Analyze.AnalyzeFile(filename , 0))) #analyze file and add it to the overall data array
            
    dataTemp = dataTemp[:,1:] # remove garbage element
    goodData = np.array(dataTemp[0,:]) != 0
    plt.figure('Sums')
    y, x = np.histogram(dataTemp[0,goodData], bins = x)
    plt.plot(bincenters, y)
    plt.figure('Maxs')
    y, x2 = np.histogram(dataTemp[1,goodData], bins = x2)
    plt.plot(bincentersMaxs, y)
    plt.figure('Scatter')
    plt.scatter(dataTemp[0,:],dataTemp[1,:],c=colors[i])

legends = ['Background','LED On']
#plot figures
plt.figure('Sums')
plt.xlabel('mV $\mu$s')
plt.ylabel('Counts per 5 minutes')
plt.title('Pulse Area Histogram')
plt.legend(legends)
plt.show()

plt.figure('Maxs')
plt.xlabel('mV')
plt.ylabel('Counts per 5 minutes')
plt.title('Pulse Maximum Histogram')
plt.legend(legends)
plt.show()

plt.figure('Scatter')
plt.legend(legends)
plt.xlabel('Pulse Area')
#plt.ylabel('Pulse Tail Area/ Total Area')
plt.ylabel('Pulse Maximum (mV)')
plt.show()
