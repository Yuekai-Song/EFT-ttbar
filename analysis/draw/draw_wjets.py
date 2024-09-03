import ROOT as r
import os, sys
import cmsstyle as CMS
import math
import include.format as form

def sete0(h1):
    for i in range(h1.GetNbinsX() + 2):
        h1.SetBinError(i, 0)
def get_esl(yup):
    if yup > 100000 and yup < 1000000:
        es = 0.05
    else:
        es = 0.35
    return es
def get_ess(yup):
    if yup < 0.5 and yup > 0.005:
        esr = 0.30
    elif yup < 0.005 and yup > 0.001:
        esr = 0.40
    else:
        esr = 0.05
    return esr

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
set_cg = sys.argv[4]
en = sys.argv[5]

legendd = "data"
lumi = {2015: 19.5, 2016: 16.8, 2017: 41.48, 2018: 59.83, 2024: 138}
cut = {"E_3jets": "e, 3jets", "E_4jets": "e, #geq4jets", "M_3jets": "#mu, 3jets", "M_4jets": "#mu, #geq4jets"}
xtitle = ["p_{T}^{t} [GeV]", "M_{t#bar{t}} [GeV]", "#Deltay_{t#bar{t}}", "cos(#theta)", "y_{t#bar{t}}", "-2lnL", "p_{T}^{leading jet} [GeV]", "p_{T}^{l} [GeV]", "p_{T}^{missing} [GeV]", "m_{t} [GeV]", "m_{th} [GeV]", "m_{tl} [GeV]"]
title = ["top_pt", "Mtt", "deltay", "cost", "ytt", "likelihood", "leading_pt", "lepton_pt", "MET_pt", "mass_t", "mass_th", "mass_tl"]
path = "./CG_roots/A/{}/".format(year)
path_wjets = "./WJets_roots/{}/".format(year)
sys_file = path + title[var] + "_ttbar_" + cut_name + ".root"
wjets_file = path_wjets + title[var] + "_" + cut_name + "_" + set_cg + en + ".root"
file = r.TFile(sys_file)
file_wjets = r.TFile(wjets_file)
hist_map = dict()
nom_sys = dict()
nom_pdf = dict()
for key in file_wjets.GetListOfKeys():
    hist = key.ReadObj()
    hist_name = hist.GetName()
    hist_map[hist_name] = hist

if en == "":
    qcd_MC = "QCD_MC"
else:
    qcd_MC = "QCD_MC_En"
names = ["ttbar_ci0000", "DYJets", "STop", "Eta", "data_obs"]
for i in range(5):
    hist_map[names[i]] = file.Get(names[i])
hist_map["WJets_MC"] = file.Get("WJets")
hist_map["QCD_MC"] = file.Get(qcd_MC)

for name in hist_map.keys():
    hist_map[name].SetDirectory(0)
    if "MC" not in name:
        sete0(hist_map[name])

# print(nom_sys)
file.Close()
file_wjets.Close()
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
pro = [["ttbar_ci0000", "Eta", "DYJets", "STop", "WJets", "QCD"], ["ttbar_ci0000", "Eta", "DYJets", "STop", "WJets", "QCD_alt"]]

pdf_name = ["", "_alt"]
name = ["t#bar{t}", "#eta_{t}", "DY", "single t", "WJets", "QCD"]
for num in range(2):
    stack = r.THStack("stack", "Stacked")
    hist_dict = {list(reversed(name))[i]: hist_map[list(reversed(pro[num]))[i]] for i in range(6)}
    # print(hist_dict["t#bar{t}"].GetMaximum())
    ymc = 0
    for i in range(6):
        # print(pro[num][i])
        # print (hist_map[pro[num][i]].GetMaximum())
        ymc += hist_map[pro[num][i]].GetMaximum()

    # CMS.AppendAdditionalInfo(cut[cut_name])
    # CMS.GetcmsCanvasHist(canv.cd(1)).GetYaxis().SetTitleOffset(1.1)
    yup = max(hdata.GetMaximum(), ymc) * 1.4
    rdn = 0.5

    canv = CMS.cmsDiCanvas("canv", xdown, xup, 0, yup, rdn, 2 - rdn, xtitle[var], "Events","#frac{data}{MC}", square=CMS.kSquare, extraSpace=get_esl(yup))
    canv.cd(1)
    leg = CMS.cmsLeg(0.78, 0.89 - 0.05 * 7, 0.95, 0.89, textSize=0.04)
    CMS.cmsDrawStack(stack, leg, hist_dict, hdata)

    hmc = r.TH1D("mc_{}".format(num), "", bins, xdown, xup)
    hmc.Sumw2()
    hdatad = hdata.Clone()
    hdatad.SetName("datad")
    text = r.TLatex()
    form.format_tex(text)
    text.DrawLatex(0.6, 0.8, cut[cut_name])
    for i in range(6):
        hmc.Add(hist_map[list(reversed(pro[num]))[i]])
    
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
    canv.Print("./wjets_pdf/{}{}/{}/".format(set_cg, en, year) + title[var] + "_" + cut_name + pdf_name[num] + ".pdf")
    del canv


pros = ["WJets", "QCD"]
legs = ["A/B", "BC/D^{2}"]
alt_leg = ["C/D", "A/" + set_cg]
high = [3, 5]
if set_cg == "C":
    legs[1] = "B/D"
for i in range(2):
    pro = pros[i]
    ori_max = max(hist_map["Ratio_" + pro + "_ori"].GetMaximum(), hist_map["Ratio_" + pro + "_alt_ori"].GetMaximum())
    smo_max = max(hist_map["Ratio_" + pro].GetMaximum(), hist_map["Ratio_" + pro + "_alt"].GetMaximum())
    if ori_max / smo_max > 20:
        yupr = smo_max * 1.5
    else:
        yupr = ori_max * 1.5

    canvr = CMS.cmsCanvas("canvr_" + pro, xdown, xup, 0, yupr, xtitle[var], "Ratio", square=CMS.kSquare, extraSpace=get_ess(yupr))
    canvr.cd()
    CMS.cmsDraw(hist_map["Ratio_" + pro], "CP", msize=0.7, lwidth=2, fstyle=0)
    CMS.cmsDraw(hist_map["Ratio_" + pro + "_ori"], "CP", msize=0.7, lwidth=2, lstyle=2, fstyle=0)
    leg = CMS.cmsLeg(0.55, 0.89 - 0.05 * high[i], 0.95, 0.89, textSize=0.04)
    form.format_leg(leg)
    leg.AddEntry(hist_map["Ratio_" + pro + "_ori"], "transfer factor of " + legs[i], "l")
    leg.AddEntry(hist_map["Ratio_" + pro], "smoothed", "l")
    CMS.cmsDraw(hist_map["Ratio_" + pro + "_alt"], "CP", msize=0.7, lwidth=2, lcolor=2, mcolor=2, fstyle=0)
    CMS.cmsDraw(hist_map["Ratio_" + pro + "_alt_ori"], "CP", msize=0.7, lwidth=2, lstyle=2, lcolor=2, mcolor=2, fstyle=0)
    leg.AddEntry(hist_map["Ratio_" + pro + "_alt_ori"], "transfer factor of " + alt_leg[i], "l")
    leg.AddEntry(hist_map["Ratio_" + pro + "_alt"], "smoothed", "l")
    leg.Draw("same")
    canvr.Print("./wjets_pdf/{}{}/{}/".format(set_cg, en, year) + title[var] + "_" + cut_name + "_r" + pro + ".pdf")

    yup = hist_map[pro + "_prompt_ori"].GetMaximum() * 1.4
    canvp = CMS.cmsCanvas("canvp_" + pro, xdown, xup, 0, hist_map[pro + "_prompt_ori"].GetMaximum() * 1.4, xtitle[var], "Events", square=CMS.kSquare, extraSpace=get_esl(yup))
    canvp.cd()
    CMS.cmsDraw(hist_map[pro + "_prompt"], "CP", msize=0.7, lwidth=2, fstyle=0)
    CMS.cmsDraw(hist_map[pro + "_prompt_ori"], "CP", msize=0.7, lwidth=2, lstyle=2, fstyle=0)
    leg = CMS.cmsLeg(0.60, 0.89 - 0.05 * high[i], 0.90, 0.89, textSize=0.04)
    form.format_leg(leg)
    leg.AddEntry(hist_map[pro + "_prompt_ori"], "data - other MC", "l")
    leg.AddEntry(hist_map[pro + "_prompt"], "smoothed", "l")
    leg.Draw("same")
    canvp.Print("./wjets_pdf/{}{}/{}/".format(set_cg, en, year) + title[var] + "_" + cut_name + "_QP" + pro + ".pdf")

    yup = max(hist_map[pro + "_MC"].GetMaximum(), hist_map[pro].GetMaximum(), hist_map[pro + "_alt"].GetMaximum(),
              hist_map[pro + "_norm"].GetMaximum(), hist_map[pro + "_norm_alt"].GetMaximum()) * 1.4
    canvc = CMS.cmsCanvas("canvc_"  + pro, xdown, xup, 0, yup, xtitle[var], "Events", square=CMS.kSquare, extraSpace=get_esl(yup))
    canvc.cd()
    CMS.cmsDraw(hist_map[pro + "_MC"], "CP", msize=0.0, lwidth=2, lcolor=2, mcolor=2, fstyle=0)
    CMS.cmsDraw(hist_map[pro], "CP", msize=0.7, lwidth=2, fstyle=0)
    CMS.cmsDraw(hist_map[pro + "_norm"], "CP", msize=0.7, lwidth=2, lstyle=2, fstyle=0)
    leg = CMS.cmsLeg(0.55, 0.89 - 0.05  * high[i], 0.95, 0.89, textSize=0.04)
    form.format_leg(leg)
    leg.AddEntry(hist_map[pro + "_MC"], "MC", "l")
    leg.AddEntry(hist_map[pro], "derived using shape " + legs[i], "l")
    leg.AddEntry(hist_map[pro + "_norm"], "derived using norm " + legs[i], "l")
    CMS.cmsDraw(hist_map[pro + "_alt"], "CP", msize=0.7, lwidth=2, lcolor=4, mcolor=4, fstyle=0)
    CMS.cmsDraw(hist_map[pro + "_norm_alt"], "CP", msize=0.7, lwidth=2, lstyle=2, lcolor=4, mcolor=4, fstyle=0)
    leg.AddEntry(hist_map[pro + "_alt"], "derived using shape " + alt_leg[i], "l")
    leg.AddEntry(hist_map[pro + "_norm_alt"],"derived using norm " + alt_leg[i], "l")
    leg.Draw("same")
    canvc.Print("./wjets_pdf/{}{}/{}/".format(set_cg, en, year) + title[var] + "_" + cut_name + "_com" + pro + ".pdf")
    


