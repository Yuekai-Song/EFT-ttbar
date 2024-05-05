import sys
import process

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
xs = {
    "4jets_2018": {"ttbar": 1373672.180, "DYJets": 42889.097, "STop": 65668.303, "WJets": 42889.097, "QCD": 13482.57},
    "3jets_2018": {"ttbar": 700886.249, "DYJets": 45213.924, "STop": 68980.932, "WJets": 45213.924, "QCD": 10478.331},
    "4jets_2017": {"ttbar": 905491.916, "DYJets": 26666.765, "STop": 42933.543, "WJets": 26666.765, "QCD": 3646.486},
    "3jets_2017": {"ttbar": 459668.940, "DYJets": 28001.779, "STop": 44918.471, "WJets": 28001.779, "QCD": 6523.975},
    "4jets_2016": {"ttbar": 369249.661, "DYJets": 9457.0650, "STop": 17078.825, "WJets": 9457.0650, "QCD": 22364.907},
    "3jets_2016": {"ttbar": 192682.512, "DYJets": 10092.127, "STop": 18414.012, "WJets": 10092.127, "QCD": 3379.509},
    "4jets_2015": {"ttbar": 436525.235, "DYJets": 12746.261, "STop": 20094.089, "WJets": 12746.261, "QCD": 5121.598},
    "3jets_2015": {"ttbar": 229796.945, "DYJets": 13594.463, "STop": 22042.574, "WJets": 13594.463, "QCD": 3857.661},
}
xs_ttx = {
    "4jets_2018": {"ttbar": 1228687.364, "DYJets": 24617.331, "STop": 49288.147, "WJets": 24617.331, "QCD": 73905.478},
    "3jets_2018": {"ttbar": 590777.400, "DYJets": 19707.288, "STop": 39934.100, "WJets": 19707.288, "QCD": 59641.388},
    "4jets_2017": {"ttbar": 816566.630, "DYJets": 15288.764, "STop": 32314.026, "WJets": 15288.764, "QCD": 47602.789},
    "3jets_2017": {"ttbar": 391812.431, "DYJets": 12619.894, "STop": 26290.719, "WJets": 12619.894, "QCD": 38910.613},
    "4jets_2016": {"ttbar": 320992.449, "DYJets": 4969.3460, "STop": 12198.072, "WJets": 4969.3460, "QCD": 17167.418},
    "3jets_2016": {"ttbar": 159621.164, "DYJets": 4154.0890, "STop": 10444.627, "WJets": 4154.0890, "QCD": 14598.717},
    "4jets_2015": {"ttbar": 372193.383, "DYJets": 6658.1440, "STop": 14128.975, "WJets": 6658.1440, "QCD": 20787.120},
    "3jets_2015": {"ttbar": 186716.825, "DYJets": 5531.7200, "STop": 12311.083, "WJets": 5531.7200, "QCD": 17842.803},
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

new_file = name_datacard + "/processed" + flat_name[bg_flat] + nom_name[renorm] + "/ttbar" + cut_name + "_{0}.root".format(year)
base_file = name_datacard + "/original/ttbar" + cut_name + ".root"
original = name_datacard + "/original/ttbar" + cut_name + "_{0}.root".format(year)

if "2cuts" in name_datacard:
    sys_type = sys_type_2cuts
    start = [0, 16, 23]
else:
    sys_type = sys_type_4cuts
    start = [0, 7, 16, 25, 33]

if "ttx" in name_datacard:
    xs_self = xs_ttx
else:
    xs_self = xs
if renorm == 0:
    xs_self = xs_22014

xs_cut = cut_name[3:] + "_" + sys.argv[3]
ch = cut_name[1:2] + cut_name[3:5]+ "_" + sys.argv[3]
qnorm_fix = qnorm_22014[ch]

process.process(new_file, original, bg_flat, sys_type, xs_22014[xs_cut], xs_self[xs_cut], qnorm_fix, start, base_file, sys_same_year)