#include "draw_1D_pre.cpp"
#include "get_1sigma.cpp"
void draw_1D(TString dir, TString poi, int option)
{
    // vector<TString> fileName[2] = {{"limit_scan4_" + poi + ".root", "limit_scan3_" + poi + ".root", "limit_scan_" + poi + ".root"},
    //                                {"limit_scan_" + poi + ".root"}};
    vector<TString> fileName[3] = {{"4cuts/limit_scan_1D" + poi + ".root", "2cuts/limit_scan_1D" + poi + ".root", "ttx_2cuts/limit_scan_1D" + poi + ".root"},
                                   {"limit_scan_" + poi + ".root"},
                                   {"limit_scan4_" + poi + ".root", "limit_scan3_" + poi + ".root", "limit_scan_" + poi + ".root"}};
    TString varname[] = {"y", "z", "k"};
    map<TString, double> upedge = {{"y", 2.0}, {"z", 3}, {"k", 2.0}, {"fcp", 1.0}};
    map<TString, double> downedge = {{"y", -3.0}, {"z", -1}, {"k", -2.0}, {"fcp", -1.0}};
    map<TString, double> legend_left = {{"y", 0.75}, {"z", 0.20}, {"k", 0.50}, {"fcp", 0.75}};
    map<TString, double> legend_right = {{"y", 0.85}, {"z", 0.30}, {"k", 0.60}, {"fcp", 0.85}};
    map<TString, TString> xtitle = {{"y", "C_{#varphiu}"}, {"z", "Re(C_{u#varphi})"}, {"k", "Im(C_{u#varphi})"}, {"fcp", "f_{CP}"}};
    // vector<TString> text[2] = {{"#geq4jets", "3jets", "combined"}, {"combined"}};
    vector<TString> text[3] = {{"4cuts", "2cuts", "TOP-22-014"}, {"combined"}, {"#geq4jets", "3jets", "combined"}};

    draw_pre(dir, fileName[option], text[option], poi, upedge[poi], downedge[poi], legend_left[poi], legend_right[poi], xtitle[poi]);
    for (int i = 0; i < fileName[option].size(); i++)
    {
        cout << "In " << dir + fileName[option][i] << ":" << endl;
        get_1sigma(dir + fileName[option][i], poi);
    }
}   