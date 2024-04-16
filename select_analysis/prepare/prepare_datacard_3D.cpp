#include "prepare_3D.cpp"
void prepare_datacard_3D(int i, int year, int option, bool is_ttx, bool is_corr = false)
{
    TString cuts[] = {"(jet_num == 3 && (!lep_flavour))", "(jet_num >= 4  && (!lep_flavour))",
                      "(jet_num == 3 && lep_flavour)", "(jet_num >= 4 && lep_flavour)"};
    TString cutsName[] = {"E_3jets", "E_4jets", "M_3jets", "M_4jets"};
    int xyz_bins[] = {270, 40, 100};
    double xyz_range[] = {300, 3000, 0, 4.0, 8, 33};
    if (year == 2018)
        cuts[i] += "*(lep_flavour || ((!lep_flavour) && lepton_pt>34)) * (MtW<=140)";
    else
        cuts[i] += "*(MtW<=140)";
    if (is_ttx)
        cuts[i] += "*(D_nu < 150)";
    cout << "begin " << cutsName[i] << ":" << endl;
    prepare_3D *p = new prepare_3D(cuts[i], cutsName[i], year, xyz_bins, xyz_range, option, is_ttx, is_corr);
    delete p;
}