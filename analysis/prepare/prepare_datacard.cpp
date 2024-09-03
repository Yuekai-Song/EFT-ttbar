#include "prepare.cpp"
void prepare_datacard(int i, int year, int option, bool is_ttx)
{
    prepare *p = new prepare(i, year, is_ttx, option, "A");
    p->set_bins(Mtt, ytt, likelihood);
    p->MC_dir = Form("../output/%d/MC/", year);
    p->data_dir = Form("../output/%d/data/", year);
    if (is_ttx)
        p->outputDir = Form("../output/%d/datacard_ttx/", year);
    else
        p->outputDir = Form("../output/%d/datacard/", year);

    p->run();
    delete p;
}