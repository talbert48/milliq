import os
import PMT_Analyze
import numpy as np
import matplotlib.pyplot as plt

#set items for binning and plotting
x = np.arange(0,5000,10)
bincenters = 0.5*(x[1:]+x[:-1])

dir = ['D:\\PMT_Analysis\\Data\\May12\\1200V','D:\\PMT_Analysis\\Data\\May12\\1200VDark','D:\\PMT_Analysis\\Data\\May12\\1200VAgain'] # Set directory for data analysis
for i in range(len(dir)):
    sumTemp = np.zeros(1)
    for file in os.listdir(dir[i]): # Look at every file in directory
        if file.endswith(".dat"): # Only care about certain extension types
            filename = dir[i] + '\\' + file
            sumTemp = np.hstack((sumTemp,PMT_Analyze.AnalyzeFile(filename))) #analyze file and add it to the overall data array
    sumTemp = np.delete(sumTemp,0) # remove garbage element
    y, x = np.histogram(sumTemp, bins = x)
    plt.plot(bincenters, y)

plt.xlabel('mV $\mu$s')
plt.ylabel('Counts per 5 minute')
plt.title('Integration Histogram')
plt.legend(['1200 V w/ Scintillator, w/ Lights on','1200 V w/ Scintillator, w/ Lights off','1200 V w/ Scintillator, w/ Lights on run 2'])
plt.show()
