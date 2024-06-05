#include "draw_1D_pre.cpp"
void draw_1D(TString dir, TString poi, int option)
{
    
    // vector<TString> fileName[2] = {{"limit_scan4_" + poi + ".root", "limit_scan3_" + poi + ".root", "limit_scan_" + poi + ".root"},
    //                                {"limit_scan_" + poi + ".root"}};
    vector<TString> fileName[2] = {{"limit_scan_" + poi + "_4cuts.root", "limit_scan_" + poi + "_2cuts.root", "limit_scan_" + poi + "_ttx.root"},
                                   {"limit_scan_" + poi + "_4cuts.root"}};
    TString varname[] = {"y", "z", "k"};
    map<TString, double> upedge = {{"y", 2.0}, {"z", 0.05}, {"k", 2.0}, {"fcp", 1.0}};
    map<TString, double> downedge = {{"y", -3.0}, {"z", -0.05}, {"k", -2.0}, {"fcp", -1.0}};
    map<TString, double> legend_left = {{"y", 0.75}, {"z", 0.20}, {"k", 0.50}, {"fcp", 0.75}};
    map<TString, double> legend_right = {{"y", 0.85}, {"z", 0.30}, {"k", 0.60}, {"fcp", 0.85}};
    map<TString, TString> xtitle = {{"y", "C_{#varphiu}"}, {"z", "Re(C_{u#varphi})"}, {"k", "Im(C_{u#varphi})"}, {"fcp", "f_{CP}"}};
    // vector<TString> text[2] = {{"#geq4jets", "3jets", "combined"}, {"combined"}};
    vector<TString> text[2] = {{"4cuts", "2cuts", "TOP-22-014"}, {"combined"}};

    draw_pre(dir, fileName[option], text[option], poi, upedge[poi], downedge[poi], legend_left[poi], legend_right[poi], xtitle[poi]);
}