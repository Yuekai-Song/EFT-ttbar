#include"convert.cpp"

void convert_3Dto1D(TString datacard_name, TString cut_name, int year) {
    double likelihood_cut;
    vector<double> ycut_user;
    vector<vector<double>> xbins_user;
    likelihood_cut = 1000;
    if (datacard_name == "datacard/") {
        ycut_user = {0.0, 0.4, 1.0, 2.0};
        xbins_user = {{0,340,380,420,460,500,600,3000}, {0,350,400,450,500,550,600,700,800,3000}, 
                               {0,450,500,550,600,650,700,800,1000,3000}, {0,650,700,750,800,900,1000,1200,3000}};
    }
    else if (datacard_name == "datacard_ttx/") {
        ycut_user = {0.0, 1.4};
        xbins_user = {{300,340,360,380,400,420,440,460,480,500,520,540,570,600,640,700,3000},
                                   {300,450,500,570,630,700,820,3000}};
        if (cut_name.Contains("3jets"))
            likelihood_cut = 13.0;
    }
    TString input, output;
    
    //input = Form("../test/sys_fix/datacard/%d/ttbar", year)+cut_name+".root";
    input = Form("../select_analysis/output/%d/", year) + datacard_name + "ttbar" + cut_name + ".root";
    output = "./" + datacard_name + "original/ttbar" + cut_name + Form("_%d.root", year);
    convert(input, output, likelihood_cut, ycut_user, xbins_user);
}