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
void add_pu_tree(TString fileName, TString tree_name, int year, bool remain_sys){
    Int_t Pileup_nPU;
    TH1D *hn, *hu, *hd;
    Float_t pu_wt, pu_wt_up, pu_wt_dn;
    TFile *file=new TFile(fileName,"update");
    cout<<"add Pileup weight on "+tree_name<<endl;
    TTree *mytree=(TTree*) file->Get(tree_name);
    mytree->SetBranchAddress("Pileup_nPU",&Pileup_nPU);
    TFile* nfile = TFile::Open(Form("./%d/pu_weight.root", year));
    hn = (TH1D*)nfile->Get("pu_nom");
    hu = (TH1D*)nfile->Get("pu_up"); 
    hd = (TH1D*)nfile->Get("pu_down"); 
    TBranch *branch1, *branch2, *branch3;
    branch1 = mytree->Branch("pu_wt", &pu_wt, "pu_wt/F");
    if(remain_sys){
        branch2 = mytree->Branch("pu_wt_up", &pu_wt_up, "pu_wt_up/F");
        branch3 = mytree->Branch("pu_wt_dn", &pu_wt_dn, "pu_wt_dn/F");
    }
    for(int i=0; i<mytree->GetEntries(); i++){
        mytree->GetEntry(i);
        Int_t bin = hn->FindBin(Pileup_nPU);
        pu_wt = hn->GetBinContent(bin);
        pu_wt_up = hu->GetBinContent(bin);
        pu_wt_dn = hd->GetBinContent(bin);
        branch1->Fill();
        if(remain_sys){
            branch2->Fill();
            branch3->Fill();
        }
    }
    file->cd();
    mytree->Write("", TObject::kOverwrite);
    delete mytree;
    file->Close();
    delete hn; delete hu; delete hd;
    nfile->Close();
}
void add_pu(TString inputFile, int year, bool is_sys){
    TString trees[]={"jerUp", "jerDown", "unclusUp", "unclusDown"};
    TString jes_source[] = {"Absolute", Form("Absolute_%d", year), "FlavorQCD", "BBEC1", "EC2", "HF", Form("BBEC1_%d", year), Form("EC2_%d", year), "RelativeBal", Form("RelativeSample_%d", year)};
    
    if(!is_sys){
        add_pu_tree(inputFile, "mytree", year, true);
        for(int i=0; i<4; i++){
            add_pu_tree(inputFile, trees[i], year, false);
        }
        for(int i=0; i<10; i++){
            add_pu_tree(inputFile, "jes_"+jes_source[i]+"Up", year, false);
            add_pu_tree(inputFile, "jes_"+jes_source[i]+"Down", year, false);
        }
    }
    else{
        add_pu_tree(inputFile, "mytree", year, false);
    }
}