#include "write.cpp"
void write_datacard(TString datacard_name, TString cut_name, int year, TString dir){
    vector<TString> sys_saved;
    bool lnN_bg = false;
    if (dir.Contains("lnN_bg"))
        lnN_bg = true;
    if (dir == "all")
        sys_saved = {"jes_Absolute", Form("jes_Absolute_%d", year), "jes_FlavorQCD", "jes_BBEC1", 
                    "jes_EC2", "jes_HF", Form("jes_BBEC1_%d", year), Form("jes_EC2_%d", year), 
                    "jes_RelativeBal", Form("jes_RelativeSample_%d", year), "jer", "unclus", 
                    "SF_Muon", "SF_Elec", "L1PF", "PU", "muR", "muF", 
                    "SF_btag", Form("SF_btag%d", year), "SF_ltag", Form("SF_ltag%d", year), 
                    "muR1", "muF1", "muR2", "muF2", "muR3", "muF3",
                    "ISR", "FSR", "mtop3", "hdamp", "TuneCP5",
                    "nnlo_wt", "EW_un", "pdf_w", "alphas", "alphas1", "alphas3", "qcds"}; //mtop to mtop3
    else if (dir.Contains("all_but_pdf"))
        sys_saved = {"jes_Absolute", Form("jes_Absolute_%d", year), "jes_FlavorQCD", "jes_BBEC1", 
                    "jes_EC2", "jes_HF", Form("jes_BBEC1_%d", year), Form("jes_EC2_%d", year), 
                    "jes_RelativeBal", Form("jes_RelativeSample_%d", year), "jer", "unclus", 
                    "SF_Muon", "SF_Elec", "L1PF", "PU", "muR", "muF", 
                    "SF_btag", Form("SF_btag%d", year), "SF_ltag", Form("SF_ltag%d", year), 
                    "muR1", "muF1", "muR2", "muF2", "muR3", "muF3",
                    "ISR", "FSR", "mtop3", "hdamp", "TuneCP5",
                    "nnlo_wt", "EW_un", "pdf_w29", "alphas", "alphas1", "alphas3", "qcds"};
    else if (dir == "mtop_only")
        sys_saved = {"mtop"};
    else if (dir == "mtop3_only")
        sys_saved = {"mtop3"};
    vector<TString> sys_of_shapeU = {"mtop", "mtop3"};
    write(datacard_name, dir, cut_name, year, lnN_bg, sys_saved, sys_of_shapeU);
}