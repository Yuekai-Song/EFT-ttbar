# !/bin/bash
cates=("E_3jets" "M_3jets" "E_4jets" "M_4jets")
years=("2015" "2016" "2017" "2018")
datacard=$1
cd $datacard
for cate in ${cates[*]}; do
    rm -rf $cate/
    mkdir $cate/
    cp *${cate}*.txt $cate/
done
for year in ${years[*]}; do
    rm -rf $year/
    mkdir $year/
    cp *${year}*.txt $year/
done
for cate in ${cates[*]}; do
    for year in ${years[*]}; do
        rm -rf ${cate}_${year}/
        mkdir ${cate}_${year}/
        cp *${cate}_${year}*.txt ${cate}_${year}/
    done
done

rm -rf combined/
mkdir combined/
mv *txt combined/
rm -f condor_list.txt

for channel in $(ls . | grep -v .txt); do
    echo $PWD/$channel >>condor_list.txt
done
