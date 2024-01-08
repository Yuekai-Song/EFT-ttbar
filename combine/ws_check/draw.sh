cd ~/cmssw/CMSSW_11_3_4/
eval `scramv1 runtime -sh`
cd $1
rm -rf free_params
rm -rf nuis
mkdir free_params
mkdir nuis

cd ~/EFT-ttbar/combine/ws_check/
root -l -q -b draw_nuis.cpp"(\"$1\")"
root -l -q -b draw_fp.cpp"(\"$1\")"