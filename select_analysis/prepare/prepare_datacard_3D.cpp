#include "prepare_3D.cpp"
#include "settings.h"
void prepare_datacard_3D(int i, int year, int option, bool is_ttx, bool is_corr = false)
{
    settings s(year, is_ttx, "A");
    cout << "begin " << s.cutsName[i] << ":" << endl;
    prepare_3D *p = new prepare_3D(s.cuts[i], s.cutsName[i], year, s.xyz_bins, s.xyz_range, option, is_ttx, is_corr);
    p->dir = Form("../output/%d", year);
    if(is_ttx)
        p->outputDir = Form("../output/%d/datacard_ttx", year);
    else
        p->outputDir = Form("../output/%d/datacard", year);
    if(is_corr)
        p->outputDir += "_nom";
    p->prepare();
    delete p;
}