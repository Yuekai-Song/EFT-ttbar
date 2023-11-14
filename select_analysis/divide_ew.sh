cutname=(M_4jets M_3jets E_4jets E_3jets)
years=(2015 2016 2017 2018)
rm -rf ew_pdf/
mkdir ew_pdf/
for c in ${cutname[*]}; do
    for b in ${years[*]}; do
        mkdir ew_pdf/${c}_${b}
    done
done
cd ./draw
for ((i = 0; i < 4; i++)); do
    for ((j = 0; j < 4; j++)); do
        root -l -q -b draw_ew.cpp"($i, $j, 0, 0, 0)"
    done
done
cd ../
