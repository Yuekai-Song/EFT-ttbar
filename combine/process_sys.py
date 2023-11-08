import sys
import process

cut_names = ["_E_3jets", "_M_3jets", "_E_4jets", "_M_4jets"]
years = [2015, 2016, 2017, 2018]
name_datacard = sys.argv[1]
cut_name = sys.argv[2]
year = int(sys.argv[3])
bg_flat = int(sys.argv[4])

sys_type_4cuts = {
    "TuneCP5": [2, 0],
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
    "unclus": [2, 0]
}
sys_type_2cuts = {
    "TuneCP5": [2, 0],
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
    "unclus": [2, 0]
}
sys_xs_fix = dict()
flat_name = {0: "", 1: "_bg_flat"}
sys_types = {"datacard_ttx/": sys_type_2cuts, "datacard/": sys_type_4cuts}

file_name = name_datacard + "processed" + flat_name[bg_flat] + "/ttbar" + cut_name+"_{0}.root".format(year)
original = name_datacard + "original/ttbar" + cut_name+"_{0}.root".format(year)
sys_type = sys_types[name_datacard]

process.process(file_name, original, bg_flat, sys_type, sys_xs_fix)
