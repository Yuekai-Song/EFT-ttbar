import math
import numpy as np
import statsmodels.api as sm
import ROOT

def copy(h0: ROOT.TH1D, h1: ROOT.TH1D, s: int) -> None:
    for i in range(0, h0.GetNbinsX()):
        h0.SetBinContent(i + 1, h1.GetBinContent(s + i + 1))


def set_val(h1: ROOT.TH1D, val: float)-> None:
    for i in range(0, h1.GetNbinsX()):
        h1.SetBinContent(i + 1, val)


def sym(hup: ROOT.TH1D, hdn: ROOT.TH1D) -> None:
    for i in range(0, hup.GetNbinsX()):
        val = max(math.fabs(hup.GetBinContent(i + 1) - 1), math.fabs(hdn.GetBinContent(i + 1) - 1))
        hup.SetBinContent(i + 1, val + 1)
        hdn.SetBinContent(i + 1, max(1 - val, 0))

def lowess(hist: ROOT.TH1D, frac: float) -> None:
    x_values, y_values = [], []
    for i in range(0, hist.GetNbinsX()):
        x_values.append(i)
        y_values.append(hist.GetBinContent(i + 1))
    x = np.array(x_values)
    y = np.array(y_values)
    yest = sm.nonparametric.lowess(y, x, frac)[:, 1]

    for i in range(0, hist.GetNbinsX()):
        hist.SetBinContent(i + 1, yest[i])

def smooth_sys(hist_up: ROOT.TH1D, hist_dn: ROOT.TH1D, hist_nom: ROOT.TH1D, start:list, option:list) -> None:
    norm_up = hist_up.GetSumOfWeights()
    norm_dn = hist_dn.GetSumOfWeights()
    norm_nom = hist_nom.GetSumOfWeights()
    hd_up = list()
    hd_dn = list()
    hd_nom = list()
    for f in range(0, len(start) - 1):
        hd_up.append(ROOT.TH1D("hd_up_{0}".format(f), "", start[f + 1] - start[f], 0, start[f + 1] - start[f]))
        hd_dn.append(ROOT.TH1D("hd_dn_{0}".format(f), "", start[f + 1] - start[f], 0, start[f + 1] - start[f]))
        hd_nom.append(ROOT.TH1D("hd_nom_{0}".format(f), "", start[f + 1] - start[f], 0, start[f + 1] - start[f]))
        copy(hd_up[f], hist_up, start[f])
        copy(hd_dn[f], hist_dn, start[f])
        copy(hd_nom[f], hist_nom, start[f])
        hd_up[f].Divide(hd_nom[f])
        hd_dn[f].Divide(hd_nom[f])
    if option[0] == 1:
        if(len(option[2]) == len(start) - 1):
            for f in range(0, len(start) - 1):
                lowess(hd_up[f], option[2][f])
                lowess(hd_dn[f], option[2][f])
        else:
            for f in range(0, len(start) - 1):
                lowess(hd_up[f], option[2][0])
                lowess(hd_dn[f], option[2][0])
    elif option[0] == 2:
        for f in range(0, len(start) - 1):
            set_val(hd_up[f], norm_up / norm_nom)
            set_val(hd_dn[f], norm_dn / norm_nom)
    if option[1] == 1:
        for f in range(0, len(start) - 1):
            if option[0] != 2 or (option[0] == 2 and (norm_up - norm_nom) * (norm_dn - norm_nom) > 0):
                sym(hd_up[f], hd_dn[f])


    for f in range(0, len(start) - 1):
        for i in range(0, start[f + 1] - start[f]):
            #print(i+1+start[f], hd_up[f].GetBinContent(i+1), hd_nom[f].GetBinContent(i+1)*hd_up[f].GetBinContent(i+1))
            hist_up.SetBinContent(i + 1 + start[f], hd_nom[f].GetBinContent(i + 1) * hd_up[f].GetBinContent(i + 1))
            hist_dn.SetBinContent(i + 1 + start[f], hd_nom[f].GetBinContent(i + 1) * hd_dn[f].GetBinContent(i + 1))
    hist_up.ResetStats()
    hist_dn.ResetStats()

def same_year(hist_up: ROOT.TH1D, hist_dn: ROOT.TH1D, hist_nom: ROOT.TH1D, hist_base: list) -> None:
    hist_base[1].Divide(hist_base[0])
    hist_base[2].Divide(hist_base[0])
    for i in range(0, hist_nom.GetNbinsX()):
        hist_up.SetBinContent(i + 1, hist_base[1].GetBinContent(i + 1) * hist_nom.GetBinContent(i + 1))
        hist_dn.SetBinContent(i + 1, hist_base[2].GetBinContent(i + 1) * hist_nom.GetBinContent(i + 1))


def get_sys_name(h1_sys_name: str) -> None:
    pos = 0
    while h1_sys_name[pos] != '_':
        pos = pos + 1
    if "ttbar" in h1_sys_name:
        pos = pos + 1
        while h1_sys_name[pos] != '_':
            pos = pos + 1
    return h1_sys_name[pos + 1 :]


def process(file_name: str, original: str, flat_bg: bool, sys_type: dict,
            xs_22014: dict, xs_self: dict, qnorm_fix: float, start: list, base_file: str, sys_same_year: list) -> None:
    file = ROOT.TFile(file_name, "recreate")
    old_file = ROOT.TFile(original, "read")
    base = ROOT.TFile(base_file, "read")
    hist_map = dict()
    sys_contained = set()
    for key in old_file.GetListOfKeys():
        hist = key.ReadObj()
        hist_name = hist.GetName()
        if (hist_name == "EW_no" or "ttbar" in hist_name or hist_name == "data_obs"):
            nom_name = "ttbar"
        elif ("Up" in hist_name or "Down" in hist_name):
            nom_name = hist_name.replace("_" + get_sys_name(hist_name), "")
        else:
            nom_name = hist_name

        hist_map[hist_name] = hist
        hist_map[hist_name].SetDirectory(0)
        hist_map[hist_name].Scale(xs_22014[nom_name] / xs_self[nom_name])

        if ("Up" in hist_name or "Down" in hist_name):
            hist_name = hist_name.replace("Up", "")
            hist_name = hist_name.replace("Down", "")
            sys_contained.add(str(hist_name))
        else:
            file.cd()
            hist_map[hist_name].Write()
    old_file.Close()

    for sys in sys_contained:
        sys_name = get_sys_name(sys)
        nom_name = sys.replace("_" + sys_name, "")
        if "pdf" in sys_name:
            sys_name = "pdf"
        
        if sys_name in sys_same_year:
            hist_base = [base.Get(nom_name), base.Get(sys + "Up"), base.Get(sys + "Down")]
            same_year(hist_map[sys + "Up"], hist_map[sys + "Down"], hist_map[nom_name], hist_base)
        
        if "qnorm" in sys_name:
            hist_map[sys + "Up"] = hist_map[nom_name].Clone()
            hist_map[sys + "Up"].SetName(sys + "Up")
            hist_map[sys + "Down"] = hist_map[nom_name].Clone()
            hist_map[sys + "Down"].SetName(sys + "Down")
            hist_map[sys + "Up"].Scale(qnorm_fix)
            hist_map[sys + "Down"].Scale(1.0 / qnorm_fix)
        
        if flat_bg and "ttbar" not in nom_name and "QCD" not in nom_name:
            option = [2, 1]
        elif sys_name not in sys_type.keys():
            option = [0, 0]
        else:
            option = sys_type[sys_name]
        smooth_sys(hist_map[sys + "Up"], hist_map[sys + "Down"], hist_map[nom_name], start, option)

        file.cd()
        hist_map[sys + "Up"].Write()
        hist_map[sys + "Down"].Write()

    file.Close()