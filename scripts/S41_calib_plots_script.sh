#!/bin/bash
calib_files20190402_normal_gain=(/scratch/data/2019_04_02_16_45_04.bin /scratch/data/2019_04_02_17_09_58.bin)

calib_files20190403_normal_gain=(/scratch/data/2019_04_03_14_20_27.bin /scratch/data/2019_04_03_14_44_39.bin)
calib_files20190403_high_gain=(/scratch/data/2019_04_03_15_11_44.bin /scratch/data/2019_04_03_15_42_04.bin)


calib_files20190404_normal_gain=(/scratch/data/2019_04_04_14_37_03.bin /scratch/data/2019_04_04_15_13_41.bin) 
calib_files20190404_normal_gain512=(/scratch/data/2019_04_04_15_01_06.bin /scratch/data/2019_04_04_15_37_00.bin)



calib_extension=.newCalib.root

#same=""

#for i in ${calib_files20190402_normal_gain[*]}
#do
	#substring=$(echo $i| cut -d'/' -f 5 | cut -d '.' -f 1)
	#same="$same $i$calib_extension"
#done

#python python/plot_producer.py $same -n pedestals_fc_k -b 0 -d 'hist e same' --legend S41-2 S41-1 -o S41_pedestal_comparison --rebin 8
#python python/plot_producer.py $same -n slope_k -b 0  -d 'hist e same'  --legend S41-2 S41-1 -o S41_slope_comparison
#python python/plot_producer.py $same -n slope_0_127_k -b 0  -d 'hist e same'   --legend S41-2 S41-1 -o S41_slope_0_127_comparison
#python python/plot_producer.py $same -n RMS_fc_k -b 0  -d 'hist e same'  --legend S41-2 S41-1 -o S41_RMSfc_comparison --rebin 8	
#python python/plot_producer.py $same -n RMS_fc_0_127_k -b 0  -d 'hist e same'  --legend S41-2 S41-1 -o S41_RMSfc_0_127_comparison --rebin 8
#python python/plot_producer.py $same -n slope_vs_channel_k  -b 0 -d 'hist e same'  --legend S41-2 S41-1 -o S41_slope_vs_channel_comparison	
#python python/plot_producer.py $same -n slope_vs_right_strip_k  -b 0 -d 'hist e same'  --legend S41-2 S41-1 -o S41_slope_vs_right_strip_comparison	

#same=""

#for i in ${calib_files20190403_normal_gain[*]}
#do
	#substring=$(echo $i| cut -d'/' -f 5 | cut -d '.' -f 1)
	#same="$same $i$calib_extension"
#done

#python python/plot_producer.py $same -n pedestals_fc_k -b 0 -d 'hist e same' --legend S41-2 S41-1 -o S41_pedestal_comparison --rebin 8
#python python/plot_producer.py $same -n slope_k -b 0  -d 'hist e same'  --legend S41-2 S41-1 -o S41_slope_comparison
#python python/plot_producer.py $same -n RMS_fc_k -b 0  -d 'hist e same'  --legend S41-2 S41-1 -o S41_RMSfc_comparison --rebin 8	
#python python/plot_producer.py $same -n slope_vs_channel_k  -b 0 -d 'hist e same'  --legend S41-2 S41-1 -o S41_slope_vs_channel_comparison	

#same=""

#for i in ${calib_files20190403_high_gain[*]}
#do
	#substring=$(echo $i| cut -d'/' -f 5 | cut -d '.' -f 1)
	#same="$same $i$calib_extension"
#done

#python python/plot_producer.py $same -n pedestals_fc_k -b 0 -d 'hist e same' --legend S41-2 S41-1 -o S41_pedestal_comparison --rebin 8
#python python/plot_producer.py $same -n slope_k -b 0  -d 'hist e same'  --legend S41-2 S41-1 -o S41_slope_comparison
#python python/plot_producer.py $same -n RMS_fc_k -b 0  -d 'hist e same'  --legend S41-2 S41-1 -o S41_RMSfc_comparison --rebin 8	
#python python/plot_producer.py $same -n slope_vs_channel_k  -b 0 -d 'hist e same'  --legend S41-2 S41-1 -o S41_slope_vs_channel_comparison	

same=""

for i in ${calib_files20190404_normal_gain[*]}
do
	substring=$(echo $i| cut -d'/' -f 5 | cut -d '.' -f 1)
	same="$same $i$calib_extension"
done

python python/plot_producer.py $same -n pedestals_fc_k -b 0 -d 'hist e same' --legend S41-2 S41-1 -o S41_pedestal_comparison --rebin 8
python python/plot_producer.py $same -n slope_k -b 0  -d 'hist e same'  --legend S41-2 S41-1 -o S41_slope_comparison
python python/plot_producer.py $same -n RMS_fc_k -b 0  -d 'hist e same'  --legend S41-2 S41-1 -o S41_RMSfc_comparison --rebin 8	
python python/plot_producer.py $same -n slope_vs_channel_k  -b 0 -d 'hist e same'  --legend S41-2 S41-1 -o S41_slope_vs_channel_comparison	

same=""

for i in ${calib_files20190404_normal_gain512[*]}
do
	substring=$(echo $i| cut -d'/' -f 5 | cut -d '.' -f 1)
	same="$same $i$calib_extension"
done

python python/plot_producer.py $same -n pedestals_fc_k -b 0 -d 'hist e same' --legend S41-2 S41-1 -o S41_pedestal_comparison --rebin 8
python python/plot_producer.py $same -n slope_k -b 0  -d 'hist e same'  --legend S41-2 S41-1 -o S41_slope_comparison
python python/plot_producer.py $same -n RMS_fc_k -b 0  -d 'hist e same'  --legend S41-2 S41-1 -o S41_RMSfc_comparison --rebin 8	
python python/plot_producer.py $same -n slope_vs_channel_k  -b 0 -d 'hist e same'  --legend S41-2 S41-1 -o S41_slope_vs_channel_comparison	
