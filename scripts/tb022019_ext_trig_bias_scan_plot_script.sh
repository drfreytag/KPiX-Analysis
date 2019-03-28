#!/bin/bash
large_file_70v=/scratch/data/testbeam201902/2019_02_13_15_37_39.bin
bias_scan_files=(/scratch/data/testbeam201902/2019_02_13_13_46_28.bin /scratch/data/testbeam201902/2019_02_13_13_50_24.bin /scratch/data/testbeam201902/2019_02_13_13_57_50.bin /scratch/data/testbeam201902/2019_02_13_14_01_29.bin /scratch/data/testbeam201902/2019_02_13_14_09_01.bin /scratch/data/testbeam201902/2019_02_13_14_13_53.bin /scratch/data/testbeam201902/2019_02_13_14_26_12.bin /scratch/data/testbeam201902/2019_02_13_14_30_39.bin /scratch/data/testbeam201902/2019_02_13_14_40_41.bin /scratch/data/testbeam201902/2019_02_13_14_45_36.bin /scratch/data/testbeam201902/2019_02_13_14_55_07.bin /scratch/data/testbeam201902/2019_02_13_14_57_20.bin /scratch/data/testbeam201902/2019_02_13_15_15_52.bin /scratch/data/testbeam201902/2019_02_13_15_26_42.bin /scratch/data/testbeam201902/2019_02_13_15_31_42.bin) 


extension=.pedestal.external.root
same=""

for i in ${bias_scan_files[*]}
do
	substring=$(echo $i| cut -d'/' -f 5 | cut -d '.' -f 1)
	same="$same $i"_"$substring$extension"
done

python2.7 python/plot_producer.py $same -n cluster_size k17 -b0  -d 'hist e same' --refuse 28 30 --legend 70V 80V 90V 100V 110V 120V 130V 140V 150V 160V 170V 180V 190V 200V -o cluster_size_k17_bias_scan
python2.7 python/plot_producer.py $same -n cluster_size k19 -b0  -d 'hist e same' --refuse 28 30 --legend 70V 80V 90V 100V 110V 120V 130V 140V 150V 160V 170V 180V 190V 200V -o cluster_size_k19_bias_scan
