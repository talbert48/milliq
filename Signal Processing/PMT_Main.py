import os
import PMT_Analyze
import PMT_AnalyzeLM
import PMT_AnalyzeLMR
import PMT_AnalyzeMCA
import numpy as np
import matplotlib.pyplot as plt

fg=27000
ecomp=1
dir = ['D:\\PMT_Analysis\\Data\\June12\\run4','D:\\PMT_Analysis\\Data\\June12\\mca4']
# dir=[]
# for i in range(1,4):
#     dir.append('D:\\PMT_Analysis\\Data\\June5\\mca' + str(i))

# legends = ['Background','3 mV Trigger','Trigger on LED','Bkg Subtracted']
legends = ['Traces','MCA']

def analyzeTraces():
    for i in range(len(dir)):
        fileCount = 0
        dataTemp = np.zeros((3,1))
        for file in os.listdir(dir[i]): # Look at every file in directory
           if file.endswith("0_trace.dat"): # Only care about certain extension types
                fileCount = fileCount + 1
                filename = dir[i] + '\\' + file
                dataTemp = np.hstack((dataTemp,PMT_Analyze.AnalyzeFile(filename , 1))) #analyze file and add it to the overall data array
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
        if fileCount > 0:    
            goodData = np.array(dataTemp[0,:]) != 0 # we want to remove all the 0s from the data set
            dataTemp = dataTemp[:,goodData]
            print(dataTemp.shape)
            filename = dir[i] + '\\' + 'SumFile.lmSum'
            np.save(filename,dataTemp)

def analyzeMCA():
    for i in range(len(dir)):
        fileCount = 0
        for file in os.listdir(dir[i]): # Look at every file in directory
            if file.endswith(".hgdat"): # Only care about certain extension types
                fileCount = fileCount + 1
                filename = dir[i] + '\\' + file
                dataTemp = np.array(PMT_AnalyzeMCA.AnalyzeFile(filename))
        if fileCount > 0:    
            filename = dir[i] + '\\' + 'SumFile.MCASum'
            filename2 = dir[i] + '\\' + 'SumFile.csv'
            np.save(filename,dataTemp)
            np.savetxt(filename2, dataTemp.astype(int), delimiter=",")
            
def plotTraces():
    for i in range(len(dir)):
        for file in os.listdir(dir[i]): # Look at every file in directory
            if file.endswith(".dat"): # Only care about certain extension types
                filename = dir[i] + '\\' + file
                test = PMT_Analyze.plotEvent(filename)
            if test:
                return 0
                
def analyzeSummary():
    plt.close()
    #set items for binning and plotting integration
    xi = np.arange(-1,4097,1)
    bincenters = 0.5*(xi[1:]+xi[:-1]) #*1.275#* 102.103960396/160#* 12.8 / 1024 # convert from ADC to micro seconds
    
    #set items for binning and plotting mca
    x1 = np.arange(0,4096,1)
    mcaCenters = 0.5*(x1[1:]+x1[:-1])
    mcaCenters = mcaCenters# *0.98#* 102.103960396/160 # convert to nA
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
                y, xt = np.histogram(dataTemp[2,:] , bins = bincenters)
                x = xi[1:-1]
                plt.plot(x, y/np.amax(y))
                print(y.sum())
                # plt.figure('Maxs')
                # y, x2 = np.histogram(dataTemp[1,:], bins = x2)
                # plt.plot(bincentersMaxs, y)
                # plt.figure('Scatter')
                # plt.scatter(dataTemp[0,:],dataTemp[1,:],c=colors[i])
            if file.endswith(".lmSum.npy"):
                filename = dir[i] + '\\' + file
                dataTemp = np.load(filename)
                dataTemp = dataTemp
                
                plt.figure('Sums')
                y, x = np.histogram(dataTemp, bins = x1)
                plt.plot(mcaCenters, y)
            if file.endswith(".MCASum.npy"):
                filename = dir[i] + '\\' + file
                dataTemp = np.load(filename)
                dataTemp = dataTemp.astype(int)
                print(dataTemp.sum())
                plt.figure('Sums')
                plt.plot(x1, dataTemp/np.amax(dataTemp))
                
    # fullData = fullData - bkg
    # plt.figure('Sums')
    # plt.plot(mcaCenters,fullData)
    #plot figures
    plt.figure('Sums')
    plt.xlabel('nA')
    plt.ylabel('Counts per 2 minutes')
    plt.title('Dark Current Histogram')
    plt.xlim((0,500))
    plt.legend(legends)
    plt.show()

    
    # plt.figure('Maxs')
    # plt.xlabel('mV')
    # plt.ylabel('Counts per 5 minutes')
    # plt.title('Pulse Maximum Histogram')
    # plt.legend(legends)
    # plt.show()
    # 
    # plt.figure('Scatter')
    # plt.legend(legends)
    # plt.xlabel('Pulse Area')
    # #plt.ylabel('Pulse Tail Area/ Total Area')
    # plt.ylabel('Pulse Maximum (mV)')
    # plt.show()
    
def plotMCASpectrums():
    x1 = np.arange(0,4097,1)
    mcaCenters = 0.5*(x1[1:]+x1[:-1]) #* 12.8 *16/ 1024# convert from ADC to micro seconds
    mcaCenters = mcaCenters * 102.103960396/160 # convert to nA
    plt.figure('Spectrums')
    plt.clf()
    
    for i in range(len(dir)):
        for file in os.listdir(dir[i]): # Look at every file in directory
            if file.endswith(".MCASum.npy"):
                filename = dir[i] + '\\' + file
                dataTemp = np.load(filename)
                dataTemp = dataTemp.astype(int)
                print(dataTemp.sum())
                
                plt.plot(mcaCenters, dataTemp)
    
    #plot figures
    plt.ylim((0,np.amax(dataTemp)))
    plt.xlim((0,50))
    plt.xlabel('Average Current (nA)')
    plt.ylabel('Counts per 1 minute')
    plt.title('Average Current Spectrums')
    plt.show()

def plotBkgSubtraction():
    x1 = np.arange(0,4097,1)
    mcaCenters = 0.5*(x1[1:]+x1[:-1]) #* 12.8 *16/ 1024# convert from ADC to micro seconds
    mcaCenters = mcaCenters * 102.103960396/160 # convert to nA
    plt.figure('BkgSubtraction')
    plt.clf()
    
    for i in range(len(dir)):
        for file in os.listdir(dir[i]): # Look at every file in directory
            if file.endswith(".MCASum.npy"):
                filename = dir[i] + '\\' + file
                dataTemp = np.load(filename)
                dataTemp = dataTemp.astype(int)
                print(dataTemp.sum())
                if i == 0: # index of background
                    bkg = dataTemp
                if i == 1: # index of spectrum to have background removed from
                    fullData = dataTemp
                plt.plot(mcaCenters, dataTemp)

    fullData = fullData - bkg
    plt.plot(mcaCenters,fullData)
    
    #plot figures
    plt.xlabel('Average Current (nA)')
    plt.ylabel('Counts per 1 minute')
    plt.title('Average Current Spectrums')
    plt.legend(legends)
    plt.ylim((0,np.amax(dataTemp)))
    plt.xlim((0,50))
    plt.show()
    
def plotMCARates():
    colors = ['blue', 'green', 'magenta', 'cyan'] # for plotting purposes
    x1 = np.arange(0,4097,1)
    mcaCenters = 0.5*(x1[1:]+x1[:-1]) #* 12.8 *16/ 1024# convert from ADC to micro seconds
    mcaCenters = mcaCenters * 102.103960396/160 # convert to nA
    plt.figure('Rates')
    plt.clf()
    rates = np.zeros(len(dir))
    currents = np.zeros(len(dir))
    
    for i in range(len(dir)):
        for file in os.listdir(dir[i]): # Look at every file in directory
            if file.endswith(".MCASum.npy"):
                filename = dir[i] + '\\' + file
                dataTemp = np.load(filename)
                dataTemp = dataTemp.astype(int)
                print(dataTemp.sum())
                # currents[i] = mcaCenters[np.argmax(dataTemp[0:100])]
                plt.scatter((dataTemp[11:100] * mcaCenters[11:100]).sum()/dataTemp[11:100].sum(),dataTemp.sum() / (60*45),c=colors[i])
                # currents[i] = (dataTemp[5:100] * mcaCenters[5:100]).sum()/dataTemp[5:100].sum()
                # rates[i] = dataTemp.sum() / (60*45)
    
    #plot figures
    
    plt.xlabel('Average Current (nA)')
    plt.ylabel('Rate (Hz)')
    plt.title('Rate Vs. Average Current')
    plt.legend(legends)
    plt.show()
    
def plotMenu():
    choice = ' ' 
    while choice != '0':
        print('\nWhat would you like to do:')
        print('  0: Exit')
        print('  1: Plot Spectrums')
        print('  2: Background Subtraction')
        print('  3: Plot Rates')
        choice=input('Select item> ')
        if choice == '1':
            plotMCASpectrums()
            break
        elif choice == '2':
            plotBkgSubtraction()
            break
        elif choice == '3':
            plotMCARates()
            break
        else:
            choice == ' '
            
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
        print('  6: Analyze MCA')
        print('  7: MCA plotting options')
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
        elif choice == '6':
            analyzeMCA()
            break
        elif choice == '7':
            plotMenu()
            break
        else:
            choice == ' '
main()