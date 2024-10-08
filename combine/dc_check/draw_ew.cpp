#include "format.h"
using namespace std;

void model(TH1D *h1, TH1D *h2[5], double y, double z, double k)
{
    double cor[5];
    double c1, c2, c3, c4, c5;
    for (int bin = 0; bin < h1->GetNbinsX(); bin++)
    {
        for (int i = 0; i < 5; i++)
        {
            cor[i] = h2[i]->GetBinContent(bin + 1);
        }
        c1 = 0.5 * (cor[3] - 2 * cor[0] + cor[4]);
        c2 = cor[3] - cor[1];
        c3 = -cor[3] + cor[2];
        c4 = 0.5 * (-3 * cor[3] + 4 * cor[0] - cor[4]);
        c5 = cor[1];
        h1->SetBinContent(bin + 1, c1 * y * y + c2 * (z - 1) * (z - 1) + c3 * k * k + c4 * y + c5);
        //h1->SetBinError(bin + 1, 0);
    }
}
void draw_pre(TString datacard_name, TString cutname, int year, int type, double range, double value[5], int index[5], bool rel, TString xtitle, vector<vector<double>> xbins, vector<TString> ycut, vector<TString> zcut)
{
    TString process[] = {"ttbar_ci0100", "ttbar_ci0010", "ttbar_ci0001", "ttbar_ci0000", "ttbar_ci0200"};
    TString outpath = "./ew_pdf/" + datacard_name + "/" + cutname + Form("_%d/", year);
    TString filename = "ttbar_" + cutname + Form("_%d.root", year);
    TFile *file = TFile::Open("../datacards/" + datacard_name + "/original/" + filename);
    TH1D *hmc1[5], *h1[5], *hd[5];
    TH1D *hno;
    int color[] = {2, 1, 8, 4, 6};

    TString legend[5];
    TString legend_l[3] = {"C_{t#varphi} = ", "C_{t#varphi}^{I} = ", "C_{#varphit} = "};
    TString name[3] = {"Kappa", "Kappat", "Ztt"};
    for (int i = 0; i < 5; i++)
        legend[i] = legend_l[type] + Form("%.2f", value[i]);
    // TString legend[] = {"#kappa = 1", "#kappa = 0.5", "#kappa = 0", "#kappa = 1.5", "#kappa = 2.0"};

    for (int c = 0; c < 5; c++)
    {
        h1[c] = (TH1D *)file->Get(process[c]);
    }
    if (rel == true)
        hno = (TH1D *)file->Get("EW_no");
    else
        hno = (TH1D *)file->Get("ttbar_ci0000")->Clone();
    hno->SetName("hno");

    for (int i = 0; i < 5; i++)
    {
        hmc1[i] = (TH1D *)h1[0]->Clone();
        hmc1[i]->SetName(Form("ttbar_%d", i));
        if (type == 0)
            model(hmc1[i], h1, 0, value[i], 0);
        else if (type == 1)
            model(hmc1[i], h1, 0, 0, value[i]);
        else if (type == 2)
            model(hmc1[i], h1, value[i], 0, 0);
        hd[i] = (TH1D *)hmc1[i]->Clone(Form("hd_%d", i));
        hd[i]->Add(hno, -1);
        hd[i]->Divide(hno);
        cout << hmc1[i]->GetSumOfWeights() << endl;
    }
    TString pdf_name;
    if (rel)
        pdf_name = outpath + name[type] + "_LO";
    else
        pdf_name = outpath + name[type] + "_SM";
    DiCanvas *dc = new DiCanvas(xtitle, xbins, pdf_name);
    map<TString, TString> cut = {{"E_3jets", "e, 3jets"}, {"E_4jets", "e, #geq4jets"}, {"M_3jets", "#mu, 3jets"}, {"M_4jets", "#mu, #geq4jets"}};
    TString lumi[] = {"19.5", "16.8", "41.48", "59.83"};
    for (int i = 0; i < 5; i++)
    {
        if (i == 0)
            dc->draw_up(hmc1[index[i]], color[index[i]], legend[index[i]], lumi[year - 2015], ycut, zcut, cut[cutname]);
        else
            dc->draw_up(hmc1[index[i]], color[index[i]], legend[index[i]]);
    }
    for (int i = 0; i < 5; i++)
        dc->draw_dn(hd[i], "h", range, color[i], 2);
    delete dc;
}
void draw_ew(TString datacard_name, TString cut_name, int year, int type, bool rel){
    int index[3][5] = {{2, 1, 0, 3, 4}, {0, 1, 2, 3, 4}, {2, 1, 0, 3, 4}}; //order of values
    double v[3][5] = {{0, 0.5, 1.0, -0.5, -1.0}, {0, 0.5, 1.0, 1.5, 2.0}, {0, 0.5, 1.0, -0.5, -1.0}};
    double ranges[3] = {0.1, 0.07, 0.1};
    vector<vector<double>> xbins;
    vector<TString> ycut, zcut;
    vector<var> x;
    TString xtitle;
    binning(datacard_name, x, xbins, ycut, zcut, xtitle);

    //TString filenames[] = {"ttbar_M_4jets.root","ttbar_M_3jets.root","ttbar_E_4jets.root","ttbar_E_3jets.root"};
    draw_pre(datacard_name, cut_name, year, type, ranges[type], v[type], index[type], rel, xtitle, xbins, ycut, zcut);
}