#include <TROOT.h>
#include <iostream>
using namespace std;

class settings
{
public:
    vector<TString> cuts = {"(jet_num == 3 && (!lep_flavour))", "(jet_num >= 4  && (!lep_flavour))",
                            "(jet_num == 3 && lep_flavour)", "(jet_num >= 4 && lep_flavour)"};
    vector<TString> cutsName = {"E_3jets", "E_4jets", "M_3jets", "M_4jets"};
    vector<int> xyz_bins = {270, 40, 100};
    vector<double> xyz_range = {300, 3000, 0, 4.0, 8, 33};
    settings(int year, bool ttx, TString cg);
};
settings::settings(int year, bool ttx, TString cg)
{
    for (int i = 0; i < cuts.size(); i++)
    {
        if (year == 2018)
            cuts[i] += "*(lep_flavour || ((!lep_flavour) && lepton_pt>34)) * (MtW<=140)";
        else
            cuts[i] += "*(MtW<=140)";
        if (ttx)
        {
            cuts[i] += "*(D_nu < 150)";
            if (cg == "A" || cg == "C")
                cuts[i] += "*(nBtag == 2)";
        }
    }
}