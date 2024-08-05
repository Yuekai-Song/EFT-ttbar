#include "SF_add_tree.h"
void SF_add_tree::get_string1(Float_t pt, Float_t eta, TString s[2])
{
    Float_t absEta[] = {0.00, 0.90, 1.20, 2.10, 2.40};
    Float_t Pt[] = {2.00, 2.50, 2.75, 3.00, 3.25, 3.50, 3.75, 4.00, 4.50, 5.00, 6.00, 8.00, 10.00, 15.00};
    int i, j;
    for (i = 0; i < 4; i++)
    {
        if (eta >= absEta[i] && eta <= absEta[i + 1])
            break;
    }
    for (j = 0; j < 13; j++)
    {
        if (pt >= Pt[i] && pt <= Pt[i + 1])
            break;
    }
    s[0] = Form("abseta:[%.2f,%.2f]", absEta[i], absEta[i + 1]);
    s[1] = Form("pt:[%.2f,%.2f]", Pt[j], Pt[j + 1]);
}
void SF_add_tree::get_string2(Float_t eta, TString s[2])
{
    Float_t absEta[] = {0.0, 0.9, 1.2, 2.1, 2.4};
    int i;
    for (i = 0; i < 4; i++)
    {
        if (eta >= absEta[i] && eta <= absEta[i + 1])
            break;
    }
    s[0] = Form("abseta:[%.1f,%.1f]", absEta[i], absEta[i + 1]);
}
void SF_add_tree::sf_electronreco(Float_t pt, Float_t eta, Float_t weight[3])
{
    Int_t nbin;
    if (pt > 500.0 || eta > 2.5)
    {
        weight[0] *= 1.0;
        weight[1] *= 1.0;
        weight[2] *= 1.0;
        return;
    }
    if (pt < 20.0)
    {
        nbin = hist_elec_reco_b20->FindBin(eta, pt);
        weight[0] *= hist_elec_reco_b20->GetBinContent(nbin);
        weight[1] *= hist_elec_reco_b20->GetBinContent(nbin) + hist_elec_reco_b20->GetBinErrorUp(nbin);
        weight[2] *= hist_elec_reco_b20->GetBinContent(nbin) - hist_elec_reco_b20->GetBinErrorLow(nbin);
    }
    else
    {
        nbin = hist_elec_reco_a20->FindBin(eta, pt);
        weight[0] *= hist_elec_reco_a20->GetBinContent(nbin);
        weight[1] *= hist_elec_reco_a20->GetBinContent(nbin) + hist_elec_reco_a20->GetBinErrorUp(nbin);
        weight[2] *= hist_elec_reco_a20->GetBinContent(nbin) - hist_elec_reco_a20->GetBinErrorLow(nbin);
    }
}
void SF_add_tree::sf_electronid(Float_t pt, Float_t eta, Float_t weight[3])
{
    Int_t nbin;
    if (pt > 500.0 || eta > 2.5)
    {
        weight[0] *= 1.0;
        weight[1] *= 1.0;
        weight[2] *= 1.0;
        return;
    }
    nbin = hist_elec_id->FindBin(eta, pt);
    weight[0] *= hist_elec_id->GetBinContent(nbin);
    weight[1] *= hist_elec_id->GetBinContent(nbin) + hist_elec_id->GetBinErrorUp(nbin);
    weight[2] *= hist_elec_id->GetBinContent(nbin) - hist_elec_id->GetBinErrorLow(nbin);
}
void SF_add_tree::sf_muoniso(Float_t pt, Float_t eta, Float_t weight[3])
{
    if (pt < 15.0 || pt > 120.0 || eta > 2.4)
    {
        weight[0] *= 1.0;
        weight[1] *= 1.0;
        weight[2] *= 1.0;
        return;
    }
    Int_t nbin = hist_muon_m_iso->FindBin(eta, pt);
    weight[0] *= hist_muon_m_iso->GetBinContent(nbin);
    weight[1] *= hist_muon_m_iso->GetBinContent(nbin) + hist_muon_m_iso->GetBinErrorUp(nbin);
    weight[2] *= hist_muon_m_iso->GetBinContent(nbin) - hist_muon_m_iso->GetBinErrorLow(nbin);
}
void SF_add_tree::sf_muonid(Float_t pt, Float_t eta, Float_t weight[3])
{
    if (pt < 2.0 || pt > 120.0 || eta > 2.4)
    {
        weight[0] *= 1.0;
        weight[1] *= 1.0;
        weight[2] *= 1.0;
        return;
    }
    if (pt < 15.0)
    {
        TString s[2];
        get_string1(pt, eta, s);
        float nom = root_muon_l_id["NUM_TightID_DEN_TrackerMuons"]["abseta_pt"][s[0]][s[1]]["value"].asDouble();
        weight[0] *= nom;
        weight[1] *= nom + root_muon_l_id["NUM_TightID_DEN_TrackerMuons"]["abseta_pt"][s[0]][s[1]]["error"].asDouble();
        weight[2] *= nom - root_muon_l_id["NUM_TightID_DEN_TrackerMuons"]["abseta_pt"][s[0]][s[1]]["error"].asDouble();
        return;
    }
    Int_t nbin = hist_muon_m_id->FindBin(eta, pt);
    weight[0] *= hist_muon_m_id->GetBinContent(nbin);
    weight[1] *= hist_muon_m_id->GetBinContent(nbin) + hist_muon_m_id->GetBinErrorUp(nbin);
    weight[2] *= hist_muon_m_id->GetBinContent(nbin) - hist_muon_m_id->GetBinErrorLow(nbin);
}
void SF_add_tree::sf_muonreco(Float_t pt, Float_t eta, Float_t weight[3])
{
    TString s[2];
    float nom;
    if (pt < 2.0 || pt > 200.0 || eta > 2.4)
    {
        weight[0] *= 1.0;
        weight[1] *= 1.0;
        weight[2] *= 1.0;
        return;
    }
    if (pt < 10.0)
    {
        get_string1(pt, eta, s);
        nom = root_muon_l_reco["NUM_TightID_DEN_TrackerMuons"][s[0]][s[1]]["value"].asDouble();
        weight[0] *= nom;
        weight[1] *= nom + root_muon_l_reco["NUM_TightID_DEN_TrackerMuons"][s[0]][s[1]]["error"].asDouble();
        weight[2] *= nom - root_muon_l_reco["NUM_TightID_DEN_TrackerMuons"][s[0]][s[1]]["error"].asDouble();
        return;
    }
    get_string2(eta, s);
    nom = root_muon_m_reco["NUM_TrackerMuons_DEN_genTracks"]["abseta_pt"][s[0]]["pt:[40,60]"]["value"].asDouble();
    weight[0] *= nom;
    weight[1] *= nom + root_muon_m_reco["NUM_TrackerMuons_DEN_genTracks"]["abseta_pt"][s[0]]["pt:[40,60]"]["error"].asDouble();
    weight[2] *= nom - root_muon_m_reco["NUM_TrackerMuons_DEN_genTracks"]["abseta_pt"][s[0]]["pt:[40,60]"]["error"].asDouble();
}
void SF_add_tree::sf_btag(BTagEntry_off::JetFlavor flav, Float_t pt, Float_t eta, Float_t sf[13])
{   
    if (flav == BTagEntry_off::FLAV_B || flav == BTagEntry_off::FLAV_C)
    {
        sf[0] = reader_bc->eval_auto_bounds("central", flav, eta, pt);
        /*sf[1] = reader->eval_auto_bounds("up_uncorrelated", flav, eta, pt);
        sf[2] = reader->eval_auto_bounds("down_uncorrelated", flav, eta, pt);
        sf[3] = reader->eval_auto_bounds("up_correlated", flav, eta, pt);
        sf[4] = reader->eval_auto_bounds("down_correlated", flav, eta, pt);*/
        sf[1] = reader_bc->eval_auto_bounds("up_jes", flav, eta, pt);
        sf[2] = reader_bc->eval_auto_bounds("down_jes", flav, eta, pt);
        sf[3] = reader_bc->eval_auto_bounds("up_pileup", flav, eta, pt);
        sf[4] = reader_bc->eval_auto_bounds("down_pileup", flav, eta, pt);
        sf[5] = reader_bc->eval_auto_bounds("up_type3", flav, eta, pt);
        sf[6] = reader_bc->eval_auto_bounds("down_type3", flav, eta, pt);
        sf[7] = reader_bc->eval_auto_bounds("up_statistic", flav, eta, pt);
        sf[8] = reader_bc->eval_auto_bounds("down_statistic", flav, eta, pt);
        for (int i = 9; i < 13; i++)
            sf[i] = sf[0];
    }
    else
    {
        sf[0] = reader_l->eval_auto_bounds("central", flav, eta, pt);
        for (int i = 1; i < 9; i++)
            sf[i] = sf[0];
        sf[9] = reader_l->eval_auto_bounds("up_uncorrelated", flav, eta, pt);
        sf[10] = reader_l->eval_auto_bounds("down_uncorrelated", flav, eta, pt);
        sf[11] = reader_l->eval_auto_bounds("up_correlated", flav, eta, pt);
        sf[12] = reader_l->eval_auto_bounds("down_correlated", flav, eta, pt);
    }
    //cout << flav << " " << sf[0] << endl;
}
Float_t SF_add_tree::btag_eff(Int_t flav, Float_t pt, Float_t eta)
{
    Int_t bin;
    if (flav == 5 || flav == -5)
    {
        bin = h2_djEff_b->FindBin(eta, pt);
        return h2_djEff_b->GetBinContent(bin);
    }
    else if (flav == 4 || flav == -4)
    {
        bin = h2_djEff_c->FindBin(eta, pt);
        return h2_djEff_c->GetBinContent(bin);
    }
    else
    {
        bin = h2_djEff_udsg->FindBin(eta, pt);
        return h2_djEff_udsg->GetBinContent(bin);
    }
}
void SF_add_tree::sf_lep(Float_t pt, Float_t eta, Bool_t flavour, Float_t weight[3])
{
    if (flavour == 0)
    { // electron
        sf_electronid(pt, eta, weight);
        sf_electronreco(pt, eta, weight);
    }
    else
    {
        sf_muonreco(pt, fabs(eta), weight);
        sf_muoniso(pt, fabs(eta), weight);
        sf_muonid(pt, fabs(eta), weight);
    }
}
void SF_add_tree::sf_jet(Float_t *pt, Float_t *eta, Int_t *flavour, Float_t *score, UInt_t jet_num, Float_t weight[13])
{
    Float_t btag_num[] = {0.2589, 0.2489, 0.3040, 0.2783};
    Float_t mc = 1;
    Float_t sf[13], data[13];
    // 0: central
    // 1: btag_up_un; 2: btag_dn_un; 3: btag_up_co; 4: btag_dn_co
    // 5: btag_up_un; 6: btag_dn_un; 7: btag_up_co; 8: btag_dn_co
    for (int i = 0; i < 13; i++)
        data[i] = 1;
    BTagEntry_off::JetFlavor flav;
    for (int i = 0; i < jet_num; i++)
    {
        if (flavour[i] == 5 || flavour[i] == -5)
            flav = BTagEntry_off::FLAV_B;
        else if (flavour[i] == 4 || flavour[i] == -4)
            flav = BTagEntry_off::FLAV_C;
        else
            flav = BTagEntry_off::FLAV_UDSG;
        sf_btag(flav, pt[i], fabs(eta[i]), sf);

        if (score[i] > btag_num[year - 2015])
        { // Medium
            mc *= btag_eff(flavour[i], pt[i], fabs(eta[i]));
            for (int j = 0; j < 13; j++)
                data[j] *= btag_eff(flavour[i], pt[i], fabs(eta[i])) * sf[j];
        }
        else
        {
            mc *= 1 - btag_eff(flavour[i], pt[i], fabs(eta[i]));
            for (int j = 0; j < 13; j++)
                data[j] *= 1 - btag_eff(flavour[i], pt[i], fabs(eta[i])) * sf[j];
        }
    }
    for (int i = 0; i < 13; i++)
    {
        weight[i] = data[i] / mc;
        if (weight[i] != weight[i] || mc == 0)
        {
            weight[i] = 1.0;
        }
    }
}
void SF_add_tree::sf_jet_it(Float_t *pt, Float_t *eta, Int_t *flavour, Float_t *score, UInt_t jet_num, Float_t *weight)
{
    Float_t mc = 1;
    Float_t sf, data = 1;
    BTagEntry_off::JetFlavor flav;
    for (int i = 0; i < jet_num; i++)
    {
        if (flavour[i] == 5 || flavour[i] == -5)
            flav = BTagEntry_off::FLAV_B;
        else if (flavour[i] == 4 || flavour[i] == -4)
            flav = BTagEntry_off::FLAV_C;
        else
            flav = BTagEntry_off::FLAV_UDSG;
        sf = reader_it->eval_auto_bounds("central", flav, fabs(eta[i]), pt[i], score[i]);
        mc *= btag_eff(flavour[i], pt[i], fabs(eta[i]));
        data *= btag_eff(flavour[i], pt[i], fabs(eta[i])) * sf;
        // cout<<sf<<" ";
    }
    // cout<<endl;
    *weight = data / mc;
    if (*weight != *weight || mc == 0)
    {
        *weight = 1.0;
    }
}
void SF_add_tree::sf_pu(Float_t *pt, Float_t *eta, UInt_t jet_num, Float_t weight[3])
{
    for (int i = 0; i < 3; i++)
        weight[i] = 1;
    BTagEntry_off::JetFlavor flav;
    for (int i = 0; i < jet_num; i++)
    {
        if (pt[i] < 50)
        {
            weight[0] *= h2_puid->GetBinContent(h2_puid->FindBin(pt[i], eta[i]));
            weight[1] *= h2_puid->GetBinContent(h2_puid->FindBin(pt[i], eta[i])) + h2_puid_un->GetBinContent(h2_puid->FindBin(pt[i], eta[i]));
            weight[2] *= h2_puid->GetBinContent(h2_puid->FindBin(pt[i], eta[i])) - h2_puid_un->GetBinContent(h2_puid->FindBin(pt[i], eta[i]));
        }
    }
}
void SF_add_tree::set_dir()
{
    TString bfile, bfile_it;
    TString indir, sf_dir, muon_m_iso, muon_m_id, elec_reco_b20, elec_reco_a20, elec_id, muon_l_id, muon_l_reco, muon_m_reco;
    // indir="/home/yksong/code/ttbar/scale_factor";
    indir = "..";
    fhist_puid = TFile::Open(indir + "/puID/PUID_106XTraining_ULRun2_EffSFandUncties_v1.root");
    // indir="/afs/cern.ch/user/r/repan/work/yuekai/scale_factor";
    if (year == 2018)
    {
        sf_dir = "UL2018";
        muon_m_iso = "Efficiencies_muon_generalTracks_Z_Run2018_UL_ISO.root";
        muon_m_id = "Efficiencies_muon_generalTracks_Z_Run2018_UL_ID.root";
        elec_reco_b20 = "egammaEffi_ptBelow20.txt_EGM2D_UL2018.root";
        elec_reco_a20 = "egammaEffi_ptAbove20.txt_EGM2D_UL2018.root";
        elec_id = "egammaEffi.txt_Ele_Tight_EGM2D.root";
        muon_l_id = "Efficiency_muon_trackerMuon_Run2018_UL_ID.json";
        muon_l_reco = "Efficiency_muon_generalTracks_Run2018_UL_trackerMuon.json";
        muon_m_reco = "NUM_TrackerMuons_DEN_genTracks_Z_abseta_pt.json";
        bfile = indir + "/" + sf_dir + "/btag/wp_deepJet_106XUL18_v2.csv";
        bfile_it = indir + "/" + sf_dir + "/btag/reshaping_deepJet_106XUL18_v2.csv";
        puid = "h2_eff_sfUL2018_T";
    }
    else if (year == 2017)
    {
        // indir="/afs/cern.ch/user/y/yuekai/ttbar/scale_factor/2017";
        sf_dir = "UL2017";
        muon_m_iso = "Efficiencies_muon_generalTracks_Z_Run2017_UL_ISO.root";
        muon_m_id = "Efficiencies_muon_generalTracks_Z_Run2017_UL_ID.root";
        elec_reco_b20 = "egammaEffi_ptBelow20.txt_EGM2D_UL2017.root";
        elec_reco_a20 = "egammaEffi_ptAbove20.txt_EGM2D_UL2017.root";
        elec_id = "egammaEffi.txt_EGM2D_Tight_UL17.root";
        muon_l_id = "Efficiency_muon_trackerMuon_Run2017_UL_ID.json";
        muon_l_reco = "Efficiency_muon_generalTracks_Run2017_UL_trackerMuon.json";
        muon_m_reco = "NUM_TrackerMuons_DEN_genTracks_Z_abseta_pt.json";
        bfile = indir + "/" + sf_dir + "/btag/wp_deepJet_106XUL17_v3.csv";
        bfile_it = indir + "/" + sf_dir + "/btag/reshaping_deepJet_106XUL17.csv";
        puid = "h2_eff_sfUL2017_T";
    }
    else if (year == 2016)
    { // 2016_post
        // indir="/afs/cern.ch/user/y/yuekai/ttbar/scale_factor/2016_post";
        sf_dir = "UL2016_post";
        muon_m_iso = "Efficiencies_muon_generalTracks_Z_Run2016_UL_ISO.root";
        muon_m_id = "Efficiencies_muon_generalTracks_Z_Run2016_UL_ID.root";
        elec_reco_b20 = "egammaEffi_ptBelow20.txt_EGM2D_UL2016postVFP.root";
        elec_reco_a20 = "egammaEffi_ptAbove20.txt_EGM2D_UL2016postVFP.root";
        elec_id = "egammaEffi.txt_Ele_Tight_postVFP_EGM2D.root";
        muon_l_id = "Efficiency_muon_trackerMuon_Run2016postVFP_UL_ID.json";
        muon_l_reco = "Efficiency_muon_generalTracks_Run2016postVFP_UL_trackerMuon.json";
        muon_m_reco = "NUM_TrackerMuons_DEN_genTracks_Z_abseta_pt.json";
        bfile = indir + "/" + sf_dir + "/btag/wp_deepJet_106XUL16postVFP_v3.csv";
        bfile_it = indir + "/" + sf_dir + "/btag/reshaping_deepJet_106XUL16postVFP_v3.csv";
        puid = "h2_eff_sfUL2016_T";
    }
    else if (year == 2015)
    { // 2016_pre
        // indir="/home/yksong/code/ttbar/scale_factor";
        sf_dir = "UL2016_pre";
        muon_m_iso = "Efficiencies_muon_generalTracks_Z_Run2016_UL_HIPM_ISO.root";
        muon_m_id = "Efficiencies_muon_generalTracks_Z_Run2016_UL_HIPM_ID.root";
        elec_reco_b20 = "egammaEffi_ptBelow20.txt_EGM2D_UL2016preVFP.root";
        elec_reco_a20 = "egammaEffi_ptAbove20.txt_EGM2D_UL2016preVFP.root";
        elec_id = "egammaEffi.txt_Ele_Tight_preVFP_EGM2D.root";
        muon_l_id = "Efficiency_muon_trackerMuon_Run2016preVFP_UL_ID.json";
        muon_l_reco = "Efficiency_muon_generalTracks_Run2016preVFP_UL_trackerMuon.json";
        muon_m_reco = "NUM_TrackerMuons_DEN_genTracks_Z_abseta_pt.json";
        bfile = indir + "/" + sf_dir + "/btag/wp_deepJet_106XUL16preVFP_v2.csv";
        bfile_it = indir + "/" + sf_dir + "/btag/reshaping_deepJet_106XUL16preVFP.csv";
        puid = "h2_eff_sfUL2016APV_T";
    }

    h2_puid = (TH2F *)fhist_puid->Get(puid);
    h2_puid_un = (TH2F *)fhist_puid->Get(puid + "_Systuncty");
    fhist_muon_m_iso = TFile::Open(indir + "/" + sf_dir + "/muon_medium_pt/" + muon_m_iso);
    fhist_muon_m_id = TFile::Open(indir + "/" + sf_dir + "/muon_medium_pt/" + muon_m_id);
    fhist_elec_reco_b20 = TFile::Open(indir + "/" + sf_dir + "/electron_reco/" + elec_reco_b20);
    fhist_elec_reco_a20 = TFile::Open(indir + "/" + sf_dir + "/electron_reco/" + elec_reco_a20);
    fhist_elec_id = TFile::Open(indir + "/" + sf_dir + "/electron_id/" + elec_id);
    if (inputFile.Contains("QCD"))
        fhist_beff = TFile::Open(indir + "/" + sf_dir + "/btag/btageff_QCD.root");
    else if (inputFile.Contains("W1") || inputFile.Contains("W2") || inputFile.Contains("W3") || inputFile.Contains("W4"))
        fhist_beff = TFile::Open(indir + "/" + sf_dir + "/btag/btageff_W.root");
    else if (inputFile.Contains("ST"))
        fhist_beff = TFile::Open(indir + "/" + sf_dir + "/btag/btageff_ST.root");
    else if (inputFile.Contains("DY"))
        fhist_beff = TFile::Open(indir + "/" + sf_dir + "/btag/btageff_DY.root");
    else
        fhist_beff = TFile::Open(indir + "/" + sf_dir + "/btag/btageff_TT.root");
    hist_muon_m_iso = (TH2F *)fhist_muon_m_iso->Get("NUM_TightRelIso_DEN_TightIDandIPCut_abseta_pt_efficiencyMC");
    hist_muon_m_id = (TH2F *)fhist_muon_m_id->Get("NUM_TightID_DEN_TrackerMuons_abseta_pt_efficiencyMC");
    hist_elec_reco_b20 = (TH2F *)fhist_elec_reco_b20->Get("EGamma_SF2D");
    hist_elec_reco_a20 = (TH2F *)fhist_elec_reco_a20->Get("EGamma_SF2D");
    hist_elec_id = (TH2F *)fhist_elec_id->Get("EGamma_SF2D");
    h2_djEff_b = (TH2F *)fhist_beff->Get("h2_djEff_b");
    h2_djEff_c = (TH2F *)fhist_beff->Get("h2_djEff_c");
    h2_djEff_udsg = (TH2F *)fhist_beff->Get("h2_djEff_udsg");

    if_muon_l_id.open(indir + "/" + sf_dir + "/muon_low_pt/" + muon_l_id);
    if_muon_l_reco.open(indir + "/" + sf_dir + "/muon_low_pt/" + muon_l_reco);
    if_muon_m_reco.open(indir + "/" + sf_dir + "/muon_medium_pt/" + muon_m_reco);

    BTagCalibration_off calib("csvv1", bfile.Data());
    BTagCalibration_off calib_it("csvv1", bfile_it.Data());
    reader_bc = new BTagCalibration_offReader(BTagEntry_off::OP_MEDIUM, "central", {"up_jes", "down_jes", "up_pileup", "down_pileup", "up_statistic", "down_statistic", "up_type3", "down_type3"});
    reader_l = new BTagCalibration_offReader(BTagEntry_off::OP_MEDIUM, "central", {"up_uncorrelated", "down_uncorrelated", "up_correlated", "down_correlated"});
    reader_bc->load(calib, BTagEntry_off::FLAV_B, "mujets");
    reader_bc->load(calib, BTagEntry_off::FLAV_C, "mujets");
    reader_l->load(calib, BTagEntry_off::FLAV_UDSG, "incl");
    reader_it = new BTagCalibration_offReader(BTagEntry_off::OP_RESHAPING, "central");
    reader_it->load(calib_it, BTagEntry_off::FLAV_B, "iterativefit");
    reader_it->load(calib_it, BTagEntry_off::FLAV_C, "iterativefit");
    reader_it->load(calib_it, BTagEntry_off::FLAV_UDSG, "iterativefit");
}
SF_add_tree::SF_add_tree(TString inputFile_s, TString tree_name, bool remain_sys, int year_s)
{
    /*TString indir="/home/yksong/code/ttbar/scale_factor/2018";
    TString sf_dir="UL2018";*/
    // TString inputFile="new_TTTo2L2Nu_TuneCP5_13TeV-powheg_10.root";
    year = year_s;
    inputFile = inputFile_s;
    set_dir();
    file = new TFile(inputFile, "update");
    TTree *mytree = (TTree *)file->Get(tree_name);
    Float_t sf_lepton, sf_btag, sf_btag_it, sf_puid;
    UInt_t jet_num;
    Float_t jet_pt[45], jet_eta[45], lep_pt, lep_eta;
    Bool_t lep_flavour;
    Int_t jet_flavour[45];
    Float_t jet_score[45];
    Float_t electron_deltaEtaSC;
    Float_t sf_lepton_up, sf_lepton_down, sf_puid_up, sf_puid_dn;
    Float_t sf_btag_jesup, sf_btag_pileupup, sf_btag_statisticup, sf_btag_type3up;
    Float_t sf_btag_jesdn, sf_btag_pileupdn, sf_btag_statisticdn, sf_btag_type3dn;
    Float_t sf_ltag_up, sf_ltag_up_co, sf_ltag_down_co, sf_ltag_down;
    TBranch *newbranch[20];
    newbranch[0] = mytree->Branch("SF_lepton", &sf_lepton, "SF_lepton/F");
    newbranch[1] = mytree->Branch("SF_btag", &sf_btag, "SF_btag/F");
    newbranch[2] = mytree->Branch("SF_btag_it", &sf_btag_it, "SF_btag_it/F");
    newbranch[3] = mytree->Branch("SF_puid", &sf_puid, "SF_puid/F");
    if (remain_sys)
    {
        newbranch[4] = mytree->Branch("SF_lepton_up", &sf_lepton_up, "sf_lepton_up/F");
        newbranch[5] = mytree->Branch("SF_lepton_down", &sf_lepton_down, "sf_lepton_down/F");
        newbranch[6] = mytree->Branch("SF_btag_jesup", &sf_btag_jesup, "sf_btag_jesup/F");
        newbranch[7] = mytree->Branch("SF_btag_jesdn", &sf_btag_jesdn, "sf_btag_jesdn/F");
        newbranch[8] = mytree->Branch("SF_btag_pileupup", &sf_btag_pileupup, "sf_btag_pileupup/F");
        newbranch[9] = mytree->Branch("SF_btag_pileupdn", &sf_btag_pileupdn, "sf_btag_pileupdn/F");
        newbranch[10] = mytree->Branch("SF_btag_type3up", &sf_btag_type3up, "sf_btag_type3up/F");
        newbranch[11] = mytree->Branch("SF_btag_type3dn", &sf_btag_type3dn, "sf_btag_type3dn/F");
        newbranch[12] = mytree->Branch("SF_btag_statisticup", &sf_btag_statisticup, "sf_btag_statisticup/F");
        newbranch[13] = mytree->Branch("SF_btag_statisticdn", &sf_btag_statisticdn, "sf_btag_statisticdn/F");

        newbranch[14] = mytree->Branch("SF_ltag_up", &sf_ltag_up, "sf_ltag_up/F");
        newbranch[15] = mytree->Branch("SF_ltag_down", &sf_ltag_down, "sf_ltag_down/F");
        newbranch[16] = mytree->Branch("SF_ltag_up_co", &sf_ltag_up_co, "sf_ltag_up_co/F");
        newbranch[17] = mytree->Branch("SF_ltag_down_co", &sf_ltag_down_co, "sf_ltag_down_co/F");
        newbranch[18] = mytree->Branch("SF_puid_up", &sf_puid_up, "SF_puid_up/F");
        newbranch[19] = mytree->Branch("SF_puid_dn", &sf_puid_dn, "SF_puid_dn/F");
    }
    mytree->SetBranchAddress("jet_num", &jet_num);
    mytree->SetBranchAddress("jet_pt", jet_pt);
    mytree->SetBranchAddress("jet_eta", jet_eta);
    mytree->SetBranchAddress("jet_hadronFlavour", jet_flavour);
    mytree->SetBranchAddress("jet_btagDeepFlavB", jet_score);
    mytree->SetBranchAddress("jet_num", &jet_num);
    mytree->SetBranchAddress("lep_flavour", &lep_flavour);
    mytree->SetBranchAddress("lepton_pt", &lep_pt);
    mytree->SetBranchAddress("lepton_eta", &lep_eta);
    mytree->SetBranchAddress("electron_deltaEtaSC", &electron_deltaEtaSC);

    if (!(if_muon_l_id.is_open() && if_muon_l_reco.is_open() && if_muon_m_reco.is_open()))
    {
        cout << "open failed!" << endl;
        return;
    }
    if (!(reader_muon_l_id.parse(if_muon_l_id, root_muon_l_id, false) && reader_muon_l_reco.parse(if_muon_l_reco, root_muon_l_reco, false) && reader_muon_m_reco.parse(if_muon_m_reco, root_muon_m_reco, false)))
    {
        cout << "parse failed!" << endl;
        return;
    }
    // csv
    // TString bfile=indir+"/"+sf_dir+"/btag/reshaping_deepJet_106XUL18_v2.csv";
    Float_t weight_jet[13], weight_lep[3], weight_jet_it, weight_puid[3];
    for (int entry = 0; entry < mytree->GetEntries(); entry++)
    {
        mytree->GetEntry(entry);
        weight_lep[0] = 1;
        weight_lep[1] = 1;
        weight_lep[2] = 1;
        sf_lep(lep_pt, lep_eta + electron_deltaEtaSC, lep_flavour, weight_lep);
        sf_jet(jet_pt, jet_eta, jet_flavour, jet_score, jet_num, weight_jet);
        sf_jet_it(jet_pt, jet_eta, jet_flavour, jet_score, jet_num, &weight_jet_it);
        sf_pu(jet_pt, jet_eta, jet_num, weight_puid);
        sf_lepton = weight_lep[0];
        sf_btag = weight_jet[0];
        sf_btag_it = weight_jet_it;
        sf_puid = weight_puid[0];
        for (int nbra = 0; nbra < 4; nbra++)
        {
            newbranch[nbra]->Fill();
            // if()
        }
        if (remain_sys)
        {
            sf_lepton_up = weight_lep[1];
            sf_lepton_down = weight_lep[2];
            sf_btag_jesup = weight_jet[1];
            sf_btag_jesdn = weight_jet[2];
            sf_btag_pileupup = weight_jet[3];
            sf_btag_pileupdn = weight_jet[4];
            sf_btag_type3up = weight_jet[5];
            sf_btag_type3dn = weight_jet[6];
            sf_btag_statisticup = weight_jet[7];
            sf_btag_statisticdn = weight_jet[8];
            sf_ltag_up = weight_jet[9];
            sf_ltag_down = weight_jet[10];
            sf_ltag_up_co = weight_jet[11];
            sf_ltag_down_co = weight_jet[12];
            sf_puid_up = weight_puid[1];
            sf_puid_dn = weight_puid[2];
            // cout<<sf_btag_down<<endl;
            for (int nbra = 4; nbra < 20; nbra++)
                newbranch[nbra]->Fill();
        }
    }
    file->cd();
    mytree->Write("", TObject::kOverwrite);
    delete mytree;
}
SF_add_tree::~SF_add_tree()
{
    delete reader_bc;
    delete reader_l;
    delete reader_it;
    file->Close();
    delete hist_elec_id;
    delete hist_elec_reco_a20;
    delete hist_elec_reco_b20;
    delete hist_muon_m_id;
    delete hist_muon_m_iso;
    delete h2_djEff_b;
    delete h2_djEff_c;
    delete h2_djEff_udsg;
    delete h2_puid;
    delete h2_puid_un;
    fhist_puid->Close();
    fhist_elec_id->Close();
    fhist_muon_m_id->Close();
    fhist_muon_m_iso->Close();
    fhist_elec_reco_a20->Close();
    fhist_elec_reco_b20->Close();
    fhist_beff->Close();
    if_muon_l_id.close();
    if_muon_l_reco.close();
    if_muon_m_reco.close();
}