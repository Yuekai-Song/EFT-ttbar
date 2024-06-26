//need cmsenv, run on lxplus
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include "draw_pre.cpp"

void draw_nuis(TString dir, std::vector<TString> sys = {}, double zval = 0, double kval = 0, double yval = 0)
{
    std::map<TString, TString> channelToTag;
    // std::vector<TString> sys;
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
    map<TString, double> sys_range = {{"sb_co", 0.25}, {"mtop3", 0.15}};
    double range;

    TH1D *h1[5];
    double vals[5] = {0, -1, -0.5, 0.5, 1};
    TString ws_name = dir + "/workspace_ttbar.root";
    TString datacard = dir + "/../ttbar.txt";
    get_ch(datacard, channelToTag);
    TFile *ws_file = TFile::Open(ws_name);
    RooWorkspace *w = (RooWorkspace *)ws_file->Get("w");
    const RooArgSet *nuis = w->set("nuisances");
    TIterator* iter = nuis->createIterator();
    if (sys.size() == 0)
    {
        RooRealVar* var;
        while ((var = dynamic_cast<RooRealVar*>(iter->Next()))) {
            if (var)
                sys.push_back(var->GetName());
        }
    }
    for (int ch_index = 0; ch_index < channelToTag.size(); ch_index++)
    {
        for (auto const& sys_name : sys)
        {
            for (int i = 0; i < 5; i++)
                get_th(h1[i], w, vector<TString> {sys_name, "y", "z", "k"}, vector<double> {vals[i], yval, zval, kval}, ch_index);
            TString legend[4];
            for (int i = 0; i < 4; i++)
                legend[i] = sys_name + " = " + Form("%.1f", vals[i + 1]);
            if (sys_range.find(sys_name) != sys_range.end())
                range = 0;
            else
                range = sys_range[sys_name];
            draw_pre(h1[0], h1 + 1, legend, dir + "/nuis/ttbar_" + sys_name + Form("z%.1fy%.1fk%.1f", zval, yval, kval) + "_"  + channelToTag[Form("ch%d", ch_index + 1)], range, xbins, ycuts);
            for (int i = 0; i < 5; i++)
                delete h1[i];
        }
    }
    delete w;
    ws_file->Close();
}