from __future__ import division
import zmq
import time
import xml.dom.minidom

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

def daq(run_time): #takes lm data for given time in seconds and saves it to a file in the given directory
	""" Connect to MDS, boot eMorpho, acquire num_buffers list_mode buffers, save data and exit """
	directory = 'D:\\CERNBox_Data\\MilliQan\\data'
	IP_Address = "tcp://localhost:5507" # on the LAN: "tcp://192.168.6.1:5507"
	context = zmq.Context()
	socket = context.socket(zmq.REQ)
	socket.connect (IP_Address)
	# always send and receive; don't break up the pair.
	socket.send ('<em_cmd cmd="version"> </em_cmd>')
	msg = socket.recv()
	attributes, data = parse_data(msg)
	version = '.'.join(data.split()[1:])
	print 'MDS version = ', version
	
	#get serial number
	socket.send ('<em_cmd cmd="hello"> </em_cmd>')
	msg = socket.recv()
	attributes, data = parse_data(msg)
	items = data.split()
	sn_list = items[1:]
	print 'serial numbers = ',sn_list
	
	LEDenable = 0
	socket.send ('<em_cmd cmd="set_pulser" engage="0" sn="{0}"> 5 21 3 0 0 {1}</em_cmd>'.format("eRC1405",LEDenable))
	msg = socket.recv()
	time.sleep(0.5)#Wait for 0.5 seconds	

	start_time = time.time() #when we started
	end_time = start_time + run_time # determine end_time
	led_time = start_time # last time we changed the LED
	itr = [0,0]
	fout=[]
        for sn in range(0,len(sn_list)):
            fout.append(open('{0}/{1}_lm_{2}.dat'.format(directory,sn_list[sn],(int)(start_time*1000)),'a'))
	    
        # http://www.bridgeportinstruments.com/products/mds/mds_doc/start_lm.php
        for sn in range(0,len(sn_list)):
            socket.send ('<em_cmd cmd="start_lm" engage="0" sn="{0}"> 2 0 1 </em_cmd>'.format(sn_list[sn])) #reset time counter
            msg = socket.recv()
        rate = 19
        LEDenable=1
        while time.time() < end_time:
            if (time.time()-led_time)>3.0: # if it's been more than 3 sec, change the pulser rate from 10 to 40 to 10...
                if (rate == 19): rate=21
                else: rate=19
                socket.send ('<em_cmd cmd="set_pulser" engage="0" sn="{0}"> 5 {2} 3 0 0 {1}</em_cmd>'.format("eRC1405",LEDenable,rate))
                msg = socket.recv()
                print "changed LED to be ",LEDenable," at rate ",rate," after time ",time.time()-led_time
                led_time=time.time()
            for sn in range(0,len(sn_list)):
                # http://www.bridgeportinstruments.com/products/mds/mds_doc/read_cal.php
                socket.send ('<em_cmd cmd="read_cal" engage="0" sn="{0}"> 1 22 </em_cmd>'.format(sn_list[sn]))
                msg = socket.recv()
                attributes, data = parse_data(msg)
                lm_done = data.split()[15] == '1'
                if lm_done:
		    itr[sn] = itr[sn] + 1
		    if itr[sn] % 5 == 0:
		        print 'Buffer Count = ',itr[sn],'sn = ',sn,' Time left = ',end_time - time.time()
		        # http://www.bridgeportinstruments.com/products/mds/mds_doc/read_lm.php
		    socket.send ('<em_cmd cmd="read_lm" engage="0" sn="{0}"> 1 1 </em_cmd>'.format(sn_list[sn]))
	            msg = socket.recv()
		    attributes, data = parse_data(msg) # attributes is a dict, data is a string
		    fout[sn].write(data+'\n')
		    socket.send ('<em_cmd cmd="start_lm" engage="0" sn="{0}"> 2 0 0 </em_cmd>'.format(sn_list[sn])) #don't reset time counter
	            msg = socket.recv()
        for sn in range(0,len(sn_list)):
            fout[sn].close()
        
        LEDenable = 0
	socket.send ('<em_cmd cmd="set_pulser" engage="0" sn="{0}"> 5 21 3 0 0 {1}</em_cmd>'.format("eRC1405",LEDenable))
	msg = socket.recv()

