import ROOT as r
import os, sys
import cmsstyle as CMS
import math
import include.format as form

def sete0(h1):
    for i in range(h1.GetNbinsX() + 2):
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
    years = ["2015", "2016", "2017", "2018"]
    for i in range(n):
        up_vs.append(0)
        down_vs.append(0)
    for p in range(4):
        for it_sys in nom_sys[pro[p]]:
            if it_sys != "muR" and it_sys != "muF":
                corr = True
                for year in years:
                    if year in it_sys:
                        corr = False
                        get_error(hist_map[pro[p] + "_" + it_sys + "Up"], hist_map[pro[p] + "_" + it_sys + "Down"], hist_map[pro[p] + "_" + year], up_vs, down_vs)
                if corr:
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
sg = sys.argv[4]
cg = sys.argv[5]
en = sys.argv[7]
qMC = sys.argv[6]


legendd = "data"
lumi = {2015: 19.5, 2016: 16.8, 2017: 41.48, 2018: 59.83, 2024: 138}
cut = {"E_3jets": "e, 3jets", "E_4jets": "e, #geq4jets", "M_3jets": "#mu, 3jets", "M_4jets": "#mu, #geq4jets"}
xtitle = ["p_{T}^{t} [GeV]", "M_{t#bar{t}} [GeV]", "#Deltay_{t#bar{t}}", "cos(#theta)", "y_{t#bar{t}}", "-2lnL", "p_{T}^{leading jet} [GeV]", "p_{T}^{l} [GeV]", "p_{T}^{missing} [GeV]", "m_{t} [GeV]", "m_{th} [GeV]", "m_{tl} [GeV]"]
title = ["top_pt", "Mtt", "deltay", "cost", "ytt", "likelihood", "leading_pt", "lepton_pt", "MET_pt", "mass_t", "mass_th", "mass_tl"]
path = "./CG_roots/{}/{}/".format(sg, year)
path_qcd = "./QCD_roots/{}/".format(year)
sys_file = path + title[var] + "_ttbar_" + cut_name + ".root"
qcd_file = path_qcd + title[var] + "_QCD_" + cut_name + ".root"
file = r.TFile(sys_file)
file_qcd = r.TFile(qcd_file)
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
    if "Up" in hist_name:
        nom_name, sys_name = sys_and_nom(hist_name)
        if nom_name not in nom_sys.keys():
            nom_sys[nom_name] = list()
        nom_sys[nom_name].append(sys_name)
    if "pdf" in hist_name:
        nom_name, sys_name = sys_and_nom(hist_name)
        if nom_name not in nom_pdf.keys():
            nom_pdf[nom_name] = list()
        nom_pdf[nom_name].append(sys_name)
names = ["QCD_{}{}".format(sg, cg)  + en + "_smooth", "Ratio_{}{}".format(sg, cg) + en, "Ratio_{}{}".format(sg, cg)  + en + "_smooth",
         "QCD_prompt_{}".format(cg), "QCD_prompt_{}_smooth".format(cg)]
hname = ["QCD", "ratio", "ratio_smooth", "QCD_prompt", "QCD_prompt_smooth"]
if not qMC:
    for i in range(5 if sg == cg else 3):
        hist_map[hname[i]] = file_qcd.Get(names[i])
        hist_map[hname[i]].SetDirectory(0)
        sete0(hist_map[hname[i]])

else:
    if en == "":
        hist_map["QCD"] = hist_map["QCD_MC"]
    else:
        hist_map["QCD"] = hist_map["QCD_MC_En"]
# print(nom_sys)
file.Close()
file_qcd.Close()
# for y in range(2015, 2019):
#     file = r.TFile("./sys_root/{}/".format(y) + title[var] + "_ttbar_" + cut_name + ".root")
#     for p in pro:
#         hist_map[p + "_{}".format(y)] = file.Get(p)
#         hist_map[p + "_{}".format(y)].SetDirectory(0)
#     file.Close()
hdata = hist_map["data_obs"]
hdata.Sumw2(0)
bins = hdata.GetNbinsX()
xdown = hdata.GetXaxis().GetBinLowEdge(1)
xup = hdata.GetXaxis().GetBinUpEdge(bins)
yup = hdata.GetMaximum() * 1.4
bin_len = (xup - xdown) / (1.0 * bins)

CMS.SetExtraText("")
CMS.SetLumi(lumi[year])
CMS.SetEnergy("13")
pro = ["ttbar_ci0000", "Eta", "DYJets", "STop", "WJets", "QCD"]
name = ["t#bar{t}", "#eta_{t}", "DY", "single t", "WJets", "QCD"]
stack = r.THStack("stack", "Stacked")
hist_dict = {list(reversed(name))[i]: hist_map[list(reversed(pro))[i]] for i in range(6)}
ymc = 0
for i in range(6):
    ymc += hist_map[pro[i]].GetMaximum()

# CMS.AppendAdditionalInfo(cut[cut_name])
# CMS.GetcmsCanvasHist(canv.cd(1)).GetYaxis().SetTitleOffset(1.1)
yup = max(hdata.GetMaximum(), ymc) * 1.4
if yup > 100000 and yup < 1000000:
    es = 0.05
else:
    es = 0.35
    
if sg == "A":
    rdn = 0.5
else:
    rdn = 0.0

canv = CMS.cmsDiCanvas("canv", xdown, xup, 0, yup, rdn, 2 - rdn, xtitle[var], "Events","#frac{data}{MC}", square=CMS.kSquare, extraSpace=es)
canv.cd(1)
leg = CMS.cmsLeg(0.78, 0.89 - 0.05 * 7, 0.95, 0.89, textSize=0.04)
CMS.cmsDrawStack(stack, leg, hist_dict, hdata)

hmc = r.TH1D("mc", "", bins, xdown, xup)
hmc.Sumw2()
hdatad = hdata.Clone()
hdatad.SetName("datad")
text = r.TLatex()
form.format_tex(text)
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
# leg.AddEntry(hdata, legendd, "p")
# hdata.Draw("PSame")
# hmcg.Draw("2Same")

# form.format_graph(hmcg, 1)
# form.format_his(hdata, "", 1, 0)

canv.cd(2)
form.format_his(hdatad, xtitle[var], 1, 2)
hdatad.Draw("same")
# hmcdg.Draw("2same")
form.format_graph(hmcdg, 2)
l1 = list()
l2 = list()
for i in range(3):
    l1.append(r.TLine(xdown, rdn + 0.5 * (1 - rdn) * (i + 1), xup, rdn + 0.5 * (1 - rdn) * (i + 1)))
    form.format_line(l1[i])
    l1[i].Draw("same")
for d in range(1, bins):
    l2.append(r.TLine(xdown + d * bin_len, rdn, xdown + d * bin_len, 2 - rdn))
    form.format_line(l2[d - 1])
    l2[d - 1].Draw("same") 
# canv.Print("test.pdf")
if qMC == "":
    canv.Print("./qcd_pdf/{}/{}{}{}/".format(year, sg, cg, en) + title[var] + "_" + cut_name + ".pdf")
else:
    if sg == cg:
        canv.Print("./CG_plots/{}/{}{}/".format(year, sg, en) + title[var] + "_" + cut_name + ".pdf")
    else:
        canv.Print("./CG_plots/{}/{}{}{}/".format(year, sg, cg, en) + title[var] + "_" + cut_name + ".pdf")
if not qMC:
    if hist_map["ratio_smooth"].GetMaximum() == 0 or hist_map["ratio"].GetMaximum() / hist_map["ratio_smooth"].GetMaximum() < 20:
        yupr = hist_map["ratio"].GetMaximum() * 1.5
    else:
        yupr = hist_map["ratio_smooth"].GetMaximum() * 1.5
    if yupr < 0.5 and yupr > 0.005:
        esr = 0.30
    elif yupr < 0.005 and yupr > 0.001:
        esr = 0.40
    else:
        esr = 0.05
        
    canvr = CMS.cmsCanvas("canvr", xdown, xup, 0, yupr, xtitle[var], "Ratio", square=CMS.kSquare, extraSpace=esr)
    canvr.cd()
    CMS.cmsDraw(hist_map["ratio_smooth"], "CP", msize=0.7, lwidth=2, fstyle=0)
    CMS.cmsDraw(hist_map["ratio"], "CP", msize=0.7, lwidth=2, lcolor=2, mcolor=2, fstyle=0)
    
    leg = CMS.cmsLeg(0.68, 0.89 - 0.05 * 2, 0.95, 0.89, textSize=0.04)
    form.format_leg(leg)
    leg.AddEntry(hist_map["ratio"], "QCD MC {}/{}".format(sg, cg), "l")
    leg.AddEntry(hist_map["ratio_smooth"], "smoothed", "l")
    leg.Draw("same")
    canvr.Print("./qcd_pdf/{}/{}{}{}/".format(year, sg, cg, en) + title[var] + "_" + cut_name + "_r.pdf")

    if sg == cg:
        canvp = CMS.cmsCanvas("canvp", xdown, xup, 0, yup, xtitle[var], "Events", square=CMS.kSquare, extraSpace=es)
        canvp.cd()
        CMS.cmsDraw(hist_map["QCD_prompt_smooth"], "CP", msize=0.7, lwidth=2, fstyle=0)
        CMS.cmsDraw(hist_map["QCD_prompt"], "CP", msize=0.7, lwidth=2, lstyle=2, fstyle=0)
        CMS.cmsDraw(hist_map["QCD_MC" + en], "CP", msize=0, lwidth=2, lcolor=2, fstyle=0)
        leg = CMS.cmsLeg(0.60, 0.89 - 0.05 * 2, 0.90, 0.89, textSize=0.04)
        form.format_leg(leg)
        leg.AddEntry(hist_map["QCD_MC" + en], "QCD MC", "l")
        leg.AddEntry(hist_map["QCD_prompt"], "QCD prompt in {}".format(cg), "l")
        leg.AddEntry(hist_map["QCD_prompt_smooth"], "smoothed", "l")
        leg.Draw("same")
        canvp.Print("./CG_plots/{}/{}{}/".format(year, cg, en) + title[var] + "_" + cut_name + "_QP.pdf")
    else:
        canvp = CMS.cmsCanvas("canvp", xdown, xup, 0, max(hist_map["QCD"].GetMaximum(), hist_map["QCD_MC" + en].GetMaximum()) * 1.4, xtitle[var], "Events", square=CMS.kSquare, extraSpace=es)
        canvp.cd()
        CMS.cmsDraw(hist_map["QCD"], "CP", msize=0.7, lwidth=2, fstyle=0)
        CMS.cmsDraw(hist_map["QCD_MC" + en], "CP", msize=0, lwidth=2, lcolor=2, fstyle=0)
        leg = CMS.cmsLeg(0.60, 0.89 - 0.05 * 2, 0.90, 0.89, textSize=0.04)
        form.format_leg(leg)
        leg.AddEntry(hist_map["QCD_MC" + en], "QCD MC", "l")
        leg.AddEntry(hist_map["QCD_prompt"], "QCD derived from {}".format(cg), "l")
        leg.Draw("same")
        canvp.Print("./qcd_pdf/{}/{}{}{}/".format(year, sg, cg, en) + title[var] + "_" + cut_name + "_Q.pdf")


