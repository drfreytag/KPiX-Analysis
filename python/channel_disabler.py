import numpy as np
import os 
import ROOT
import argparse
import argcomplete
import sys

RMS_hist = None
slope_hist = None
acquire_hist = None

class MyParser(argparse.ArgumentParser):
    def error(self, message):
        sys.stderr.write('error: %s\n' % message)
        self.print_help()
        sys.exit(2)

def loopdir(keys):  # loop through all subdirectories of the root file and look for the vs channel histograms
	global RMS_hist, slope_hist, acquire_hist ##declare that the beforementioned variables are global variables
	for key_object in keys:
		if ('TDirectory' in key_object.GetClassName()):
			loopdir(key_object.ReadObj().GetListOfKeys())
		else:
			#print key_object.GetName()
			if 'slope_vs_channel' == key_object.GetName():
				slope_hist = key_object
			elif 'RMSfc_vs_channel' == key_object.GetName():
				RMS_hist = key_object
			elif 'Channel_entries_k_' in key_object.GetName() and '_total' in key_object.GetName() and 'timed' not in key_object.GetName() and 'no' not in key_object.GetName():
				acquire_hist = key_object

######### Main code #########
parser = MyParser()
#parser = argparse.ArgumentParser()
#parser.add_argument('file_in', nargs='+', help='name of the input files')
parser.add_argument('--calib', dest='calib', nargs='?', help='input calibration file')
parser.add_argument('--acquire', dest='acquire', nargs='?', help='input pedestal file')
parser.add_argument('-k', dest='kpix', nargs='+', help='which kpix to check, eg: 1 or 1 2')

if len(sys.argv) < 2:
	print parser.print_help()
	sys.exit(1)

args = parser.parse_args()
root_file_list = []

#for root_file in args.file_in:
#    root_file_list.append(ROOT.TFile(root_file))
if args.calib:
    root_file_list.append(ROOT.TFile(args.calib))
if args.acquire:
    root_file_list.append(ROOT.TFile(args.acquire))

for x in root_file_list:
	key_root = x.GetListOfKeys()
	loopdir(key_root)


mapping_file_name = './include/kpix_left_and_right.h'
mapping_file = open(mapping_file_name, "r")
	
		
noisy_channels = []
noisy_channels_acquire = []
dead_channels_slope = []
dead_channels_RMS = []
dc_channels = []

if args.calib:
    slope_obj = slope_hist.ReadObj()
    RMS_obj = RMS_hist.ReadObj()
if args.acquire:
    acquire_obj = acquire_hist.ReadObj()

for chan in xrange(1024):
	if (slope_obj.GetBinContent(chan+1) <= 1.0): #GetBinContent(0) is the underflow bin, here the bin counting starts at 1 therefore chan+1
		dead_channels_slope.append(chan)
	if (RMS_obj.GetBinContent(chan+1) <= 0.05):
		dead_channels_RMS.append(chan)
	if (RMS_obj.GetBinContent(chan+1) >= 4):
		noisy_channels.append(chan)
        if args.acquire:
            if (acquire_obj.GetBinContent(chan+1) >= 0.02):
		noisy_channels_acquire.append(chan)
for line in mapping_file:
	if 'm1.insert' in line:
		line=line[22:-4]
		line_split=line.split(',')
		if (line_split[1] == '9999') and (int(line_split[0]) not in dc_channels):
			dc_channels.append(int(line_split[0]))		

kpix = np.chararray(1024)		#create mapper kpix
filename_map = './data/disable.txt'		#choose filename

print 'Dead channels from calibration slope = ', dead_channels_slope
print 'Dead channels from calibration RMS = ', dead_channels_RMS
print 'Very high noise channels = ', noisy_channels
print 'Channels not connected to strips based on mapping = ', dc_channels
print 'High noise channels in acquire running = ', noisy_channels_acquire
print ''


for chan in xrange(1024):		#create the character mapping of the channels
	if (chan in dc_channels) or (chan in dead_channels_slope) or (chan in dead_channels_RMS) or (chan in noisy_channels) or (chan in noisy_channels_acquire):		#A for active and D for deactivated channels
		kpix[chan] = 'D'
	else :
		kpix[chan] = 'A'

for i in xrange(1024):
		if ((i%32 == 0) and (i is not 0)):
			print ''
			sys.stdout.write(kpix[i])
		else:
			sys.stdout.write(kpix[i])
		if (i == 7):
			sys.stdout.write(' ')
		elif (i > 7 and (i-7)%8 == 0 and i is not 0 and i%32 is not 0):
			sys.stdout.write(' ')

	
kpix = np.reshape(kpix, (32, 32))	#reshape it for fitting in xml file
print ''



print ''		
np.savetxt(filename_map, kpix, fmt= ['%.1c' ,'%.1c', '%.1c' , '%.1c', '%.1c' , '%.1c', '%.1c' , '%.1c', '%2.1c' , '%.1c', '%.1c' , '%.1c', '%.1c' , '%.1c', '%.1c' , '%.1c', '%2.1c' , '%.1c', '%.1c' , '%.1c', '%.1c' , '%.1c', '%.1c' , '%.1c', '%2.1c' , '%.1c', '%.1c' , '%.1c', '%.1c' , '%.1c', '%.1c' , '%.1c'], delimiter = '')
print 'Saving deactivation mapping in: ', filename_map

##save it in filename with a format that fits in the xml file, it adds the space after 8 characters so it's easier to see (but a pain to write it like this, must be another way) 

mapping_file.close() #close tracker_to_kpix


for x in root_file_list:
	ROOT.gROOT.GetListOfFiles().Remove(x)
	x.Close()






		




