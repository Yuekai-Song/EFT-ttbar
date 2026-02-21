#!/bin/bash
#voms-proxy-init --voms cms -valid 192:00 -out ~/temp/x509up
# sumbit asssinment: condor_submit condor.sub
#source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.${2}.04/x86_64-centos7-gcc48-opt/bin/thisroot.sh
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /grid_mnt/data__data.polcms/cms/song/CMSSW_14_1_0_pre4/src/
eval `scramv1 runtime -sh`
cd /grid_mnt/data__data.polcms/cms/song/CMSSW_14_1_0_pre4/src/ttbar/select/20${2}/condor_out_ttjets/$1
file=$(ls ${1}.txt)
dir_f=$(cat $file)
#dir="root://cms-xrd-global.cern.ch/"$dir
dir="root://xrootd-cms.infn.it/"$dir_f

inputFile=${file%.txt*}

echo "input file: $dir"

cd /grid_mnt/data__data.polcms/cms/song/CMSSW_14_1_0_pre4/src/ttbar/select
root -l -q -b ./process_test.cpp"(\"./output\", \"$inputFile\", \"$dir\", 20${2})"