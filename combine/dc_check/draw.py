import ROOT as r
import os, sys
import cmsstyle as CMS
import math
import include.format as form

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

def set_error(hg: r.TGraphAsymmErrors, h1: r.TH1D, bin_len: float, hist_map: dict, nom_sys: dict, is_abs: bool):
    n = h1.GetNbinsX()
    up_vs =list()
    down_vs=list()
    pro = ["ttbar_ci0000", "DYJets", "STop", "WJets"]
    for i in range(n):
        up_vs.append(0)
        down_vs.append(0)
    for p in range(4):
        for it_sys in nom_sys[pro[p]]:
            if it_sys != "muR" and it_sys != "muF":
                get_error(hist_map[pro[p] + "_" + it_sys + "Up"], hist_map[pro[p] + "_" + it_sys + "Down"], hist_map[pro[p]], up_vs, down_vs)
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
# # var = 3
# cut_name = "M_4jets"
year = int(sys.argv[1])
cut_name = sys.argv[2]

legendd = "data"
lumi = {2015: 19.5, 2016: 16.8, 2017: 41.48, 2018: 59.83}
cutn = {"E_3jets": "e, 3jets", "E_4jets": "e, #geq4jets", "M_3jets": "#mu, 3jets", "M_4jets": "#mu, #geq4jets"}
xtitle = "M_{t#bar{t}} [GeV]"
path = "../datacards/datacard/processed_bg_flat_nu/"

sys_file = path + "ttbar_" + cut_name + "_{}.root".format(year)
file = r.TFile(sys_file)
hist_map = dict()
nom_sys = dict()
for key in file.GetListOfKeys():
    hist = key.ReadObj()
    hist_name = hist.GetName()
    hist_name = hist_name.replace("_sub", "")
    hist.SetName(hist_name)
    hist_map[hist_name] = hist
    hist_map[hist_name].SetDirectory(0)
    if "Up" in hist_name:
        nom_name, sys_name = sys_and_nom(hist_name)
        if nom_name not in nom_sys.keys():
            nom_sys[nom_name] = list()
        nom_sys[nom_name].append(sys_name)
# print(nom_sys)
file.Close()
hdata = hist_map["data_obs"]
hdata.Sumw2(0)
bins = hdata.GetNbinsX()
xdown = 0
xup = bins
yup = hdata.GetMaximum() * 1.4
bin_len = (xup - xdown) / (1.0 * bins)
ycuts = [0.0, 0.4, 1.0, 2.0]
xbins = [[0,340,380,420,460,500,600,3000], [0,350,400,450,500,550,600,700,800,3000], 
                               [0,450,500,550,600,650,700,800,1000,3000], [0,650,700,750,800,900,1000,1200,3000]]
ndiv = len(xbins) - 1
nnbins = len(xbins) + 1
ncuts = len(ycuts)
div = []
nbins = []
nbins.append(0)
for i in range(1, nnbins):
    nbins.append(nbins[i - 1] + len(xbins[i - 1]) - 1)
for i in range(ndiv):
    div.append(nbins[i + 1])

cut = []
cut.append("|#Deltay| < {:.1f}".format(ycuts[1]))
for i in range(1, ncuts - 1):
    cut.append("{:.1f} < |#Deltay| < {:.1f}".format(ycuts[i], ycuts[i + 1]))
cut.append("|#Deltay| > {:.1f}".format(ycuts[ncuts - 1]))

CMS.SetExtraText("")
CMS.SetLumi(lumi[year])
CMS.SetEnergy("13")
# CMS.AppendAdditionalInfo(cut[cut_name])
if (yup > 100000):
    es = 0.05
else:
    es = 0.35

canv = CMS.cmsDiCanvas("canv", xdown, xup, 0, yup, 0.5, 1.5, xtitle, "Events","#frac{data}{MC}", xbins, square=CMS.kSquare, extraSpace=es, iPos=0)
# CMS.GetcmsCanvasHist(canv.cd(1)).GetYaxis().SetTitleOffset(1.1)
pro = ["ttbar_ci0010", "Eta", "DYJets", "STop", "WJets", "QCD"]
name = ["t#bar{t}", "#eta_{t}", "DY", "single t", "WJets", "QCD"]
stack = r.THStack("stack", "Stacked")
leg = CMS.cmsLeg(0.78, 0.75 - 0.05 * 7, 0.95, 0.75, textSize=0.04)
hist_dict = {list(reversed(name))[i]: hist_map[list(reversed(pro))[i]] for i in range(6)}
canv.cd(1)
CMS.cmsDrawStack(stack, leg, hist_dict)
hmc = r.TH1D("mc", "", bins, xdown, xup)
hmc.Sumw2()
hdatad = hdata.Clone()
hdatad.SetName("datad")
l0 = []
for d in range(ndiv):
    l0.append(r.TLine(div[d], 0, div[d], yup))
    form.format_line(l0[d], 0)
    l0[d].Draw("same")
t = []
for tex in range(ncuts):
    mid_row = (nbins[tex] + nbins[tex + 1]) / 2.0
    mid_col = yup * 0.9
    t.append(r.TPaveText(mid_row - 1, mid_col * 0.95, mid_row + 1, mid_col * 1.05))
    if tex == ncuts / 2:
        sys_text = r.TPaveText(mid_row - 1, mid_col * 0.75, mid_row + 1, mid_col * 0.85)
        form.format_text(sys_text, 1)
        sys_text.AddText(cutn[cut_name])
        sys_text.Draw("same")
    form.format_text(t[tex], 0)
    t[tex].AddText(cut[tex])
    t[tex].Draw("same")

# text = r.TLatex()
# form.format_tex(text)
# text.DrawLatex(0.6, 0.8, cut[cut_name])
for i in range(6):
    hmc.Add( hist_map[list(reversed(pro))[i]])
sete0(hmc)
hdatad.Divide(hmc)
hratio = hmc.Clone()
hratio.Divide(hmc)
# text = r.TText(0.8, 0.6, 0.9, 0.8, cut[cut_name])
hmcdg = r.TGraphAsymmErrors(hratio)
hmcg = r.TGraphAsymmErrors(hmc)
set_error(hmcdg, hmc, bin_len, hist_map, nom_sys, 0)
set_error(hmcg, hmc, bin_len, hist_map, nom_sys, 1)
leg.AddEntry(hdata, legendd, "p")
hdata.Draw("PSame")
hmcg.Draw("2Same")

form.format_graph(hmcg, 1)
form.format_his(hdata, "", 1, 0)

canv.cd(2)
form.format_his(hdatad, xtitle, 1, 2)
hdatad.Draw("same")
hmcdg.Draw("2same")
form.format_graph(hmcdg, 2)
l1 = list()
l2 = list()
l3 = list()
for i in range(3):
    l1.append(r.TLine(xdown, 0.75 + 0.25 * i, xup, 0.75 + 0.25 * i))
    form.format_line(l1[i])
    l1[i].Draw("same"); 

for d in range(ndiv):
    l2.append(r.TLine(div[d], 0.5, div[d], 1.5))
    form.format_line(l2[d], 0)
    l2[d].Draw("same")
net_num = 0
for d in range(ndiv + 1):
    for bin in range(nbins[d] + 1, nbins[d + 1]):
        l3.append(r.TLine(bin, 0.5, bin, 1.5))
        form.format_line(l3[net_num], 1)
        l3[net_num].Draw("same")
        net_num = net_num + 1
# for d in range(1, bins):
#     l2.append(r.TLine(xdown + d * bin_len, 0.5, xdown + d * bin_len, 1.5))
#     form.format_line(l2[d - 1])
#     l2[d - 1].Draw("same") 
# canv.Print("test.pdf")
canv.Print("./qcd_pdf/data_" + cut_name + "_{}".format(year) + ".pdf")