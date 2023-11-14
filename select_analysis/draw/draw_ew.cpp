#include "draw_ew_pre.cpp"
void draw_ew(int i, int y, int s, int t, bool rel){
    TString cutNames[] = {"M_4jets","M_3jets","E_4jets","E_3jets"};
    int years[] = {2015, 2016, 2017, 2018};
    TString sys[] = {"", "_mtopUp", "_nnlo_wtUp", "_pdfUp", "_EW_unUp"};
    int index[3][5] = {{2, 1, 0, 3, 4}, {2, 1, 0, 3, 4}, {0, 1, 2, 3, 4}};
    double v[3][5] = {{0, 0.5, 1.0, -0.5, -1.0}, {0, 0.5, 1.0, 1.5, 2.0}, {0, 0.5, 1.0, -0.5, -1.0}};

    vector<double> ycuts;
    vector<vector<double>> xbins;
    ycuts = {0.0, 0.4, 1.0, 2.0};
    xbins = {{0,340,380,420,460,500,600,3000}, {0,350,400,450,500,550,600,700,800,3000}, 
                    {0,450,500,550,600,650,700,800,1000,3000}, {0,650,700,750,800,900,1000,1200,3000}};
    //TString filenames[] = {"ttbar_M_4jets.root","ttbar_M_3jets.root","ttbar_E_4jets.root","ttbar_E_3jets.root"};
    draw_pre(cutNames[i], years[y], sys[s], t, v[t], index[t], rel, xbins, ycuts);
}