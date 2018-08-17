#!usr/bin/python

import numpy as np
import matplotlib.pyplot as plt
import string
import ROOT
import argparse
import argcomplete
from operator import add
import sys


left_strip = None
left_strip_b0 = None
left_strip_b1 = None
left_strip_b2 = None
left_strip_b3 = None
right_strip = None
right_strip_b0 = None
right_strip_b1 = None
right_strip_b2 = None
right_strip_b3 = None

class MyParser(argparse.ArgumentParser):
    def error(self, message):
        sys.stderr.write('error: %s\n' % message)
        self.print_help()
        sys.exit(2)


def loopdir(keys):  # loop through all subdirectories of the root file and look for the vs channel histograms
	global left_strip, left_strip_b0, left_strip_b1, left_strip_b2, left_strip_b3
	global right_strip, right_strip_b0, right_strip_b1, right_strip_b2, right_strip_b3 ##declare that the beforementioned variables are global variables
	for key_object in keys:
		if ('TDirectory' in key_object.GetClassName()):
			loopdir(key_object.ReadObj().GetListOfKeys())
		else:
			#print key_object.GetName()
			if 'Left_Strip_entries_k_26' in key_object.GetName() and '_total' in key_object.GetName():
				left_strip = key_object
			if 'Left_Strip_entries_k_26' in key_object.GetName() and '_b0' in key_object.GetName():
				left_strip_b0 = key_object
			if 'Left_Strip_entries_k_26' in key_object.GetName() and '_b1' in key_object.GetName():
				left_strip_b1 = key_object
			if 'Left_Strip_entries_k_26' in key_object.GetName() and '_b2' in key_object.GetName():
				left_strip_b2 = key_object
			if 'Left_Strip_entries_k_26' in key_object.GetName() and '_b3' in key_object.GetName():
				left_strip_b3 = key_object
			if 'Right_Strip_entries_k_28' in key_object.GetName() and '_total' in key_object.GetName():
				right_strip = key_object
			if 'Right_Strip_entries_k_28' in key_object.GetName() and '_b0' in key_object.GetName():
				right_strip_b0 = key_object
			if 'Right_Strip_entries_k_28' in key_object.GetName() and '_b1' in key_object.GetName():
				right_strip_b1 = key_object
			if 'Right_Strip_entries_k_28' in key_object.GetName() and '_b2' in key_object.GetName():
				right_strip_b2 = key_object
			if 'Right_Strip_entries_k_28' in key_object.GetName() and '_b3' in key_object.GetName():
				right_strip_b3 = key_object

parser = MyParser()
parser.add_argument('file_in', help='name of the input file')
args = parser.parse_args()

if len(sys.argv) < 2:
	print parser.print_help()
	sys.exit(1)


rootfile = ROOT.TFile(args.file_in)
key_root = rootfile.GetListOfKeys()
file_base_name = args.file_in[args.file_in.find('/20')+1:args.file_in.rfind('.bin')]
loopdir(key_root)
if left_strip:
    left_strip_hist = left_strip.ReadObj()
if left_strip_b0:
	left_strip_b0_hist = left_strip_b0.ReadObj()
if left_strip_b1:
	left_strip_b1_hist = left_strip_b1.ReadObj()        
if left_strip_b2:
	left_strip_b2_hist = left_strip_b2.ReadObj()        
if left_strip_b3:
	left_strip_b3_hist = left_strip_b3.ReadObj()        

if right_strip:
	right_strip_hist = right_strip.ReadObj()
if right_strip_b0:
	right_strip_b0_hist = right_strip_b0.ReadObj()
if right_strip_b1:
	right_strip_b1_hist = right_strip_b1.ReadObj()
if right_strip_b2:
	right_strip_b2_hist = right_strip_b2.ReadObj()
if right_strip_b3:
	right_strip_b3_hist = right_strip_b3.ReadObj()


Input_left = []
Input_left_b0 = []
Input_left_b1 = []
Input_left_b2 = []
Input_left_b3 = []
Input_right = []
Input_right_b0 = []
Input_right_b1 = []
Input_right_b2 = []
Input_right_b3 = []
C = []

for count in xrange(920):
	if right_strip:
		Input_right.append(0)
	if right_strip_b0:
		Input_right_b0.append(0)
	if right_strip_b1:
		Input_right_b1.append(0)
	if right_strip_b2:
		Input_right_b2.append(0)
	if right_strip_b3:
		Input_right_b3.append(0)

for chan in xrange(920):
	if left_strip:
		Input_left.append(left_strip_hist.GetBinContent(chan+1))
	if left_strip_b0:
		Input_left_b0.append(left_strip_b0_hist.GetBinContent(chan+1))
	if left_strip_b1:
		Input_left_b1.append(left_strip_b1_hist.GetBinContent(chan+1))
	if left_strip_b2:
		Input_left_b2.append(left_strip_b2_hist.GetBinContent(chan+1))
	if left_strip_b3:
		Input_left_b3.append(left_strip_b3_hist.GetBinContent(chan+1))            
	if right_strip:
		Input_right.append(right_strip_hist.GetBinContent(chan+1))
	if right_strip_b0:
		Input_right_b0.append(right_strip_b0_hist.GetBinContent(chan+1))
	if right_strip_b1:
		Input_right_b1.append(right_strip_b1_hist.GetBinContent(chan+1))
	if right_strip_b2:
		Input_right_b2.append(right_strip_b2_hist.GetBinContent(chan+1))
	if right_strip_b3:
		Input_right_b3.append(right_strip_b3_hist.GetBinContent(chan+1))

max_tot=max(Input_left)
if (max(Input_right)>max_tot):
	max_tot=max(Input_right)
max_b0=max(Input_left_b0)
if (max(Input_right_b0)>max_b0):
	max_b0=max(Input_right_b0)
max_b1=max(Input_left_b1)
if (max(Input_right_b1)>max_b1):
	max_b1=max(Input_right_b1)
max_b2=max(Input_left_b2)
if (max(Input_right_b2)>max_b2):
	max_b2=max(Input_right_b2)
max_b3=max(Input_left_b3)
if (max(Input_right_b3)>max_b3):
	max_b3=max(Input_right_b3)

	
if Input_left:
    C = [Input_left]
    fig = plt.figure()
    ax = fig.add_subplot(111)
    axes = plt.axes() 
    axes.get_yaxis().set_visible(False)
    axes.set_xlim(0,920)
    pcm = ax.pcolormesh(C, cmap='viridis', vmin=0, vmax=max_tot)
    plt.colorbar(pcm)
    filename = '/home/lycoris-dev/Documents/'+file_base_name+'_strip_map_left.png'
    print "File is saved in " +  filename
    plt.savefig(filename, dpi = 300)

if Input_left_b0:
	C = [Input_left_b0]
	fig = plt.figure()
	ax = fig.add_subplot(111)
	axes = plt.axes() 
	axes.get_yaxis().set_visible(False)
	axes.set_xlim(0,920)
	pcm = ax.pcolormesh(C, cmap='viridis', vmin=0, vmax=max_b0)
	plt.colorbar(pcm)
	filename = '/home/lycoris-dev/Documents/'+file_base_name+'_strip_map_left_b0.png'
	print "File is saved in " +  filename
	plt.savefig(filename, dpi = 300)
    
if Input_left_b1:
	C = [Input_left_b1]
	fig = plt.figure()
	ax = fig.add_subplot(111)
	axes = plt.axes() 
	axes.get_yaxis().set_visible(False)
	axes.set_xlim(0,920)
	pcm = ax.pcolormesh(C, cmap='viridis', vmin=0, vmax=max_b1)
	plt.colorbar(pcm)
	filename = '/home/lycoris-dev/Documents/'+file_base_name+'_strip_map_left_b1.png'
	print "File is saved in " +  filename
	plt.savefig(filename, dpi = 300)
    
if Input_left_b2:
	C = [Input_left_b2]
	fig = plt.figure()
	ax = fig.add_subplot(111)
	axes = plt.axes() 
	axes.get_yaxis().set_visible(False)
	axes.set_xlim(0,920)
	pcm = ax.pcolormesh(C, cmap='viridis', vmin=0, vmax=max_b2)
	plt.colorbar(pcm)
	filename = '/home/lycoris-dev/Documents/'+file_base_name+'_strip_map_left_b2.png'
	print "File is saved in " +  filename
	plt.savefig(filename, dpi = 300)
    
if Input_left_b3:
	C = [Input_left_b3]
	fig = plt.figure()
	ax = fig.add_subplot(111)
	axes = plt.axes() 
	axes.get_yaxis().set_visible(False)
	axes.set_xlim(0,920)
	pcm = ax.pcolormesh(C, cmap='viridis', vmin=0, vmax=max_b3)
	plt.colorbar(pcm)
	filename = '/home/lycoris-dev/Documents/'+file_base_name+'_strip_map_left_b3.png'
	print "File is saved in " +  filename
	plt.savefig(filename, dpi = 300)

if Input_right:
	C = [Input_right]
	fig = plt.figure()
	ax = fig.add_subplot(111)
	axes = plt.axes() 
	axes.get_yaxis().set_visible(False)
	axes.set_xlim(920,1840)
	pcm = ax.pcolormesh(C, cmap='viridis', vmin=0, vmax=max_tot)
	plt.colorbar(pcm)
	filename = '/home/lycoris-dev/Documents/'+file_base_name+'_strip_map_right.png'
	print "File is saved in " +  filename
	plt.savefig(filename, dpi = 300)
    
if Input_right_b0:
	C = [Input_right_b0]
	fig = plt.figure()
	ax = fig.add_subplot(111)
	axes = plt.axes() 
	axes.get_yaxis().set_visible(False)
	axes.set_xlim(920,1840)
	pcm = ax.pcolormesh(C, cmap='viridis', vmin=0, vmax=max_b0)
	plt.colorbar(pcm)
	filename = '/home/lycoris-dev/Documents/'+file_base_name+'_strip_map_right_b0.png'
	print "File is saved in " +  filename
	plt.savefig(filename, dpi = 300)
    
if Input_right_b1:
	C = [Input_right_b1]
	fig = plt.figure()
	ax = fig.add_subplot(111)
	axes = plt.axes() 
	axes.get_yaxis().set_visible(False)
	axes.set_xlim(920,1840)
	pcm = ax.pcolormesh(C, cmap='viridis', vmin=0, vmax=max_b1)
	plt.colorbar(pcm)
	filename = '/home/lycoris-dev/Documents/'+file_base_name+'_strip_map_right_b1.png'
	print "File is saved in " +  filename
	plt.savefig(filename, dpi = 300)
    
if Input_right_b2:
	C = [Input_right_b2]
	fig = plt.figure()
	ax = fig.add_subplot(111)
	axes = plt.axes() 
	axes.get_yaxis().set_visible(False)
	axes.set_xlim(920,1840)
	pcm = ax.pcolormesh(C, cmap='viridis', vmin=0, vmax=max_b2)
	plt.colorbar(pcm)
	filename = '/home/lycoris-dev/Documents/'+file_base_name+'_strip_map_right_b2.png'
	print "File is saved in " +  filename
	plt.savefig(filename, dpi = 300)
    
if Input_right_b3:
	C = [Input_right_b3]
	fig = plt.figure()
	ax = fig.add_subplot(111)
	axes = plt.axes() 
	axes.get_yaxis().set_visible(False)
	axes.set_xlim(920,1840)
	pcm = ax.pcolormesh(C, cmap='viridis', vmin=0, vmax=max_b3)
	plt.colorbar(pcm)
	filename = '/home/lycoris-dev/Documents/'+file_base_name+'_strip_map_right_b3.png'
	print "File is saved in " +  filename
	plt.savefig(filename, dpi = 300)

plt.close()
