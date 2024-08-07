#include <TROOT.h>
#include <map>
#include <vector>
#include <iostream>
using namespace std;

class settings
{
public:
    const int nsample = 72;
    vector<TString> fileNames;
    map<TString, pair<double, double>> xsection;
    map<TString, pair<int, int>> index_pro = {{"ttbar", pair<int, int>(0, 3)}, {"DYJets", pair<int, int>(3, 11)}, {"STop", pair<int, int>(11, 16)},
                                              {"WJets", pair<int, int>(16, 20)}, {"other", pair<int, int>(0, 20)}, {"QCD_HT", pair<int, int>(44, 53)}, 
                                              {"QCD_EMEn", pair<int, int>(53, 59)}, {"QCD_MuEn", pair<int, int>(59, 69)}, {"Eta", pair<int, int>(69, 72)}};
    TString cut, cut_name, ch, suffix;
    TString data_name = "new_data.root"; 
    vector<int> xyz_bins = {270, 40, 100};
    vector<double> xyz_range = {300, 3000, 0, 4.0, 8, 33};
    double lumi;
    bool ttx;
    int year;
    pair<double, double> xs(TString file);
    double pre_scale = 1;
    void set_suf(TString cg);
    settings(int cut_nums, int year, bool ttxs);
    TString fileName(int i);
    TString dataName();
};
settings::settings(int cut_num, int years, bool ttxs)
{
    ttx = ttxs;
    year = years;
    TString fileName[72] = {"new_TTToSemiLeptonic_TuneCP5_13TeV-powheg.root",
                            "new_TTTo2L2Nu_TuneCP5_13TeV-powheg.root",
                            "new_TTToHadronic_TuneCP5_13TeV-powheg.root",

                            "new_DYJetsToLL_M-50_HT-70to100_TuneCP5_PSweights_13TeV-madgraphMLM.root",
                            "new_DYJetsToLL_M-50_HT-100to200_TuneCP5_PSweights_13TeV-madgraphMLM.root",
                            "new_DYJetsToLL_M-50_HT-200to400_TuneCP5_PSweights_13TeV-madgraphMLM.root",
                            "new_DYJetsToLL_M-50_HT-400to600_TuneCP5_PSweights_13TeV-madgraphMLM.root",
                            "new_DYJetsToLL_M-50_HT-600to800_TuneCP5_PSweights_13TeV-madgraphMLM.root",
                            "new_DYJetsToLL_M-50_HT-800to1200_TuneCP5_PSweights_13TeV-madgraphMLM.root",
                            "new_DYJetsToLL_M-50_HT-1200to2500_TuneCP5_PSweights_13TeV-madgraphMLM.root",
                            "new_DYJetsToLL_M-50_HT-2500toInf_TuneCP5_PSweights_13TeV-madgraphMLM.root",

                            "new_ST_s-channel_4f_leptonDecays_TuneCP5_13TeV-amcatnlo.root",
                            "new_ST_t-channel_antitop_4f_InclusiveDecays_TuneCP5_13TeV-powheg-madspin.root",
                            "new_ST_t-channel_top_4f_InclusiveDecays_TuneCP5_13TeV-powheg-madspin.root",
                            "new_ST_tW_antitop_5f_inclusiveDecays_TuneCP5_13TeV-powheg.root",
                            "new_ST_tW_top_5f_inclusiveDecays_TuneCP5_13TeV-powheg.root",

                            "new_W1JetsToLNu_TuneCP5_13TeV-madgraphMLM.root",
                            "new_W2JetsToLNu_TuneCP5_13TeV-madgraphMLM.root",
                            "new_W3JetsToLNu_TuneCP5_13TeV-madgraphMLM.root",
                            "new_W4JetsToLNu_TuneCP5_13TeV-madgraphMLM.root",

                            "new_TTToSemiLeptonic_mtop173p5_TuneCP5_13TeV-powheg.root",
                            "new_TTTo2L2Nu_mtop173p5_TuneCP5_13TeV-powheg.root",
                            "new_TTToHadronic_mtop173p5_TuneCP5_13TeV-powheg.root",
                            "new_TTToSemiLeptonic_mtop171p5_TuneCP5_13TeV-powheg.root",
                            "new_TTTo2L2Nu_mtop171p5_TuneCP5_13TeV-powheg.root",
                            "new_TTToHadronic_mtop171p5_TuneCP5_13TeV-powheg.root",

                            "new_TTToSemiLeptonic_mtop175p5_TuneCP5_13TeV-powheg.root",
                            "new_TTTo2L2Nu_mtop175p5_TuneCP5_13TeV-powheg.root",
                            "new_TTToHadronic_mtop175p5_TuneCP5_13TeV-powheg.root",
                            "new_TTToSemiLeptonic_mtop169p5_TuneCP5_13TeV-powheg.root",
                            "new_TTTo2L2Nu_mtop169p5_TuneCP5_13TeV-powheg.root",
                            "new_TTToHadronic_mtop169p5_TuneCP5_13TeV-powheg.root",

                            "new_TTToSemiLeptonic_hdampUP_TuneCP5_13TeV-powheg.root",
                            "new_TTTo2L2Nu_hdampUP_TuneCP5_13TeV-powheg.root",
                            "new_TTToHadronic_hdampUP_TuneCP5_13TeV-powheg.root",
                            "new_TTToSemiLeptonic_hdampDOWN_TuneCP5_13TeV-powheg.root",
                            "new_TTTo2L2Nu_hdampDOWN_TuneCP5_13TeV-powheg.root",
                            "new_TTToHadronic_hdampDOWN_TuneCP5_13TeV-powheg.root",

                            "new_TTToSemiLeptonic_TuneCP5up_13TeV-powheg.root",
                            "new_TTTo2L2Nu_TuneCP5up_13TeV-powheg.root",
                            "new_TTToHadronic_TuneCP5up_13TeV-powheg.root",
                            "new_TTToSemiLeptonic_TuneCP5down_13TeV-powheg.root",
                            "new_TTTo2L2Nu_TuneCP5down_13TeV-powheg.root",
                            "new_TTToHadronic_TuneCP5down_13TeV-powheg.root",
                            
                            "new_QCD_HT50to100_TuneCP5_PSWeights_13TeV-madgraph.root",
                            "new_QCD_HT100to200_TuneCP5_PSWeights_13TeV-madgraph.root",
                            "new_QCD_HT200to300_TuneCP5_PSWeights_13TeV-madgraph.root",
                            "new_QCD_HT300to500_TuneCP5_PSWeights_13TeV-madgraph.root",
                            "new_QCD_HT500to700_TuneCP5_PSWeights_13TeV-madgraph.root",
                            "new_QCD_HT700to1000_TuneCP5_PSWeights_13TeV-madgraph.root",
                            "new_QCD_HT1000to1500_TuneCP5_PSWeights_13TeV-madgraph.root",
                            "new_QCD_HT1500to2000_TuneCP5_PSWeights_13TeV-madgraph.root",
                            "new_QCD_HT2000toInf_TuneCP5_PSWeights_13TeV-madgraph.root",

                            "new_QCD_Pt-30to50_EMEnriched_TuneCP5_13TeV.root",
                            "new_QCD_Pt-50to80_EMEnriched_TuneCP5_13TeV.root",
                            "new_QCD_Pt-80to120_EMEnriched_TuneCP5_13TeV.root",
                            "new_QCD_Pt-120to170_EMEnriched_TuneCP5_13TeV.root",
                            "new_QCD_Pt-170to300_EMEnriched_TuneCP5_13TeV.root",
                            "new_QCD_Pt-300toInf_EMEnriched_TuneCP5_13TeV.root",

                            "new_QCD_Pt-30To50_MuEnrichedPt5_TuneCP5_13TeV.root",
                            "new_QCD_Pt-50To80_MuEnrichedPt5_TuneCP5_13TeV.root",
                            "new_QCD_Pt-80To120_MuEnrichedPt5_TuneCP5_13TeV.root",
                            "new_QCD_Pt-120To170_MuEnrichedPt5_TuneCP5_13TeV.root",
                            "new_QCD_Pt-170To300_MuEnrichedPt5_TuneCP5_13TeV.root",
                            "new_QCD_Pt-300To470_MuEnrichedPt5_TuneCP5_13TeV.root",
                            "new_QCD_Pt-470To600_MuEnrichedPt5_TuneCP5_13TeV.root",
                            "new_QCD_Pt-600To800_MuEnrichedPt5_TuneCP5_13TeV.root",
                            "new_QCD_Pt-800To1000_MuEnrichedPt5_TuneCP5_13TeV.root",
                            "new_QCD_Pt-1000_MuEnrichedPt5_TuneCP5_13TeV.root",
                            "new_EtaTToSemileptonic_0J_M-337To349_TuneCP5_13TeV_madgraph.root",
                            "new_EtaTTo2L2Nu_0J_M-337To349_TuneCP5_13TeV_madgraph.root",
                            "new_EtaTToHadronic_0J_M-337To349_TuneCP5_13TeV_madgraph.root"};
    Float_t cross_section[72] = {365.34, 88.29, 377.96,
                                 169.9, 147.4, 41.0, 5.7, 1.4, 0.63, 0.15, 0.0036,
                                 3.36, 136.02, 80.95, 35.6, 35.6,
                                 8927.0, 2809.0, 826.3, 544.3,
                                 355.50, 85.91, 367.78, 375.45, 90.73, 388.41,
                                 336.79, 81.39, 348.42, 396.76, 95.88, 410.47,
                                 365.34, 88.29, 377.96, 365.29, 88.28, 377.90,
                                 365.34, 88.29, 377.96, 365.34, 88.29, 377.96,
                                 186100000.0, 23590000, 1555000, 324500, 30310, 6444, 1127, 109.8, 21.98,
                                 6401000.0, 1993000.0, 364000.0, 66600.0, 16620.0, 1101.0,
                                 1367000.0, 381700.0, 87740.0, 21280.0, 7000.0, 622.6, 58.9, 18.12, 3.318, 1.085,
                                 2.82, 0.683, 2.92};
    Float_t K_Factor[72] = {1.0, 1.0, 1.0,
                            1.23, 1.23, 1.23, 1.23, 1.23, 1.23, 1.23, 1.23,
                            1.0, 1.0, 1.0, 1.0, 1.0,
                            1.21, 1.21, 1.21, 1.21,
                            1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                            1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                            1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                            1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                            1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                            1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                            1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                            1.0, 1.0, 1.0};
    vector<TString> cuts = {"(jet_num == 3 && (!lep_flavour))", "(jet_num >= 4  && (!lep_flavour))",
                            "(jet_num == 3 && lep_flavour)", "(jet_num >= 4 && lep_flavour)"};
    vector<TString> cutsName = {"E_3jets", "E_4jets", "M_3jets", "M_4jets"};
    vector<TString> chs = {"E3j", "E4j", "M3j", "M4j"};
    double lumi_s[4] = {19.5, 16.8, 41.48, 59.83};
    lumi = lumi_s[year - 2015];
    cut = cuts[cut_num];
    cut_name = cutsName[cut_num];
    ch = chs[cut_num];
    if (year == 2018)
        cut += "*(lep_flavour || ((!lep_flavour) && lepton_pt>34)) * (MtW<=140)";
    else if (year == 2017)
        cut += "*(lep_flavour || ((!lep_flavour) && lepton_pt>37)) * (MtW<=140)";
    else
        cut += "*(MtW<=140)";
    for (int i = 0; i < nsample; i++)
    {
        fileNames.push_back(fileName[i]);
        xsection[fileName[i]] = pair<double, double>(cross_section[i], K_Factor[i]);
    }
    if (ttx)
        cut += "*(D_nu < 150)*(nBtag == 2)";

}
void settings::set_suf(TString cg)
{
    if (ttx)
    {
        if ((cg == "B" || cg == "D") && cut.Contains("*(nBtag == 2)"))
            cut.ReplaceAll("*(nBtag == 2)", "");
        else if ((cg == "A" || cg == "C" || cg == "") && (!cut.Contains("*(nBtag == 2)")))
            cut += "*(nBtag == 2)";
    }
    
    Double_t pre_scale_year[][2] = {{369.84, 130.38}, {1570.17, 162.22}, {1085.83, 224.41}, {1536.28, 474.95}};
    if (cg != "B" && cg != "A" && cg != "")
    {
        if (cut_name.Contains("M"))
            pre_scale = pre_scale_year[year - 2015][1];
        else
            pre_scale = pre_scale_year[year - 2015][0];
    }
    else
        pre_scale = 1;
    TString in = "";
    if (ttx)
        in = "_ttx";
    if (cg != "")
        suffix =  "*_" + cg + in;
    else
        suffix = "*";
}
TString settings::fileName(int i)
{
    TString temp = fileNames[i];
    return temp.ReplaceAll(".root", suffix + ".root");
}
pair<double, double> settings::xs(TString file)
{
    return xsection[file.ReplaceAll(suffix, "")];
}
TString settings::dataName()
{
    TString temp = data_name;
    return temp.ReplaceAll(".root", suffix + ".root");
}