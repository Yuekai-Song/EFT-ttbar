cut_names=("_E_3jets" "_E_4jets" "_M_3jets" "_M_4jets")
years=(2015 2016 2017 2018)
datacards=("datacard/" "datacard_ttx/")
for datacard in ${datacards[*]}
do
    rm -rf $datacard
    mkdir $datacard
    mkdir ${datacard}original/
    for cut_name in ${cut_names[*]}
    do
        for year in ${years[*]}
        do
            root -l -q -b convert_3Dto1D.cpp"(\"${datacard}original/\", \"$cut_name\", $year)"
        done
    done
done