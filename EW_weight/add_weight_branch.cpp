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
void add_weight_branch_tree(TString fileName, TString tree_name){
    TH2F* hist[9];
    //TString dir="/home/yksong/code/ttbar/EW_weight/correction_roots/";
    TString dir = "./correction_roots/";
    TString mtop = "";
    TString infile;
    TFile *fhist[5];

    if(fileName.Contains("mtop171p5"))
        mtop = "_mt171p5";
    if(fileName.Contains("mtop169p5"))
        mtop = "_mt169p5";
    if(fileName.Contains("mtop173p5"))
        mtop = "_mt173p5";
    if(fileName.Contains("mtop175p5"))
        mtop = "_mt175p5";
    
    Int_t Cpq3[5] = {0, 0, 0, 0, 0};
    Int_t Cpu[5] = {0, 2, 1, 0, 0};
    Int_t ReCup[5] = {0, 0, 0, 1, 0};
    Int_t ImCup[5] = {0, 0, 0, 0, 1};
    for(Int_t i=0; i<5; i++){
        infile = Form("EWci%d%d%d%d",Cpq3[i],Cpu[i],ReCup[i],ImCup[i]) + mtop + ".root";
        fhist[i] = TFile::Open(dir+infile);
        hist[i] = (TH2F*)fhist[i]->Get("h2");
        hist[i]->SetName(Form("2D_%d", i));
    }

    TFile *file = new TFile(fileName, "update");
    cout<<"add NLO_EW weight on "<<tree_name<<endl;
    TTree *mytree=(TTree*)file->Get(tree_name);
    Float_t M_tt, delta_rapidity;
    mytree->SetBranchAddress("M_tt_gen", &M_tt);
    mytree->SetBranchAddress("delta_rapidity_gen", &delta_rapidity);
    Float_t weight[9];TBranch* branch[9];
    for(int i=0; i<5; i++){
        TString weight_name = Form("weight_ci%d%d%d%d", Cpq3[i],Cpu[i],ReCup[i],ImCup[i]);
        branch[i] = mytree->Branch(weight_name, &weight[i], weight_name+"/F");
    }
    Int_t entries=mytree->GetEntries();
    cout<<"total number of events: "<<entries<<endl;
    for(Int_t i=0; i<entries; i++){
        mytree->GetEntry(i);
        for(Int_t w=0; w<5; w++){
            Int_t nbin = hist[w]->FindBin(M_tt, delta_rapidity);
            weight[w] = 1.0 + hist[w]->GetBinContent(nbin);
            branch[w]->Fill();
            //cout<<"weight[i]: "<<weight[i]<<endl;
        }
    }
    file->cd();
    mytree->Write("", TObject::kOverwrite);
    delete mytree;
    file->Close();
    for(int i=0; i<5; i++){
        delete hist[i];
        fhist[i]->Close();
    }
}
void add_weight_branch(TString inputFile, bool is_sys, int year){
    TString trees[]={"jerUp", "jerDown", "unclusUp", "unclusDown"};
    TString jes_source[] = {"Absolute", Form("Absolute_%d", year), "FlavorQCD", "BBEC1", "EC2", "HF", Form("BBEC1_%d", year), Form("EC2_%d", year), "RelativeBal", Form("RelativeSample_%d", year)};
    add_weight_branch_tree(inputFile, "mytree");
    if(!is_sys){
        for(int i=0; i<4; i++){
            add_weight_branch_tree(inputFile, trees[i]);
        }
        for(int i=0; i<10; i++){
            add_weight_branch_tree(inputFile, "jes_"+jes_source[i]+"Up");
            add_weight_branch_tree(inputFile, "jes_"+jes_source[i]+"Down");
        }
    }

    cout<<"EW weights has been added to "<<inputFile<<endl;
}