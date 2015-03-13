from __future__ import division
import zmq
import sys
import emorpho_io
import ftdi
import time

		
def dataAq1(num_traces): #Save a particular number of traces
	""" Boot eMorpho, acquire num_traces pulses, save data and exit """
	
	use_bpi_vid_only = 1 # only recognize devices with the BPI vendor id 
	settings_path = 'settings/'
	all_morpho = ftdi.ft245()
	
	ret_find = all_morpho.find_all(use_bpi_vid_only)
	if(ret_find >= 0):
		emorpho_io.boot_all(all_morpho, settings_path)
	
	print 'serial numbers = ',all_morpho.sn
	sn = all_morpho.sn[0] # use the first device only
	print "active device: ", sn
	
	startTime = time.time()
	filesize = 1000
	itr = 0
	testcase = num_traces/filesize
	while itr < testcase:
		with open('data/{0}_trace.dat'.format(itr),'w') as fout:			
			for n in range(filesize):
				# http://www.bridgeportinstruments.com/products/mds/mds_doc/start_trace.php
				cmd_data = [1, 100]
				emorpho_io.start_trace(all_morpho, sn, cmd_data)
				while True:
					# http://www.bridgeportinstruments.com/products/mds/mds_doc/read_cal.php
					status = emorpho_io.get_status(all_morpho, sn)
					trace_done = status[11] == 1
					if trace_done:
						# http://www.bridgeportinstruments.com/products/mds/mds_doc/read_trace.php
						trace = emorpho_io.read(all_morpho, sn, emorpho_io.MA_TRACE, 2*1024, 2)
						trace = [t/32 for t in trace]
						d_lst = [1, sn, len(trace)+1, 1] + trace
						data  = ' '.join(map(str,d_lst))
						fout.write(data+'\n')
						break
		itr = itr + 1
		print itr*filesize
		fout.close()
	endTime = time.time()
	print "run time = ", endTime - startTime
		
def dataAq2(runTime): #takes data for given time in minutes
	""" Boot eMorpho, acquire num_traces pulses, save data and exit """
	
	use_bpi_vid_only = 1 # only recognize devices with the BPI vendor id 
	settings_path = 'settings/'
	all_morpho = ftdi.ft245()
	
	ret_find = all_morpho.find_all(use_bpi_vid_only)
	if(ret_find >= 0):
		emorpho_io.boot_all(all_morpho, settings_path)
	
	print 'serial numbers = ',all_morpho.sn
	sn = all_morpho.sn[0] # use the first device only
	print "active device: ", sn
	
	runTime = runTime * 60 # convert runTime to seconds
	endTime = time.time() + runTime # determine endTime
	filesize = 1000 #events per .dat file
	ind = 0 #keep track of event number
	fout = open('data/{0}_trace.dat'.format(ind),'w') #open file to save data in
	while time.time() < endTime:
		ind = ind + 1
		# http://www.bridgeportinstruments.com/products/mds/mds_doc/start_trace.php
		cmd_data = [1, 100]
		emorpho_io.start_trace(all_morpho, sn, cmd_data)
		while True:
			# http://www.bridgeportinstruments.com/products/mds/mds_doc/read_cal.php
			status = emorpho_io.get_status(all_morpho, sn)
			trace_done = status[11] == 1
			if trace_done:
				# http://www.bridgeportinstruments.com/products/mds/mds_doc/read_trace.php
				trace = emorpho_io.read(all_morpho, sn, emorpho_io.MA_TRACE, 2*1024, 2)
				trace = [t/32 for t in trace]
				d_lst = [1, sn, len(trace)+1, 1] + trace
				data  = ' '.join(map(str,d_lst))
				fout.write(data+'\n')
				break
		if ind % filesize == 0:
			print ind
			fout.close()
			fout = open('data/{0}_trace.dat'.format(ind/filesize),'w')
	print "Data acquisition complete"