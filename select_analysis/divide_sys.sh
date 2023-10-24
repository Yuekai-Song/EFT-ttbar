cutname=(_M_4jets _M_3jets _E_4jets _E_3jets)
years=(2015 2016 2017 2018)
path=(smooth)
rm -rf sys_pdf/
mkdir sys_pdf/
for c in ${path[*]}
do
    mkdir sys_pdf/$c
    for b in ${years[*]}
    do
        for a in ${cutname[*]}
        do
            cd ./draw
            root -l -q -b draw_sys.cpp"(\"$a\", $b, \"$c\")"
            cd ../
        done
    done
done    