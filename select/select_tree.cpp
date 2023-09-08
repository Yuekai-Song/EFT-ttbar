#include "select_tree.h"
#include "reco.cpp"

read_object::read_object(TString input, int type){
    TChain* chain = new TChain("Events");
    UInt_t nMuon, nJet, nElectron, nGenJet;
    chain->Add(input);
    cout<<input<<" is reading and processing"<<endl;
    cout<<"total number of events: "<<chain->GetEntries()<<endl;
    chain->SetBranchAddress("nMuon", &nMuon);
    chain->SetBranchAddress("nJet", &nJet);
    chain->SetBranchAddress("nElectron", &nElectron);
    if(type != 0)
        chain->SetBranchAddress("nGenJet", &nGenJet);
    nm = 0, ne = 0, nj = 0, ng = 0;
    for(int num=0; num<chain->GetEntries(); num++){
        chain->GetEntry(num);
        if(nm < nMuon)
            nm = nMuon;
        if(ne < nElectron)
            ne = nElectron;
        if(nj < nJet)
            nj=nJet;
        if(type == 1){
            if(ng < nGenJet)
                ng = nGenJet;
        }
    }
    delete chain;
}
Bool_t select_tree::is_lep_from_jet(TLorentzVector mom_lep){
    Bool_t flag=false;
    for(int i=0; i<jet_num; i++){

        if(mom_lep.DeltaR(mom_jets[i])<0.4){
            flag=true;
            break;
        }
    }
    return flag;
}
select_tree::select_tree(TString inputFile, TString outputFile, TString name_tree, TString name_jet, TString name_MET,  int sample_year, int s_type, int num_j, int num_e, int num_m, int num_g){ //type: 0:data; 1:MC nom; 2:MC sys 3:sys nom
    input = outputFile;
    year = sample_year;
    tree_name = name_tree;
    type = s_type;
    Float_t btag_num[]={0.2589,0.2489,0.3040,0.2783};
    btag_criteria = btag_num[year-2015];
    TString recreate;
    if(type == 2)
        recreate = "update";
    else
        recreate = "RECREATE";
    nj = num_j;
    ne = num_e;
    nm = num_m;
    ng = num_g;
    chain = new TChain("Events");
    chain->Add(inputFile);
    output = new TFile(outputFile, recreate);
    chain->SetBranchAddress("nMuon", &nMuon);
    chain->SetBranchAddress("nJet", &nJet);
    chain->SetBranchAddress("nElectron", &nElectron);

    if(type == 1){
        GenJet_pt = new Float_t[ng];
        GenJet_mass = new Float_t[ng];
        GenJet_phi = new Float_t[ng];
        GenJet_eta = new Float_t[ng];
        chain->SetBranchAddress("nGenJet", &nGenJet);
        chain->SetBranchAddress("GenJet_eta", GenJet_eta);
        chain->SetBranchAddress("GenJet_pt", GenJet_pt);
        chain->SetBranchAddress("GenJet_phi", GenJet_phi);
        chain->SetBranchAddress("GenJet_mass", GenJet_mass);
        chain->SetBranchAddress("nLHEScaleWeight",&nLHEScaleWeight);
        chain->SetBranchAddress("nLHEPdfWeight",&nLHEPdfWeight);
        chain->SetBranchAddress("nPSWeight",&nPSWeight);
        chain->SetBranchAddress("LHEScaleWeight",LHEScaleWeight);
        chain->SetBranchAddress("PSWeight",PSWeight);
        chain->SetBranchAddress("LHEPdfWeight",LHEPdfWeight);
    }
    if(type != 0){
        jet_partonFlavour = new Int_t[nj];
        Jet_partonFlavour = new Int_t[nj];
        jet_hadronFlavour = new Int_t[nj];
        Jet_hadronFlavour = new Int_t[nj];
        chain->SetBranchAddress("Jet_partonFlavour", Jet_partonFlavour);
        chain->SetBranchAddress("Jet_hadronFlavour", Jet_hadronFlavour);
        chain->SetBranchAddress("Generator_weight",&Generator_weight);

        chain->SetBranchAddress("L1PreFiringWeight_Nom", &L1PreFiringWeight_Nom);
        chain->SetBranchAddress("L1PreFiringWeight_Up", &L1PreFiringWeight_Up);
        chain->SetBranchAddress("L1PreFiringWeight_Dn", &L1PreFiringWeight_Dn);

        chain->SetBranchAddress("Pileup_nPU", &Pileup_nPU);
    }
    Electron_eta = new Float_t[ne];
    Electron_mass = new Float_t[ne];
    Electron_pt = new Float_t[ne];
    Electron_phi = new Float_t[ne];
    Electron_charge = new Int_t[ne];
    Electron_cutBased = new Int_t[ne];
    Electron_deltaEtaSC = new Float_t[ne];
    Electron_dxy = new Float_t[ne];
    Electron_dz = new Float_t[ne];
    
    Muon_mass = new Float_t[nm];
    Muon_phi = new Float_t[nm];
    Muon_pt = new Float_t[nm];
    Muon_eta = new Float_t[nm];
    Muon_charge = new Int_t[nm];
    Muon_pfRelIso04_all = new Float_t[nm];
    Muon_tightId = new Bool_t[nm];
    Muon_looseId = new Bool_t[nm];

    Jet_btagDeepB = new Float_t[nj];
    Jet_btagDeepFlavB = new Float_t[nj];
    Jet_mass = new Float_t[nj];
    Jet_pt = new Float_t[nj];
    Jet_eta = new Float_t[nj];
    Jet_phi = new Float_t[nj];
    Jet_jetId = new Int_t[nj];

    jet_btagDeepB = new Float_t[nj];
    jet_btagDeepFlavB = new Float_t[nj];
    jet_mass = new Float_t[nj];
    jet_pt = new Float_t[nj];
    jet_eta = new Float_t[nj];
    jet_phi = new Float_t[nj];
    mom_jets = new TLorentzVector[nj];

    if(input.Contains("TT")){
        chain->SetBranchAddress("LHEPart_eta", LHEPart_eta);
        chain->SetBranchAddress("LHEPart_mass", LHEPart_mass);
        chain->SetBranchAddress("LHEPart_phi", LHEPart_phi);
        chain->SetBranchAddress("LHEPart_pt", LHEPart_pt);
        chain->SetBranchAddress("LHEPart_pdgId", LHEPart_pdgId);
        chain->SetBranchAddress("nLHEPart", &nLHEPart);
    }
    chain->SetBranchAddress(name_MET, &MET_pt);
    chain->SetBranchAddress(name_MET.ReplaceAll("_pt","_phi"), &MET_phi);
    chain->SetBranchAddress("Electron_phi", Electron_phi);
    chain->SetBranchAddress("Electron_pt", Electron_pt);
    chain->SetBranchAddress("Electron_mass", Electron_mass);
    chain->SetBranchAddress("Electron_eta", Electron_eta);
    chain->SetBranchAddress("nElectron", &nElectron);
    chain->SetBranchAddress("Electron_charge", Electron_charge);
    chain->SetBranchAddress("nMuon", &nMuon);
    chain->SetBranchAddress("nJet", &nJet);
    chain->SetBranchAddress("Muon_eta", Muon_eta);
    chain->SetBranchAddress("Muon_pt", Muon_pt);
    chain->SetBranchAddress("Muon_phi", Muon_phi);
    chain->SetBranchAddress("Muon_mass", Muon_mass);
    chain->SetBranchAddress("Muon_charge", Muon_charge);
    chain->SetBranchAddress("Jet_btagDeepB", Jet_btagDeepB);
    chain->SetBranchAddress("Jet_btagDeepFlavB", Jet_btagDeepFlavB);
    chain->SetBranchAddress("Jet_eta", Jet_eta);
    chain->SetBranchAddress(name_jet, Jet_pt);
    chain->SetBranchAddress(name_jet.ReplaceAll("_pt","_mass"), Jet_mass);
    chain->SetBranchAddress("Jet_phi", Jet_phi);

    chain->SetBranchAddress("Muon_pfRelIso04_all",Muon_pfRelIso04_all);
    chain->SetBranchAddress("Muon_tightId",Muon_tightId);
    chain->SetBranchAddress("Muon_looseId",Muon_looseId);
    chain->SetBranchAddress("Electron_cutBased",Electron_cutBased);
    chain->SetBranchAddress("Jet_jetId",Jet_jetId);
    chain->SetBranchAddress("Electron_deltaEtaSC",Electron_deltaEtaSC);
    chain->SetBranchAddress("Electron_dz",Electron_dz);
    chain->SetBranchAddress("Electron_dxy",Electron_dxy);
    chain->SetBranchAddress("PV_npvsGood", &PV_npvsGood);
    chain->SetBranchAddress("PV_npvs", &PV_npvs);
    
    chain->SetBranchAddress("Flag_goodVertices", &Flag_met[0]);
    chain->SetBranchAddress("Flag_globalSuperTightHalo2016Filter", &Flag_met[1]);
    chain->SetBranchAddress("Flag_HBHENoiseFilter", &Flag_met[2]);
    chain->SetBranchAddress("Flag_HBHENoiseIsoFilter", &Flag_met[3]);
    chain->SetBranchAddress("Flag_EcalDeadCellTriggerPrimitiveFilter", &Flag_met[4]);
    chain->SetBranchAddress("Flag_BadPFMuonFilter", &Flag_met[5]);
    chain->SetBranchAddress("Flag_BadPFMuonDzFilter", &Flag_met[6]);
    chain->SetBranchAddress("Flag_hfNoisyHitsFilter", &Flag_met[7]);
    chain->SetBranchAddress("Flag_BadChargedCandidateFilter", &Flag_met[8]);
    chain->SetBranchAddress("Flag_eeBadScFilter", &Flag_met[9]);

    chain->SetBranchAddress("luminosityBlock",&luminosityBlock);
    chain->SetBranchAddress("event",&event);
    chain->SetBranchAddress("run",&run);
    if(year==2018){
        chain->SetBranchAddress("HLT_Ele32_WPTight_Gsf",&HLT_Ele32_WPTight_Gsf);
        chain->SetBranchAddress("HLT_IsoMu24",&HLT_IsoMu24);
        chain->SetBranchAddress("Flag_ecalBadCalibFilter", &Flag_met[10]);
        nFlag_met = 11;
    }
    else if(year==2016||year==2015){//2015 means 2016_pre
        chain->SetBranchAddress("HLT_Ele27_WPTight_Gsf",&HLT_Ele27_WPTight_Gsf);
        chain->SetBranchAddress("HLT_IsoMu24",&HLT_IsoMu24);
        chain->SetBranchAddress("HLT_IsoTkMu24",&HLT_IsoTkMu24);
        nFlag_met = 10;
    }
    else{
        chain->SetBranchAddress("HLT_Ele35_WPTight_Gsf",&HLT_Ele35_WPTight_Gsf);
        chain->SetBranchAddress("HLT_IsoMu27",&HLT_IsoMu27);
        chain->SetBranchAddress("Flag_ecalBadCalibFilter", &Flag_met[10]);
        nFlag_met = 11;
    }
}

Bool_t select_tree::select_jet(){
    nBtag = 0;
    jet_num = 0; 
    max_score = 0;
    int jet_index[nj];
    Bool_t jet_flag = false; 
    for(int i = 0; i < nJet; i++) {   
        if(fabs(Jet_eta[i]) < 2.4 && Jet_pt[i] > 30 && Jet_jetId[i]==6){
            //mom_jets[i].SetPtEtaPhiM(Jet_pt[i], Jet_eta[i], Jet_phi[i],Jet_mass[i]);
            jet_index[jet_num] = i;
            jet_num = jet_num + 1;
            if (Jet_btagDeepFlavB[i] > btag_criteria)
                nBtag++;
            if(Jet_btagDeepFlavB[i]>max_score)
                max_score = Jet_btagDeepFlavB[i];
        }
    }      
    if(jet_num >= 3 && nBtag >= 2){
        jet_flag = true;
        for(int i=0;i<jet_num;i++){
            mom_jets[i].SetPtEtaPhiM(Jet_pt[jet_index[i]], Jet_eta[jet_index[i]], Jet_phi[jet_index[i]],Jet_mass[jet_index[i]]);  
            jet_eta[i]=Jet_eta[jet_index[i]];
            jet_pt[i]=Jet_pt[jet_index[i]];
            jet_phi[i]=Jet_phi[jet_index[i]];
            jet_mass[i]=Jet_mass[jet_index[i]];
            jet_btagDeepB[i]=Jet_btagDeepB[jet_index[i]];
            jet_btagDeepFlavB[i]=Jet_btagDeepFlavB[jet_index[i]];
            if(type != 0){
                jet_partonFlavour[i]=Jet_partonFlavour[jet_index[i]];
                jet_hadronFlavour[i]=Jet_hadronFlavour[jet_index[i]];
            }
        }
    }
    return jet_flag;
}
Bool_t select_tree::select_lep(){
    Int_t nlepton = nMuon+nElectron;
    Bool_t lepton_flag = false; // if true pass the selction
    int num_select=0;
    bool is_from_jet;
    TLorentzVector p4_lepton;
    for(int i = 0; i < nlepton; i++){
        if(i < nElectron){
            p4_lepton.SetPtEtaPhiM(Electron_pt[i],Electron_eta[i],Electron_phi[i],Electron_mass[i]);
            is_from_jet=is_lep_from_jet(p4_lepton);
            if(Electron_cutBased[i]>=2 && fabs(Electron_eta[i]) <2.4 && (fabs(Electron_eta[i])<1.4442
                ||fabs(Electron_eta[i])>1.5660)&&Electron_pt[i]>15){
                if((fabs(Electron_deltaEtaSC[i]+Electron_eta[i])<1.479&&fabs(Electron_dxy[i])<0.05&&fabs(Electron_dz[i])<0.1)
                    ||(fabs(Electron_deltaEtaSC[i]+Electron_eta[i])>=1.479&&fabs(Electron_dxy[i])<0.1&&fabs(Electron_dz[i])<0.2))
                {
                    num_select++;
                    if(Electron_cutBased[i]==4 && fabs(Electron_eta[i]) <2.4 && (fabs(Electron_eta[i])<1.4442
                        ||fabs(Electron_eta[i])>1.5660)&&Electron_pt[i]>30 && (!is_from_jet))
                    {  
                        lep_c = Electron_charge[i];
                        mom_lep = p4_lepton;
                        lep_flavour=false;
                        lepton_flag=true;
                        electron_deltaEtaSC = Electron_deltaEtaSC[i];
                    }
                }      
            }
        }       
        else{
            p4_lepton.SetPtEtaPhiM(Muon_pt[i-nElectron],Muon_eta[i-nElectron],Muon_phi[i-nElectron],Muon_mass[i-nElectron]);
            is_from_jet=is_lep_from_jet(p4_lepton);
            if(Muon_looseId[i-nElectron]==1 && Muon_pfRelIso04_all[i-nElectron]<=0.25 && Muon_pt[i-nElectron]>15 && fabs(Muon_eta[i-nElectron])<2.4)
            {    
                num_select++;
                if(Muon_tightId[i-nElectron]==1 && Muon_pfRelIso04_all[i-nElectron]<=0.15 && Muon_pt[i-nElectron]>30 && fabs(Muon_eta[i-nElectron])<2.4 && (!is_from_jet))
                {
                    lep_c = Muon_charge[i-nElectron];
                    mom_lep = p4_lepton;
                    lep_flavour=true;
                    lepton_flag=true;
                    electron_deltaEtaSC = 0;
                }
            }
        }
        if(num_select > 1) {
            lepton_flag=false;
            break;
        }
    }
    return lepton_flag;
}

void select_tree::read_LHE(){
    if(!input.Contains("TT"))
        return;
    LHE_nhad = 0;
    LHE_nlep = 0;
    for (int i = nLHEPart-6; i < nLHEPart; i++) {
        if(LHEPart_pdgId[i] == 2 || LHEPart_pdgId[i] == 4 || LHEPart_pdgId[i] == -2 || LHEPart_pdgId[i] == -4)
            LHE_nhad++;
        if(LHEPart_pdgId[i] == 1 || LHEPart_pdgId[i] == 3 || LHEPart_pdgId[i] == -1 || LHEPart_pdgId[i] == -3)
            LHE_nhad++;
        if(LHEPart_pdgId[i] == 11 || LHEPart_pdgId[i] == 13 || LHEPart_pdgId[i] == -11 || LHEPart_pdgId[i] == -13)
            LHE_nlep++;
        if(LHEPart_pdgId[i] == 15 || LHEPart_pdgId[i] == -15)
            LHE_nlep++;
    }
    if (LHE_nhad==2 && LHE_nlep==1) {
        // get information of ttbar process at parton level from LHEPart
        for (int i = 0; i < nLHEPart; i++){
            if(LHEPart_pdgId[i] == 5)
                index_b = i;
            else if(LHEPart_pdgId[i] == -5)
                index_antib = i;
            else if(LHEPart_pdgId[i] == 2||LHEPart_pdgId[i] == 4||LHEPart_pdgId[i] == -2||LHEPart_pdgId[i] == -4)
                index_up = i;
            else if (LHEPart_pdgId[i] == 1||LHEPart_pdgId[i] == 3||LHEPart_pdgId[i] == -1||LHEPart_pdgId[i] == -3)
                index_down = i;
            else if (LHEPart_pdgId[i] == 11||LHEPart_pdgId[i] == 13||LHEPart_pdgId[i] == 15||LHEPart_pdgId[i]== -11||LHEPart_pdgId[i] == -13||LHEPart_pdgId[i] == -15)
                index_lep = i;
            else if (LHEPart_pdgId[i] == 12||LHEPart_pdgId[i] == 14||LHEPart_pdgId[i] == 16||LHEPart_pdgId[i] == -12||LHEPart_pdgId[i] == -14||LHEPart_pdgId[i] == -16)
                index_nu = i;
        }
        p4_b.SetPtEtaPhiM(LHEPart_pt[index_b], LHEPart_eta[index_b], LHEPart_phi[index_b], LHEPart_mass[index_b]);
        p4_antib.SetPtEtaPhiM(LHEPart_pt[index_antib], LHEPart_eta[index_antib],LHEPart_phi[index_antib], LHEPart_mass[index_antib]);
        p4_up.SetPtEtaPhiM(LHEPart_pt[index_up], LHEPart_eta[index_up], LHEPart_phi[index_up], LHEPart_mass[index_up]);
        p4_down.SetPtEtaPhiM(LHEPart_pt[index_down], LHEPart_eta[index_down], LHEPart_phi[index_down], LHEPart_mass[index_down]);
        p4_lep.SetPtEtaPhiM(LHEPart_pt[index_lep], LHEPart_eta[index_lep], LHEPart_phi[index_lep], LHEPart_mass[index_lep]);
        p4_nu.SetPtEtaPhiM(LHEPart_pt[index_nu], LHEPart_eta[index_nu], LHEPart_phi[index_nu], LHEPart_mass[index_nu]);
        if(LHEPart_pdgId[index_lep] > 0){
            p4_antitop = p4_antib + p4_lep + p4_nu;
            p4_top = p4_b + p4_up + p4_down;
            lep_charge = -1;
        } 
        else{
            p4_top = p4_b + p4_lep + p4_nu;
            p4_antitop = p4_antib + p4_up + p4_down;
            lep_charge = 1;
        }
    }
    if(LHE_nhad==0 && LHE_nlep==2){
        // get information of ttbar process at parton level from LHEPart
        for(int i = 0; i < nLHEPart; i++){
            if (LHEPart_pdgId[i] == 5) index_b = i;
            else if (LHEPart_pdgId[i] == -5) index_antib = i;
            else if (LHEPart_pdgId[i] == 11|| LHEPart_pdgId[i] == 13||LHEPart_pdgId[i] == 15)
                index_lepn = i;
            else if (LHEPart_pdgId[i] == 12 || LHEPart_pdgId[i] == 14 || LHEPart_pdgId[i] == 16)
                index_nun = i;
            else if (LHEPart_pdgId[i] == -11 || LHEPart_pdgId[i] == -13 || LHEPart_pdgId[i] == -15)
                index_lepp = i;
            else if (LHEPart_pdgId[i] == -12 ||LHEPart_pdgId[i] == -14 || LHEPart_pdgId[i] == -16)
                index_nup = i;
        } 
        p4_b.SetPtEtaPhiM(LHEPart_pt[index_b], LHEPart_eta[index_b],LHEPart_phi[index_b], LHEPart_mass[index_b]);
        p4_antib.SetPtEtaPhiM(LHEPart_pt[index_antib], LHEPart_eta[index_antib],LHEPart_phi[index_antib],LHEPart_mass[index_antib]);
        p4_lepp.SetPtEtaPhiM(LHEPart_pt[index_lepp], LHEPart_eta[index_lepp],LHEPart_phi[index_lepp], LHEPart_mass[index_lepp]);
        p4_lepn.SetPtEtaPhiM(LHEPart_pt[index_lepn], LHEPart_eta[index_lepn],LHEPart_phi[index_lepn], LHEPart_mass[index_lepn]);
        p4_nup.SetPtEtaPhiM(LHEPart_pt[index_nup], LHEPart_eta[index_nup],LHEPart_phi[index_nup], LHEPart_mass[index_nup]);
        p4_nun.SetPtEtaPhiM(LHEPart_pt[index_nun], LHEPart_eta[index_nun],LHEPart_phi[index_nun], LHEPart_mass[index_nun]);
        p4_top = p4_b + p4_lepp + p4_nun;
        p4_antitop = p4_antib + p4_lepn + p4_nup; 
    }
    if(LHE_nhad==4 && LHE_nlep==0){
        for(int i = 0; i < nLHEPart; i++) {
            if (LHEPart_pdgId[i] == 5) index_b = i;
            else if (LHEPart_pdgId[i] == -5) index_antib = i;
            else if (LHEPart_pdgId[i] == 2|| LHEPart_pdgId[i] == 4)
                index_up = i;
            else if (LHEPart_pdgId[i] == -2 || LHEPart_pdgId[i] == -4)
                index_upbar = i;
            else if (LHEPart_pdgId[i] == 1 || LHEPart_pdgId[i] == 3)
                index_down = i;
            else if (LHEPart_pdgId[i] == -1 ||LHEPart_pdgId[i] == -3)
                index_downbar = i;
        } 
        p4_b.SetPtEtaPhiM(LHEPart_pt[index_b], LHEPart_eta[index_b],LHEPart_phi[index_b], LHEPart_mass[index_b]);
        p4_antib.SetPtEtaPhiM(LHEPart_pt[index_antib], LHEPart_eta[index_antib],LHEPart_phi[index_antib],LHEPart_mass[index_antib]);
        p4_up.SetPtEtaPhiM(LHEPart_pt[index_up], LHEPart_eta[index_up],LHEPart_phi[index_up], LHEPart_mass[index_up]);
        p4_upbar.SetPtEtaPhiM(LHEPart_pt[index_upbar], LHEPart_eta[index_upbar],LHEPart_phi[index_upbar], LHEPart_mass[index_upbar]);
        p4_down.SetPtEtaPhiM(LHEPart_pt[index_down], LHEPart_eta[index_down],LHEPart_phi[index_down], LHEPart_mass[index_down]);
        p4_downbar.SetPtEtaPhiM(LHEPart_pt[index_downbar], LHEPart_eta[index_downbar],LHEPart_phi[index_downbar], LHEPart_mass[index_downbar]);
        p4_top = p4_b + p4_up + p4_downbar;
        p4_antitop = p4_antib + p4_upbar + p4_down; 
    }
    if(input.Contains("TT")){
        top_pt = p4_top.Pt();
        top_eta = p4_top.Eta();
        top_phi = p4_top.Phi();
        top_mass = p4_top.M();
        antitop_pt = p4_antitop.Pt();
        antitop_eta = p4_antitop.Eta();
        antitop_phi = p4_antitop.Phi();
        antitop_mass = p4_antitop.M();
        TLorentzVector p4_top_cms = p4_top;
        TLorentzVector p4_cms_lab = p4_antitop + p4_top;
        TVector3 p3_cms = p4_cms_lab.BoostVector();
        p4_top_cms.Boost(-p3_cms);
        TVector3 p3_top_cms = p4_top_cms.Vect();
        TVector3 p3_cms_lab = p4_cms_lab.Vect();
        ctstar = p3_top_cms.Dot(p3_cms_lab)/(p3_top_cms.Mag() * p3_cms_lab.Mag());
        M_tt_gen = (p4_top + p4_antitop).M();
        delta_rapidity_gen = p4_top.Rapidity() - p4_antitop.Rapidity();
    }
}
void select_tree::pdf_w(Float_t LHEPdfWeight[103], Float_t &alphas_up, Float_t &alphas_dn, Float_t &pdf_up, Float_t &pdf_dn){
    pdf_up = LHEPdfWeight[0];
    pdf_dn = LHEPdfWeight[0];
    alphas_dn = LHEPdfWeight[101];
    alphas_up = LHEPdfWeight[102];
    for(int i=1; i<101; i++){
        if(pdf_up < LHEPdfWeight[i])
            pdf_up = LHEPdfWeight[i];
        if(pdf_dn > LHEPdfWeight[i])
            pdf_dn = LHEPdfWeight[i];
    }
}
void select_tree::read_sys(){
    muR_up = 1.0*LHEScaleWeight[3]/LHEScaleWeight[4];
    muR_down = 1.0*LHEScaleWeight[5]/LHEScaleWeight[4];

    muF_up = 1.0*LHEScaleWeight[1]/LHEScaleWeight[4];
    muF_down = 1.0*LHEScaleWeight[7]/LHEScaleWeight[4];
    //cout<<muF_down<<endl;

    ISR_up = PSWeight[0];
    ISR_down = PSWeight[2];

    FSR_up = PSWeight[1];
    FSR_down = PSWeight[3];
}
void select_tree::loop(TTree* mytree, TTree* rawtree){
    Bool_t ele_trigger, mu_trigger;
    Bool_t jet_flag, lep_flag, trigger_flag;
    for(int entry=0; entry<chain->GetEntries(); entry++){
        nLHEPart = 0;
        chain->GetEntry(entry);
        if(type == 1 || type == 3)
            rawtree->Fill();
        index = entry;
        if(year==2018){
            ele_trigger=HLT_Ele32_WPTight_Gsf;
            mu_trigger=HLT_IsoMu24;
        }
        else if(year==2016||year==2015){
            ele_trigger=HLT_Ele27_WPTight_Gsf;
            mu_trigger=HLT_IsoMu24||HLT_IsoTkMu24;
        }
        else{
            ele_trigger=HLT_Ele35_WPTight_Gsf;
            mu_trigger=HLT_IsoMu27;
        }
        met_match = true;
        for(int i=0; i<nFlag_met; i++){
            met_match *= Flag_met[i];
        }
        if((mu_trigger||ele_trigger) && met_match){
            jet_flag = select_jet();
            if(!jet_flag)
                continue;
            lep_flag = select_lep();
            if(!lep_flag)
                continue;
            trigger_flag=false;
            if(((!lep_flavour) && ele_trigger) || (lep_flavour && mu_trigger))
                trigger_flag=true;
            if(!trigger_flag || PV_npvsGood<1)
                continue;
            RECO* reco = new RECO(jet_num, mom_jets, mom_lep, lep_c, jet_btagDeepFlavB, MET_pt, MET_phi);
            Double_t nu_px = RECO::nu_px;
            Double_t nu_py = RECO::nu_py;
            neutrino_pz = RECO::nu_pz;
            mass_thad = reco->mass_thad;
            mass_tlep = reco->mass_tlep;
            mass_whad = reco->mass_whad;
            mass_wlep = reco->mass_wlep;
            mass_t = reco->mass_t;
            mass_at = reco->mass_at;   
            mass_bjj = reco->mass_bjj;
            mass_jj = reco->mass_jj;
            mass_lb = reco->mass_lb;
            rectop_pt = reco->rectop_pt;
            recantitop_pt = reco->recantitop_pt;
            mass_tt = reco->mass_tt;
            rapidity_tt = reco->rapidity_tt;
            like = reco->like;
            chi = reco->chi;
            MtW=sqrt(2*(mom_lep.Pt()*MET_pt-mom_lep.Px()*nu_px-mom_lep.Py()*nu_py));         
            //if(MtW<140){ 
            if(MtW<140 && like<10000){
                //cout<<entry<<" "<<like<<" "<<neutrino_pz<<endl;
                lepton_pt =mom_lep.Pt();
                lepton_eta = mom_lep.Eta();
                lepton_mass = mom_lep.M();
                lepton_phi = mom_lep.Phi();
                if(type == 1){
                    read_sys();
                    pdf_w(LHEPdfWeight, alphas_up, alphas_dn, pdf_up, pdf_dn);
                }
                if(type != 0)
                    read_LHE();
                mytree->Fill();
            } // end of cut of minimum
            delete reco;
        }
    }
}

void select_tree::write(){
    TTree *rawtree;
    TTree *mytree = new TTree(tree_name, " tree with branches of "+tree_name);

    mytree->Branch("lepton_eta", &lepton_eta, "lepton_eta/F");
    mytree->Branch("lepton_pt", &lepton_pt, "lepton_pt/F");
    mytree->Branch("lep_flavour", &lep_flavour, "lep_flavour/O");
    mytree->Branch("jet_num", &jet_num,"jet_num/i"); // number of jets satisfy the  seletion criteria

    mytree->Branch("jet_btagDeepFlavB", jet_btagDeepFlavB, "jet_btagDeepFlavB[jet_num]/F");
    mytree->Branch("jet_eta", jet_eta, "jet_eta[jet_num]/F");
    mytree->Branch("jet_pt", jet_pt, "jet_pt[jet_num]/F");

    mytree->Branch("rapidity_tt", &rapidity_tt, "rapidity_tt/F");
    mytree->Branch("mass_tt", &mass_tt, "mass_tt/F");
    mytree->Branch("likelihood", &like, "likelihood/D" );
    mytree->Branch("MtW",&MtW,"MtW/F");

    mytree->Branch("mass_whad", &mass_whad, "mass_whad/F");
    mytree->Branch("mass_wlep", &mass_wlep, "mass_wlep/F");
    mytree->Branch("mass_t", &mass_t, "mass_t/F");
    mytree->Branch("rectop_pt", &rectop_pt, "rectop_pt/F");

    mytree->Branch("PV_npvsGood",&PV_npvsGood,"PV_npvsGood/I");
    if(type == 0){
        mytree->Branch("run",&run,"run/i");
        mytree->Branch("luminosityBlock",&luminosityBlock,"luminosityBlock/i");
        mytree->Branch("event",&event,"event/l");
    }
    if(type == 1|| type == 3){
        rawtree = new TTree("rawtree", "tree without selection");
        rawtree->Branch("nJet", &nJet, "nJet/i");
        rawtree->Branch("Generator_weight",&Generator_weight,"Generator_weight/F");
        rawtree->Branch("PV_npvsGood",&PV_npvsGood,"PV_npvsGood/I");
    }
    if(type == 1){
        if(input.Contains("TT")){
            mytree->Branch("top_pt", &top_pt, "top_pt/F");
        }
        mytree->Branch("muR_up", &muR_up, "muR_up/F");
        mytree->Branch("muR_down", &muR_down, "muR_down/F");
        mytree->Branch("muF_up", &muF_up, "muF_up/F");
        mytree->Branch("muF_down", &muF_down, "muF_down/F");
        mytree->Branch("ISR_up", &ISR_up, "ISR_up/F");
        mytree->Branch("ISF_down", &ISR_down, "ISR_down/F");
        mytree->Branch("FSR_up", &FSR_up, "FSR_up/F");
        mytree->Branch("FSR_down", &FSR_down, "FSR_down/F");
        mytree->Branch("pdf_up", &pdf_up, "pdf_up/F");
        mytree->Branch("pdf_dn", &pdf_dn, "pdf_dn/F");
        mytree->Branch("alphas_up", &alphas_up, "alphas_up/F");
        mytree->Branch("alphas_dn", &alphas_dn, "alphas_dn/F");
        //mytree->Branch("nLHEPdfWeight", &nLHEPdfWeight, "nLHEPdfWeight/i");
        //mytree->Branch("LHEPdfWeight", LHEPdfWeight, "LHEPdfWeight[nLHEPdfWeight]/F");
        mytree->Branch("L1PreFiringWeight_Up",&L1PreFiringWeight_Up,"L1PreFiringWeight_Up/F");
        mytree->Branch("L1PreFiringWeight_Dn",&L1PreFiringWeight_Dn,"L1PreFiringWeight_Dn/F");
    }
    if(type != 0){
        if(input.Contains("TT")){
            mytree->Branch("ctstar",&ctstar,"ctstar/F");
            mytree->Branch("M_tt_gen", &M_tt_gen, "M_tt_gen/F");
            mytree->Branch("delta_rapidity_gen", &delta_rapidity_gen,"delta_rapidity_gen/F");
        }
        mytree->Branch("Generator_weight",&Generator_weight,"Generator_weight/F");
        mytree->Branch("jet_hadronFlavour", jet_hadronFlavour, "jet_hadronFlavour[jet_num]/I");
        mytree->Branch("electron_deltaEtaSC",&electron_deltaEtaSC, "electron_deltaEtaSC/F");
        mytree->Branch("L1PreFiringWeight_Nom",&L1PreFiringWeight_Nom,"L1PreFiringWeight_Nom/F");
        mytree->Branch("Pileup_nPU",&Pileup_nPU,"Pileup_nPU/I");
    }

    loop(mytree, rawtree);
    output->cd();
    if(type == 1 || type == 3){
        rawtree->Write();
        cout<<"there are "<<rawtree->GetEntries()<<" events"<<endl;
        delete rawtree;
    }
    mytree->Write();
    cout<<mytree->GetEntries()<<" events selected"<<endl;
    cout<<"there are "<<mytree->GetEntries("jet_num>=4")<<" events of 4 jets"<<endl;
    delete mytree;
}
select_tree::~select_tree(){
    if(type == 1){
        delete[] GenJet_pt ;
        delete[] GenJet_mass;
        delete[] GenJet_phi;
        delete[] GenJet_eta;
    }
    if(type != 0){
        delete[] jet_partonFlavour;
        delete[] Jet_partonFlavour;
        delete[] jet_hadronFlavour;
        delete[] Jet_hadronFlavour;
    }
    delete[] Electron_eta;
    delete[] Electron_mass;
    delete[] Electron_pt;
    delete[] Electron_phi;
    delete[] Electron_charge;
    delete[] Electron_cutBased;
    delete[] Electron_deltaEtaSC;
    delete[] Electron_dxy;
    delete[] Electron_dz;
    
    delete[] Muon_mass;
    delete[] Muon_phi;
    delete[] Muon_pt;
    delete[] Muon_eta;
    delete[] Muon_charge;
    delete[] Muon_pfRelIso04_all;
    delete[] Muon_tightId;
    delete[] Muon_looseId;

    delete[] Jet_btagDeepB;
    delete[] Jet_btagDeepFlavB;
    delete[] Jet_mass;
    delete[] Jet_pt;
    delete[] Jet_phi;
    delete[] Jet_jetId;

    delete[] jet_btagDeepB;
    delete[] jet_btagDeepFlavB;
    delete[] jet_mass;
    delete[] jet_pt;
    delete[] jet_eta;
    delete[] jet_phi;
    delete[] mom_jets;
    output->Close();
    delete chain;
}

