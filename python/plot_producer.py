#!usr/bin/python

import numpy as np
import string
from matplotlib import cm
import ROOT
import argparse
import argcomplete
from operator import add
import sys

ROOT.gROOT.SetBatch(True)

hist_list= []  #Global list of all chosen histograms (only works because its a list, with single variables they need to be declared as global in the function
graph_list = []



class MyParser(argparse.ArgumentParser):
    def error(self, message):
        sys.stderr.write('error: %s\n' % message)
        self.print_help()
        sys.exit(2)



def loopdir(keys):  # loop through all subdirectories of the root file and add all plots+histograms that contain the same string as given by the name, channel and bucket arguments to the global list of all chosen histograms hist_list
	for key_object in keys:
		if ('TDirectory' in key_object.GetClassName()):
			loopdir(key_object.ReadObj().GetListOfKeys())
		else:
			if ('TGraph' in key_object.GetClassName()):
				if(args.name == 'everything'):
					#print key_object.GetName()
					graph_list.append(key_object)
				else:
					if args.exact:
						for name in args.name:
							if (name == key_object.GetName()): 
								graph_list.append(key_object)
					else:	
						for name in args.name:
							for refuse in args.refuse:
								for channel in args.channel:	
									for strip in args.strip:
										for bucket in args.bucket:
											for kpix in args.kpix:
												if (bucket == 4):
													if ((name in key_object.GetName() and refuse not in key_object.GetName()) \
													and ('_c'+str(channel)+'_' in key_object.GetName() or channel == '9999') and ('_s'+str(strip)+'_' in key_object.GetName() or strip == '9999') and ('b' not in key_object.GetName()) and ('k'+str(kpix) in key_object.GetName() or 'k_'+str(kpix) in key_object.GetName() or kpix == 9999)):
														#print 'Histogram found: ', key_object.GetName()
														graph_list.append(key_object)
												else:
													if ((name in key_object.GetName() and refuse not in key_object.GetName()) \
													and ('_c'+str(channel)+'_' in key_object.GetName() or channel == '9999') and ('_s'+str(strip)+'_' in key_object.GetName() or strip == '9999') and ('b'+str(bucket) in key_object.GetName() or bucket == 9999) and ('k'+str(kpix) in key_object.GetName() or 'k_'+str(kpix) in key_object.GetName() or kpix == 9999)):
														#print 'Histogram found: ', key_object.GetName()
														graph_list.append(key_object)
			else:
				if(args.name == 'everything' and key_object.ReadObj().GetEntries() != 0):
					#print key_object.GetName()
					hist_list.append(key_object)
				else:
					if args.exact:
						for name in args.name:
							if (name == key_object.GetName()): 
								hist_list.append(key_object)
					else:	
						for name in args.name:
							for refuse in args.refuse:
								for channel in args.channel:	
									for strip in args.strip:
										for bucket in args.bucket:
											for kpix in args.kpix:
												if (bucket == 4):
													if ((name in key_object.GetName() and refuse not in key_object.GetName()) \
													and (key_object.ReadObj().GetEntries() != 0) \
													and ('_c'+str(channel)+'_' in key_object.GetName() or channel == '9999') and ('_s'+str(strip)+'_' in key_object.GetName() or strip == '9999') and ('b' not in key_object.GetName()) and ('k'+str(kpix) in key_object.GetName() or 'k_'+str(kpix) in key_object.GetName() or kpix == 9999)):
														#print 'Histogram found: ', key_object.GetName()
														hist_list.append(key_object)
												else:
													if ((name in key_object.GetName() and refuse not in key_object.GetName()) \
													and (key_object.ReadObj().GetEntries() != 0) \
													and ('_c'+str(channel)+'_' in key_object.GetName() or channel == '9999') and ('_s'+str(strip)+'_' in key_object.GetName() or strip == '9999') and ('b'+str(bucket) in key_object.GetName() or bucket == 9999) and ('k'+str(kpix) in key_object.GetName() or 'k_'+str(kpix) in key_object.GetName() or kpix == 9999)):
														#print 'Histogram found: ', key_object.GetName()
														hist_list.append(key_object)

def hist_plotter():
	if ('same' in args.draw_option):
		##------------------
		##initialize a canvas, a stack histogram and further variables.
		drawing_option = args.draw_option.replace('same', 'NOSTACK') #exchange the same with a NOSTACK as I am using THStack
		c1 = ROOT.TCanvas( args.output_name, 'Test', 1200, 900 )
		c1.cd()
		c1.SetFillColor(0)
		legend = ROOT.TLegend(legend_location[0], legend_location[1], legend_location[2], legend_location[3])
		hist_comp = ROOT.THStack()
		counter = 1
		x_title = None
		y_title = None
		x_low = None
		x_high = None
		y_low = None
		y_high = None
		new_hist_list = []
		if args.order:
			for i in args.order:
				new_hist_list.append(hist_list[i]) 
		else:
			new_hist_list = hist_list
		for histogram in new_hist_list:
			##------------------
			##loop through the histograms, get all parameters
			obj = histogram.ReadObj()
			#print 'Number of entries =', obj.GetEntries()
			x_axis = obj.GetXaxis()
			y_axis = obj.GetYaxis()
			##------------------
			##rebin the histogram
			if (args.rebin is not 1):
					obj.Rebin(args.rebin)
			##------------------
			##adjust the xrange
			if 9999 in args.xaxisrange:
				if (x_low is None):
					x_low = obj.FindFirstBinAbove(0)-10
				elif (x_low > obj.FindFirstBinAbove(0)-10):
					x_low = obj.FindFirstBinAbove(0)-10
				if (x_high is None):
					x_high = obj.FindLastBinAbove(0)+10
				elif (x_high < obj.FindLastBinAbove(0)+10):
					x_high = obj.FindLastBinAbove(0)+10
				if (x_high > obj.GetNbinsX()):  #avoids overflow bin
					x_high = obj.GetNbinsX()
				if (x_low <= 0): #avoids underflow bin
					x_low = 1
				x_axis.SetRange(x_low, x_high)
			else:
				x_low = args.xaxisrange[0]
				x_high = args.xaxisrange[1]
				x_axis.SetRangeUser(x_low, x_high)
			if 9999 not in args.yaxisrange:
				y_low = args.yaxisrange[0]
				y_high = args.yaxisrange[1]
				print 'test'
				y_axis.SetRangeUser(y_low, y_high) 
			#print x_low, x_high
			#x_axis.SetRangeUser(x_low, x_high)
			obj.SetLineColor(args.color[counter-1])
			obj.SetMarkerColor(args.color[counter-1])

			
			##------------------
			##draw histograms into the same canvas (equivalent to option same)
			hist_comp.Add(obj)
			##------------------
			##adjust legend and the x and y title name if chosen
			if (not args.legend):
				if len(filename_list) > 1:
					legend.AddEntry(obj, filename_list[counter-1]+'_'+histogram.GetName())
				else:
					legend.AddEntry(obj, '_'+histogram.GetName())
			else:
				legend.AddEntry(obj, args.legend[counter-1])
			counter +=1
			if (args.xtitle):
				x_title = args.xtitle
			else:
				x_title = x_axis.GetTitle()
			if (args.ytitle):
				y_title = args.ytitle
			else:
				y_title = y_axis.GetTitle()
		##------------------
		##set y axis to log
		if args.ylog:
			c1.SetLogy()
			ROOT.gPad.SetLogy()
		##------------------
		##draw histogram + components and save the file
		hist_comp.Draw(drawing_option)
		xaxis = hist_comp.GetXaxis()
		xaxis.SetTitle(x_title)
		print x_low
		print x_high
		if 9999 in args.xaxisrange:
			xaxis.SetRange(x_low, x_high)
		else:
			xaxis.SetRangeUser(x_low, x_high)
		yaxis = hist_comp.GetYaxis()
		if 9999 not in args.yaxisrange:
			yaxis.SetRangeUser(y_low, y_high) 
			print 'test'
		yaxis.SetTitle(y_title)
		#if (args.ylog is True):
		legend.Draw()
		if ('test' not in args.output_name):
			print 'Creating /home/lycoris-dev/Documents/'+save_loc+filename_list[0]+'_'+args.output_name
			c1.SaveAs('/home/lycoris-dev/Documents/'+save_loc+filename_list[0]+'_'+args.output_name)
		else:
			print 'Creating /home/lycoris-dev/Documents/'+save_loc+filename_list[0]+'_'+histogram.GetName()+'.png'
			c1.SaveAs('/home/lycoris-dev/Documents/'+save_loc+filename_list[0]+'_'+histogram.GetName()+'.png')
		c1.Close()
	else:
		counter = 0
		for histogram in hist_list:
			##------------------
			##loop through the histograms, get all parameters and adjust the xrange
			#ROOT.gROOT.SetBatch(1)
			c1 = ROOT.TCanvas( 'test', 'Test', 1200,900 ) #
			obj = histogram.ReadObj()
			#print 'Number of entries =', obj.GetEntries()
			x_axis = obj.GetXaxis()
			y_axis = obj.GetYaxis()
			c1.cd()
			#print obj.GetEntries()
			if 9999 in args.xaxisrange:
				x_high = obj.FindLastBinAbove(0)+10
				if (x_high > obj.GetNbinsX()):  #avoids overflow bin
					x_high = obj.GetNbinsX()
				x_low = obj.FindFirstBinAbove(0)-10
				if (x_low <= 0): #avoids underflow bin
					x_low = 1
				x_axis.SetRange(x_low, x_high)
			else:
				x_low = args.xaxisrange[0]
				x_high = args.xaxisrange[1]
				x_axis.SetRangeUser(x_low, x_high)
			if 9999 not in args.yaxisrange:
				y_low = args.yaxisrange[0]
				y_high = args.yaxisrange[1]
				y_axis.SetRangeUser(y_low, y_high)
			obj.SetLineColor(4) #Blue
			##------------------
			##rebin the histogram
			if (args.rebin is not 1):
				obj.Rebin(args.rebin)
			##------------------
			##set y axis to log
			if args.ylog:
				c1.SetLogy()
				ROOT.gPad.SetLogy()
			##------------------
			##set axis titles if chosen through options.
			if (args.xtitle):
				x_axis.SetTitle(args.xtitle)
			if (args.ytitle):
				y_axis.SetTitle(args.ytitle)
			obj.Draw(args.draw_option)
			#ROOT.gROOT.SetBatch(0)
			#raw_input('Press Enter to look at the next histogram')
			if (args.output_name):
				print 'Creating /home/lycoris-dev/Documents/'+save_loc+filename_list[0]+'_'+args.output_name
				c1.SaveAs('/home/lycoris-dev/Documents/'+save_loc+filename_list[0]+'_'+args.output_name)
			else:
				print 'Creating /home/lycoris-dev/Documents/'+save_loc+filename_list[0]+'_'+histogram.GetName()+'.png'
				c1.SaveAs('/home/lycoris-dev/Documents/'+save_loc+filename_list[0]+'_'+histogram.GetName()+'.png')
			c1.Close()
			counter= counter+1
			#for i in xrange(obj.FindFirstBinAbove(0),obj.FindLastBinAbove(0),1):
			#'	print obj.GetBinContent(i)							
	
def graph_plotter():
	if ('same' in args.draw_option):
		##------------------
		##initialize a canvas, a stack histogram and further variables.
		drawing_option = args.draw_option.replace('same', 'A') #exchange the same with a NOSTACK as I am using THStack
		c1 = ROOT.TCanvas( args.output_name, 'Test', 1200, 900 )
		c1.cd()
		c1.SetFillColor(0)
		legend = ROOT.TLegend(legend_location[0], legend_location[1], legend_location[2], legend_location[3])
		multi_graph = ROOT.TMultiGraph()
		counter = 1
		x_title = None
		y_title = None
		x_low = None
		x_high = None
		y_low = None
		y_high = None
		
		new_graph_list = []
		if args.order:
			for i in args.order:
				new_graph_list.append(graph_list[i]) 
		else:
			new_graph_list = graph_list
		
		for graph in new_graph_list:
			##------------------
			##loop through the histograms, get all parameters
			obj = graph.ReadObj()
			#print 'Number of entries =', obj.GetEntries()
			x_axis = obj.GetXaxis()
			y_axis = obj.GetYaxis()
			##------------------
			##adjust the xrange
			obj.SetLineColor(args.color[counter-1])
			obj.SetMarkerColor(args.color[counter-1])

			
			##------------------
			##draw histograms into the same canvas (equivalent to option same)
			multi_graph.Add(obj)
			##------------------
			##adjust legend and the x and y title name if chosen
			if (not args.legend):
				if len(filename_list) > 1:
					legend.AddEntry(obj, filename_list[counter-1]+'_'+graph.GetName())
				else:
					legend.AddEntry(obj, '_'+graph.GetName())
			else:
				legend.AddEntry(obj, args.legend[counter-1])
			counter +=1
			if (args.xtitle):
				x_title = args.xtitle
			else:
				x_title = x_axis.GetTitle()
			if (args.ytitle):
				y_title = args.ytitle
			else:
				y_title = y_axis.GetTitle()
		##------------------
		##set y axis to logcolor
		if args.ylog:
			c1.SetLogy()
			ROOT.gPad.SetLogy()
		##------------------
		##draw histogram + components and save the file
		multi_graph.Draw(drawing_option)
		xaxis = multi_graph.GetXaxis()
		xaxis.SetTitle(x_title)
		if 9999 not in args.xaxisrange:
			xaxis.SetRangeUser(args.xaxisrange[0], args.xaxisrange[1])
		yaxis = multi_graph.GetYaxis()
		if 9999 not in args.yaxisrange:
			yaxis.SetRangeUser(args.yaxisrange[0], args.yaxisrange[1]) 
			print 'test'
		yaxis.SetTitle(y_title)
		#if (args.ylog is True):
		legend.Draw()
		if ('test' not in args.output_name):
			print 'Creating /home/lycoris-dev/Documents/'+save_loc+filename_list[0]+'_'+args.output_name
			c1.SaveAs('/home/lycoris-dev/Documents/'+save_loc+filename_list[0]+'_'+args.output_name)
		else:
			print 'Creating /home/lycoris-dev/Documents/'+save_loc+filename_list[0]+'_'+graph.GetName()+'.png'
			c1.SaveAs('/home/lycoris-dev/Documents/'+save_loc+filename_list[0]+'_'+graph.GetName()+'.png')
		c1.Close()
	else:
		counter = 0
		for graph in graph_list:
			##------------------
			##loop through the histograms, get all parameters and adjust the xrange
			#ROOT.gROOT.SetBatch(1)
			c1 = ROOT.TCanvas( 'test', 'Test', 1200,900 ) #
			obj = graph.ReadObj()
			#print 'Number of entries =', obj.GetEntries()
			x_axis = obj.GetXaxis()
			y_axis = obj.GetYaxis()
			c1.cd()
			#print obj.GetEntries()
			if 9999 not in args.xaxisrange:
				x_axis.SetRange(args.xaxisrange[0], args.xaxisrange[1])
			obj.SetLineColor(4) #Blue
			##------------------
			##set y axis to log
			if args.ylog:
				c1.SetLogy()
				ROOT.gPad.SetLogy()
			##------------------
			##set axis titles if chosen through options.
			if (args.xtitle):
				x_axis.SetTitle(args.xtitle)
			if (args.ytitle):
				y_axis.SetTitle(args.ytitle)
			obj.Draw(args.draw_option)
			#ROOT.gROOT.SetBatch(0)
			#raw_input('Press Enter to look at the next histogram')
			if (args.output_name):
				print 'Creating /home/lycoris-dev/Documents/'+save_loc+filename_list[0]+'_'+args.output_name
				c1.SaveAs('/home/lycoris-dev/Documents/'+save_loc+filename_list[0]+'_'+args.output_name)
			else:
				print 'Creating /home/lycoris-dev/Documents/'+save_loc+filename_list[0]+'_'+graph.GetName()+'.png'
				c1.SaveAs('/home/lycoris-dev/Documents/'+save_loc+filename_list[0]+'_'+graph.GetName()+'.png')
			c1.Close()
			counter= counter+1
			#for i in xrange(obj.FindFirstBinAbove(0),obj.FindLastBinAbove(0),1):
			#'	print obj.GetBinContent(i)


mystyle = ROOT.TStyle("mystyle", "My Style")

#set the background color to white
mystyle.SetFillColor(10)
mystyle.SetFrameFillColor(10)
mystyle.SetCanvasColor(10)
mystyle.SetPadColor(10)
mystyle.SetTitleFillColor(0)
mystyle.SetStatColor(10)

#dont put a colored frame around the plots
mystyle.SetFrameBorderMode(0)
mystyle.SetCanvasBorderMode(0)
mystyle.SetPadBorderMode(0)
mystyle.SetLegendBorderSize(0)
#
##use the primary color palette
##mystyle.SetPalette(1,0)
#
##set the default line color for a histogram to be black
#mystyle.SetHistLineColor(1)
#
##set the default line color for a fit function to be red
#mystyle.SetFuncColor(2)
#
##make the axis labels black
#mystyle.SetLabelColor(1,"xyz")
#
##set the default title color to be black
#mystyle.SetTitleColor(1)
#
##set the margins
##mystyle.SetPadBottomMargin(0.18)
##mystyle.SetPadTopMargin(0.08)
##mystyle.SetPadRightMargin(0.08)
##mystyle.SetPadLeftMargin(0.17)
#
##set axis label and title text sizes
#mystyle.SetLabelFont(42,"xyz")
#mystyle.SetLabelSize(0.06,"xyz")
#mystyle.SetLabelOffset(0.015,"xyz")
#mystyle.SetTitleFont(42,"xyz")
mystyle.SetTitleSize(0.04,"xyz")
mystyle.SetTitleOffset(1.2,"yz")
#mystyle.SetTitleOffset(1.0,"x")
#mystyle.SetStatFont(42)
#mystyle.SetStatFontSize(0.07)
#mystyle.SetTitleBorderSize(0)
#mystyle.SetStatBorderSize(0)
#mystyle.SetTextFont(42)

##set legend text size etc.
mystyle.SetLegendTextSize(0.03)
#
##set line widths
mystyle.SetFrameLineWidth(2)
mystyle.SetFuncWidth(2)
mystyle.SetHistLineWidth(2)
#
##set the number of divisions to show
#mystyle.SetNdivisions(506, "xy")
#
##turn off xy grids
#mystyle.SetPadGridX(0)
#mystyle.SetPadGridY(0)
#
##set the tick mark style
#mystyle.SetPadTickX(1)
#mystyle.SetPadTickY(1)
#
##turn off stats
mystyle.SetOptStat(0)
#mystyle.SetOptFit(0)
#
##marker settings
mystyle.SetMarkerStyle(20)
mystyle.SetMarkerSize(0.7)
mystyle.SetLineWidth(2) 

#done
mystyle.cd()
ROOT.gROOT.ForceStyle()
ROOT.gStyle.ls()







#parser = argparse.ArgumentParser() #Command line argument parser.
parser = MyParser()
parser.add_argument('file_in', nargs='+', help='name of the input file')
parser.add_argument('-n', '--name', dest='name', default=['everything'], nargs='*',  help='used to specify the name of the plot which should be used')
parser.add_argument('-c', '--channel', dest='channel', default=['9999'], nargs='*', help='used to specify the channel of the plot which should be used')
parser.add_argument('-s', '--strip', dest='strip', default=['9999'], nargs='*', help='used to specify the strip of the plot which should be used')
parser.add_argument('-b', '--bucket', dest='bucket', default=[9999], nargs='*', type=int, help='used to specify the bucket of the plot which should be used | type=int')
parser.add_argument('-k', '--kpix', dest='kpix', default=[9999], nargs='*', type=int, help='used to specify the bucket of the plot which should be used | type=int')
parser.add_argument('-d', '--draw', dest='draw_option', default='', help='specify the drawing option as given by the root draw option, needs to be given as a single string (e.g. hist same or hist same multifile')
parser.add_argument('-o', '--output', dest='output_name', help='specifies the name and type of the output file (e.g. test.png, comparison.root etc...')
parser.add_argument('-f', '--refuse', dest='refuse', default= ['nothing'], nargs='*', help='add string that should be exluded from histogram search')
parser.add_argument('-r', '--rebin', dest='rebin', default=1, type = int, help='add number to rebin the histograms | type=int')
parser.add_argument('--exact', dest='exact', default=False, help='if set to True, only histograms with the exact name will be used')
parser.add_argument('--xrange', dest='xaxisrange', default=[9999], nargs='*', type=float, help='set a xrange for the plot to used with xmin xmax as the two arguments | type=float')
parser.add_argument('--yrange', dest='yaxisrange', default=[9999], nargs='*', type=float, help='set a yrange for the plot to used with ymin ymax as the two arguments | type=float')
parser.add_argument('--legend', dest='legend', nargs='*', help='list of names to be used as legend titles instead of the default filename+histogram name')
parser.add_argument('--ylog', dest='ylog', help='if given as an option, set y axis to logarithmic. Remember to set the yrange to start above 0!')
parser.add_argument('--color', dest='color', default=[60, 632, 416, 1, 432, 402, 880, 860, 900, 800, 840], nargs='*', type=int, help='list of colors to be used | type=int')
parser.add_argument('--xtitle', dest='xtitle', help='choose the name of the x axis title')
parser.add_argument('--ytitle', dest='ytitle', help='choose the name of the y axis title')
parser.add_argument('--order', dest='order', nargs='+', type=int,  help='choose the order of plotting with same (to ensure no histograms overlap)')

args = parser.parse_args()
if len(sys.argv) < 2:
	print parser.print_help()
	sys.exit(1)
print ''




legend_location = [0.15,0.75,0.3,0.85]

##-----------------	
##produce empty root file and filename lists.

root_file_list = []
filename_list = []
##-----------------
##loop through all given files and add them to the list. then loop through the keys for every file..
for root_file in args.file_in:
	root_file_list.append(ROOT.TFile(root_file))
	filename_list.append(root_file[root_file.find('/20')+1:root_file.rfind('.bin')])

for x in root_file_list:
	key_root = x.GetListOfKeys()
	loopdir(key_root)

save_loc = 'testbeam_august/'
##-----------------	
##general output
#print args.color


print 'Looking for histograms/graphs'
print '----------------------'
print 'Name contains ', args.name
print 'Channel [9999 = everything] ',args.channel
print 'Strip [9999 = everything] ',args.strip
print 'Bucket [9999 = everything; 4 = only total] ',args.bucket
print 'KPiX [9999 = everything] ',args.kpix

print 'Number of histograms found is: ', len(hist_list)
print hist_list	
print 'Number of graphs found is: ', len(graph_list)
print graph_list	
if (args.ylog and args.yaxisrange[0] is 0):
	print 'Setting y axis to log, only works if the range was specified to start at y_min > 0'
##------------------
##start of the plotting.


if (len(hist_list) is not 0):
	hist_plotter()
elif (len(graph_list) is not 0):
	graph_plotter()
else:
	print 'There are NO valid histograms/graphs in the current selection'
	print ''
	print ''
for x in root_file_list:
	ROOT.gROOT.GetListOfFiles().Remove(x)



#raw_input('Press Enter to look at the next histogram')



