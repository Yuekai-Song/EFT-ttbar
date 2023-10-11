import math
import numpy as np
import statsmodels.api as sm
import ROOT

def copy(h0: ROOT.TH1D, h1: ROOT.TH1D, s: int) -> None:
    for i in range(0, h0.GetNbinsX()):
        h0.SetBinContent(i+1, h1.GetBinContent(s+i+1))


def set_val(h1: ROOT.TH1D, val: float)-> None:
    for i in range(0, h1.GetNbinsX()):
        h1.SetBinContent(i+1, val)


def sym(hup: ROOT.TH1D, hdn: ROOT.TH1D) -> None:
    for i in range(0, hup.GetNbinsX()):
        val = max(math.fabs(hup.GetBinContent(i+1)-1), math.fabs(hdn.GetBinContent(i+1)-1))
        hup.SetBinContent(i+1, val)
        hdn.SetBinContent(i+1, 2-val)


def lowess(hist: ROOT.TH1D, frac: float) -> None:
    x_values, y_values = [], []
    for i in range(0, hist.GetNbinsX()):
        x_values.append(i)
        y_values.append(hist.GetBinContent(i+1))
    x = np.array(x_values)
    y = np.array(y_values)
    yest = sm.nonparametric.lowess(y, x, frac)[:,1]
    for i in range(0, hist.GetNbinsX()):
        hist.SetBinContent(i+1, yest[i])


def smooth_sys(hist_up: ROOT.TH1D, hist_dn: ROOT.TH1D, hist_nom: ROOT.TH1D, start:list, option:list) -> None:
    norm_up = hist_up.GetSumOfWeights()
    norm_dn = hist_dn.GetSumOfWeights()
    norm_nom = hist_nom.GetSumOfWeights()
    hd_up = list()
    hd_dn = list()
    hd_nom = list()
    for f in range(0, len(start) - 1):
        hd_up.append(ROOT.TH1D("hd_up_{0}".format(f), "", start[f+1]-start[f], 0, start[f+1]-start[f]))
        hd_dn.append(ROOT.TH1D("hd_dn_{0}".format(f), "", start[f+1]-start[f], 0, start[f+1]-start[f]))
        hd_nom.append(ROOT.TH1D("hd_nom_{0}".format(f), "", start[f+1]-start[f], 0, start[f+1]-start[f]))
        copy(hd_up[f], hist_up, start[f])
        copy(hd_dn[f], hist_dn, start[f])
        copy(hd_nom[f], hist_nom, start[f])
        hd_up[f].Divide(hd_nom[f])
        hd_dn[f].Divide(hd_nom[f])
    if(option[0] == 1):  
        if(len(option[2]) == len(start) - 1):
            for f in range(0, len(start) - 1):
                lowess(hd_up[f], option[2][f])
                lowess(hd_dn[f], option[2][f])
        else:
            for f in range(0, len(start) - 1):
                lowess(hd_up[f], option[2][0])
                lowess(hd_dn[f], option[2][0])
    elif(option[0] == 2):
        for f in range(0, len(start) - 1):
            set_val(hd_up[f], norm_up/norm_nom)
            set_val(hd_dn[f], norm_dn/norm_nom)
    if(option[1] == 1):
        for f in range(0, len(start) - 1):
            sym(hd_up[f], hd_dn[f])
    for f in range(0, len(start) - 1):
        for i in range(0, start[f+1]-start[f]):
            #print(i+1+start[f], hd_up[f].GetBinContent(i+1), hd_nom[f].GetBinContent(i+1)*hd_up[f].GetBinContent(i+1))
            hist_up.SetBinContent(i+1+start[f], hd_nom[f].GetBinContent(i+1)*hd_up[f].GetBinContent(i+1))
            hist_dn.SetBinContent(i+1+start[f], hd_nom[f].GetBinContent(i+1)*hd_dn[f].GetBinContent(i+1))
    hist_up.ResetStats()
    hist_dn.ResetStats()

def xs_fix(hist_up: ROOT.TH1D, hist_dn: ROOT.TH1D, scale: tuple) -> None:
    hist_up.Scale(scale[0])
    hist_dn.Scale(scale[1])


def get_sys_name(h1_sys_name: str) -> None:
    pos = 0
    while(h1_sys_name[pos] != '_'):
        pos = pos + 1
    if("ttbar" in h1_sys_name):
        pos = pos + 1
        while(h1_sys_name[pos] != '_'):
            pos = pos + 1
    return h1_sys_name[pos+1: ]


def process(file_name: str, original: str, sys_type: dict, sys_xs_fix: dict) -> None:
    file = ROOT.TFile(file_name, "recreate")
    old_file = ROOT.TFile(original, "read")
    hist_map = dict()
    sys_contained = set()
    if("ttx" in file_name):
        start = [0, 16, 23]
    else:
        start = [0, 9, 20, 30, 41]
    for key in old_file.GetListOfKeys():
        hist = key.ReadObj()
        hist_name = hist.GetName()
        hist_map[hist_name] = hist.Clone()
        if("Up" in hist_name or "Down" in hist_name):
            hist_name = hist_name.replace("Up", "")
            hist_name = hist_name.replace("Down", "")
            sys_contained.add(str(hist_name))
        else:
            file.cd()
            hist_map[hist_name].Write()

    for sys in sys_contained:
        sys_name = get_sys_name(sys)
        nom_name = sys.replace("_"+sys_name, "")
        if sys_name in sys_xs_fix.keys():
            xs_fix(hist_map[sys+"Up"], hist_map[sys+"Down"], sys_xs_fix[sys_name])
        if "pdf" in sys_name:
            sys_name = "pdf"
        smooth_sys(hist_map[sys+"Up"], hist_map[sys+"Down"], hist_map[nom_name], start, sys_type[sys_name])
        file.cd()
        hist_map[sys+"Up"].Write()
        hist_map[sys+"Down"].Write()
        
    file.Close()