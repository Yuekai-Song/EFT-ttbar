#include<TROOT.h>
#include<Math/PdfFuncMathCore.h>
#include<TFile.h>
#include<TChain.h>
#include<TLorentzVector.h>
#include<TMath.h>
#include<TF1.h>
#include<TH1F.h>
#include<TH2F.h>
#include<string.h>
#include<fstream>
#include<iostream>
using namespace std;
void add_pre(int year)
{
    TString files[] = {Form("corr_semi_%d.root", year), Form("corr_lep_%d.root", year), Form("corr_had_%d.root", year)};
    Double_t xs[] = {365.34, 88.29, 377.96};
    Double_t lumi[] = {19.5, 16.8, 41.48, 59.83};

    TString hists[] = {"ecorr_vs_rm", "ecorr_vs_rm_ttx", "ecorr_vs_rm_ttx_like_cut", "ecorr_vs_rm_ttx_like_Jet_cut", "ecorr_vs_rm_ttx_like_jet_cut"};
    TString gen[] = {"", "_gw"};
    TH1 *hist;
    TH1 *h1[4];
    TFile *file[3];
    TFile *new_file = new TFile(Form("corr_%d.root", year), "recreate");
    for (int i = 0; i < 3; i++)
    {
        file[i] = TFile::Open(files[i]);
    }
    for (int i = 0; i < 5; i++)
    {
        for (int g = 0; g < 2; g++)
        {
            for (int j = 0; j < 3; j++)
            {
                h1[3] = (TH1 *)file[j]->Get("raw_nJets" + gen[g]);
                cout << hists[i] + gen[g] << endl;
                h1[j] = (TH1 *)file[j]->Get(hists[i] + gen[g]);
                h1[j]->Scale(xs[j] * 1000 * lumi[year - 15] / h1[3]->GetSumOfWeights());
                if (j == 0)
                    hist = h1[j];
                else
                    hist->Add(h1[j]);   
            }
            new_file->cd();
            hist->Write();
        }
    }
    new_file->Close();
}
void add()
{
    for (int year = 15; year < 19; year++)
        add_pre(year);
}