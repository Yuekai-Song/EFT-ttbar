#include "prepare/prepare.cpp"
void derive_sys(int i, int year, int var, bool is_ttx)
{
    prepare *p = new prepare(i, year, is_ttx, 6);
    TString title[] = {"likelihood", "mass_thad", "mass_tlep", "mass_wlep", "mass_whad", "lepton_pt", "leading_pt", "jet_num", "top_pt", "Mtt_un", "Mtt", "deltay"};
    TString xvars[] = {"likelihood", "mass_thad/corr_f", "mass_tlep", "mass_wlep", "mass_whad", "lepton_pt", "jet_pt[0]", "jet_num", "rectop_pt", "mass_tt_uncorr", "mass_tt", "rapidity_tt"};
    Double_t xup[] = {33, 450, 450, 140, 140, 250, 400, 8, 500, 700, 700, 4};
    Double_t xdown[] = {10, 50, 50, 50, 50, 30, 30, 3, 50, 300, 300, -4};
    Int_t bins[] = {46, 40, 40, 36, 36, 22, 20, 5, 20, 40, 40, 32};

    p->QCD_dir = Form("../QCD_analysis/output/%d/", year);
    if (is_ttx)
    {
        p->MC_dir = Form("./output/%d/MC_ttx/", year);
        p->data_dir = Form("./output/%d/data_ttx/", year);
        p->outputDir = Form("./sys_root_ttx/%d/", year);
    }
    else
    {
        p->MC_dir = Form("./output/%d/MC/", year);
        p->data_dir = Form("./output/%d/data/", year);
        p->outputDir = Form("./sys_root/%d/", year);
    }
    // p->set_bins(false);
    p->set_bins(xvars[var], title[var], bins[var], xdown[var], xup[var]);
    p->run();
}