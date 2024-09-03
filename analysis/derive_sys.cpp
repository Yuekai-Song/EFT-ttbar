#include "prepare/prepare.cpp"
void derive_sys(int i, int year, int varn, TString sg)
{
    var vars[] = {top_pt, Mtt, deltay, cost, ytt, likelihood, MET_pt, mass_th, mass_tl, lepton_pt, leading_pt, mass_t};

    prepare *p = new prepare(i, year, 0, 6, sg);

    p->QCD_dir = Form("./output/%d/", year);

    p->MC_dir = Form("./output/%d/MC/", year);
    p->data_dir = Form("./output/%d/data/", year);
    p->outputDir = Form("./CG_roots/" + sg + "/%d/", year);
    // p->set_bins(false);
    p->set_bins(vars[varn]);
    p->run();
}