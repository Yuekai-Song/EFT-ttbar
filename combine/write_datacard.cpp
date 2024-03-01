#include "write.cpp"
void write_datacard(TString datacard_name, TString cut_name, int year, TString type, TString dir){
    vector<TString> sys_saved;
    bool lnN_bg = false;
    vector<TString> lnNed_sys;//intersection with sys_saved
    if (type.Contains("lnN_bg"))
        lnN_bg = true;
    
    if (type.Contains("all") && !type.Contains("but_pdf"))
    {
        sys_saved = {"jes_Absolute", Form("jes_Absolute_%d", year), "jes_FlavorQCD", "jes_BBEC1", 
                    "jes_EC2", "jes_HF", Form("jes_BBEC1_%d", year), Form("jes_EC2_%d", year), 
                    "jes_RelativeBal", Form("jes_RelativeSample_%d", year), "jer", "unclus", 
                    "SF_Muon", "SF_Elec", Form("L1PF_%d", year), "PU", "muR", "muF", 
                    "SF_btag_type3", "SF_btag_pileup", "SF_btag_jes", Form("SF_btag_stat%d", year),
                    "SF_ltag", Form("SF_ltag%d", year), 
                    "muR1", "muF1", "muR2", "muF2", "muR3", "muF3", 
                    "ISR", "FSR", "mtop3", "hdamp", "TuneCP5", "pdfD", "pdfS", "pdfW", 
                    "nnlo_wt", "EW_un", "alphas", "alphas1", "alphas3", "qcdnE", "qcdnM", "cms_lumi", 
                    "pdf_w13", "pdf_w11", "pdf_w12", "pdf_w16", "pdf_w25", "pdf_w22", "pdf_w2", "pdf_w3", "pdf_w26", "pdf_w35"};
                    //"pdf_w20", "pdf_w18", "pdf_w8", "pdf_w9", "pdf_w30"};
                    //Form("qshape_mu_3J_%d", year), Form("qshape_mu_4J_%d", year), Form("qshape_elec_3J_%d", year), Form("qshape_elec_4J_%d", year)};
                    //Form("qnorm_mu_3J_%d", year), Form("qnorm_mu_4J_%d", year), Form("qnorm_elec_3J_%d", year), Form("qnorm_elec_4J_%d", year)}; //mtop to mtop3
        lnNed_sys = {Form("qnorm_mu_3J_%d", year), Form("qnorm_mu_4J_%d", year), "pdf_w11", "pdf_w16", "pdf_w22", "pdf_w2", "pdf_w3", "pdf_w35", 
                     "pdf_w18", "pdf_w8", "pdf_w9", "pdf_w30", "pdf_w24", "pdf_w5", "pdf_w26", "pdf_w28", 
                     "pdf_w14", "pdf_w19", "pdf_w37", "pdf_w45", "pdf_w10", "pdf_w77", "pdf_w4", "pdf_w38", "pdf_w39", "pdf_w36"};
    }
    else if (type.Contains("pdf_only"))
    {
        for (int i = 0; i < 100; i++)
            sys_saved.push_back(Form("pdf_w%d", i));
    }
    else if (type.Contains("mtop3_only"))
        sys_saved = {"mtop3"};
    else if (type == "stat_only")
        sys_saved = {"stat"};
    vector<TString> sys_of_shapeU = {"mtop", "mtop3", Form("qnorm_elec_3J_%d", year), Form("qnorm_elec_4J_%d", year)};
    map<int, TString> sys_lumi_year = {{2015, "1.012"}, {2016, "1.012"}, {2017, "1.023"}, {2018, "1.025"}};
    write(datacard_name, dir, cut_name, year, lnN_bg, lnNed_sys, sys_saved, sys_of_shapeU);
}