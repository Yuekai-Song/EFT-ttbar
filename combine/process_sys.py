import sys
import process

cut_names = ["_E_3jets", "_M_3jets", "_E_4jets", "_M_4jets"]
years = [2015, 2016, 2017, 2018]
name_datacard = sys.argv[1]
cut_name = sys.argv[2]
year = int(sys.argv[3])
bg_flat = int(sys.argv[4])

sys_type = {
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
sys_xs_fix = dict()
flat_name = {0: "", 1: "_bg_flat"}
'''sys_xs_fix = {"all": dict(),  "mtop3_only": dict(),
            "mtop_only": dict(), "xs_mtop_only": {"mtop": (1.028, 0.973)},
            "all_xs_mtop3": {"mtop3": (1.085, 0.921)}, "xs_mtop3_only": {"mtop3": (1.085, 0.921)},
            "all_xs_mtop3_big": {"mtop": (0.8, 1.2)}, "xs_mtop3_big_only": {"mtop": (0.8, 1.2)}}'''

'''sys_saved_all = ["jes_Absolute", "jes_Absolute_{0}".format(year), "jes_FlavorQCD", "jes_BBEC1", 
            "jes_EC2", "jes_HF", "jes_BBEC1_{0}".format(year), "jes_EC2_{0}".format(year), 
            "jes_RelativeBal", "jes_RelativeSample_{0}".format(year), "jer", "unclus", 
            "SF_Muon", "SF_Elec", "L1PF", "PU", "muR", "muF", 
            "SF_btag", "SF_btag{0}".format(year), "SF_ltag", "SF_ltag{0}".format(year), 
            "muR1", "muF1", "muR2", "muF2", "muR3", "muF3",
            "ISR", "FSR", "mtop3", "hdamp", "TuneCP5",
            "nnlo_wt", "EW_un","pdf", "alphas", "alphas1", "alphas3", "qcds"] #mtop to mtop3'''


file_name = name_datacard + "processed" + flat_name[bg_flat] + "/ttbar" + cut_name+"_{0}.root".format(year)
original = name_datacard + "original/ttbar" + cut_name+"_{0}.root".format(year)
process.process(file_name, original, bg_flat, sys_type, sys_xs_fix)
