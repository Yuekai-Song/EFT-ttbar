#!/bin/bash
#voms-proxy-init --voms cms -valid 192:00 -out ~/temp/x509up
# sumbit asssinment: condor_submit condor.sub
#source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.18.04/x86_64-centos7-gcc48-opt/bin/thisroot.sh
mkdir -p myout
output=$PWD/myout
echo "output: $output"
wrong="f"
cd /afs/cern.ch/user/y/yuekai/EFT-ttbar/select/2016/condor_out_MC/$1
file=$(ls ${1}.txt)
dir_f=$(cat $file)
#dir="root://cms-xrd-global.cern.ch/"$dir
dir="root://xrootd-cms.infn.it/"$dir_f
eos="/eos/user/y/yuekai/ttbar/select/2016/MC"
inputFile=${file%.txt*}
inputFile=${inputFile}.root
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
mv $output/out*.txt /afs/cern.ch/user/y/yuekai/EFT-ttbar/select/2016/condor_out_MC/$1
if [[ $wrong == "f" ]]
then
    echo "input file: $dir"
    ofile=$(ls $output/*.root)
    cd /afs/cern.ch/user/y/yuekai/ttbar/CMSSW_10_6_19_patch2/src/PhysicsTools/NanoAODTools/crab
    eval `scramv1 runtime -sh`
    python crab_script_16.py $ofile $output
    cd /afs/cern.ch/user/y/yuekai/EFT-ttbar/select
    input=$(ls $output|grep Skim)
    root -l -q -b ./process.cpp"(\"$output\",\"$inputFile\",\"$output/$input\",2016,1)"
    cd /afs/cern.ch/user/y/yuekai/EFT-ttbar/scale_factor/code
    for outputFile in $(ls $output/new*.root)
    do
        root -l -q -b ./SF_add.cpp"(\"$outputFile\",0,2016)"
        #root -l -q -b ../../../EW_weight/add_weight_branch.c"(\"$outputFile\")"
    done
    if [[ $inputFile =~ "TTTo" ]]
    then
        cd /afs/cern.ch/user/y/yuekai/EFT-ttbar/nnlo
        for outputFile in $(ls $output/new*.root)
        do
            root -l -q -b ./nnlo_add.cpp"(\"$outputFile\",0,2016)"
        done
        cd /afs/cern.ch/user/y/yuekai/EFT-ttbar/EW_weight
        for outputFile in $(ls $output/new*.root)
        do
            root -l -q -b ./add_weight_branch.cpp"(\"$outputFile\",0,2016)"
        done
        cd /afs/cern.ch/user/y/yuekai/EFT-ttbar/EW_un
        for outputFile in $(ls $output/new*.root)
        do
            root -l -q -b ./un_weight_add.cpp"(\"$outputFile\")"
        done
    fi
    cd /afs/cern.ch/user/y/yuekai/EFT-ttbar/pileup
    for outputFile in $(ls $output/new*.root)
    do
        root -l -q -b add_pu.cpp"(\"$outputFile\",2016,0)"
    done
    num=$(ls $output|grep new|wc -l)
    if [ $num -eq 1 ]
    then
        mv $(ls $output/new*.root)  ${eos}
    else
	    echo "Failed. task unfinished"
    fi
fi
rm -rf $output
echo "root files are storied in $eos"