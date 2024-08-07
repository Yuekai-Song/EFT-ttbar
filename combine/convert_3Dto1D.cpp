#include"convert.cpp"

void convert_3Dto1D(TString input, TString datacard_name, TString cut_name, int year, double lhcut, bool qcd_ttx) {
    double likelihood_cut = 1000;
    vector<double> ycut_user;
    vector<vector<double>> xbins_user;
    if (cut_name.Contains("3jets"))
        likelihood_cut = lhcut;
    if (datacard_name.Contains("2cuts")) {
        ycut_user = {0.0, 1.4};
        xbins_user = {{300,340,360,380,400,420,440,460,480,500,520,540,570,600,640,700,3000},
                                   {300,450,500,570,630,700,820,3000}};
    }
    else {
        ycut_user = {0.0, 0.4, 1.0, 2.0};
        xbins_user = {{0,340,380,420,460,500,600,3000}, {0,350,400,450,500,550,600,700,800,3000}, 
                               {0,450,500,550,600,650,700,800,1000,3000}, {0,650,700,750,800,900,1000,1200,3000}};
    }
    
    TString output;
    map<TString, TString> name_change = {{"SF_Elec", Form("SF_Elec_%d", year)}, {"SF_Muon", Form("SF_Muon_%d", year)}};
    //input = Form("../select_analysis/output/%d/datacard_old/ttbar", year)+cut_name+".root";
    //input = Form("../select_analysis/output/%d/", year) + datacard_name + "ttbar" + cut_name + ".root";
    output = datacard_name + "/original/ttbar" + cut_name + Form("_%d.root", year);
    convert c(input, output, likelihood_cut, ycut_user, xbins_user, name_change, qcd_ttx);
}