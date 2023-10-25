cutname=(_M_4jets)
years=(2015 2016 2017 2018)
path=(mtop3_only_shape)
rm -rf sys_pdf/
mkdir sys_pdf/
for c in ${path[*]}
do
    mkdir sys_pdf/$c
    for b in ${years[*]}
    do
        for a in ${cutname[*]}
        do
            root -l -q -b draw_sys.cpp"(\"$a\", $b, \"$c\")"
        done
    done
done