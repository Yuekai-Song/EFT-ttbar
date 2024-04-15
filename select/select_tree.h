#include<TROOT.h>
#include<Math/PdfFuncMathCore.h>
#include<TFile.h>
#include<TChain.h>
#include<TLorentzVector.h>
#include<TMath.h>
#include<TF1.h>
#include<TH1F.h>
#include<TH2F.h>
#include<iostream>
#include <random>
using namespace std;

int getRandomIndex(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}

enum DATA_TYPE {
    data,
    MC,
    tree_sys,
    MC_sys
};
enum CATEGORY {
    A,
    B,
    C,
    D
};
enum OP_TYPE {
    select_reco,
    select_reco_ttx,
    dis_reco_need,
    e_corr_3jets
};
enum OBJECT_SELECT_ORDER {
    jet_lepton,
    lepton_jet
};
enum OBJECT_TYPE {
    lepton,
    jet
};
class read_object{
public:
    read_object(TString input, int type);
    UInt_t nj, ne, nm, ng;
};


class select_tree{
private:
    TString input;
    TString tree_name;
    int year;
    DATA_TYPE data_type;
    OP_TYPE op_type;
    OBJECT_SELECT_ORDER order;
    TChain* chain;
    TFile* output;
    Float_t btag_criteria;
    TLorentzVector *mom_jets, mom_lep;
    UInt_t jet_num;
    UInt_t nBtag;
    Bool_t lep_flavour;
    Int_t lep_c;
    Float_t MET_pt, MET_phi;
    UInt_t nMuon, nElectron, nJet, nGenJet;
    UInt_t nm, ne, nj, ng;
    Float_t *Electron_eta, *Electron_mass, *Electron_pt, *Electron_phi;
    Float_t *Muon_mass, *Muon_phi, *Muon_pt, *Muon_eta;
    Int_t *Muon_charge, *Electron_charge;
    Float_t *Jet_btagDeepB, *Jet_btagDeepFlavB, *Jet_eta, *Jet_mass, *Jet_phi, *Jet_pt;
    Float_t *GenJet_pt, *GenJet_eta, *GenJet_phi, *GenJet_mass;
    Int_t *Jet_partonFlavour, *Jet_hadronFlavour;
    Float_t *Muon_pfRelIso04_all;
    Int_t *Electron_cutBased, *Jet_jetId;
    Bool_t *Muon_tightId, *Muon_looseId;
    Float_t *Electron_deltaEtaSC, *Electron_dxy, *Electron_dz;
    Bool_t HLT_Ele32_WPTight_Gsf, HLT_IsoMu27, HLT_IsoMu24, HLT_Mu50, HLT_OldMu100, HLT_TkMu100, HLT_Ele115_CaloIdVT_GsfTrkIdT;
    Bool_t HLT_TkMu50, HLT_Photon175, HLT_IsoTkMu24, HLT_Ele27_WPTight_Gsf, HLT_Ele35_WPTight_Gsf, HLT_Photon200;
    Bool_t HLT_Mu27, HLT_Ele23_CaloIdM_TrackIdM_PFJet30;
    ULong64_t event;
    Float_t Generator_weight;
    UInt_t luminosityBlock, run;
    Float_t LHEPart_eta[9], LHEPart_mass[9], LHEPart_phi[9], LHEPart_pt[9];
    Int_t LHEPart_pdgId[9], LHEPart_status[9];
    UInt_t nLHEPart;
    Float_t LHEScaleWeight[9], PSWeight[4],LHEPdfWeight[150];
    UInt_t nLHEPdfWeight, nLHEScaleWeight, nPSWeight;

    Float_t L1PreFiringWeight_Nom, L1PreFiringWeight_Up, L1PreFiringWeight_Dn; 
    Bool_t Flag_met[11];
    UInt_t nFlag_met;
    Bool_t met_match;
    Int_t PV_npvsGood, Pileup_nPU, PV_npvs;

    Float_t lepton_mass, lepton_phi, lepton_eta, lepton_pt;
    Float_t *jet_btagDeepB, *jet_btagDeepFlavB, *jet_eta, *jet_pt, *jet_phi, *jet_mass;
    Int_t *jet_partonFlavour, *jet_hadronFlavour;
    Float_t max_score;
    Float_t MtW;
    Float_t electron_deltaEtaSC;
    Float_t mass_tt_uncorr;
    Double_t corr_f;
    Float_t rectop_pt, mass_tt, rapidity_tt, recantitop_pt;
    Float_t mass_tlep, mass_thad, neutrino_pz;
    Float_t mass_wlep, mass_whad, mass_t, mass_at;
    Float_t mass_bjj, mass_jj, mass_lb;
    Double_t like, chi, D_nu;
    Float_t M_tt_gen, delta_rapidity_gen;
    Int_t lep_charge;
    Float_t top_pt, top_eta, top_phi, top_mass; 
    Float_t antitop_pt, antitop_eta, antitop_phi, antitop_mass;
    int index_b, index_antib, index_up, index_down, index_lep, index_nu;
    TLorentzVector p4_b, p4_antib, p4_up, p4_down, p4_lep, p4_nu, p4_top,p4_antitop;
    int index_lepn, index_nun, index_lepp, index_nup;
    TLorentzVector  p4_lepn, p4_nun, p4_lepp, p4_nup;
    int  index_upbar, index_downbar;
    TLorentzVector  p4_upbar, p4_downbar;
    Float_t ctstar;
    int index;
    Int_t LHE_nhad, LHE_nlep;
    Float_t muR_up, muF_up, ISR_up, FSR_up;
    Float_t muR_down, muF_down, ISR_down, FSR_down;
    Float_t pdf_up, pdf_dn, alphas_dn, alphas_up;
    int category;
    CATEGORY cate;
    Bool_t ele_trigger, mu_trigger;
    Bool_t select_jet();
    Bool_t select_lep();
    void loop(TTree* trees[2], TH1* hists[20]);
    Bool_t is_lep_from_jet(TLorentzVector mom, OBJECT_TYPE object_type);
    void read_LHE();
    void read_sys();
    void pdf_w(Float_t LHEPdfWeight[103], Float_t &alphas_up, Float_t &alphas_dn, Float_t &pdf_up, Float_t &pdf_dn);
    //elctron cut-based id break down
    Int_t *Electron_vidNestedWPBitmap;
    Bool_t id_noiso(Int_t i, Int_t wp);
    Int_t iso_select(Int_t i);
public:
    static TF1* h_ecorr;
    select_tree(TString inputfile, TString outputFile, TString name_tree, TString name_jet, TString name_MET, int s_year, DATA_TYPE data_types, OP_TYPE op_types, OBJECT_SELECT_ORDER order_type, CATEGORY cates, int num_j, int num_e, int num_m, int num_g = 0);//type: 0:data; 1:MC nom; 2:MC sys 3:sys nom
    void write_select();
    void write_distribution();
    void write_ecorr();
    void write();
    ~select_tree(); 
};

TF1* select_tree::h_ecorr = NULL;