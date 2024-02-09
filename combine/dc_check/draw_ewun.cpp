#include <TROOT.h>
#include <TLine.h>
#include <TStyle.h>
#include <Math/PdfFuncMathCore.h>
#include <TFile.h>
#include <TChain.h>
#include <TLorentzVector.h>
#include <TGraphAsymmErrors.h>
#include <TMath.h>
#include <TLegend.h>
#include <TF1.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TH2F.h>
#include <THStack.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <TPaveText.h>
using namespace std;
void seterror0(TH1D *h1)
{
    for (int i = 1; i <= h1->GetNbinsX(); i++)
    {
        h1->SetBinError(i, 0);
    }
}
void format_text(TPaveText *lumi)
{
    lumi->SetFillColor(0);
    lumi->SetTextFont(42);
    lumi->SetTextSize(0.035);
    lumi->SetTextColor(1);
    lumi->SetBorderSize(0);
}
void format_leg(TLegend *leg)
{
    leg->SetNColumns(1);
    leg->SetColumnSeparation(0.5);
    leg->SetEntrySeparation(0.0);
    leg->SetBorderSize(0);
    leg->SetTextFont(42);
    leg->SetTextSize(0.03);
    leg->SetLineColor(1);
    leg->SetLineStyle(1);
    leg->SetLineWidth(1);
    leg->SetFillColor(0);
    leg->SetFillStyle(0);
}
void format_canvas(TCanvas *c)
{
    c->SetFillColor(0);
    c->SetBorderMode(0);
    c->SetBorderSize(2);
    c->SetTickx(1);
    c->SetTicky(1);
    c->SetLeftMargin(0.14);
    c->SetRightMargin(0.05);
    c->SetTopMargin(0.01);
    c->SetBottomMargin(0.15);
    c->SetFrameFillStyle(0);
    c->SetFrameBorderMode(0);
    c->SetFrameFillStyle(0);
    c->SetFrameBorderMode(0);
}

void format_th(TH1D *h1, TString xtitle, double *low, double *high)
{
    int ydivisions = 509;
    float up = h1->GetMaximum();
    float down = h1->GetMinimum();
    int bins = h1->GetNbinsX();
    up = fabs(up - 1);
    down = fabs(down - 1);
    h1->SetMarkerStyle(20);
    h1->SetMarkerSize(0.4);
    h1->SetStats(kFALSE);
    h1->GetXaxis()->SetTitle(xtitle);
    h1->GetYaxis()->SetTitle("Ratio");
    h1->SetTitle("");
    h1->GetXaxis()->CenterTitle();
    h1->GetYaxis()->CenterTitle();
    h1->GetYaxis()->SetNdivisions(ydivisions);
    h1->GetXaxis()->SetTitleSize(0.04);
    h1->GetYaxis()->SetTitleSize(0.04);
    h1->GetXaxis()->SetTitleOffset(2.0);
    h1->GetYaxis()->SetTitleOffset(1.1);
    h1->GetXaxis()->SetLabelSize(0.04);
    h1->GetYaxis()->SetLabelSize(0.04);
    h1->GetYaxis()->SetRangeUser(-0.2, 0.25);
    *low = -0.3;
    *high = 0.3;
    return;
}

void format_line(TLine *l1)
{
    l1->SetLineStyle(9);
    l1->SetLineWidth(2);
    l1->SetLineColor(1);
}
void set_th_lable(TH1D *h1, vector<vector<double>> xbins)
{
    double nbin = 0;
    for (int i = 0; i < xbins.size(); i++)
    {
        for (int j = nbin; j < nbin + xbins[i].size() - 1; j++)
            h1->GetXaxis()->SetBinLabel(j + 1, TString::Format("%.0f-%.0f", xbins[i][j - nbin], xbins[i][j - nbin + 1]));
        nbin += xbins[i].size() - 1;
    }
    h1->LabelsDeflate("X");
    h1->LabelsDeflate("Y");
    h1->LabelsOption("v");
}
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
        h1->SetBinError(bin + 1, 0);
    }
}
void draw_pre(TString datacard_name, TString cutname, int year, int type, double value[2], vector<vector<double>> xbins, vector<double> ycuts)
{
    double low2, high2;
    TString process[] = {"ttbar_ci0100", "ttbar_ci0010", "ttbar_ci0001", "ttbar_ci0000", "ttbar_ci0200"};

    TString outpath = "./ewun_pdf/" + datacard_name + "/" + cutname + Form("_%d/", year);
    TString filename = "ttbar_" + cutname + Form("_%d.root", year);
    TFile *file = TFile::Open("../" + datacard_name + "/original/" + filename);
    TH1D *hnom[5], *hup[5], *hdn[5];
    TH1D *hmc_nom[2], *hmc_up[2], *hmc_dn[2];
    TGraphAsymmErrors *hg[2];
    TH1D *hno, *hd;
    int color[] = {2, 4, 8, 1, 6};

    TString xtitle = "M_{t#bar{t}}";
    TString legend[5];
    TString legend_l[3] = {"#kappa_{t} = ", "Im(C_{u#varphi}) = ", "C_{#varphiu}) = "};
    TString name[3] = {"Kappa", "Kappat", "Ztt"};
    for (int i = 0; i < 2; i++)
    {
        if (type == 0)
            legend[i] = legend_l[type] + Form("%.2f", 1 - value[i]);
        else
            legend[i] = legend_l[type] + Form("%.2f", value[i]);
    }
    // TString legend[] = {"#kappa = 1", "#kappa = 0.5", "#kappa = 0", "#kappa = 1.5", "#kappa = 2.0"};

    const int ndiv = xbins.size() - 1;
    const int nnbins = xbins.size() + 1;
    const int ncuts = ycuts.size();
    int div[ndiv];
    int nbins[nnbins];
    nbins[0] = 0;
    for (int i = 1; i < nnbins; i++)
        nbins[i] = nbins[i - 1] + xbins[i - 1].size() - 1;
    for (int i = 0; i < ndiv; i++)
        div[i] = nbins[i + 1];
    int bins = nbins[nnbins - 1];

    TString cut[ncuts];
    cut[0] = Form("|#Deltay| < %.1f", ycuts[1]);
    cut[ncuts - 1] = Form("|#Deltay| > %.1f", ycuts[ncuts - 1]);
    for (int i = 1; i < ncuts - 1; i++)
        cut[i] = Form("%.1f<|#Deltay| < %.1f", ycuts[i], ycuts[i]);
    
    TLine *l2[ndiv];
    TPaveText *t[ncuts];

    for (int c = 0; c < 5; c++)
    {
        hnom[c] = (TH1D *)file->Get(process[c]);
        hup[c] = (TH1D *)file->Get(process[c] + "_EW_unUp");
        hdn[c] = (TH1D *)file->Get(process[c] + "_EW_unDown");
    }
    hno = (TH1D *)file->Get("EW_no");
    hno->SetName("hno");
    hd = (TH1D *)hno->Clone();
    hd->SetName("hd");
    hd->Add(hno, -1);
    hd->Divide(hno);
    seterror0(hd);
    TCanvas *c2 = new TCanvas("c1", "c1", 8, 30, 650, 650);
    c2->cd();
    TLegend *leg = new TLegend(0.70, .85, 0.95, .95);
    format_leg(leg);
    format_canvas(c2);
    for (int i = 0; i < 2; i++)
    {
        hmc_nom[i] = (TH1D *)hnom[0]->Clone();
        hmc_up[i] = (TH1D *)hup[0]->Clone();
        hmc_dn[i] = (TH1D *)hdn[0]->Clone();
        hmc_nom[i]->SetName(Form("hmc_nom_%d", i));
        if (type == 0)
        {
            model(hmc_nom[i], hnom, 0, value[i], 0);
            model(hmc_up[i], hup, 0, value[i], 0);
            model(hmc_dn[i], hdn, 0, value[i], 0);
        }
        else if (type == 1)
        {
            model(hmc_nom[i], hnom, 0, 0, value[i]);
            model(hmc_up[i], hup, 0, 0, value[i]);
            model(hmc_dn[i], hdn, 0, 0, value[i]);
        }
        else if (type == 2)
        {
            model(hmc_nom[i], hnom, value[i], 0, 0);
            model(hmc_up[i], hup, value[i], 0, 0);
            model(hmc_dn[i], hdn, value[i], 0, 0);
        }
        hmc_nom[i]->Add(hno, -1);
        hmc_nom[i]->Divide(hno);
        hmc_up[i]->Add(hno, -1);
        hmc_up[i]->Divide(hno);
        hmc_dn[i]->Add(hno, -1);
        hmc_dn[i]->Divide(hno);
        hg[i] = new TGraphAsymmErrors(hmc_nom[i]);
        for(int bin = 0; bin < hmc_nom[i]->GetNbinsX(); bin++){
            hg[i]->SetPointEXlow(bin, 0.5);
            hg[i]->SetPointEXhigh(bin, 0.5);
            hg[i]->SetPointEYhigh(bin, fabs(hmc_up[i]->GetBinContent(bin + 1) - hmc_nom[i]->GetBinContent(bin + 1)));
            hg[i]->SetPointEYlow(bin, fabs(hmc_dn[i]->GetBinContent(bin + 1) - hmc_nom[i]->GetBinContent(bin + 1)));
            //cout << hg[i]->GetPointY(bin) << hmc_nom[i]->GetBinContent(bin + 1) << endl;
            cout << hmc_up[i]->GetBinContent(bin + 1) - hmc_nom[i]->GetBinContent(bin + 1) << hmc_dn[i]->GetBinContent(bin + 1) - hmc_nom[i]->GetBinContent(bin + 1) <<endl;
        }
        cout << hmc_nom[i]->GetSumOfWeights() << endl;
    }
    c2->cd();
    hd->Draw("hist");
    hd->SetLineStyle(9);
    hd->SetLineWidth(2);
    hd->SetLineColor(1);
    cout << hd->GetSumOfWeights()<<endl;
    format_th(hd, xtitle, &low2, &high2);
    set_th_lable(hd, xbins);

    for (int i = 0; i < 2; i++)
        leg->AddEntry(hmc_nom[i], legend[i], "l");
    leg->Draw("Same");

    for (int tex = 0; tex < ncuts; tex++)
    {
        double mid_row = (nbins[tex] + nbins[tex + 1]) / 2.0;
        double mid_col = high2 * 0.9;
        //cout << mid << endl;
        t[tex] = new TPaveText(mid_row - 1, mid_col * 0.95, mid_row + 1, mid_col * 1.05);
        format_text(t[tex]);
        t[tex]->AddText(cut[tex]);
        t[tex]->Draw("same");
    }

    for (int i = 0; i < 2; i++)
    {
        hg[i]->Draw("2Same");
        hg[i]->SetLineColor(color[i]);
        hg[i]->SetFillColorAlpha(color[i], 0.3);
        //hg[i]->SetFillStyle(3001);
        hmc_nom[i]->Draw("histSame");
        hmc_nom[i]->SetLineColor(color[i]);
        hmc_nom[i]->SetLineWidth(2);
        
        /*hmc_up[i]->Draw("histSame");
        hmc_up[i]->SetLineColor(color[i]);
        hmc_up[i]->SetFillColorAlpha(color[i], 0.3);
        hmc_up[i]->SetLineWidth(0);
        hmc_dn[i]->Draw("histSame");
        hmc_dn[i]->SetLineColor(color[i]);
        hmc_dn[i]->SetFillColorAlpha(color[i], 0.3);
        hmc_dn[i]->SetLineWidth(0);*/
        //seterror0(hd1[i]);
    }

    for (int d = 0; d < ndiv; d++)
    {
        l2[d] = new TLine(div[d], low2, div[d], high2);
        format_line(l2[d]);
        l2[d]->Draw("same");
    }
    if (type == 0)
        c2->Print(outpath + name[type] + Form("_%.1f_%.1f.pdf", 1 - value[0], 1 - value[1]));
    else
        c2->Print(outpath + name[type] + Form("_%.1f_%.1f.pdf", value[0], value[1]));
    for (int d = 0; d < ndiv; d++)
        delete l2[d];

    for (int tex = 0; tex < ncuts; tex++)
        delete t[tex];
    for (int i = 0; i < 5; i++)
    {
        delete hnom[i];
        delete hup[i];
        delete hdn[i];
    }
    for (int i = 0; i < 2; i++)
        delete hmc_nom[i];
    delete hno;
    delete hd;
    delete leg;
    delete c2;
}
void draw_ewun(TString datacard_name, TString cut_name, int year, int type, double v1, double v2)
{
    double v[2] = {v1, v2};

    vector<double> ycuts;
    vector<vector<double>> xbins;
    if (datacard_name.Contains("4cuts"))
    {
        ycuts = {0.0, 0.4, 1.0, 2.0};
        xbins = {{0,340,380,420,460,500,600,3000}, {0,350,400,450,500,550,600,700,800,3000}, 
                               {0,450,500,550,600,650,700,800,1000,3000}, {0,650,700,750,800,900,1000,1200,3000}};
    }
    else if (datacard_name.Contains("2cuts")) {
        ycuts = {0.0, 1.4};
        xbins = {{300,340,360,380,400,420,440,460,480,500,520,540,570,600,640,700,3000},
                                   {300,450,500,570,630,700,820,3000}};
    }
    
    //TString filenames[] = {"ttbar_M_4jets.root","ttbar_M_3jets.root","ttbar_E_4jets.root","ttbar_E_3jets.root"};
    draw_pre(datacard_name, cut_name, year, type, v, xbins, ycuts);
}