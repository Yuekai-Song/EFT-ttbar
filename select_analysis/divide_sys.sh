cutname=(M_4jets M_3jets E_3jets E_4jets)
years=(2015 2016 2017 2018)
names=("processed_bg_flat")
rm -rf sys_pdf/
mkdir sys_pdf/
for c in ${names[*]}
do
    mkdir sys_pdf/${c}/
    for b in ${years[*]}
    do
        for a in ${cutname[*]}
        do
            mkdir sys_pdf/${c}/${a}_${b}
            cd ./draw/
            root -l -q -b draw_sys.cpp"(\"$a\", $b, \"$c\")"
            cd ../
        done
    done
done    