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
void norm()
{
    TString files[] = {"dis_2015.root", "dis_2016.root", "dis_2017.root", "dis_2018.root"};
    double bin_width[] = {4, 4, 4, 2, 10, 10, 4, 2};
    TString hists[] = {"mth_vs_mwh_4", "mth_vs_mwh_ttx_4", "mtl_vs_mwl_4", "Dnu_4", "mth_3", "mth_ttx_3", "mtl_vs_mwl_3", "Dnu_3"};
    TH1 *hist;
    TH1 *h1;
    TFile *file[4];
    TFile *new_file[4];
    for (int j = 0; j < 4; j++)
    {
        file[j] = TFile::Open("./original/" + files[j]);
        new_file[j] = new TFile(files[j], "recreate");
        for (int i = 0; i < 8; i++)
        {
            h1 = (TH1 *)file[j]->Get(hists[i]);
            h1->Scale(1.0 / (bin_width[i] * h1->GetSumOfWeights()));
            new_file[j]->cd();
            h1->Write();
        }
        new_file[j]->Close();   
    }
}