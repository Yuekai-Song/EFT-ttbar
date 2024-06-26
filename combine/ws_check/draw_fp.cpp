//need cmsenv, run on lxplus
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include "draw_pre.cpp"

void draw_fp(TString dir)
{
    std::map<TString, TString> channelToTag;
    gSystem->Load("libHiggsAnalysisCombinedLimit.so");//cmsenv needed!
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    vector<double> ycuts;
    vector<vector<double>> xbins;
    ycuts = {0.0, 1.4};
    xbins = {{300,340,360,380,400,420,440,460,480,500,520,540,570,600,640,700,3000},
                                   {300,450,500,570,630,700,820,3000}};
    if (xbins.size() != ycuts.size())
    {
        cout << "the number of cuts on deltay is not the same" << endl;
        return;
    }
    map<TString, vector<double>> fp_vals = {{"y", {0, -1, -0.5, 0.5, 1}}, {"z", {0, -1, -0.5, 0.5, 1}}, 
                            {"k", {0, -2, -1, 1, 2}}, {"norm", {1, 0.5, 0.8, 1.2, 1.5}}};

    TH1D *h1[5];
    TString ws_name = dir + "/workspace_ttbar.root";
    TString datacard = dir + "/../ttbar.txt";
    get_ch(datacard, channelToTag);
    TFile *ws_file = TFile::Open(ws_name);
    RooWorkspace *w = (RooWorkspace *)ws_file->Get("w");
    for (int ch_index = 0; ch_index < channelToTag.size(); ch_index++)
    {
        for (auto const& pair : fp_vals)
        {
            for (int i = 0; i < 5; i++)
                get_th(h1[i], w, vector<TString> {pair.first}, vector<double> {pair.second[i]}, ch_index);
            TString legend[4];
            for (int i = 0; i < 4; i++)
                legend[i] = pair.first + " = " + Form("%.1f", pair.second[i]);

            draw_pre(h1[0], h1 + 1, legend, dir + "/free_params/ttbar_" + pair.first + "_" + channelToTag[Form("ch%d", ch_index + 1)], 0, xbins, ycuts);
            for (int i = 0; i < 5; i++)
                delete h1[i];
        }
    }
    delete w;
    ws_file->Close();
}