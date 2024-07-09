import ROOT as r
import os, sys
import cmsstyle as CMS
import math
dir = os.path.dirname(os.path.abspath("../select_analysis/draw/format.py"))
sys.path.append(dir)
import format

def sete0(h1):
    for i in range(h1.GetNbinsX()):
        h1.SetBinError(i, 0)

# year = 2018
# var = 3
# cut_name = "M_4jets"
# isEnriched = 0
year = int(sys.argv[1])
cut_name = sys.argv[2]
cg = sys.argv[3]
var = int(sys.argv[4])
isEnriched = int(sys.argv[5])
en = "_En"
if isEnriched == 0:
    Enrich_name = "_HT"
    en = "_HT"
elif "M" in cut_name:
    Enrich_name = "_MuEn"
else:
    Enrich_name = "_EMEn"

process = ["ttbar", "DYJets", "STop", "WJets", "QCD" + Enrich_name]
legend = ["t#bar{t}", "DY", "single t", "WJets", "QCD" + en]
xtitle = ["lnL","M_{t}","p_{T}^{t} [GeV]","M_{t#bar{t}} [GeV]","#Deltay_{t#bar{t}}"]
title = ["likelihood","mass_t","top_pt","Mtt", "deltay"]
legendd = "data"
lumi = {2015: 19.5, 2016: 16.8, 2017: 41.48, 2018: 59.83}
cut = {"E_3jets": "e, 3jets", "E_4jets": "e, #geq4jets", "M_3jets": "#mu, 3jets", "M_4jets": "#mu, #geq4jets"}
path = "./output/{}/CG_roots/".format(year)

CG_file = path + "QCD_" + cut_name + "_" + cg + ".root"
file = r.TFile(CG_file)
hist_map = dict()
nom_sys = dict()
nom_pdf = dict()
for key in file.GetListOfKeys():
    hist = key.ReadObj()
    hist_name = hist.GetName()
    hist.SetName(hist_name)
    hist_map[hist_name] = hist
    hist_map[hist_name].SetDirectory(0)
file.Close()
hdata = hist_map[title[var] + "_data"]
hdata.Sumw2(0)
bins = hdata.GetNbinsX()
xdown = hdata.GetXaxis().GetBinLowEdge(1)
xup = hdata.GetXaxis().GetBinUpEdge(bins)
yup = hdata.GetMaximum() * 1.4
bin_len = (xup - xdown) / (1.0 * bins)
stack = r.THStack("stack", "Stacked")
hist_dict = {list(reversed(legend))[i]: hist_map[title[var] + "_" + list(reversed(process))[i]] for i in range(5)}
hmc = r.TH1D("mc", "", bins, xdown, xup)
hmc.Sumw2()
for i in range(5):
    hmc.Add(hist_map[title[var] + "_" + list(reversed(process))[i]])
yup = max(hmc.GetMaximum(), hdata.GetMaximum()) * 1.4
CMS.SetExtraText("Simulation")
CMS.SetLumi(lumi[year])
CMS.SetEnergy("13")
# CMS.AppendAdditionalInfo(cut[cut_name])
canv = CMS.cmsDiCanvas("canv", xdown, xup, 0, yup, 0, 2, xtitle[var], "Events","#frac{data}{MC}", square=CMS.kSquare, extraSpace=0.05)
leg = CMS.cmsLeg(0.78, 0.89 - 0.05 * 7, 0.95, 0.89, textSize=0.04)
canv.cd(1)
CMS.cmsDrawStack(stack, leg, hist_dict)

hdatad = hdata.Clone()
hdatad.SetName("datad")
text = r.TLatex()
format.format_tex(text)
text.DrawLatex(0.6, 0.8, cut[cut_name])

sete0(hmc)
hdatad.Divide(hmc)
# text = r.TText(0.8, 0.6, 0.9, 0.8, cut[cut_name])

leg.AddEntry(hdata, legendd, "p")
hdata.Draw("PSame")

format.format_his(hdata, "", False)

canv.cd(2)
format.format_his(hdatad, xtitle[var], 1)
hdatad.Draw("same")
l1 = list()
l2 = list()
for i in range(3):
    l1.append(r.TLine(xdown, 0.5 + 0.5 * i, xup, 0.5 + 0.5 * i))
    format.format_line(l1[i])
    l1[i].Draw("same"); 
for d in range(1, bins):
    l2.append(r.TLine(xdown + d * bin_len, 0, xdown + d * bin_len, 2))
    format.format_line(l2[d - 1])
    l2[d - 1].Draw("same") 
canv.Print("./CG_pdf/{}/".format(year) + title[var] + "_" + cut_name + "_" + cg + en + ".pdf")