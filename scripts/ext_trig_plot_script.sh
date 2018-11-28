#!/bin/bash

data_files_normal_gain_320ns=(/scratch/data/tracker_test/2018_08_19_21_34_17.bin_external.root)

for i in ${data_files_normal_gain_320ns[*]}
do
	python python/plot_producer.py $i -n fc_response_ subtracted -k 26 -b 0 -d "h e same" -o "fc_subtraction_tracker_new2_all.png" --xrange -10 15 --ylog true --legend mean CM_mean CM_median CM_gauss median gauss --refuse _c
	python python/plot_producer.py $i -n fc_response_ subtracted -k 26 -b 0 -d "h e same" -o "fc_subtraction_tracker_new2_all_noylog.png" --xrange -10 15 --legend mean CM_mean CM_median CM_gauss median gauss --refuse _c
	python python/plot_producer.py $i -n fc_response_ subtracted -k 30 -b 0 -d "h e same" -o "fc_subtraction_ECAL_new2_all.png" --xrange -10 15 --ylog true --legend mean CM_mean CM_median CM_gauss median gauss --refuse _c
	python python/plot_producer.py $i -n fc_response_ subtracted -k 30 -b 0 -d "h e same" -o "fc_subtraction_ECAL_new2_all_noylog.png" --xrange -10 15 --legend mean CM_mean CM_median CM_gauss median gauss --refuse _c
	python python/plot_producer.py $i -n fc_response_ gauss subtracted -k 30 -b 0 -d "h e same" -o "fc_subtraction_ECAL_gauss_new2.png" --xrange -10 15 --legend CM_gauss gauss --refuse _c --ylog true
	python python/plot_producer.py $i -n fc_response_ gauss subtracted -k 26 -b 0 -d "h e same" -o "fc_subtraction_tracker_gauss_new2.png" --xrange -10 15 --legend CM_gauss gauss --refuse _c --ylog true
	python python/plot_producer.py $i -n fc_response_ median subtracted -k 30 -b 0 -d "h e same" -o "fc_subtraction_ECAL_median_new2.png" --xrange -10 15 --legend CM_median median --refuse _c --ylog true
	python python/plot_producer.py $i -n fc_response_ median subtracted -k 26 -b 0 -d "h e same" -o "fc_subtraction_tracker_median_new2.png" --xrange -10 15 --legend CM_median median --refuse _c --ylog true
	python python/plot_producer.py $i -n fc_response_ subtracted -k 30 -b 0 -d "h e same" -o "fc_subtraction_ECAL_mean_new2.png" --xrange -10 15 --legend CM_mean mean --refuse _c median gauss --ylog true
	python python/plot_producer.py $i -n fc_response_ subtracted -k 26 -b 0 -d "h e same" -o "fc_subtraction_tracker_mean_new2.png" --xrange -10 15 --legend CM_mean mean --refuse _c median gauss --ylog true
done

#python python/plot_producer.py /scratch/data/tracker_test/2018_06_01_15_00_35.bin.root /scratch/data/tracker_test/2018_06_01_15_18_30.bin.root -n Channel_entries_k_ -d 'hist e same' -b 4 -o 'block_comparison_zoom.png' -f no --xrange 750 850 --legend all_enabled_2018_06_01_15_00_35 304_disabled_2018_06_01_15_18_30 --yrange 0 1.5
#python python/plot_producer.py /scratch/data/tracker_test/2018_06_04_17_04_45.bin.root /scratch/data/tracker_test/2018_06_04_17_03_25.bin.root -n Channel_entries_k_ -d 'hist e same' -b 4 -o 'block_comparison_zoom.png' -f no --xrange 750 850 --legend all_enabled_2018_06_04 304_disabled_2018_06_04 --yrange 0 1.5

#python python/plot_producer.py /scratch/data/tracker_test/2018_06_05_16_55_46.bin /scratch/data/tracker_test/2018_06_05_16_57_05.bin /scratch/data/tracker_test/2018_06_05_16_58_18.bin /scratch/data/tracker_test/2018_06_05_16_59_45.bin /scratch/data/tracker_test/2018_06_05_17_01_28.bin /scratch/data/tracker_test/2018_06_05_17_02_53.bin -n Channel_entries_k_ -d 'hist e same' -b 4 -o 'block_comparison.png' -f no 

#python python/plot_producer.py  /scratch/data/tracker_test/2018_06_04_17_22_01.bin$calib_extension /scratch/data/tracker_test/2018_06_04_17_56_49.bin$calib_extension -n RMSfc  -d 'P same'
#python python/plot_producer.py /scratch/data/tracker_test/2018_06_04_17_03_25.bin.root /scratch/data/tracker_test/2018_06_04_17_04_45.bin.root /scratch/data/tracker_test/2018_06_05_16_05_38.bin.root /scratch/data/tracker_test/2018_06_05_16_13_58.bin.root /scratch/data/tracker_test/2018_06_05_16_26_07.bin.root /scratch/data/tracker_test/2018_06_05_16_39_15.bin.root /scratch/data/tracker_test/2018_06_05_16_54_03.bin.root -n Channel_entries_k_ -d 'hist same' -o 'board_comparison.png' -f no --yrange 0 4


#source_files=(/scratch/data/tracker_test/2018_05_07_16_49_42.bin) #/scratch/data/tracker_test/2018_04_19_16_28_33.bin /scratch/data/tracker_test/2018_04_19_18_01_13.bin /scratch/data/tracker_test/2018_04_30_16_42_27.bin)
#for i in ${source_files[*]}
#do
	#./bin/analysis $i
	#python python/plot_producer.py $i$analysis_extension -n entr  -d 'hist'
	#python python/plot_producer.py $i$analysis_extension -n timesta -d 'hist'
#done



