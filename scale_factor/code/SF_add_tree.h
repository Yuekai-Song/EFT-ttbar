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
#include"json/json.h"
#include"lib_json/json_reader.cpp"
#include"lib_json/json_value.cpp"
#include"lib_json/json_writer.cpp"
#include "BTagCalibrationStandalone.h"
#include "BTagCalibrationStandalone.cpp"
using namespace std;
class SF_add_tree{
private:
    int year;
    TString inputFile;
    TH2F *hist_elec_id, *hist_elec_reco_a20, *hist_elec_reco_b20;
    TH2F *hist_muon_m_id, *hist_muon_m_iso;
    TH2F *h2_djEff_b, *h2_djEff_c, *h2_djEff_udsg;
    TString puid;
    TH2F *h2_puid, *h2_puid_un;
    TFile *fhist_puid;
    TFile *file, *fhist_elec_id, *fhist_elec_reco_a20, *fhist_elec_reco_b20, *fhist_muon_m_id, *fhist_muon_m_iso, *fhist_beff;
    ifstream if_muon_m_reco, if_muon_l_reco, if_muon_l_id;
    Json::Reader reader_muon_m_reco, reader_muon_l_reco, reader_muon_l_id;
    Json::Value root_muon_m_reco, root_muon_l_reco, root_muon_l_id;
    BTagCalibration_offReader *reader, *reader_bc, *reader_l, *reader_it;
    void get_string1(Float_t pt, Float_t eta, TString s[2]);
    void get_string2(Float_t eta, TString s[2]);
    void sf_electronreco(Float_t pt, Float_t eta, Float_t weight[3]);
    void sf_electronid(Float_t pt, Float_t eta, Float_t weight[3]);
    void sf_muoniso(Float_t pt, Float_t eta, Float_t weight[3]);
    void sf_muonid(Float_t pt, Float_t eta, Float_t weight[3]);
    void sf_muonreco(Float_t pt, Float_t eta, Float_t weight[3]);
    void sf_btag(BTagEntry_off::JetFlavor flav, Float_t pt, Float_t eta, Float_t sf[13]);
    Float_t btag_eff(Int_t flav, Float_t pt, Float_t eta);
    void sf_lep(Float_t pt, Float_t eta, Bool_t flavour, Float_t weight[3]);
    void sf_jet(Float_t *pt, Float_t *eta, Int_t *flavour, Float_t* score, UInt_t jet_num, Float_t weight[13]);
    void sf_jet_it(Float_t *pt, Float_t *eta, Int_t *flavour, Float_t* score, UInt_t jet_num, Float_t* weight);
    void sf_pu(Float_t *pt, Float_t *eta, UInt_t jet_num, Float_t weight[3]);
    void set_dir();
public:
    SF_add_tree(TString inputFile_s, TString tree_name, bool remain_sys, int year_s);
    ~SF_add_tree();
};