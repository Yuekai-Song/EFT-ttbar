WAIT_FOR_PIDS="";
cut=(0 1 2 3)
cut_names=("E_3jets" "E_4jets" "M_3jets" "M_4jets")
year=(2015 2016 2017 2018)
cg=(1 2 3)
ens=(0 1)
rm -rf run_log/
mkdir run_log/
for a in ${cut[*]}
do
    for c in ${year[*]}
    do
        for b in ${cg[*]}
        do
            for en in ${ens[*]}
            do
                nohup root -l -q -b derive_qcd_3D.cpp"($a, $b, $c, $en)"  >run_log/qcd_$a$b${c}_$en.txt 2>&1 &
                WAIT_FOR_PIDS="$WAIT_FOR_PIDS $!"
            done
        done
        wait $WAIT_FOR_PIDS
        rm ./output/${c}/QCD_root/QCD_${cut_names[$a]}.root
        hadd ./output/${c}/QCD_root/QCD_${cut_names[$a]}.root ./output/${c}/QCD_root/QCD_${cut_names[$a]}_*.root
        rm ./output/${c}/QCD_root/QCD_${cut_names[$a]}_*.root
    done
done