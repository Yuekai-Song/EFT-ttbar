import os
import sys
import process
import ROOT

cut_names = {"_E_3jets": "_elec_3J", "_M_3jets": "_mu_3J", "_E_4jets" : "_elec_4J", "_M_4jets": "_mu_4J"}
years = [2015, 2016, 2017, 2018]
name_datacard = sys.argv[1]
cut_name = sys.argv[2]
year = int(sys.argv[3])
bg_flat = int(sys.argv[4])
renorm = int(sys.argv[5])

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
    "unclus": [2, 1],
    "qshape" + cut_names[cut_name] + "_{0}".format(year): [1, 1, [0.7]]
}

xs_22014 = {
    "4jets_2018": {"ttbar": 1137810.2, "DYJets": 14427.6, "STop": 32311.8, "WJets": 14427.6, "QCD": 11543.4},
    "3jets_2018": {"ttbar": 714101.4, "DYJets": 30092.2, "STop": 45300.8, "WJets": 30092.2, "QCD": 31575.2},
    "4jets_2017": {"ttbar": 808084.7, "DYJets": 11335.1, "STop": 23001.5, "WJets": 11335.1, "QCD": 10723.2},
    "3jets_2017": {"ttbar": 477649.3, "DYJets": 21436.5, "STop": 31498.6, "WJets": 21436.5, "QCD": 25309.5},
    "4jets_2016": {"ttbar": 322904.5, "DYJets": 3203.5, "STop": 8876.4, "WJets": 3203.5, "QCD": 1169.2},
    "3jets_2016": {"ttbar": 190818.9, "DYJets": 6307.9, "STop": 12240.9, "WJets": 6307.9, "QCD": 5673.1},
    "4jets_2015": {"ttbar": 372038.0, "DYJets": 4993.9, "STop": 10336.1, "WJets": 4993.9, "QCD": 2538.7},
    "3jets_2015": {"ttbar": 225685.1, "DYJets": 8980.4, "STop": 14718.5, "WJets": 8980.4, "QCD": 6696.0},
}

qnorm_22014 = {"E3j_2015": 1.5, "M3j_2015": 1.2, "E4j_2015": 1.5, "M4j_2015": 1.2,
               "E3j_2016": 1.5, "M3j_2016": 1.2, "E4j_2016": 1.5, "M4j_2016": 3.5,
               "E3j_2017": 1.5, "M3j_2017": 1.2, "E4j_2017": 1.5, "M4j_2017": 1.2,
               "E3j_2018": 1.5, "M3j_2018": 1.2, "E4j_2018": 1.5, "M4j_2018": 3.5}

sys_same_year = ["hdamp"]
for syss in sys_same_year:
    if "{0}".format(year) in syss:
        exit()
    
flat_name = {0: "", 1: "_bg_flat"}
nom_name = {0: "", 1: "_renormed"}

out_dir = name_datacard + "/processed" + flat_name[bg_flat] + nom_name[renorm]
if not os.path.exists(out_dir):
    os.mkdir(out_dir)
new_file = out_dir + "/ttbar" + cut_name + "_{0}.root".format(year)
base_file = name_datacard + "/original/ttbar" + cut_name + ".root"
original = name_datacard + "/original/ttbar" + cut_name + "_{0}.root".format(year)

xs_cut = cut_name[3:] + "_" + sys.argv[3]
if renorm == 0:
    xs_self = xs_22014[xs_cut]
else:
    renorm_file1 = ROOT.TFile(name_datacard + "/original/ttbar_E_" + xs_cut + ".root", "read")
    renorm_file2 = ROOT.TFile(name_datacard + "/original/ttbar_M_" + xs_cut + ".root", "read")
    xs_self = {"ttbar": renorm_file1.Get("ttbar_ci0000").GetSumOfWeights() + renorm_file2.Get("ttbar_ci0000").GetSumOfWeights(),
               "DYJets": renorm_file1.Get("DYJets").GetSumOfWeights() + renorm_file2.Get("DYJets").GetSumOfWeights(),
               "STop": renorm_file1.Get("STop").GetSumOfWeights() + renorm_file2.Get("STop").GetSumOfWeights(),
               "WJets": renorm_file1.Get("WJets").GetSumOfWeights() + renorm_file2.Get("WJets").GetSumOfWeights()}
    ori_file = ROOT.TFile(original, "read")
    if ori_file.Get("QCD"):
        ori_file.Close()
        xs_self["QCD"] = renorm_file1.Get("QCD").GetSumOfWeights() + renorm_file2.Get("QCD").GetSumOfWeights()
    xs_self["DYJets"] = xs_self["DYJets"] + xs_self["WJets"]
    xs_self["WJets"] = xs_self["DYJets"]

if "2cuts" in name_datacard:
    sys_type = sys_type_2cuts
    start = [0, 16, 23]
else:
    sys_type = sys_type_4cuts
    start = [0, 7, 16, 25, 33]

ch = cut_name[1:2] + cut_name[3:5]+ "_" + sys.argv[3]
qnorm_fix = qnorm_22014[ch]

process.process(new_file, original, bg_flat, sys_type, xs_22014[xs_cut], xs_self, qnorm_fix, start, base_file, sys_same_year)