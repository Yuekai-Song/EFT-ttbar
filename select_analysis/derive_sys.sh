cut=(0)
year=(2015)
var=(8)
rm -rf run_log/
mkdir run_log/
rm -rf sys_root
mkdir sys_root
for c in ${year[*]}
do
    mkdir sys_root/$c
    for a in ${var[*]}
    do
        for b in ${cut[*]}
        do
            nohup root -l -q -b derive_sys.cpp"($b, $c, $a, 0)" > run_log/derive_sys_${b}_${c}_${a}.txt 2>&1 &
        done
    done
done
