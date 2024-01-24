import sys
import process

cut_names = ["_E_3jets", "_M_3jets", "_E_4jets", "_M_4jets"]
years = [2015, 2016, 2017, 2018]
name_datacard = sys.argv[1]
cut_name = sys.argv[2]
year = int(sys.argv[3])
bg_flat = int(sys.argv[4])

sys_type_4cuts = {
    "TuneCP5": [2, 1],
    "mtop": [1, 0, [0.5]],
    "mtop3": [1, 0, [0.5]],
    "hdamp": [1, 0, [0.5]],
    "jes_Absolute": [1, 0, [0.5]],
    "jes_Absolute_{0}".format(year): [1, 0, [0.5]],
    "jes_FlavorQCD": [1, 0, [0.5]],
    "jes_BBEC1": [1, 0, [0.5]],
    "jes_EC2": [1, 0, [0.5]],
    "jes_HF": [1, 0, [0.5]],
    "jes_BBEC1_{0}".format(year): [1, 0, [0.5]],
    "jes_EC2_{0}".format(year): [1, 0, [0.5]],
    "jes_RelativeBal": [1, 0, [0.5]],
    "jes_RelativeSample_{0}".format(year): [1, 0, [0.5]],
    "jer": [1, 0, [0.5]],
    "unclus": [2, 1]
}
sys_type_2cuts = {
    "TuneCP5": [2, 1],
    "mtop": [1, 0, [0.4]],
    "mtop3": [1, 0, [0.4]],
    "hdamp": [1, 0, [0.6]],
    "jes_Absolute": [1, 0, [0.3]],
    "jes_Absolute_{0}".format(year): [1, 0, [0.3]],
    "jes_FlavorQCD": [1, 0, [0.3]],
    "jes_BBEC1": [1, 0, [0.3]],
    "jes_EC2": [1, 0, [0.3]],
    "jes_HF": [1, 0, [0.3]],
    "jes_BBEC1_{0}".format(year): [1, 0, [0.3]],
    "jes_EC2_{0}".format(year): [1, 0, [0.3]],
    "jes_RelativeBal": [1, 0, [0.3]],
    "jes_RelativeSample_{0}".format(year): [1, 0, [0.3]],
    "jer": [1, 0, [0.5]],
    "unclus": [2, 1]
}
sys_xs_fix = dict()
sys_same_year = ["hdamp"]
for syss in sys_same_year:
    if "{0}".format(year) in syss:
        exit()
    
flat_name = {0: "", 1: "_bg_flat"}

new_file = name_datacard + "processed" + flat_name[bg_flat] + "/ttbar" + cut_name + "_{0}.root".format(year)
base_file = name_datacard + "original/ttbar" + cut_name + ".root"
original = name_datacard + "original/ttbar" + cut_name + "_{0}.root".format(year)

if "2cuts" in name_datacard:
    sys_type = sys_type_2cuts
    start = [0, 16, 23]
else:
    sys_type = sys_type_4cuts
    start = [0, 7, 16, 25, 33]

process.process(new_file, original, bg_flat, sys_type, sys_xs_fix, start, base_file, sys_same_year)
