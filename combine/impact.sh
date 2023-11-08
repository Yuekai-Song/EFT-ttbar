#!/bin/bash
cd /afs/cern.ch/user/y/yuekai/tth_cms/CMSSW_11_3_4/src/
eval `scramv1 runtime -sh`

cd $1
output=impact_z
rm -rf $output
mkdir $output

rm -f ttbar.txt
combineCards.py ttbar_*_*_*.txt > ttbar.txt

text2workspace.py ttbar.txt -o workspace_ttbar.root  --PO donorm -P HiggsAnalysis.CombinedLimit.stagex_ttwc:stagex_ttwc -m 125 --X-allow-no-background  -v 7 > out

combineTool.py -M Impacts -d ./workspace_ttbar.root -m 125 -t -1 --robustFit 1 --doInitialFit -v 7  --redefineSignalPOIs z --freezeParameters y,k &> initial.txt

combineTool.py -M Impacts -d ./workspace_ttbar.root -m 125 -t -1 --robustFit 1 --doFits -v 7  --redefineSignalPOIs z --freezeParameters y,k  &> dofit.txt

combineTool.py -M Impacts -d ./workspace_ttbar.root -m 125 -t -1 --robustFit 1 --redefineSignalPOIs z --exclude y,k --output impacts.json &> pjson.txt

plotImpacts.py -i impacts.json -o impacts #--P z


mv ttbar.txt $output
mv combine* $output
mv workspace* $output
mv *Fit* $output
mv initial.txt $output
mv dofit.txt $output
mv pjson.txt $output
mv impacts* $output
mv out $output