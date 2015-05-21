import os
import PMT_Analyze
import PMT_AnalyzeLM
import PMT_AnalyzeLMR
import numpy as np
import matplotlib.pyplot as plt

dir = ['D:\\PMT_Analysis\\Data\\May20\\run1','D:\\PMT_Analysis\\Data\\May20\\run3','D:\\PMT_Analysis\\Data\\May20\\run4'] # Set directory for analysis
legends = ['Background','Trigger on LED','Trigger on pulse, LED on']

def analyzeTraces():
    for i in range(len(dir)):
        fileCount = 0
        dataTemp = np.zeros((3,1))
        for file in os.listdir(dir[i]): # Look at every file in directory
            if file.endswith(".dat"): # Only care about certain extension types
                fileCount = fileCount + 1
                filename = dir[i] + '\\' + file
                dataTemp = np.hstack((dataTemp,PMT_Analyze.AnalyzeFile(filename , 0))) #analyze file and add it to the overall data array
                #second value determines if looking at full trace (0) or just first pulse (1)
        if fileCount > 0:
            goodData = np.array(dataTemp[0,:]) != 0 # we want to remove all the 0s from the data set
            dataTemp = dataTemp[:,goodData]
            filename = dir[i] + '\\' + 'SumFile.tSum'
            np.save(filename,dataTemp)

def analyzeListMode():
    for i in range(len(dir)):
        dataTemp = np.zeros((1,1))
        fileCount = 0
        for file in os.listdir(dir[i]): # Look at every file in directory
            if file.endswith(".lmdat"): # Only care about certain extension types
                fileCount = fileCount + 1
                filename = dir[i] + '\\' + file
                dataTemp = np.hstack((dataTemp,PMT_AnalyzeLM.AnalyzeFile(filename))) #analyze file and add it to the overall data array
                #second value determines if looking at full trace (0) or just first pulse (1)
            
        if fileCount > 0:
            goodData = np.array(dataTemp[0,:]) != 0 # we want to remove all the 0s from the data set
            dataTemp = dataTemp[:,goodData]
            print(dataTemp.shape)
            filename = dir[i] + '\\' + 'SumFile.lmSum'
            np.save(filename,dataTemp)
            
def analyzeListModeRaw():
    for i in range(len(dir)):
        fileCount = 0
        dataTemp = np.zeros((1,1))
        for file in os.listdir(dir[i]): # Look at every file in directory
            if file.endswith(".rlmdat"): # Only care about certain extension types
                fileCount = fileCount + 1
                filename = dir[i] + '\\' + file
                dataTemp = np.hstack((dataTemp,PMT_AnalyzeLMR.AnalyzeFile(filename))) #analyze file and add it to the overall data array
                #second value determines if looking at full trace (0) or just first pulse (1)
        if fileCount > 0:    
            goodData = np.array(dataTemp[0,:]) != 0 # we want to remove all the 0s from the data set
            dataTemp = dataTemp[:,goodData]
            print(dataTemp.shape)
            filename = dir[i] + '\\' + 'SumFile.lmSum'
            np.save(filename,dataTemp)
            
def plotTraces():
    for i in range(len(dir)):
        for file in os.listdir(dir[i]): # Look at every file in directory
            if file.endswith(".dat"): # Only care about certain extension types
                filename = dir[i] + '\\' + file
                test = PMT_Analyze.plotEvent(filename)
            if test:
                return 0
                
def analyzeSummary():
    #set items for binning and plotting integration
    x = np.arange(0,50000,5)
    bincenters = 0.5*(x[1:]+x[:-1]) * 12.8 / 1024 # convert from ADC to micro seconds
    
    #set items for binning and plotting maximums
    x2 = np.arange(0,1100,1)
    bincentersMaxs = 0.5*(x2[1:]+x2[:-1])
    
    colors = ['blue', 'green', 'magenta', 'cyan'] # for plotting purposes
    
    for i in range(len(dir)):
        dataTemp = np.zeros((3,1))
        for file in os.listdir(dir[i]): # Look at every file in directory
            if file.endswith(".tSum.npy"): # Only care about certain extension types
                filename = dir[i] + '\\' + file
                dataTemp = np.load(filename)
                
                plt.figure('Sums')
                y, x = np.histogram(dataTemp[0,:], bins = x)
                plt.plot(bincenters, y)
                plt.figure('Maxs')
                y, x2 = np.histogram(dataTemp[1,:], bins = x2)
                plt.plot(bincentersMaxs, y)
                plt.figure('Scatter')
                plt.scatter(dataTemp[0,:],dataTemp[1,:],c=colors[i])
            if file.endswith(".lmSum.npy"):
                filename = dir[i] + '\\' + file
                dataTemp = np.load(filename)
                
                plt.figure('Sums')
                y, x = np.histogram(dataTemp[0,:], bins = x)
                plt.plot(bincenters, y)
                

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
    
#Beginning of Main function
def main():
    choice = ' '
    while choice != '0':
        print('\nWhat would you like to do:')
        print('  0: Exit')
        print('  1: Analyze Traces')
        print('  2: Analyze Listmode')
        print('  3: Plot Traces')
        print('  4: Analyze Summary Files')
        print('  5: Analyze ListmodeRaw')
        choice=input('Select item> ')
        if choice == '1':
            analyzeTraces()
            break
        elif choice == '2':
            analyzeListMode()
            break
        elif choice == '3':
            plotTraces()
            break
        elif choice == '4':
            analyzeSummary()
            break
        elif choice == '5':
            analyzeListModeRaw()
            break
        else:
            choice == ' '
main()