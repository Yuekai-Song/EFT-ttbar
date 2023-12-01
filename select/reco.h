#include <TROOT.h>
#include <Math/PdfFuncMathCore.h>
#include <TFile.h>
#include <TChain.h>
#include <TLorentzVector.h>
#include <TMath.h>
#include <TF1.h>
#include <TH1F.h>
#include <TH2F.h>
#include <iostream>
using namespace std;

class RECO
{
private:
    int num_jets;
    TLorentzVector LHE_bl, LHE_bh, LHE_j1, LHE_j2;
    Float_t *btag_score;
    int *index;
    bool ttx = false;
    bool gen_reco = 0; // 0: reco using likelihood and compare with gen; 1:reco using gen
    bool LHE = 0;
    // static is only due to I have to use the function pointer likelihood that is needed to be (Double_t *, Double_t* )
    TLorentzVector *mom_jets;
    TLorentzVector mom_lep;
    Float_t met_pt, met_phi;
    int reco_index[4]; // 0: b_lep, 1: b_had, 2: l_jet1, 3: l_jet2
    int gen_index[4];  // 0: b_lep, 1: b_had, 2: l_jet1, 3: l_jet2 from gen level particles
    int chi2_index[4]; // 0: b_lep, 1: b_had, 2: l_jet1, 3: l_jet2 //only for jet_num>=4
    void set_index();
    void btag_sort();
    bool gen_sort();

    void chi2_sort();
    void like_sort();
    double nusolver(int index_bl);
    double nusolver1(int index_bl);
    double nusolver2(int index_bl);
    Double_t likelihood_had(int bh, int j1, int j2);
    Double_t likelihood_had(int bh, int j1);
    Double_t chi2(int bh, int j1, int j2);
    int select_LHE(TLorentzVector lhe_part);
    bool reco_top();
    void reco_chi();

public:
    static TH2D *mth_vs_mwh_4, *mtl_vs_mwl_4, *mtl_vs_mwl_3, *mth_vs_mwh_ttx_4;
    static TH1D *Dnu_4, *Dnu_3, *mth_ttx_3, *mth_3;
    Double_t mass_bjj, mass_jj, mass_lb;
    TLorentzVector mom_th, mom_tl, mom_wh, mom_wl;

    double like = 0;
    double D_nu = 0;
    double chi = 0;
    int category = 0; // 0: non_reco; 1: correct reco; 2: wrong reco
    TLorentzVector mom_nu{0, 0, 0, -1};
    bool diff();
    RECO(int jet_num, TLorentzVector *mom_alljets, TLorentzVector mom_lepton, Float_t MET_pt, Float_t MET_phi, Float_t *btag_scores);
    void set_ttx(bool ttxs);
    void set_gen(bool gen);
    void set_LHE(TLorentzVector LHE_bl, TLorentzVector LHE_bh, TLorentzVector LHE_j1, TLorentzVector LHE_j2);
    bool reco();
    ~RECO();
};

void RECO::set_gen(bool gen)
{
    gen_reco = gen;
}

void RECO::set_index()
{
    for (int i = 0; i < num_jets; i++)
        index[i] = i;
}

void RECO::set_ttx(bool ttxs)
{
    ttx = ttxs;
}

void RECO::set_LHE(TLorentzVector bl, TLorentzVector bh, TLorentzVector j1, TLorentzVector j2)
{
    LHE_bl = bl;
    LHE_bh = bh;
    LHE_j1 = j1;
    LHE_j2 = j2;
    LHE = true;
}

RECO::RECO(int jet_num, TLorentzVector *mom_alljets, TLorentzVector mom_lepton, Float_t MET_pt, Float_t MET_phi, Float_t *btag_scores)
{
    num_jets = jet_num;
    mom_lep = mom_lepton;
    mom_jets = new TLorentzVector[jet_num];
    index = new int[jet_num];
    btag_score = new Float_t[jet_num];
    for (int i = 0; i < jet_num; i++)
    {
        mom_jets[i] = mom_alljets[i];
        btag_score[i] = btag_scores[i];
    }
    set_index();
    met_pt = MET_pt;
    met_phi = MET_phi;
}

TH2D *RECO::mth_vs_mwh_ttx_4 = NULL;
TH2D *RECO::mth_vs_mwh_4 = NULL;
TH2D *RECO::mtl_vs_mwl_4 = NULL;
TH2D *RECO::mtl_vs_mwl_3 = NULL;
TH1D *RECO::mth_3 = NULL;
TH1D *RECO::mth_ttx_3 = NULL;
TH1D *RECO::Dnu_4 = NULL;
TH1D *RECO::Dnu_3 = NULL;


RECO::~RECO()
{
    delete[] mom_jets;
    delete[] index;
    delete[] btag_score;
}