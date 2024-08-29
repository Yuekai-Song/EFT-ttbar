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
void nnlo_add_tree(TString inputFile, TString tree_name, bool remain_sys){
    Float_t nnlo_wt_nom, nnlo_wt, nnlo_wt_up, nnlo_wt_down;
    Float_t mtt, ctstar;
    TFile* file=new TFile(inputFile,"update");
    cout<<"add NNLO_QCD weight on "<<tree_name<<endl;
    TTree* mytree=(TTree*) file->Get(tree_name);
    TBranch* nnlo = mytree->Branch("nnlo_wt", &nnlo_wt, "nnlo_wt/F"); 
    TBranch* nnlo_nom = mytree->Branch("nnlo_wt_nom", &nnlo_wt_nom, "nnlo_wt_nom/F"); 
    TBranch *nnlo_up, *nnlo_down;
    if(remain_sys){
        nnlo_up = mytree->Branch("nnlo_wt_up", &nnlo_wt_up, "nnlo_wt_up/F");
        nnlo_down = mytree->Branch("nnlo_wt_down", &nnlo_wt_down, "nnlo_wt_down/F");
    }
    mytree->SetBranchAddress("M_tt_gen", &mtt);
    mytree->SetBranchAddress("ctstar", &ctstar);
    TFile* nfile = TFile::Open("./NNLO_to_Powheg_Ratios_Summer20UL_CombineYears.root");
    TH2D* h2_nom = (TH2D*) nfile->Get("MATRIX_xsec_central_over_AllYears_Summer20UL_Normalized_Ratios");
    TH2D* h2 = (TH2D*) nfile->Get("MATRIX_xec_central");
    TH2D* h2_up = (TH2D*) nfile->Get("MATRIX_xec_up");
    TH2D* h2_down = (TH2D*) nfile->Get("MATRIX_xec_down");
    TH2D* h2_d = (TH2D*) nfile->Get("AllYears_Summer20UL_xsec");
    h2_up->Divide(h2);
    h2_down->Divide(h2);
    h2->Scale(1.0/h2->Integral());
    //h2_up->Scale(1.0/h2_up->Integral());
    //h2_down->Scale(1.0/h2_down->Integral());
    h2_d->Scale(1.0/h2_d->Integral());
    for(int num=0; num<mytree->GetEntries();num++){
        mytree->GetEntry(num);
        if(mtt<=3500){
            int bin_nom = h2_nom->FindBin(mtt,ctstar);
            int bin_d = h2_d->FindBin(mtt,ctstar);
            int bin_r = h2->FindBin(ctstar,mtt);
            int bin_ru = h2_up->FindBin(ctstar,mtt);
            int bin_rd = h2_down->FindBin(ctstar,mtt);
            nnlo_wt_nom = h2_nom->GetBinContent(bin_nom);
            nnlo_wt_up = h2_up->GetBinContent(bin_ru) * h2_nom->GetBinContent(bin_nom);
            nnlo_wt_down = h2_down->GetBinContent(bin_rd) * h2_nom->GetBinContent(bin_nom);
            nnlo_wt = h2->GetBinContent(bin_r)/h2_d->GetBinContent(bin_d);
            if(nnlo_wt!=nnlo_wt)
                nnlo_wt = 1.0;
        }
        else{
            nnlo_wt_nom = 1.0;
            nnlo_wt = 1.0;
            nnlo_wt_up = 1.0;
            nnlo_wt_down = 1.0;
        }
        nnlo->Fill();
        nnlo_nom->Fill();
        if(remain_sys){
            nnlo_up->Fill();
            nnlo_down->Fill();
        }
    }
    file->cd();
    mytree->Write("", TObject::kOverwrite);
    delete mytree;
    file->Close();
    delete h2_nom;
    delete h2;
    delete h2_d;
    delete h2_up;
    delete h2_down;
    nfile->Close();
}

void nnlo_add(TString inputFile, bool is_sys, int year){
    TString trees[]={"jerUp", "jerDown", "unclusUp", "unclusDown"};
    TString jes_source[] = {"Absolute", Form("Absolute_%d", year), "FlavorQCD", "BBEC1", "EC2", "HF", Form("BBEC1_%d", year), Form("EC2_%d", year), "RelativeBal", Form("RelativeSample_%d", year)};
    
    if(!is_sys){
        nnlo_add_tree(inputFile, "mytree", true);
        for(int i=0; i<4; i++){
            nnlo_add_tree(inputFile, trees[i], false);
        }
        for(int i=0; i<10; i++){
            nnlo_add_tree(inputFile, "jes_"+jes_source[i]+"Up", false);
            nnlo_add_tree(inputFile, "jes_"+jes_source[i]+"Down", false);
        }
    }
    else{
        nnlo_add_tree(inputFile, "mytree", false);
    }
}
