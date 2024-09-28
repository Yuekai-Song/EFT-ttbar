#include <TROOT.h>
#include <TChain.h>
#include <TString.h>
#include <TLorentzVector.h>
#include <TH3D.h>
#include <iostream>
#include <vector>
#include <map>
#include <regex>
#include <string>
#include <sstream>
using namespace std;

struct var
{
    TString name;
    double xlow, xup;
    int bins;
};

var top_pt = {"top_pt", 50, 500, 24};
var top_pt_fine = {"top_pt", 0, 1000, 100};
var Mtt = {"Mtt", 300, 1500, 24};
var Mtt_fine = {"Mtt", 300, 3000, 270};
var deltay = {"deltay", -4, 4, 20};
var abs_deltay = {"abs_deltay", 0, 4, 40};
var cost = {"cost", -1, 1, 20};
var abs_cost = {"abs_cost", 0, 1, 10};
var ytt = {"ytt", -3, 3, 30};
var abs_ytt = {"abs_ytt", 0, 3, 30};
var likelihood = {"likelihood", 13, 50, 37};
var lepton_pt = {"lepton_pt", 30, 250, 22};
var leading_pt = {"leading_pt", 30, 400, 37};
var mass_t = {"mass_t", 50, 450, 40};
var mass_th = {"mass_th", 50, 450, 40};
var mass_tl = {"mass_tl", 50, 450, 40};
var MET_pt = {"MET_pt", 0, 400, 40};

class tree_draw;
class settings
{    
public:
    const int nsample = 72;
    vector<TString> fileNames;
    map<TString, pair<double, double>> xsection;
    map<TString, pair<int, int>> index_pro = {{"ttbar", pair<int, int>(0, 3)}, {"DYJets", pair<int, int>(3, 11)}, {"STop", pair<int, int>(11, 16)},
                                              {"WJets", pair<int, int>(16, 20)}, {"other", pair<int, int>(0, 20)}, {"QCD_HT", pair<int, int>(44, 53)}, 
                                              {"QCD_EMEn", pair<int, int>(53, 59)}, {"QCD_MuEn", pair<int, int>(59, 69)}, {"Eta", pair<int, int>(69, 72)}};
    TString cut_name, ch, suffix;
    TString data_name = "new_data.root"; 
    vector<int> xyz_bins = {270, 40, 100};
    vector<double> xyz_range = {300, 3000, 0, 4.0, 8, 33};
    double lumi;
    const bool ttx;
    int year;
    TString cg = "";
    pair<double, double> xs(TString file) const;
    double pre_scale = 1;
    void set_suf(TString cg);
    settings(int cut_nums, int year, bool ttxs);
    TString fileName(int i);
    TString dataName();
};

settings::settings(int cut_num, int years, bool ttxs) : ttx(ttxs)
{
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
    vector<TString> cutsName = {"E_3jets", "E_4jets", "M_3jets", "M_4jets"};
    vector<TString> chs = {"E3j", "E4j", "M3j", "M4j"};
    double lumi_s[4] = {19.5, 16.8, 41.48, 59.83};
    lumi = lumi_s[year - 2015];
    cut_name = cutsName[cut_num];
    ch = chs[cut_num];

    for (int i = 0; i < nsample; i++)
    {
        fileNames.push_back(fileName[i]);
        xsection[fileName[i]] = pair<double, double>(cross_section[i], K_Factor[i]);
    }

}
void settings::set_suf(TString cg_n)
{
    cg = cg_n;
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
        suffix = "*_" + cg + in;
    else
        suffix = "*";
}
TString settings::fileName(int i)
{
    TString temp = fileNames[i];
    return temp.ReplaceAll(".root", suffix + ".root");
}
pair<double, double> settings::xs(TString file) const
{
    return xsection.at(file.ReplaceAll(suffix, ""));
    // return {xyz_bins[1], xyz_bins[2]};
}
TString settings::dataName()
{
    TString temp = data_name;
    return temp.ReplaceAll(".root", suffix + ".root");
}

class tree_draw
{
    friend class settings;

public:
    TChain mytree;
    Bool_t HLT_Ele35_WPTight_Gsf;
    Float_t top_px, top_py, top_pz, top_E;
    Float_t atop_px, atop_py, atop_pz, atop_E;
    Float_t lepton_pt;
    Int_t  lep_c;
    Float_t D_nu;
    Bool_t lep_flavour;
    UInt_t jet_num, nBtag;
    Float_t MET_pt;
    Double_t likelihood;
    Float_t *extra;
    Float_t jet_pt[20];
    // Double_t pu_wt = 1, nnlo_wt = 1;
    Float_t LHEPdfWeight[103];
    vector<int> index = {};
    int extra_size = 0;
    Float_t extra_n;
    void setadd();
    const settings scut;
    Float_t calculate(int entry, TString var);
    Float_t get_weight(int entry);
    Float_t gb_w(TString file);
    Float_t get_cut();
public:
    tree_draw(TString name, TString files, settings scuts, string extra1 = "", Float_t extra2 = 1);
    void draw(TH1D *h1, TString var);
    void draw(TH2D *h1, TString varx, TString vary);
    void draw(TH3D *h1, TString varx, TString vary, TString varz);
};

Float_t tree_draw::get_cut()
{
    bool cut_v = true;
    if (scut.cut_name.Contains("3jets"))
        cut_v *= (jet_num == 3);
    else
        cut_v *= (jet_num >= 4);
    
    if (scut.cut_name.Contains("E"))
    {
        cut_v *= !lep_flavour;
        if (scut.year == 2018)
            cut_v *= (lepton_pt > 34);
        else if (scut.year == 2017)
        {
            cut_v *= (lepton_pt > 37);
            if (scut.cg == "A" || scut.cg == "B")
                cut_v *= HLT_Ele35_WPTight_Gsf;
        }
    }
    else
        cut_v *= lep_flavour;
    if (scut.ttx)
    {
        cut_v *= (D_nu < 150);
        if (scut.cg == "A" || scut.cg == "C" || scut.cg == "")
            cut_v *=  (nBtag == 2);
    }
    return cut_v;
}

void tree_draw::setadd()
{
    mytree.SetBranchAddress("top_px", &top_px);
    mytree.SetBranchAddress("top_py", &top_py);
    mytree.SetBranchAddress("top_pz", &top_pz);
    mytree.SetBranchAddress("top_E", &top_E);
    mytree.SetBranchAddress("atop_px", &atop_px);
    mytree.SetBranchAddress("atop_py", &atop_py);
    mytree.SetBranchAddress("atop_pz", &atop_pz);
    mytree.SetBranchAddress("atop_E", &atop_E);
    mytree.SetBranchAddress("lepton_charge", &lep_c);
    mytree.SetBranchAddress("jet_num", &jet_num);
    mytree.SetBranchAddress("nBtag", &nBtag);
    mytree.SetBranchAddress("lepton_pt", &lepton_pt);
    mytree.SetBranchAddress("lep_flavour", &lep_flavour);
    mytree.SetBranchAddress("likelihood", &likelihood);
    mytree.SetBranchAddress("jet_pt", jet_pt);
    mytree.SetBranchAddress("MET_pt", &MET_pt);
    if (scut.cut_name.Contains("E") && scut.year == 2017 && (scut.cg == "A" || scut.cg == "B"))
        mytree.SetBranchAddress("HLT_Ele35_WPTight_Gsf", &HLT_Ele35_WPTight_Gsf);
    if (scut.ttx)
        mytree.SetBranchAddress("D_nu", &D_nu);

}
tree_draw::tree_draw(TString name, TString files, settings scuts, string extra1 = "", Float_t extra2 = 1) : scut(scuts)
{
    mytree.SetName(name);
    mytree.Add(files);
    setadd();
    stringstream ss(extra1);
    string token;
    std::vector<TString> tokens;
    std::regex re("\\[(\\d+)\\]");
        
    while (std::getline(ss, token, '*'))
    {
        if (!TString(token).Contains("LHEPdfWeight"))
            tokens.push_back(token);
        else
        {
            std::smatch match;
            if (std::regex_search(token, match, re))
                index.push_back(std::stoi(match[1]));
            else
                cout << "LHEPdfWeight no index" << endl;
        }
    }
    if (index.size() != 0)
        mytree.SetBranchAddress("LHEPdfWeight", LHEPdfWeight);
    extra_size = tokens.size();
    if (extra_size >  0)
        extra = new Float_t[extra_size];
    for (int i = 0; i < extra_size; i++)
        mytree.SetBranchAddress(tokens[i], &extra[i]);
    extra_n = extra2 * gb_w(files);
}
Float_t tree_draw::get_weight(int entry)
{
    mytree.GetEntry(entry);
    Float_t weight = get_cut();
    for (int i = 0; i < extra_size; i++)
        weight *= extra[i];
    weight *= extra_n;
    for (int i = 0; i < index.size(); i++)
        weight *= LHEPdfWeight[i];
    return weight;
}
Float_t tree_draw::gb_w(TString file)
{
    if (!file.Contains("data"))
    {
        string rfile(file);
        int pos = rfile.rfind('/');
        if (pos != std::string::npos)
            rfile = rfile.substr(pos + 1);
        auto c0 = new TCanvas("c0", "c0", 8, 30, 600, 600);
        TChain *tree0 = new TChain("rawtree");
        tree0->Add(file);
        TH1D *nmc = new TH1D("nmc", "", 50, 0, 100);
        nmc->Sumw2();
        c0->cd();
        tree0->Draw("nJet>>nmc", "Generator_weight");
        double cross_section = scut.xs(rfile).first;
        double K_Factor = scut.xs(rfile).second;
        double weight = cross_section * scut.lumi / (nmc->GetSumOfWeights()) * K_Factor * 1000;
        delete tree0;
        delete nmc;
        delete c0;
        return weight;
    }
    return scut.pre_scale;
    
}
Float_t tree_draw::calculate(int entry, TString var)
{
    double mass_tt, rapidity_tt, top_pt, cost, ytt, mt, mth, mtl;
    TLorentzVector mom_top;
    TLorentzVector mom_atop;
    mytree.GetEntry(entry);
    mom_top.SetPxPyPzE(top_px, top_py, top_pz, top_E);
    mom_atop.SetPxPyPzE(atop_px, atop_py, atop_pz, atop_E);

    TLorentzVector p4_top_cms = mom_top;
    TLorentzVector p4_cms_lab = mom_top + mom_atop;
    TVector3 p3_cms = p4_cms_lab.BoostVector();
    p4_top_cms.Boost(-p3_cms);
    TVector3 p3_top_cms = p4_top_cms.Vect();
    TVector3 p3_cms_lab = p4_cms_lab.Vect();


    cost = p3_top_cms.Dot(p3_cms_lab) / (p3_top_cms.Mag() * p3_cms_lab.Mag());
    mass_tt = p4_cms_lab.M();
    rapidity_tt = mom_top.Rapidity() - mom_atop.Rapidity();
    top_pt = mom_top.Pt();
    ytt = p4_cms_lab.Rapidity();
    mt = mom_top.M();
    if (lep_c > 0)
    {
        mtl = mt;
        mth = mom_atop.M();
    }
    else
    {
        mth = mt;
        mtl = mom_atop.M();
    }
    
    map<TString, double> val = {{"Mtt", mass_tt},
                                {"deltay", rapidity_tt},
                                {"top_pt", top_pt},
                                {"cost", cost},
                                {"ytt", ytt},
                                {"likelihood", likelihood},
                                {"mass_t", mt},
                                {"mass_th", mth},
                                {"mass_tl", mtl},
                                {"MET_pt", MET_pt},
                                {"lepton_pt", lepton_pt},
                                {"leading_pt", jet_pt[0]},
                                {"abs_cost", fabs(cost)},
                                {"abs_ytt",fabs(ytt)},
                                {"abs_deltay", fabs(rapidity_tt)}};
    return val[var];
}
void tree_draw::draw(TH1D *h1, TString var)
{
    for(int i = 0; i < mytree.GetEntries(); i++)
        h1->Fill(calculate(i, var), get_weight(i));
}
void tree_draw::draw(TH2D *h1, TString varx, TString vary)
{
    for(int i = 0; i < mytree.GetEntries(); i++)
        h1->Fill(calculate(i, varx), calculate(i, vary), get_weight(i));
}
void tree_draw::draw(TH3D *h1, TString varx, TString vary, TString varz)
{
    for(int i = 0; i < mytree.GetEntries(); i++)
    {
        h1->Fill(calculate(i, varx), calculate(i, vary), calculate(i, varz),  get_weight(i));
        // cout << calculate(i, varx) << " " << calculate(i, vary) << " " << calculate(i, varz) << endl;
    }
}