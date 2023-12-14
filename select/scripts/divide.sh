#!bin/bash
#writen by Yuekai Song in Jul 21,2023
# to write dataset files into .txt and divide
type_name=$1
cd ../20$2 || exit 1
input=${type_name}.txt
i=0
for dataset in $(cat $input); do
    echo $dataset
    var="Chunk$i"
    rm -rf $var
    mkdir $var
    cd $var
    if [[ $dataset =~ "-pythia" ]]; then
        temp=${dataset%%-pythia8*}
    else
        temp=${dataset/\/Run/_Run}
        temp=${temp%%-UL*}
    fi
    process=${temp:1}
    #dasgoclient --query "file dataset=$dataset" > ${process}.txt
    dasgoclient -query="file dataset=$dataset" >${process}.txt
    cd ../
    let i=i+1
done

echo "divide each dataset into several pieces "
max=0
for var in $(ls | grep Chunk); do
    cd $var
    echo $var
    process=$(ls *.txt)
    process=${process%%.txt*}
    total=$(cat *.txt | wc -l)
    if [[ $max -le $total ]]; then
        max=$total
    fi
    #divide=$total  #edit this line
    #lines=$(($total / $divide ))
    #remind=$(($total % $divide ))
    #echo "total=$total line=$lines divide=$divide remind=$remind"
    echo "total=$total"
    num_txt=1
    #touch ${process}_{1..8}.txt
    for line in $(cat *.txt); do
        mkdir ../${process}_${num_txt}
        cd ../${process}_${num_txt}
        txt_name=${process}_${num_txt}.txt
        echo $line >>$txt_name
        let num_txt=num_txt+1
        cd ../$var
    done
    cd ../
done
out=condor_out_$type_name
rm -rf $out
mkdir $out
exp="mv *_{1.."$max"} "$out
eval $exp 2>/dev/null
rm -rf Chunk*
ls $out >condor_list_$type_name.txt
echo "directories are written into condor_list.txt"

rm -f condor_${type_name}.sub
cat >condor_${type_name}.sub <<EOF
executable              = ../scripts/run_${type_name}.sh
arguments               = \$(dir) $2
Initialdir              = /afs/cern.ch/user/y/yuekai/EFT-ttbar/select/20${2}/condor_out_${type_name}/\$(dir)

output                  = run.out
error                   = run.err
log                     = run.log

RequestMemory           = 2000
ShouldTransferFiles     = NO
+JobFlavour             = "testmatch"
x509userproxy           = \$ENV(HOME)/temp/x509up
Queue dir from ./condor_list_${type_name}.txt
EOF
