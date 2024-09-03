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
    TString EW[] = {"_weight_ci0000","_weight_ci0010","_weight_ci0100","_weight_ci0001"};
    TString weight_ew[] = {"weight_ci0000","weight_ci0010","weight_ci0100", "weight_ci0001"};
    for(int i=0;i<nsample;i++)
        fileNames[i]=fileNames[i].ReplaceAll(".root","*.root");
    TString file_up, file_down; 
    TString title[] = {"Mtt", "deltay"};
    TString xvars[] = {"mass_tt","rapidity_tt"};
    Double_t xup[] = {1500, 3};
    Double_t xdown[] = {200, -3};
    Int_t bins[]={26, 24};
    TString weight_up, weight_down;
    TH1D *hmc0, *hmc1, *hmc2, *hmc3;
    TH1D *hmc_s0, *hmc_s1, *hmc_s2, *hmc_s3;
    TH1D *hist0, *hist1, *hist2, *hist3; 
    TString path = Form("./%d/",year);
    TFile* file = new TFile(path+"EW_un/"+file_name, "recreate");
    for(int w=0; w<4; w++){
        for(int i=0; i<2; i++){
            for(int j=0; j<3; j++){
                TString weight = "Generator_weight*SF_btag*SF_lepton*L1PreFiringWeight_Nom*nnlo_wt"+other_con1+other_con2;
                TChain* tree=new TChain("mytree");
                hist1 = new TH1D("CG1","", bins[i], xdown[i], xup[i]);
                hist2 = new TH1D("CG2","", bins[i], xdown[i], xup[i]);
                hist3 = new TH1D("CG3","", bins[i], xdown[i], xup[i]);
                hist1->Sumw2();
                hist2->Sumw2();
                hist3->Sumw2();
                renew_weight(&weight, fileNames[j], j, path);
                tree->Add(path+"MC/"+fileNames[j]);
                if(w == 0){
                    hist0 = new TH1D("CG","", bins[i], xdown[i], xup[i]);
                    tree->Draw(xvars[i]+">>CG", weight);
                }
                tree->Draw(xvars[i]+">>CG1", weight+"*"+weight_ew[w]);
                tree->Draw(xvars[i]+">>CG2", weight+"*"+weight_ew[w]+"*"+weight_ew[w]+"_EWUp");
                tree->Draw(xvars[i]+">>CG3", weight+"*"+weight_ew[w]+"*"+weight_ew[w]+"_EWDown");
                //cout<<hist1->GetSumOfWeights()<<" "<<hist2->GetSumOfWeights()<<endl;
                if(j==0){
                    if(w == 0){ 
                        hmc0 = (TH1D*)hist0->Clone();
                        hmc0->SetName(title[i]+"_nom");
                    }
                    hmc1 = (TH1D*)hist1->Clone();
                    hmc1->SetName(title[i]+EW[w]+"_nom");
                    hmc2 = (TH1D*)hist2->Clone();
                    hmc2->SetName(title[i]+EW[w]+"_up");
                    hmc3 = (TH1D*)hist3->Clone();
                    hmc3->SetName(title[i]+EW[w]+"_dn");
                }
                else{
                    if(w == 0){
                        hmc0->Add(hist0);
                    }
                    hmc1->Add(hist1);
                    hmc2->Add(hist2);
                    hmc3->Add(hist3);
                } 
                if(w == 0)
                    delete hist0;             
                delete tree;
                delete hist1;
                delete hist2;
                delete hist3;
            }
            cout<<weight_ew[w]<<" "<<title[i]<<" finished"<<endl;
            file->cd();
            if(w == 0){
                hmc0->Write();
                delete hmc0;
            }
            hmc2->Write();
            hmc3->Write();
            hmc1->Write();
            delete hmc1; 
            delete hmc2;
            delete hmc3;
        }
    }
    file->Close();
}
void derive_ew(){
    TString other_con1[] = {"*(jet_num>=4)", "*(jet_num==3 && jet_pt[0]>50)"};
    TString other_con2[] = {"*lep_flavour", "*((!lep_flavour) && (lepton_pt>34))"};
    TString other_con3 = "*(jet_num>=4 || (jet_num==3 && jet_pt[0]>50))";
    TString other_con4 = "*(lep_flavour || ((!lep_flavour) && lepton_pt>34))";
    TString lepton[] ={"_M", "_E"};
    TString jet[] = {"_4jets", "_3jets"};
    //draw_pre(2018, other_con3, other_con4, "EWun.root");
    for(int l=0; l<2; l++){
        for(int j=0; j<2; j++){
            draw_pre(2018, other_con1[j], other_con2[l], "EWun"+lepton[l]+jet[j]+".root");
        }
    }
}