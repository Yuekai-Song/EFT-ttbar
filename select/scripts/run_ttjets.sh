#!/bin/bash
#voms-proxy-init --voms cms -valid 192:00 -out ~/temp/x509up
# sumbit asssinment: condor_submit condor.sub
#source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.${2}.04/x86_64-centos7-gcc48-opt/bin/thisroot.sh
mkdir -p myout
output=$PWD/myout
echo "output: $output"
wrong="f"

source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /grid_mnt/data__data.polcms/cms/song/CMSSW_14_1_0_pre4/src/
eval `scramv1 runtime -sh`
cd /grid_mnt/data__data.polcms/cms/song/CMSSW_14_1_0_pre4/src/ttbar/select/20${2}/condor_out_ttjets/$1
file=$(ls ${1}.txt)
dir_f=$(cat $file)

#dir="root://cms-xrd-global.cern.ch/"$dir
dir="root://xrootd-cms.infn.it/"$dir_f

inputFile=${file%.txt*}

echo $dir >$output/out1.txt
xrdcp $dir $output 2>>$output/out1.txt
#python crab_script.py $dir $output 2>>$output/out1.txt
jug=`cat $output/out1.txt|grep "ERROR"`
n=$(ls $output|grep root|wc -l)
if [[ $n -ne 1 || $jug != "" ]]
then
    rm -f $output/*.root 
    dir="root://cms-xrd-global.cern.ch/"$dir_f
    echo $dir >$output/out2.txt
    xrdcp $dir $output 2>>$output/out2.txt
    #python crab_script.py $dir $output 2>>$output/out2.txt
    jug=`cat $output/out2.txt|grep "ERROR"`
    n=$(ls $output|grep root|wc -l)
    if [[ $n -ne 1 || $jug != "" ]]
    then
        rm -f $output/*.root
        dir="root://cmsxrootd.fnal.gov/"$dir_f
        echo $dir >$output/out3.txt
        xrdcp $dir $output 2>>$output/out3.txt
        #python crab_script.py $dir $output 2>>$output/out3.txt
        jug=`cat $output/out3.txt|grep "ERROR"`
        n=$(ls $output|grep root|wc -l)
        if [[ $n -ne 1 || $jug != "" ]]
        then
            wrong="t"
        fi
    fi
fi
mv $output/out*.txt /grid_mnt/data__data.polcms/cms/song/CMSSW_14_1_0_pre4/src/ttbar/select/20${2}/condor_out_ttjets/$1
if [[ $wrong == "f" ]]
then
    echo "input file: $dir"
    ofile=$(ls $output/*.root)
    cd /grid_mnt/data__data.polcms/cms/song/CMSSW_14_1_0_pre4/src/ttbar/select
    root -l -q -b ./process_test.cpp"(\"./output\", \"$inputFile\", \"$ofile\", 20${2})"
fi
rm -rf $output