#include "draw_1D_pre.cpp"
#include "get_1sigma.cpp"
void draw_1D(TString dir, TString poi, int option, vector<double> range = {})
{
    // vector<TString> fileName[2] = {{"limit_scan4_" + poi + ".root", "limit_scan3_" + poi + ".root", "limit_scan_" + poi + ".root"},
    //                                {"limit_scan_" + poi + ".root"}};
    vector<TString> fileName[6] = {{"4cuts/limit_scan_1D" + poi + ".root", "2cuts/limit_scan_1D" + poi + ".root", "ttx_2cuts/limit_scan_1D" + poi + ".root"},
                                   {"4cuts/limit_scan_1D" + poi + "_fix.root", "2cuts/limit_scan_1D" + poi + "_fix.root"},
                                   {"limit_scan_1D" + poi + "_fix.root", "limit_scan_1D" + poi + ".root"},
                                   {"topo/limit_scan_1D" + poi + ".root", "noEta/limit_scan_1D" + poi + ".root"},
                                   {"2cuts/topo/limit_scan_1D" + poi + ".root", "ttx/topo/limit_scan_1D" + poi + ".root"},
                                   {"limit_scan4_" + poi + ".root", "limit_scan3_" + poi + ".root", "limit_scan_" + poi + ".root"}};
    TString varname[] = {"y", "z", "k"};
    map<TString, double> upedge = {{"y", 2.0}, {"z", 3}, {"k", 6.0}, {"fcp", 1.0}};
    map<TString, double> downedge = {{"y", -3.0}, {"z", -1}, {"k", -6.0}, {"fcp", -1.0}};
    map<TString, double> legend_left = {{"y", 0.30}, {"z", 0.20}, {"k", 0.50}, {"fcp", 0.50}};
    map<TString, double> legend_right = {{"y", 0.40}, {"z", 0.30}, {"k", 0.60}, {"fcp", 0.60}};
    map<TString, TString> xtitle = {{"y", "C_{#varphit}"}, {"z", "C_{t#varphi}"}, {"k", "C_{t#varphi}^{I}"}, {"fcp", "f_{CP}"}};
    // vector<TString> text[2] = {{"#geq4jets", "3jets", "combined"}, {"combined"}};
    vector<TString> text[6] = {{"4 #deltay_{t#bar{t}} cuts", "2 #deltay_{t#bar{t}} cuts", "TOP-22-014"}, {"4 #Deltay_{t#bar{t}} bins", "2 #Deltay_{t#bar{t}} bins"}, {"other POIs fixed", "other POIs floated"}, {"with #eta_{t}", "no #eta_{t}"},
                               {"2cuts with #eta_{t}", "TOP-22-014 with #eta_{t}"}, {"#geq4jets", "3jets", "combined"}};
    if (range.size() != 2)
    {
        downedge[poi] = range[0];
        upedge[poi] = range[1]; 
    }
    draw_pre(dir, fileName[option], text[option], poi, upedge[poi], downedge[poi], legend_left[poi], legend_right[poi], xtitle[poi]);
    for (int i = 0; i < fileName[option].size(); i++)
    {
        cout << "In " << dir + fileName[option][i] << ":" << endl;
        get_1sigma(dir + fileName[option][i], poi);
    }
}  