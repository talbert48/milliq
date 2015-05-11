from __future__ import division
import zmq
import time
import sys
import xml.dom.minidom
import os

def parse_data(xml_cmd):

	"""Returns a list of attributes and the command data as the second list."""
	default_attributes = {'cmd' : 'hello', 'engage' : '1', 'sn' : 'eRC0000', 'fast' : '0'}
	doc = xml.dom.minidom.parseString(xml_cmd)
	
	if(doc.documentElement.tagName != "em_data"):
		return {}, []
	
	data_tag = doc.getElementsByTagName("em_data")[0]
	if(len(data_tag.childNodes) == 1):
		data_string = data_tag.childNodes[0].data
	else:
		data_string = ""
		
	cmd_attributes = dict(data_tag.attributes.items())
	
	# merge the two attribute dictionaries 
	attributes = default_attributes.copy()
	attributes.update(cmd_attributes)
	 
	return attributes, data_string 	
def multi_trace_daq(run_time, directory): #takes data for given time in minutes
	""" Connect to MDS, boot eMorpho, acquire num_traces pulses, save data and exit """
	IP_Address = "tcp://localhost:5507" # on the LAN: "tcp://192.168.6.1:5507"
	context = zmq.Context()
	socket = context.socket(zmq.REQ)
	socket.connect (IP_Address)
	
	# always send and receive; don't break up the pair.
	socket.send ('<em_cmd cmd="version"> </em_cmd>')
	msg = socket.recv()
	attributes, data = parse_data(msg)
	version = '.'.join(data.split()[1:])
	print '\nMDS version = ', version
	
	#get serial number
	socket.send ('<em_cmd cmd="hello"> </em_cmd>')
	msg = socket.recv()
	attributes, data = parse_data(msg)
	items = data.split()
	num_det = int(items[0])
	sn_list = items[1]
	sn = sn_list[0] # We will use just one eMorpho here
	print 'serial numbers = ',sn_list
	
	run_time = run_time * 60 # convert runTime to seconds
	end_time = time.time() + run_time # determine end_time
	filesize = 1000 #events per .dat file
	ind = 0 #keep track of event number
	fout = open('{0}/{1}_trace.dat'.format(directory,ind),'w') #open file to save data in
	print 'Beginning data acquisition'
	while time.time() < end_time:
		ind = ind + 1
		# http://www.bridgeportinstruments.com/products/mds/mds_doc/start_trace.php
		socket.send ('<em_cmd cmd="start_trace" engage="0" sn="{0}"> 2 1 100 </em_cmd>'.format(sn))
		msg = socket.recv()
		while True:
			# http://www.bridgeportinstruments.com/products/mds/mds_doc/read_cal.php
			socket.send ('<em_cmd cmd="read_cal" engage="0" sn="{0}"> 1 22 </em_cmd>'.format(sn))
			msg = socket.recv()
			attributes, data = parse_data(msg)
			trace_done = data.split()[14] == '1'
			if trace_done:
				# http://www.bridgeportinstruments.com/products/mds/mds_doc/read_trace.php
				socket.send ('<em_cmd cmd="read_trace" engage="0" sn="{0}" fast="1"> 1 1024 </em_cmd>'.format(sn))
				msg = socket.recv()
				attributes, data = parse_data(msg) # attributes is a dict, data is a string
				fout.write(data+'\n')
				break
				
		if ind % filesize == 0:
			print ind
			fout.close()
			fout = open('{0}/{1}_trace.dat'.format(directory,ind/filesize),'w')
	print "Data acquisition complete"
def getParameters():
	IP_Address = "tcp://localhost:5507" # on the LAN: "tcp://192.168.6.1:5507"
	context = zmq.Context()
	socket = context.socket(zmq.REQ)
	socket.connect (IP_Address)
	
	# always send and receive; don't break up the pair.
	socket.send ('<em_cmd cmd="version"> </em_cmd>')
	msg = socket.recv()
	attributes, data = parse_data(msg)
	version = '.'.join(data.split()[1:])
	print '\nMDS version = ', version
	
	#get serial number
	socket.send ('<em_cmd cmd="hello"> </em_cmd>')
	msg = socket.recv()
	attributes, data = parse_data(msg)
	items = data.split()
	num_det = int(items[0])
	sn_list = items[1]
	sn = sn_list[0] # We will use just one eMorpho here
	print 'serial numbers = ',sn_list
	
	#http://www.bridgeportinstruments.com/products/mds/mds_doc/get_set_dsp.php
	socket.send ('<em_cmd cmd="get_dsp" engage="0" sn="{0}"> </em_cmd>'.format(sn))
	msg = socket.recv()
	attributes, data = parse_data(msg)
	items = data.split()
	print '\n Current Values'
	print 'Trigger Threshold: {0} mV'.format(items[3])
	print 'Integration time: {0} microseconds'.format(items[4])
	print 'Hold-off time: {0} microseconds'.format(items[5])
	
	print '\n New Values'
	trigger = raw_input('Trigger Threshold (mV): ')
	intTime = raw_input('Integration Time (microseconds): ')
	holdTime = raw_input('Hold-off Time (microseconds): ')
	items = items[2:]
	items[1] = trigger
	items[2] = intTime
	items[3] = holdTime
	dspRecord = ' '.join(items)
	
	socket.send ('<em_cmd cmd="set_dsp" engage="0" sn="{0}"> {1} </em_cmd>'.format(sn, dspRecord))
	msg = socket.recv()
	attributes, data = parse_data(msg)
def getLED():
	IP_Address = "tcp://localhost:5507" # on the LAN: "tcp://192.168.6.1:5507"
	context = zmq.Context()
	socket = context.socket(zmq.REQ)
	socket.connect (IP_Address)
	
	# always send and receive; don't break up the pair.
	socket.send ('<em_cmd cmd="version"> </em_cmd>')
	msg = socket.recv()
	attributes, data = parse_data(msg)
	version = '.'.join(data.split()[1:])
	print '\nMDS version = ', version
	
	#get serial number
	socket.send ('<em_cmd cmd="hello"> </em_cmd>')
	msg = socket.recv()
	attributes, data = parse_data(msg)
	items = data.split()
	num_det = int(items[0])
	sn_list = items[1]
	sn = sn_list[0] # We will use just one eMorpho here
	print 'serial numbers = ',sn_list
	
	#http://www.bridgeportinstruments.com/products/mds/mds_doc/get_set_pulser.php
	socket.send ('<em_cmd cmd="get_pulser" engage="0" sn="{0}"> </em_cmd>'.format(sn))
	msg = socket.recv()
	attributes, data = parse_data(msg)
	items = data.split()
	pulse_on_off = 'On'
	if items[7] == '0':
		pulse_on_off = 'Off'
	trigger_on_off = 'Yes'
	if items[6] == '0':
		trigger_on_off = 'No'
	print '\n Current Values'
	print 'The pulser is: {0}'.format(pulse_on_off)
	print 'Trigger on the LED pulse: {0}'.format(trigger_on_off)
	print 'Pulse rate: {0}'.format(items[3])
	print 'Pulse width: {0}'.format(items[4])
	
	print '\n New Values'
	pulserOnOff = raw_input('Pulser on/off (0->Off, 1->On): ')
	if pulserOnOff != '1' and pulserOnOff != '0':
		print 'Invalid Input'
		return
	triggerOnOff = raw_input('Trigger on pulse (0->Off, 1->On): ')
	if triggerOnOff != '1' and triggerOnOff != '0':
		print 'Invalid Input'
		return
	PR = raw_input('Pulse rate 1<=PR<=31: ')
	PW = raw_input('Pulse width 0<=PW<=15: ')

	items = items[2:]
	items[1] = PR
	items[2] = PW
	items[5] = pulserOnOff
	items[4] = triggerOnOff
	pulseRecord = ' '.join(items)
	
	socket.send ('<em_cmd cmd="set_pulser" engage="0" sn="{0}"> {1} </em_cmd>'.format(sn, pulseRecord))
	msg = socket.recv()
	attributes, data = parse_data(msg)
def printParameters():
	IP_Address = "tcp://localhost:5507" # on the LAN: "tcp://192.168.6.1:5507"
	context = zmq.Context()
	socket = context.socket(zmq.REQ)
	socket.connect (IP_Address)
	
	# always send and receive; don't break up the pair.
	socket.send ('<em_cmd cmd="version"> </em_cmd>')
	msg = socket.recv()
	attributes, data = parse_data(msg)
	version = '.'.join(data.split()[1:])
	print '\nMDS version = ', version
	
	#get serial number
	socket.send ('<em_cmd cmd="hello"> </em_cmd>')
	msg = socket.recv()
	attributes, data = parse_data(msg)
	items = data.split()
	num_det = int(items[0])
	sn_list = items[1]
	sn = sn_list[0] # We will use just one eMorpho here
	print 'serial numbers = ',sn_list
	
	#http://www.bridgeportinstruments.com/products/mds/mds_doc/get_set_dsp.php
	socket.send ('<em_cmd cmd="get_dsp" engage="0" sn="{0}"> </em_cmd>'.format(sn))
	msg = socket.recv()
	attributes, data = parse_data(msg)
	items = data.split()
	print '\n Current Values'
	print 'Trigger Threshold: {0} mV'.format(items[3])
	print 'Integration time: {0} microseconds'.format(items[4])
	print 'Hold-off time: {0} microseconds'.format(items[5])
	
	#http://www.bridgeportinstruments.com/products/mds/mds_doc/get_set_pulser.php
	socket.send ('<em_cmd cmd="get_pulser" engage="0" sn="{0}"> </em_cmd>'.format(sn))
	msg = socket.recv()
	attributes, data = parse_data(msg)
	items = data.split()
	pulse_on_off = 'On'
	if items[7] == '0':
		pulse_on_off = 'Off'
	trigger_on_off = 'Yes'
	if items[6] == '0':
		trigger_on_off = 'No'
	print 'The pulser is: {0}'.format(pulse_on_off)
	print 'Trigger on the LED pulse: {0}'.format(trigger_on_off)
	print 'Pulse rate: {0}'.format(items[3])
	print 'Pulse width: {0}'.format(items[4])
	
def convertStrFloat(s):
	ret = 0
	try:
		ret = float(s)
	except ValueError:
		pass
	return ret
def convertStrInt(s):
	ret = 0
	try:
		ret = int(s)
	except ValueError:
		pass
	return ret

#Beginning of Main function
def main():
	choice = ' '
	while choice != '0':
		print '\nWhat would you like to do:'
		print '  0: Exit'
		print '  1: Get/Set instrument parameters'
		print '  2: Take timed run'
		print '  3: Turn on/off LED pulser'
		print '  4: Show all parameters'
		choice=raw_input('Select item> ')
		if choice == '1': #change parameters
			getParameters()
		elif choice == '2': #take timed data run
			length = raw_input('Length of Run: ')
			runTime = convertStrFloat(length)
			if runTime == 0:
				print 'Invalid Input'
				continue;
			print '% Input the name for the directory where the data will be saved'
			print '% If the directory does not exist it will be created'
			print '% Format: data\\\\run1'
			dataDir = raw_input('Data directory: ')
			if not os.path.exists(dataDir):
				os.makedirs(dataDir)
			multi_trace_daq(runTime, dataDir)
		elif choice == '3': #Turn on/off LED pulser
			getLED()
		elif choice == '4':#display parameters
			printParameters()
		else:
			choice == ' '

main()