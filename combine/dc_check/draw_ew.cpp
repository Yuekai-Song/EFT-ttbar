#include "draw_ew_pre.cpp"
void draw_ew(TString datacard_name, TString cut_name, int year, int type, bool rel){
    TString cutNames[] = {"M_4jets","M_3jets","E_4jets","E_3jets"};
    int years[] = {2015, 2016, 2017, 2018};

    int index[3][5] = {{2, 1, 0, 3, 4}, {2, 1, 0, 3, 4}, {0, 1, 2, 3, 4}}; //order of values
    double v[3][5] = {{0, 0.5, 1.0, -0.5, -1.0}, {0, 0.5, 1.0, 1.5, 2.0}, {0, 0.5, 1.0, -0.5, -1.0}};

    vector<double> ycuts;
    vector<vector<double>> xbins;
    if (datacard_name.Contains("2cuts")) {
        ycuts = {0.0, 1.4};
        xbins = {{300,340,360,380,400,420,440,460,480,500,520,540,570,600,640,700,3000},
                                   {300,450,500,570,630,700,820,3000}};
    }
    else
    {
        ycuts = {0.0, 0.4, 1.0, 2.0};
        xbins = {{0,340,380,420,460,500,600,3000}, {0,350,400,450,500,550,600,700,800,3000}, 
                               {0,450,500,550,600,650,700,800,1000,3000}, {0,650,700,750,800,900,1000,1200,3000}};
    }
    //TString filenames[] = {"ttbar_M_4jets.root","ttbar_M_3jets.root","ttbar_E_4jets.root","ttbar_E_3jets.root"};
    draw_pre(datacard_name, cut_name, year, type, v[type], index[type], rel, xbins, ycuts);
}