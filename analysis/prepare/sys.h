#include <TROOT.h>
#include <TString.h>
#include <vector>
#include <map>
using namespace std;
struct stype
{
    TString title, name, up, dn;
    int type, pros;
};
class sys
{
private:
    sys(int year, TString cut);
    static sys *p;
    map<int, vector<TString>> pro_type;
    
public:
    vector<stype> sys_all;
    static sys *instance(int year, TString cut);
    bool match(stype s, TString pro);
    vector<TString> give_sys_name(const TString file, const TString weight, stype s, TString pro);
};

sys::sys(int year, TString cut)
{
    pro_type.clear();
    pro_type = {{0, {"ttbar_ci0200", "ttbar_ci0100", "ttbar_ci0010", "ttbar_ci0001", "ttbar_ci0000", "DYJets", "STop", "WJets"}},
                {1, {"ttbar_ci0200", "ttbar_ci0100", "ttbar_ci0010", "ttbar_ci0001", "ttbar_ci0000"}},
                {2, {"ttbar_ci0200"}}, {3, {"ttbar_ci0100"}}, {4, {"ttbar_ci0010"}},
                {5, {"ttbar_ci0001"}}, {6, {"ttbar_ci0000"}}, {7, {"DYJets"}}, {8, {"STop"}}, {9, {"WJets"}}};
    sys_all.clear();
    TString lep;
    if (cut.Contains("E"))
        lep = "Elec";
    else
        lep = "Muon";
    sys_all = {{"jes", "jes_Absolute", "jes_AbsoluteUp", "jes_AbsoluteDown", 1, 0},
               {"jes",  Form("jes_Absolute_%d", year), Form("jes_Absolute_%dUp", year), Form("jes_Absolute_%dDown", year), 1, 0},
               {"jes", "jes_FlavorQCD", "jes_FlavorQCDUp", "jes_FlavorQCDDown", 1, 0},
               {"jes", "jes_BBEC1", "jes_BBEC1Up", "jes_BBEC1Down", 1, 0},
               {"jes", "jes_EC2", "jes_EC2Up", "jes_EC2Down", 1, 0},
               {"jes", "jes_HF", "jes_HFUp", "jes_HFDown", 1, 0},
               {"jes", Form("jes_BBEC1_%d", year), Form("jes_BBEC1_%dUp", year), Form("jes_BBEC1_%dDown", year), 1, 0},
               {"jes", Form("jes_EC2_%d", year), Form("jes_EC2_%dUp", year),  Form("jes_EC2_%dDown", year), 1, 0},
               {"jes", "jes_RelativeBal", "jes_RelativeBalUp", "jes_RelativeBalDown", 1, 0},
               {"jes", Form("jes_RelativeSample_%d", year), Form("jes_RelativeSample_%dUp", year), Form("jes_RelativeSample_%dDown", year), 1, 0},
               {"jer", "jer", "jerUp", "jerDown", 1, 0}, 
               {"unclus", "unclus", "unclusUp", "unclusDown", 1, 0},
               {"L1PreFiringWeight_Nom", Form("L1PF_%d", year), "L1PreFiringWeight_Up", "L1PreFiringWeight_Dn", 2, 0},
               {"SF_puid", "SF_puid", "SF_puid_up", "SF_puid_dn", 2, 0},
               {"SF_btag", "SF_btag_jes", "SF_btag_jesup", "SF_btag_jesdn", 2, 0},
               {"SF_btag", "SF_btag_type3", "SF_btag_type3up", "SF_btag_type3dn", 2, 0},
               {"SF_btag", "SF_btag_pileup", "SF_btag_pileupup", "SF_btag_pileupdn", 2, 0},
               {"SF_btag", Form("SF_btag_stat%d", year), "SF_btag_statisticup", "SF_btag_statisticdn", 2, 0},
               {"SF_btag", "SF_ltag", "SF_ltag_up_co", "SF_ltag_down_co", 2, 0},
               {"SF_btag", Form("SF_ltag%d", year), "SF_ltag_up", "SF_ltag_down", 2, 0},
               {"SF_lepton", "SF_" + lep, "SF_lepton_up", "SF_lepton_down", 2, 0},
               {"pu_wt", "PU", "pu_wt_up", "pu_wt_dn", 2, 0},
               {"muR", "muR", "muR_up", "muR_down", 3, 0},
               {"muF", "muF", "muF_up", "muF_down", 3, 0},
               {"muR", "muR1", "muR_up", "muR_down", 3, 7},
               {"muF", "muF1", "muF_up", "muF_down", 3, 7},
               {"muR", "muR2", "muR_up", "muR_down", 3, 8},
               {"muF", "muF2", "muF_up", "muF_down", 3, 8},
               {"muR", "muR3", "muR_up", "muR_down", 3, 9},
               {"muF", "muF3", "muF_up", "muF_down", 3, 9},
               {"ISR", "ISR", "ISR_up", "ISF_down", 3, 0},
               {"FSR", "FSR", "FSR_up", "FSR_down", 3, 0},
               {"nnlo_wt", "nnlo_wt", "nnlo_wt_up", "nnlo_wt_down", 2, 1},
               {"weight_ci0200", "EW_un", "weight_ci0200_EWUp", "weight_ci0200_EWDown", 3, 2},
               {"weight_ci0100", "EW_un", "weight_ci0100_EWUp", "weight_ci0100_EWDown", 3, 3},
               {"weight_ci0010", "EW_un", "weight_ci0010_EWUp", "weight_ci0010_EWDown", 3, 4},
               {"weight_ci0001", "EW_un", "weight_ci0001_EWUp", "weight_ci0001_EWDown", 3, 5},
               {"weight_ci0000", "EW_un", "weight_ci0000_EWUp", "weight_ci0000_EWDown", 3, 6},
               {"mtop", "mtop", "mtop173p5_TuneCP5", "mtop171p5_TuneCP5", 4, 1},
               {"mtop", "mtop3", "mtop175p5_TuneCP5", "mtop169p5_TuneCP5", 4, 1},
               {"hdamp", "hdamp", "hdampUP_TuneCP5", "hdampDOWN_TuneCP5", 4, 1},
               {"TuneCP5", "TuneCP5", "TuneCP5up", "TuneCP5down", 4, 1}};
}
sys *sys::p = nullptr;
sys *sys::instance(int year, TString cut)
{
    if (p == nullptr)
        p = new sys(year, cut);
    return p;
}
bool sys::match(stype s, TString pro)
{
    if (find(pro_type[s.pros].begin(), pro_type[s.pros].end(), pro) == pro_type[s.pros].end())
        return false;
    return true;
}
vector<TString> sys::give_sys_name(const TString file, const TString weight, stype s, TString pro)
{
    TString tree_up, tree_dn, file_up, file_dn, weight_up, weight_dn;
    switch (s.type)
    {
    case 1:
        tree_up = s.up;
        tree_dn = s.dn;
        file_up = file;
        file_dn = file;
        weight_up = weight;
        weight_dn = weight;
        break;
    case 2:
        tree_up = "mytree";
        tree_dn = "mytree";
        file_up = file;
        file_dn = file;
        weight_up = weight;
        weight_up.ReplaceAll(s.title, s.up);
        weight_dn = weight;
        weight_dn.ReplaceAll(s.title, s.dn);
        break;
    case 3:
        tree_up = "mytree";
        tree_dn = "mytree";
        file_up = file;
        file_dn = file;
        weight_up = weight + "*" + s.up;
        weight_dn = weight + "*" + s.dn;
        break;
    case 4:
        tree_up = "mytree";
        tree_dn = "mytree";
        file_up = file;
        file_up.ReplaceAll("TuneCP5", s.up);
        file_dn = file;
        file_dn.ReplaceAll("TuneCP5", s.dn);
        weight_up = weight;
        weight_dn = weight;
        break;
    default:
        break;
    }
    return {file_up, file_dn, tree_up, tree_dn, weight_up, weight_dn};
}