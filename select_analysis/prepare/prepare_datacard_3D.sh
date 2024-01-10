cut=(0 1 2 3)
year=(2015 2016 2017 2018)
option=(0 1 2 3 4 5)
rm -rf run_log/
mkdir run_log/
for a in ${cut[*]}
do
    for c in ${year[*]}
    do
        for b in ${option[*]}
        do
            nohup root -l -q -b prepare_datacard_3D.cpp"($a, $c, $b)" > run_log/prepare_$a${c}_$b.txt 2>&1 &
        done
    done
done