import ROOT as r
import os, sys
import cmsstyle as CMS
import math
import format

def sete0(h1):
    for i in range(h1.GetNbinsX()):
        h1.SetBinError(i, 0)

def sys_and_nom(hist_name: str):
    pos = 0
    hist_name = hist_name.replace("Up", "")
    hist_name = hist_name.replace("Down", "")
    while hist_name[pos] != "_":
        pos = pos + 1
    if "ttbar" in hist_name:
        pos = pos + 1
        while hist_name[pos] != "_":
            pos = pos + 1
    nom_name = hist_name[:pos]
    sys_name = hist_name[pos + 1:]
    return nom_name, sys_name

def get_error(h1_up: r.TH1D, h1_dn: r.TH1D, h1_nom: r.TH1D, up_vs, down_vs, envelope: bool = 0):
    n = h1_nom.GetNbinsX()
    for b in range(n):
        up_v = h1_up.GetBinContent(b + 1)
        nom_v = h1_nom.GetBinContent(b + 1)
        if h1_dn != 0:
            down_v = h1_dn.GetBinContent(b + 1)
        else:
            if up_v == 0:
                down_v = 0
            else:
                down_v = nom_v * nom_v / up_v
        up_max = max(max(up_v, down_v) - nom_v, 0)
        down_min = min(min(up_v, down_v) - nom_v, 0)
        if not envelope:
            up_vs[b] += up_max * up_max
            down_vs[b] += down_min * down_min
        else:
            if up_max > up_vs[b]:
                up_vs[b] = abs(up_max)
            if down_min < down_vs[b]:
                down_vs[b] = abs(down_min)

def set_error(hg: r.TGraphAsymmErrors, h1: r.TH1D, bin_len: float, hist_map: dict, nom_sys: dict, nom_pdf: dict, is_abs: bool):
    n = h1.GetNbinsX()
    up_vs =list()
    down_vs=list()
    up_en=list()
    down_en =list()
    pro = ["ttbar_ci0000", "DYJets", "STop", "WJets"]
    for i in range(n):
        up_vs.append(0)
        down_vs.append(0)
    for p in range(4):
        for it_sys in nom_sys[pro[p]]:
            if it_sys != "muR" and it_sys != "muF":
                get_error(hist_map[pro[p] + "_" + it_sys + "Up"], hist_map[pro[p] + "_" + it_sys + "Down"], hist_map[pro[p]], up_vs, down_vs)
        if p == 0:
            for it_sys in nom_pdf[pro[p]]:
                get_error(hist_map[pro[p] + "_" + it_sys], 0, hist_map[pro[p]], up_vs, down_vs)
        else:
            for i in range(n):
                up_en.append(0)
                down_en.append(0)
            for it_sys in nom_pdf[pro[p]]:
                get_error(hist_map[pro[p] + "_" + it_sys], 0, hist_map[pro[p]], up_en, down_en, 1)
            for i in range(i):
                up_vs[i] += up_en[i]
                down_vs[i] += down_en[i]
    if not is_abs:
        for i in range(n):
            nom = h1.GetBinContent(i + 1)
            if nom == 0:
                up_vs[i] = 0
                down_vs[i] = 0
            else:
                up_vs[i] /= nom * nom
                down_vs[i] /= nom * nom
    for i in range(n):
        hg.SetPointEXlow(i, bin_len / 2.0)
        hg.SetPointEXhigh(i, bin_len / 2.0)
        if (up_vs[i] < 0 or down_vs[i] < 0):
            print("why?")
        hg.SetPointEYhigh(i, math.sqrt(up_vs[i]))
        hg.SetPointEYlow(i, math.sqrt(down_vs[i]))

# year = 2018
# var = 3
# cut_name = "M_4jets"
year = int(sys.argv[1])
cut_name = sys.argv[2]
var = int(sys.argv[3])

legendd = "data"
lumi = {2015: 19.5, 2016: 16.8, 2017: 41.48, 2018: 59.83}
cut = {"E_3jets": "e, 3jets", "E_4jets": "e, #geq4jets", "M_3jets": "#mu, 3jets", "M_4jets": "#mu, #geq4jets"}
xtitle = ["-lnL",  "M_{th} [GeV]", "M_{tl} [GeV]", "P_{T}^{l} [GeV]", "p_{T}^{t} [GeV]"]
title = ["likelihood", "mass_thad", "mass_tlep", "lepton_pt", "top_pt"]
path = "../sys_root/{}/".format(year)

sys_file = path + title[var] + "_ttbar_" + cut_name + ".root"
file = r.TFile(sys_file)
hist_map = dict()
nom_sys = dict()
nom_pdf = dict()
for key in file.GetListOfKeys():
    hist = key.ReadObj()
    hist_name = hist.GetName()
    hist_name = hist_name.replace("_sub", "")
    hist.SetName(hist_name)
    hist_map[hist_name] = hist
    hist_map[hist_name].SetDirectory(0)
    # if "Up" in hist_name:
    #     nom_name, sys_name = sys_and_nom(hist_name)
    #     if nom_name not in nom_sys.keys():
    #         nom_sys[nom_name] = list()
    #     nom_sys[nom_name].append(sys_name)
    # if "pdf" in hist_name:
    #     nom_name, sys_name = sys_and_nom(hist_name)
    #     if nom_name not in nom_pdf.keys():
    #         nom_pdf[nom_name] = list()
    #     nom_pdf[nom_name].append(sys_name)
# print(nom_sys)
file.Close()
hdata = hist_map["data_obs"]
hdata.Sumw2(0)
bins = hdata.GetNbinsX()
xdown = hdata.GetXaxis().GetBinLowEdge(1)
xup = hdata.GetXaxis().GetBinUpEdge(bins)
yup = hdata.GetMaximum() * 1.4
bin_len = (xup - xdown) / (1.0 * bins)

CMS.SetExtraText("Simulation")
CMS.SetLumi(lumi[year])
CMS.SetEnergy("13")
# CMS.AppendAdditionalInfo(cut[cut_name])
if (yup > 100000):
    es = 0.05
else:
    es = 0.35
canv = CMS.cmsDiCanvas("canv", xdown, xup, 0, yup, 0.5, 1.5, xtitle[var], "Events","#frac{data}{MC}", square=CMS.kSquare, extraSpace=es)
# CMS.GetcmsCanvasHist(canv.cd(1)).GetYaxis().SetTitleOffset(1.1)
pro = ["ttbar_ci0000", "Eta", "DYJets", "STop", "WJets", "QCD"]
name = ["t#bar{t}", "#eta_{t}", "DY", "single t", "WJets", "QCD"]
stack = r.THStack("stack", "Stacked")
leg = CMS.cmsLeg(0.78, 0.89 - 0.05 * 7, 0.95, 0.89, textSize=0.04)
hist_dict = {list(reversed(name))[i]: hist_map[list(reversed(pro))[i]] for i in range(6)}
canv.cd(1)
CMS.cmsDrawStack(stack, leg, hist_dict)
hmc = r.TH1D("mc", "", bins, xdown, xup)
hmc.Sumw2()
hdatad = hdata.Clone()
hdatad.SetName("datad")
text = r.TLatex()
format.format_tex(text)
text.DrawLatex(0.6, 0.8, cut[cut_name])
for i in range(6):
    hmc.Add( hist_map[list(reversed(pro))[i]])
sete0(hmc)
hdatad.Divide(hmc)
hratio = hmc.Clone()
hratio.Divide(hmc)
# text = r.TText(0.8, 0.6, 0.9, 0.8, cut[cut_name])
hmcdg = r.TGraphAsymmErrors(hratio)
hmcg = r.TGraphAsymmErrors(hmc)
# set_error(hmcdg, hmc, bin_len, hist_map, nom_sys, nom_pdf, 0)
# set_error(hmcg, hmc, bin_len, hist_map, nom_sys, nom_pdf, 1)
leg.AddEntry(hdata, legendd, "p")
hdata.Draw("PSame")
# hmcg.Draw("2Same")

format.format_graph(hmcg, 1)
format.format_his(hdata, "", False)

canv.cd(2)
format.format_his(hdatad, xtitle[var], 1)
hdatad.Draw("same")
# hmcdg.Draw("2same")
format.format_graph(hmcdg, 2)
l1 = list()
l2 = list()
for i in range(3):
    l1.append(r.TLine(xdown, 0.75 + 0.25 * i, xup, 0.75 + 0.25 * i))
    format.format_line(l1[i])
    l1[i].Draw("same"); 
for d in range(1, bins):
    l2.append(r.TLine(xdown + d * bin_len, 0.5, xdown + d * bin_len, 1.5))
    format.format_line(l2[d - 1])
    l2[d - 1].Draw("same") 
# canv.Print("test.pdf")
canv.Print("../qcd_pdf/{}/".format(year) + title[var] + "_" + cut_name + ".pdf")