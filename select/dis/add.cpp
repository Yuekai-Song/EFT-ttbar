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
void add()
{
    TString files[] = {"dis_2015.root", "dis_2016.root", "dis_2017.root", "dis_2018.root"};
    Double_t lumis[] = {0.142, 0.122, 0.301, 0.435};
    double bin_width[] = {4, 4, 4, 2, 10, 10, 4, 2};
    TString hists[] = {"mth_vs_mwh_4", "mth_vs_mwh_ttx_4", "mtl_vs_mwl_4", "Dnu_4", "mth_3", "mth_ttx_3", "mtl_vs_mwl_3", "Dnu_3"};
    TH1 *hist;
    TH1 *h1[4];
    TFile *file[4];
    TFile *new_file = new TFile("dis.root", "recreate");
    for (int i = 0; i < 4; i++)
    {
        file[i] = TFile::Open(files[i]);
    }
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            h1[j] = (TH1 *)file[j]->Get(hists[i]);
            h1[j]->Scale(lumis[j] / (bin_width[i] * h1[j]->GetSumOfWeights()));
            if (j == 0)
                hist = h1[j];
            else
                hist->Add(h1[j]);   
        }
        new_file->cd();
        hist->Write();
    }
    new_file->Close();
}