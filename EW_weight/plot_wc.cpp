#include <TROOT.h>
#include <TStyle.h>
#include <Math/PdfFuncMathCore.h>
#include <TFile.h>
#include <TChain.h>
#include <TLorentzVector.h>
#include <TMath.h>
#include <TLegend.h>
#include <TF1.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TH2F.h>
#include <THStack.h>
#include <string.h>
#include <TLine.h>
#include <TGraph.h>
#include <TKey.h>
#include <TGraphAsymmErrors.h>
#include <fstream>
#include <iostream>
#include <map>
using namespace std;
void format_canvas(TCanvas *c)
{
    c->SetFillColor(0);
    c->SetBorderMode(0);
    c->SetBorderSize(2);
    c->SetTickx(1);
    c->SetTicky(1);
    c->SetLeftMargin(0.17);
    c->SetRightMargin(0.12);
    c->SetTopMargin(0.07);
    c->SetBottomMargin(0.13);
    c->SetFrameFillStyle(0);
    c->SetFrameBorderMode(0);
    c->SetFrameFillStyle(0);
    c->SetFrameBorderMode(0);
}

void plot(int c, bool lo)
{
    TString fileName[3][5] = {{"ci0-200.root", "ci0-100.root", "ci0100.root", "ci0200.root", "ci0300.root"},
                              {"ci0010.root", "ci0020.root", "ci0030.root", "ci0040.root", "ci0050.root"},
                              {"ci0001.root", "ci0002.root", "ci0003.root", "ci0004.root", "ci0005.root"}};
    TString SM = "ci0000.root";
    TString name[3] = {"C_{#varphit} = ", "C_{t#varphi} = ", "C_{t#varphi}^{I} = "};
    int legName[3][5] = {{-2, -1, 1, 2, 3}, {1, 2, 3, 4, 5}, {1, 2, 3, 4, 5}};
    // TString var_ew[] = {"id7", "id10", "id13", "id16"}; // mtt,pt,ytt, yt
    TString var_ew[] = {"id7", "id10", "id13", "id16"};
    float xlow[] = {350, 0, -4.0, -4.0};
    float xup[] = {1200, 1200, 4.0, 4.0};
    // float ylow[] = {-0.20, -0.15, -0.15, -0.15};
    // float yup[] = {0.1, 0.12, 0.10, 0.12};
    float ylow = -0.6, yup = 0.6;
    TString LO = "LO";
    if (!lo)
        LO = "SM";
    TString xtitle[] = {"M_{t#bar{t}} [GeV]", "p_{T}^{t} [GeV]", "#Deltay_{t#bar{t}}", "y_{t}"};
    TString ytitle[] = {"(d#delta#sigma/dM_{t#bar{t}})/(d#sigma_{" + LO + "}/dM_{t#bar{t}})",
                        "(d#delta#sigma/dp_{T}^{t})/(d#sigma_{" + LO + "}/dp_{T}^{t})",
                        "(d#delta#sigma/d#Deltay_{t#bar{t}})/(d#sigma_{" + LO + "}/d#Deltay_{t#bar{t}})",
                        "(d#delta#sigma_{weak}/dy_{t})/(d#sigma_{" + LO + "}/dy_{t})"};
    TString title[] = {"dist_mtt", "dist_pt", "dist_ytt", "dis_yt"};
    TString var_name[] = {"y", "z", "k"};
    int color[] = {2, 1, 4, 226, 6, kOrange + 2, kViolet + 1, kAzure + 10, 93};

    for (int i = 0; i < 3; i++)
    {													   // loop over variables
        auto c2 = new TCanvas("c2", "", 30, 30, 800, 800); // draw on this canvas
        format_canvas(c2);
        c2->cd();
        auto leg = new TLegend(.30, .70, .75, .90);
        leg->SetFillColor(0);
        leg->SetLineColor(0);
        leg->SetBorderSize(0);
        leg->SetFillStyle(0);
        leg->SetTextFont(42);
        leg->SetTextSize(0.05);
        leg->SetNColumns(2);
        leg->SetColumnSeparation(0.03);
        TFile *SM_file = TFile::Open("./EW_files/" + SM);
        TH1F *SM_hist = (TH1F *)SM_file->Get(var_ew[i]);
        SM_hist->Smooth();
        for (int k = 0; k < 5; k++)
        { // loop over files
            // TH1F *ew_hist = (TH1F*) file->Get(varName[i]);
            TFile *file = TFile::Open("./EW_files/" + fileName[c][k]);
            TH1F *ew_hist = (TH1F *)file->Get(var_ew[i]);
            ew_hist->Smooth();
            if (!lo)
                ew_hist->Add(SM_hist, -1.0);
            //ew_hist->Divide(SM_hist);
            //ew_hist->Smooth();
            int nbins = ew_hist->GetNbinsX();
            double x[nbins], y[nbins];
            for (int ii = 0; ii < nbins; ii++)
            {
                x[ii] = ew_hist->GetXaxis()->GetBinCenter(ii + 1);
                y[ii] = ew_hist->GetBinContent(ii + 1);
                //	cout<<x[ii]<<" "<<y[ii]<<endl;
            }
            TGraph *h1 = new TGraph(nbins, x, y);

            // TH1F* h1=(TH1F*)ew_hist->Clone();
            h1->SetLineColor(color[k]);
            h1->SetLineWidth(3);
            // h1->SetStats(kFALSE);
            h1->GetXaxis()->SetTitle(xtitle[i]);
            h1->GetYaxis()->SetTitle(ytitle[i]);
            h1->SetTitle("");
            h1->GetXaxis()->CenterTitle();
            h1->GetYaxis()->CenterTitle();
            h1->GetXaxis()->SetTitleSize(0.05);
            h1->GetYaxis()->SetTitleSize(0.05);
            h1->GetYaxis()->SetTitleOffset(1.4);
            h1->GetYaxis()->SetRangeUser(ylow, yup);
            h1->GetXaxis()->SetRangeUser(xlow[i], xup[i]);

            c2->cd();
            if (k < 1)
                h1->Draw("AC");
            else
                h1->Draw("Csame");
            TLegendEntry *leg_entry0 = leg->AddEntry(h1, name[c] + Form("%d", legName[c][k]), "l");
            delete ew_hist;
            file->Close();
        }
        c2->cd();
        leg->Draw("same");
        // c2->Print(title[i]+"_wc.png");
        c2->Print("./pdf/" + title[i] + "_" + var_name[c] + "_" + LO + ".pdf");
        delete SM_hist;
        SM_file->Close();
        delete leg;
        delete c2;
    }
}
void plot_wc(bool lo)
{
    for (int i = 0; i < 3; i++)
        plot(i, lo);
}