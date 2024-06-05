#include "prepare.cpp"
void prepare_datacard(int i, int year, int option, bool is_ttx, bool is_corr = false)
{
    prepare *p = new prepare(i, year, is_ttx, option);
    p->set_bins(is_corr);
    p->QCD_dir = Form("../../QCD_analysis/output/%d/", year);
    if(is_ttx)
    {
        p->MC_dir = Form("../output/%d/MC_ttx/", year);
        p->data_dir = Form("../output/%d/data_ttx/", year);
        if (!is_corr)
            p->outputDir = Form("../output/%d/datacard_ttx/", year);
        else
            p->outputDir = Form("../output/%d/datacard_ttx_corr/", year);
    }
    else
    {
        p->MC_dir = Form("../output/%d/MC/", year);
        p->data_dir = Form("../output/%d/data/", year);
        if (!is_corr)
            p->outputDir = Form("../output/%d/datacard/", year);
        else
            p->outputDir = Form("../output/%d/datacard_corr/", year);
    }
    p->run();
    delete p;
}