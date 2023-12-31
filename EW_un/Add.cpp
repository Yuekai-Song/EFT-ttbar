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
void Add(){
    TString files[] = { "NNLO_18_TTToSemiLeptonic_TuneCP5_13TeV-powheg",
                       "NNLO_18_TTTo2L2Nu_TuneCP5_13TeV-powheg",
                       "NNLO_18_TTToHadronic_TuneCP5_13TeV-powheg",
                       "NNLO_17_TTToSemiLeptonic_TuneCP5_13TeV-powheg",
                       "NNLO_17_TTTo2L2Nu_TuneCP5_13TeV-powheg",
                       "NNLO_17_TTToHadronic_TuneCP5_13TeV-powheg",
                       "NNLO_16_TTToSemiLeptonic_TuneCP5_13TeV-powheg",
                       "NNLO_16_TTTo2L2Nu_TuneCP5_13TeV-powheg",
                       "NNLO_16_TTToHadronic_TuneCP5_13TeV-powheg",
                       "NNLO_15_TTToSemiLeptonic_TuneCP5_13TeV-powheg",
                       "NNLO_15_TTTo2L2Nu_TuneCP5_13TeV-powheg",
                       "NNLO_15_TTToHadronic_TuneCP5_13TeV-powheg"};
    TH2D* h2;
    Double_t xsec[12]={366.91*0.435, 89.05*0.435, 377.96*0.435, 366.91*0.301, 89.05*0.301, 377.96*0.301, 
                        366.91*0.122, 89.05*0.122, 377.96*0.122, 366.91*0.142, 89.05*0.142, 377.96*0.142};
    TString name[12] = {"_semi_18", "_2l_18", "_4q_18","_semi_17", "_2l_17", "_4q_17","_semi_16", "_2l_16", "_4q_16","_semi_15", "_2l_15", "_4q_15"};
    const int nbinsx=24;
    const int nbinsy=40;
    const double ylow=-5.0;
    const double yhigh=5.0;
    double xbins[]={340,360,380,400,420,440,460,480,500,520,540,560,580,600,620,640,660,680,700,750,800,900,1100,1500,3000};
    TFile* file;
    TFile* nnlo_file = new TFile("NNLO.root", "RECREATE");
    TH2D* nnlo =  new TH2D("NNLO", "", nbinsx, xbins, nbinsy, ylow, yhigh);
    for(int i=0; i<12; i++){
        file = TFile::Open("./output/"+files[i]+".root");
        h2 = (TH2D*)file->Get("h2_NNLO");
        h2->Scale(xsec[i]/h2->GetSumOfWeights());
        nnlo->Add(h2);
        delete h2;
        delete file;
    }
    nnlo_file->cd();
    nnlo->Write();
    nnlo_file->Close();
}