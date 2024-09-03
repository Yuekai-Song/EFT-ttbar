#include<TROOT.h>
#include<TStyle.h>
#include<Math/PdfFuncMathCore.h>
#include<TFile.h>
#include<TChain.h>
#include<TLorentzVector.h>
#include<TMath.h>
#include<TLegend.h>
#include<TF1.h>
#include<TH1D.h>
#include<TCanvas.h>
#include<TH2F.h>
#include<THStack.h>
#include<string.h>
#include<fstream>
#include<iostream>
using namespace std;
double p2weight=0.65/0.3;
void renew_weight(TString* weight, TString file, int j, TString path){
    const int nsample = 3;
    Float_t cross_sections[nsample]={366.91, 89.05, 377.96};
    Float_t K_Factor[nsample]={1.0, 1.0, 1.0,};
    float lumi=59.83;
    auto c1 = new TCanvas("c0", "c0", 8, 30, 600, 600); // temporary canvas
    TChain* tree1=new TChain("rawtree"); 
    tree1->Add(path+"MC/"+file);
    TH1D *nmc=new TH1D("nmc","",50,0,100);
    nmc->Sumw2();
    c1->cd();
    tree1->Draw("nJet>>nmc","Generator_weight");
    //cout<<nmc->GetSumOfWeights()<<endl;
    float weight1=cross_sections[j]*lumi/(nmc->GetSumOfWeights())*K_Factor[j]*1000;
    //cout<<weight1<<endl;
    *weight = Form("%f*", weight1) + (*weight);
    delete tree1;
    delete nmc;
    delete c1;
}
void draw_pre(int year, TString other_con1, TString other_con2, TString file_name){
    const int nsample = 3;
    TString fileNames[nsample] = {  
                            "new_TTToSemiLeptonic_TuneCP5_13TeV-powheg.root",
                            "new_TTTo2L2Nu_TuneCP5_13TeV-powheg.root",
                            "new_TTToHadronic_TuneCP5_13TeV-powheg.root"
                            };
    for(int i=0;i<nsample;i++)
        fileNames[i]=fileNames[i].ReplaceAll(".root","*.root");
    TString file_up, file_down; 
    TString title[] = {"top_pt_gen", "Mtt_gen", "deltay_gen","top_pt", "Mtt", "deltay"};
    TString xvars[] = {"top_pt", "M_tt_gen", "delta_rapidity_gen", "rectop_pt",  "mass_tt", "rapidity_tt"};
    Double_t xup[] = {500, 1500, 3, 500, 1500, 3};
    Double_t xdown[] = {0, 300, -3, 0, 300, -3};
    Int_t bins[]={25, 24, 24, 25, 24, 24};
    TString weight_up, weight_down;
    TH1D *hmc0, *hmc1;
    TH1D *hist0, *hist1; 
    TString path = Form("./%d/",year);
    TFile* file = new TFile(path+file_name, "update");
    for(int i=0; i<3; i++){
        for(int j=0; j<3; j++){
            TString weight = "Generator_weight*SF_btag*SF_lepton*L1PreFiringWeight_Nom"+other_con1+other_con2;
            TString weight1 = "Generator_weight*SF_btag*SF_lepton*L1PreFiringWeight_Nom*nnlo_wt"+other_con1+other_con2;
            TChain* tree=new TChain("mytree");
            hist1 = new TH1D("CG1","", bins[i], xdown[i], xup[i]);
            hist0 = new TH1D("CG","", bins[i], xdown[i], xup[i]);
            
            hist0->Sumw2();
            hist1->Sumw2();

            renew_weight(&weight, fileNames[j], j, path);
            renew_weight(&weight1, fileNames[j], j, path);
            tree->Add(path+"MC/"+fileNames[j]);
            tree->Draw(xvars[i]+">>CG", weight);
            tree->Draw(xvars[i]+">>CG1", weight1);

            //cout<<hist1->GetSumOfWeights()<<" "<<hist2->GetSumOfWeights()<<endl;
            if(j==0){
                hmc0 = (TH1D*)hist0->Clone();
                hmc0->SetName(title[i]+"_nlo");
                hmc1 = (TH1D*)hist1->Clone();
                hmc1->SetName(title[i]+"_nnlo");
            }
            else{
                hmc0->Add(hist0);
                hmc1->Add(hist1);
            } 
            delete hist0;             
            delete tree;
            delete hist1;
        }
        cout<<title[i]<<" finished"<<endl;
        file->cd();
        hmc0->Write();
        delete hmc0;
        hmc1->Write();
        delete hmc1; 
    }
    file->Close();
}
void derive_nnlo(){
    TString other_con1[] = {"*(jet_num>=4)", "*(jet_num==3 && jet_pt[0]>50)"};
    TString other_con2[] = {"*lep_flavour", "*((!lep_flavour) && (lepton_pt>34))"};
    TString other_con3 = "*(jet_num>=4 || (jet_num==3 && jet_pt[0]>50))";
    TString other_con4 = "*(lep_flavour || ((!lep_flavour) && lepton_pt>34))";
    TString lepton[] ={"_M", "_E"};
    TString jet[] = {"_4jets", "_3jets"};
    //draw_pre(2018, other_con3, other_con4, "EWun.root");
    for(int l=0; l<1; l++){
        for(int j=0; j<1; j++){
            draw_pre(2018, other_con1[j], other_con2[l], "nnlo"+lepton[l]+jet[j]+".root");
        }
    }
}