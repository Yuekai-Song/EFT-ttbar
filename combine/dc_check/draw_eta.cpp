#include "format.h"

void draw_pre(TString datacard_name, TString cutname, int year, double range, TString xtitle, vector<vector<double>> xbins, vector<TString> ycut, vector<TString> zcut)
{
    TString process[] = {"Eta", "ttbar_ci0000"};
    TString outpath = "./eta_pdf/" + datacard_name + "/" + cutname + Form("_%d/", year);
    TString filename = "ttbar_" + cutname + Form("_%d.root", year);
    TFile *file = TFile::Open("../datacards/" + datacard_name + "/original/" + filename);
    TH1D *hmc[2], *hd;
    int color[] = {2, 1, 8, 4, 6};
    map<TString, TString> cut = {{"E_3jets", "e, 3jets"}, {"E_4jets", "e, #geq4jets"}, {"M_3jets", "#mu, 3jets"}, {"M_4jets", "#mu, #geq4jets"}};
    TString lumi[] = {"19.5", "16.8", "41.48", "59.83"};
    TString legend[2] = {"t#bar{t}+#eta_{t}", "t#bar{t}"};
    // TString legend[] = {"#kappa = 1", "#kappa = 0.5", "#kappa = 0", "#kappa = 1.5", "#kappa = 2.0"};
    DiCanvas *dc = new DiCanvas(xtitle, xbins, outpath + "eta");
    for (int c = 0; c < 2; c++)
    {
        hmc[c] = (TH1D *)file->Get(process[c]);
    }
    hmc[0]->Add(hmc[1]);
    dc->draw_up(hmc[1], color[1], legend[1], lumi[year - 2015], ycut, zcut, cut[cutname]);
    dc->draw_up(hmc[0], color[0], legend[0]);
    hd = (TH1D *)hmc[0]->Clone("hd");
    hd->Add(hmc[1], -1);
    hd->Divide(hmc[1]);
    dc->draw_dn(hd, "h", range, color[0], 2);
    delete dc;
}
void draw_eta(TString datacard_name, TString cut_name, int year){
    TString cutNames[] = {"M_4jets","M_3jets","E_4jets","E_3jets"};
    int years[] = {2015, 2016, 2017, 2018};
    vector<vector<double>> xbins;
    vector<TString> ycut, zcut;
    vector<var> x;
    TString xtitle;
    binning(datacard_name, x, xbins, ycut, zcut, xtitle);
    //TString filenames[] = {"ttbar_M_4jets.root","ttbar_M_3jets.root","ttbar_E_4jets.root","ttbar_E_3jets.root"};
    draw_pre(datacard_name, cut_name, year, 0.1, xtitle, xbins, ycut, zcut);
}