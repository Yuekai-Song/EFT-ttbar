#!/bin/bash
#voms-proxy-init --voms cms -valid 192:00 -out ~/temp/x509up
# sumbit asssinment: condor_submit condor.sub
#source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.${2}.04/x86_64-centos7-gcc48-opt/bin/thisroot.sh
mkdir -p myout
output=$PWD/myout
echo "output: $output"
wrong="f"
cd /afs/cern.ch/user/y/yuekai/EFT-ttbar/QCD_ES/20${2}/condor_out_MC/$1
file=$(ls ${1}.txt)
dir_f=$(cat $file)
#dir="root://cms-xrd-global.cern.ch/"$dir
dir="root://xrootd-cms.infn.it/"$dir_f
eos="/eos/user/y/yuekai/ttbar/QCD_ES/20${2}/MC"
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
mv $output/out*.txt /afs/cern.ch/user/y/yuekai/EFT-ttbar/QCD_ES/20${2}/condor_out_MC/$1
if [[ $wrong == "f" ]]
then
    echo "input file: $dir"
    ofile=$(ls $output/*.root)
    cd /afs/cern.ch/user/y/yuekai/cmssw/CMSSW_13_3_0/src/PhysicsTools/mytools/scripts/jme
    eval `scramv1 runtime -sh`
    source jme.sh $output $ofile MC $2
    cd /afs/cern.ch/user/y/yuekai/EFT-ttbar/QCD_ES
    input=$(ls $output|grep Skim)
    if [[ $inputFile =~ "QCD" ]];then
        root -l -q -b ./process.cpp"(\"$output\",\"$inputFile\",\"$output/$input\",20${2},1,0,0)"
        # root -l -q -b ./process.cpp"(\"$output\",\"$inputFile\",\"$output/$input\",20${2},1,1,0)"
    fi
    root -l -q -b ./process.cpp"(\"$output\",\"$inputFile\",\"$output/$input\",20${2},1,0,1)"
    root -l -q -b ./process.cpp"(\"$output\",\"$inputFile\",\"$output/$input\",20${2},1,0,2)"
    root -l -q -b ./process.cpp"(\"$output\",\"$inputFile\",\"$output/$input\",20${2},1,0,3)"
    # root -l -q -b ./process.cpp"(\"$output\",\"$inputFile\",\"$output/$input\",20${2},1,1,1)"
    # root -l -q -b ./process.cpp"(\"$output\",\"$inputFile\",\"$output/$input\",20${2},1,1,2)"
    # root -l -q -b ./process.cpp"(\"$output\",\"$inputFile\",\"$output/$input\",20${2},1,1,3)"
    cd /afs/cern.ch/user/y/yuekai/EFT-ttbar/scale_factor/code
    for outputFile in $(ls $output/new*.root)
    do
        cg=$(echo "$outputFile" | sed -n 's/.*_\([A-Z]\)\.root/\1/p')
        root -l -q -b ./SF_add.cpp"(\"$outputFile\",1,20${2},\"$cg\")"
        #root -l -q -b ../../../EW_weight/add_weight_branch.c"(\"$outputFile\")"
    done
    if [[ $inputFile =~ "TTTo" ]]
    then
        cd /afs/cern.ch/user/y/yuekai/EFT-ttbar/nnlo
        for outputFile in $(ls $output/new*.root)
        do
            root -l -q -b ./nnlo_add.cpp"(\"$outputFile\",1,20${2})"
        done
        cd /afs/cern.ch/user/y/yuekai/EFT-ttbar/EWweight
        for outputFile in $(ls $output/new*.root)
        do
            root -l -q -b ./add_weight_branch.cpp"(\"$outputFile\",1,20${2})"
        done
    fi
    cd /afs/cern.ch/user/y/yuekai/EFT-ttbar/pileup
    for outputFile in $(ls $output/new*.root)
    do
        root -l -q -b add_pu.cpp"(\"$outputFile\",20${2},1)"
    done
    num=$(ls $output|grep new|wc -l)
    if [ $num -gt 0 ]
    then
        mv $(ls $output/new*.root) $eos
    else
	    echo "Failed. task unfinished"
    fi
fi
rm -rf $output
echo "root files are storied in $eos"
