#include "prepare.h"

void prepare::Floor(TH1D *hist)
{
    for (int i = 0; i < hist->GetNbinsX(); i++)
    {
        if (!(hist->GetBinContent(i + 1) > 1.E-6))
            hist->SetBinContent(i + 1, 1.E-6);
    }
}
/*void sum_h1(TH1D* hs, TH1D* h1, TH1D *h2){
    for(int i=0; i<h1->GetNbinsX(); i++){
        hs->SetBinContent(i+1, h1->GetBinContent(i+1));
    }
    for(int i=0; i<h2->GetNbinsX(); i++){
        hs->SetBinContent(h1->GetNbinsX()+i+1, h2->GetBinContent(i+1));
    }
}*/
void prepare::give_sys_name(TString file, TString weight, int s, int c)
{
    if (s < 12)
    { // JES*10, JER, MET_uncluster
        tree_up = sys_up[s];
        tree_dn = sys_down[s];
        file_up = file;
        file_dn = file;
        weight_up = weight;
        weight_dn = weight;
    }
    else if (s < 19 || s == 28)
    { // SF_btag*4, SF_lepton, L1_PreFiring, Pileup, and NNLO_QCD
        tree_up = "mytree";
        tree_dn = "mytree";
        file_up = file;
        file_dn = file;
        weight_up = weight;
        weight_up.ReplaceAll(sys[s], sys_up[s]);
        weight_dn = weight;
        weight_dn.ReplaceAll(sys[s], sys_down[s]);
    }
    else if (s < 25)
    { // muR, muF, ISR, FSR, pdf, alphas
        tree_up = "mytree";
        tree_dn = "mytree";
        file_up = file;
        file_dn = file;
        weight_up = weight + "*" + sys_up[s];
        weight_dn = weight + "*" + sys_down[s];
    }
    else if (s < 27)
    { // mtop, hdamp
        tree_up = "mytree";
        tree_dn = "mytree";
        file_up = file;
        file_up.ReplaceAll("TuneCP5", sys_up[s] + "_TuneCP5");
        file_dn = file;
        file_dn.ReplaceAll("TuneCP5", sys_down[s] + "_TuneCP5");
        weight_up = weight;
        weight_dn = weight;
    }
    else if (s < 29)
    { // TuneCP5
        tree_up = "mytree";
        tree_dn = "mytree";
        file_up = file;
        file_up.ReplaceAll("TuneCP5", sys_up[s]);
        file_dn = file;
        file_dn.ReplaceAll("TuneCP5", sys_down[s]);
        weight_up = weight;
        weight_dn = weight;
    }
    else if (s < 30)
    { // EW_un
        tree_up = "mytree";
        tree_dn = "mytree";
        file_up = file;
        file_dn = file;
        weight_up = weight;
        weight_up = weight_up + EW[c] + "_EWUp";
        weight_dn = weight;
        weight_dn = weight_dn + EW[c] + "_EWDown";
    }
    // cout<<weight_up<<" "<<weight_dn<<" "<<weight<<" "<<endl;
}

void prepare::renew_weight(TString *weight, TString file)
{ // global weight
    double lumi_s[4] = {19.5, 16.8, 41.48, 59.83};
    double lumi = lumi_s[year - 2015];
    auto c0 = new TCanvas("c0", "c0", 8, 30, 600, 600);
    TChain *tree1 = new TChain("rawtree");
    tree1->Add(dir + "/MC/" + file);
    TH1D *nmc = new TH1D("nmc", "", 50, 0, 100);
    // nmc->Sumw2();
    c0->cd();
    tree1->Draw("nJet>>nmc", "Generator_weight");
    // cout<<nmc->GetSumOfWeights()<<endl;
    double cross_section = xsection[file].first;
    double K_Factor = xsection[file].second;
    double weight1 = cross_section * lumi / (nmc->GetSumOfWeights()) * K_Factor * 1000;
    // cout<<weight1<<endl;
    *weight = Form("%f*", weight1) + (*weight);
    delete tree1;
    delete nmc;
    delete c0;
}
void prepare::draw(TH1D *h1, TString file, TString tree, TString weight)
{
    auto c0 = new TCanvas("c0", "c0", 8, 30, 600, 600);
    TChain chain(tree);
    chain.Add(dir + "/MC/" + file);
    TH1D *hm1;
    if (equal_bin)
        hm1 = new TH1D("hm1", "", nbins, xlow, xup);
    else
        hm1 = new TH1D("hm1", "", nbins, xbins);

    hm1->Sumw2();
    c0->cd();
    chain.Draw(xvar + ">>hm1", weight + "*" + cut);
    for (int i = 0; i < nbins + 1; i++)
    {
        h1->SetBinContent(i, hm1->GetBinContent(i));
        h1->SetBinError(i, hm1->GetBinError(i));
    }
    c0->cd();
    Floor(h1);
    delete hm1;
    delete c0;
}
void prepare::draw(int c)
{
    TString other_con1 = "*((jet_num>=4)||(jet_num==3 && jet_pt[0]>50))";
    TString other_con2;
    if (year == 2018)
        other_con2 = "*(lep_flavour||((!lep_flavour) && lepton_pt>34))";
    else
        other_con2 = "*1";
    TString weight = "Generator_weight*SF_btag*SF_lepton*pu_wt*L1PreFiringWeight_Nom" + other_con1 + other_con2;
    TH1D *h1, *hist;
    if (equal_bin)
        hist = new TH1D(process[c], "", nbins, xlow, xup);
    else
        hist = new TH1D(process[c], "", nbins, 0, nbins);
    if (c < 5)
        weight = weight + EW[c] + "*nnlo_wt";
    if (c == 5)
        weight = weight + "*nnlo_wt";
    for (int i = edge_i[c]; i < edge_f[c]; i++)
    {
        weight_nom = weight;
        renew_weight(&weight_nom, fileNames[i]);
        if (equal_bin)
            h1 = new TH1D("h1", "", nbins, xlow, xup);
        else
            h1 = new TH1D("h1", "", nbins, 0, nbins);
        h1->Sumw2();
        draw(h1, fileNames[i], "mytree", weight_nom);
        hist->Add(h1);
        delete h1;
    }
    file->cd();
    hist->Write();
    delete hist;
}

void prepare::draw(int c, int s)
{
    TString other_con1 = "*((jet_num>=4)||(jet_num==3 && jet_pt[0]>50))";
    TString other_con2;
    if (year == 2018)
        other_con2 = "*(lep_flavour||((!lep_flavour) && lepton_pt>34))";
    else
        other_con2 = "*1";
    TString weight = "Generator_weight*SF_btag*SF_lepton*pu_wt*L1PreFiringWeight_Nom" + other_con1 + other_con2;
    TH1D *hist_up, *hist_dn, *h1_up, *h1_dn;
    if (equal_bin)
    {
        hist_up = new TH1D("hist_up", "", nbins, xlow, xup);
        hist_dn = new TH1D("hist_dn", "", nbins, xlow, xup);
    }
    else
    {
        hist_up = new TH1D("hist_up", "", nbins, 0, nbins);
        hist_dn = new TH1D("hist_dn", "", nbins, 0, nbins);
    }
    if (c > 5 && (sys_n[s].Contains("muR") || sys_n[s].Contains("muF")))
    {
        hist_up->SetName(process[c] + "_" + sys_n[s] + Form("%dUp", c - 4));
        hist_dn->SetName(process[c] + "_" + sys_n[s] + Form("%dDown", c - 4));
    }
    else if (sys_n[s].Contains("SF_lepton") && cut_name.Contains("E_"))
    {
        hist_up->SetName(process[c] + "_SF_ElecUp");
        hist_dn->SetName(process[c] + "_SF_ElecDown");
    }
    else if (sys_n[s].Contains("SF_lepton") && cut_name.Contains("M_"))
    {
        hist_up->SetName(process[c] + "_SF_MuonUp");
        hist_dn->SetName(process[c] + "_SF_MuonDown");
    }
    else
    {
        hist_up->SetName(process[c] + "_" + sys_n[s] + "Up");
        hist_dn->SetName(process[c] + "_" + sys_n[s] + "Down");
    }
    if (c < 5)
        weight = weight + EW[c] + "*nnlo_wt";
    if (c == 5)
        weight = weight + "*nnlo_wt";
    for (int i = edge_i[c]; i < edge_f[c]; i++)
    {
        give_sys_name(fileNames[i], weight, s, c);
        renew_weight(&weight_up, file_up);
        renew_weight(&weight_dn, file_dn);
        if (equal_bin)
        {
            h1_up = new TH1D("h1_up", "", nbins, xlow, xup);
            h1_dn = new TH1D("h1_dn", "", nbins, xlow, xup);
        }
        else
        {
            h1_up = new TH1D("h1_up", "", nbins, 0, nbins);
            h1_dn = new TH1D("h1_dn", "", nbins, 0, nbins);
        }
        h1_up->Sumw2();
        h1_dn->Sumw2();
        draw(h1_up, file_up, tree_up, weight_up);
        draw(h1_dn, file_dn, tree_dn, weight_dn);

        hist_up->Add(h1_up);
        hist_dn->Add(h1_dn);

        delete h1_up;
        delete h1_dn;
    }
    file->cd();
    hist_up->Write();
    hist_dn->Write();
    delete hist_up;
    delete hist_dn;
}

void prepare::set_dir(){
    const int nsample = 44;
    TString fileName[nsample] = {  
                            "new_TTToSemiLeptonic_TuneCP5_13TeV-powheg.root",
                            "new_TTTo2L2Nu_TuneCP5_13TeV-powheg.root",
                            "new_TTToHadronic_TuneCP5_13TeV-powheg.root",

                            "new_DYJetsToLL_M-50_HT-70to100_TuneCP5_PSweights_13TeV-madgraphMLM.root",
                            "new_DYJetsToLL_M-50_HT-100to200_TuneCP5_PSweights_13TeV-madgraphMLM.root",
                            "new_DYJetsToLL_M-50_HT-200to400_TuneCP5_PSweights_13TeV-madgraphMLM.root",
                            "new_DYJetsToLL_M-50_HT-400to600_TuneCP5_PSweights_13TeV-madgraphMLM.root",
                            "new_DYJetsToLL_M-50_HT-600to800_TuneCP5_PSweights_13TeV-madgraphMLM.root",
                            "new_DYJetsToLL_M-50_HT-800to1200_TuneCP5_PSweights_13TeV-madgraphMLM.root",
                            "new_DYJetsToLL_M-50_HT-1200to2500_TuneCP5_PSweights_13TeV-madgraphMLM.root",                           
                            "new_DYJetsToLL_M-50_HT-2500toInf_TuneCP5_PSweights_13TeV-madgraphMLM.root",
                        
                            "new_ST_s-channel_4f_leptonDecays_TuneCP5_13TeV-amcatnlo.root",
                            "new_ST_t-channel_antitop_4f_InclusiveDecays_TuneCP5_13TeV-powheg-madspin.root",
                            "new_ST_t-channel_top_4f_InclusiveDecays_TuneCP5_13TeV-powheg-madspin.root",
                            "new_ST_tW_antitop_5f_inclusiveDecays_TuneCP5_13TeV-powheg.root",
                            "new_ST_tW_top_5f_inclusiveDecays_TuneCP5_13TeV-powheg.root",                                                                               
                            
                            "new_W1JetsToLNu_TuneCP5_13TeV-madgraphMLM.root",
                            "new_W2JetsToLNu_TuneCP5_13TeV-madgraphMLM.root",
                            "new_W3JetsToLNu_TuneCP5_13TeV-madgraphMLM.root",
                            "new_W4JetsToLNu_TuneCP5_13TeV-madgraphMLM.root",

                            "new_TTToSemiLeptonic_mtop173p5_TuneCP5_13TeV-powheg.root",
                            "new_TTTo2L2Nu_mtop173p5_TuneCP5_13TeV-powheg.root",
                            "new_TTToHadronic_mtop173p5_TuneCP5_13TeV-powheg.root",
                            "new_TTToSemiLeptonic_mtop171p5_TuneCP5_13TeV-powheg.root",
                            "new_TTTo2L2Nu_mtop171p5_TuneCP5_13TeV-powheg.root",
                            "new_TTToHadronic_mtop171p5_TuneCP5_13TeV-powheg.root",

                            "new_TTToSemiLeptonic_mtop175p5_TuneCP5_13TeV-powheg.root",
                            "new_TTTo2L2Nu_mtop175p5_TuneCP5_13TeV-powheg.root",
                            "new_TTToHadronic_mtop175p5_TuneCP5_13TeV-powheg.root",
                            "new_TTToSemiLeptonic_mtop169p5_TuneCP5_13TeV-powheg.root",
                            "new_TTTo2L2Nu_mtop169p5_TuneCP5_13TeV-powheg.root",
                            "new_TTToHadronic_mtop169p5_TuneCP5_13TeV-powheg.root",

                            "new_TTToSemiLeptonic_hdampUP_TuneCP5_13TeV-powheg.root",
                            "new_TTTo2L2Nu_hdampUP_TuneCP5_13TeV-powheg.root",
                            "new_TTToHadronic_hdampUP_TuneCP5_13TeV-powheg.root",
                            "new_TTToSemiLeptonic_hdampDOWN_TuneCP5_13TeV-powheg.root",
                            "new_TTTo2L2Nu_hdampDOWN_TuneCP5_13TeV-powheg.root",
                            "new_TTToHadronic_hdampDOWN_TuneCP5_13TeV-powheg.root",

                            "new_TTToSemiLeptonic_TuneCP5up_13TeV-powheg.root",
                            "new_TTTo2L2Nu_TuneCP5up_13TeV-powheg.root",
                            "new_TTToHadronic_TuneCP5up_13TeV-powheg.root",
                            "new_TTToSemiLeptonic_TuneCP5down_13TeV-powheg.root",
                            "new_TTTo2L2Nu_TuneCP5down_13TeV-powheg.root",
                            "new_TTToHadronic_TuneCP5down_13TeV-powheg.root"};
    Float_t cross_section[nsample] = {365.34, 88.29, 377.96,
                                      169.9, 147.4, 41.0, 5.7, 1.4, 0.63, 0.15, 0.0036,
                                      3.36, 136.02, 80.95, 35.6, 35.6,
                                      8927.0, 2809.0, 826.3, 544.3,
                                      355.50, 85.91, 367.78, 375.45, 90.73, 388.41,
                                      336.79, 81.39, 348.42, 396.76, 95.88, 410.47,
                                      366.34, 88.29, 377.96, 365.29, 88.28, 377.90,
                                      365.34, 88.29, 377.96, 365.34, 88.29, 377.96,};
    Float_t K_Factor[nsample] = {1.0, 1.0, 1.0,
                                 1.23, 1.23, 1.23, 1.23, 1.23, 1.23, 1.23, 1.23,
                                 1.0, 1.0, 1.0, 1.0, 1.0,
                                 1.21, 1.21, 1.21, 1.21,
                                 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                                 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                                 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                                 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
    TString sys_s[] = {"jer", "unclus", "SF_btag", "SF_btag", "SF_btag", "SF_btag", "SF_lepton", "L1PreFiringWeight_Nom", "pu_wt", "muR", "muF", "ISR", "FSR", "pdf", "alphas", "mtop", "hdamp", "TuneCP5", "nnlo_wt", "EW_un"};
    TString sys_ns[] = {"jer", "unclus", "SF_btag", Form("SF_btag%d", year), "SF_ltag", Form("SF_ltag%d", year), "SF_lepton", "L1PF", "PU", "muR", "muF", "ISR", "FSR", "pdf", "alphas", "mtop", "hdamp", "TuneCP5", "nnlo_wt", "EW_un"};
    TString sys_up_s[] = {"jerUp", "unclusUp", "SF_btag_up_co", "SF_btag_up", "SF_ltag_up_co", "SF_ltag_up", "SF_lepton_up", "L1PreFiringWeight_Up", "pu_wt_up", "muR_up", "muF_up", "ISR_up", "FSR_up", "pdf_up", "alphas_up", "mtop173p5", "hdampUP", "TuneCP5up", "nnlo_wt_up", "EW_un_up"};
    TString sys_down_s[] = {"jerDown", "unclusDown", "SF_btag_down_co", "SF_btag_down", "SF_ltag_down_co", "SF_ltag_down", "SF_lepton_down", "L1PreFiringWeight_Dn", "pu_wt_dn", "muR_down", "muF_down", "ISR_down", "FSR_down", "pdf_dn", "alphas_dn", "mtop171p5", "hdampDOWN", "TuneCP5down", "nnlo_wt_down", "EW_un_down"};
    
    TString jes_source[] = {"Absolute", Form("Absolute_%d", year), "FlavorQCD", "BBEC1", "EC2", "HF", Form("BBEC1_%d", year), Form("EC2_%d", year), "RelativeBal", Form("RelativeSample_%d", year)};
    TString sf_bl[] = {"SF_btag_co", "SF_ltag_co", "SF_btag_un", "SF_btag_un"};
    for (int i = 0; i < nsample; i++)
    {
        if (i < 20)
            fileNames[i] = fileName[i].ReplaceAll(".root", "_*.root");
        xsection[fileName[i]] = pair(cross_section[i], K_Factor[i]);
    }

    TString process_s[] = {"ttbar_ci0000","ttbar_ci0100", "ttbar_ci0010", "ttbar_ci0001", "ttbar_ci0200", "EW_no", "DYJets","STop", "WJets"};
    TString EWs[5] = {"*weight_ci0000", "*weight_ci0100", "*weight_ci0010", "*weight_ci0001", "*weight_ci0200"};

    int edge_is[] = {0, 0, 0, 0, 0, 0, 3, 11, 16};//23,31}; 
    int edge_fs[] = {3, 3, 3, 3, 3, 0, 11, 16, 20,};
    for (int i = 0; i < 9; i++)
    {
        edge_i[i] = edge_is[i];
        edge_f[i] = edge_fs[i];
    }
    for (int i = 0; i < 10; i++)
    {
        sys[i] = "jes";
        sys_n[i] = "jes_" + jes_source[i];
        sys_up[i] = "jes_" + jes_source[i] + "Up";
        sys_down[i] = "jes_" + jes_source[i] + "Down";
    }
    for (int i = 0; i < 20; i++)
    {
        sys[i + 10] = sys_s[i];
        sys_n[i + 10] = sys_ns[i];
        sys_up[i + 10] = sys_up_s[i];
        sys_down[i + 10] = sys_down_s[i];
    }
    for (int i = 0; i < 9; i++)
        process[i] = process_s[i];
    for (int i = 0; i < 5; i++)
        EW[i] = EWs[i];
}
void prepare::set_bins(TString xvar_s, TString title_s, int nbin, double *xbin)
{
    xvar = xvar_s;
    title = title_s;
    nbins = nbin;
    equal_bin = false;
    for (int i = 0; i < nbin + 1; i++)
        xbins[i] = xbin[i];
}
void prepare::set_bins(TString xvar_s, TString title_s, int nbin, double xlow_s, double xup_s)
{
    xvar = xvar_s;
    title = title_s;
    nbins = nbin;
    equal_bin = true;
    xlow = xlow_s;
    xup = xup_s;
}
void prepare::draw_data()
{
    TChain chain_data("mytree");
    chain_data.Add(dir + "/data/new_data*.root"); // fake data now
    TString data_name = "data_obs";
    auto c0 = new TCanvas("c0", "c0", 8, 30, 600, 600); // temporary canvas
    TH1D *h1;
    if (equal_bin)
        h1 = new TH1D(data_name, "", nbins, xlow, xup);
    else
        h1 = new TH1D(data_name, "", nbins, 0, nbins);
    c0->cd();
    chain_data.Draw(xvar + ">>" + data_name, cut);
    file->cd();
    h1->Write();
    delete h1;
    delete c0;
}
prepare::prepare(TString dir_s, TString outputDir_s)
{
    dir = dir_s;
    outputDir = outputDir_s;
}
void prepare::run(TString cut_s, TString cut_name_s, int year_s, int s_num)
{
    signal_num = s_num;
    year = year_s;
    set_dir();
    cut = cut_s;
    cut_name = cut_name_s;
    TString category = title + "_" + cut_name;
    file = new TFile(outputDir + "/" + category + ".root", "recreate");
    for (int c = 0; c < 8; c++)
    {
        if (c < 5 && c >= signal_num)
            continue;
        draw(c);
        cout << "finished nom of " << process[c] << endl;
    }
    // add_qcd();
    for (int s = 0; s < 30; s++)
    {
        for (int c = 0; c < 8; c++)
        {
            if (c < 5 && c >= signal_num)
                continue;
            if (c > 4 && s > 24) // sys only for signal
                break;
            if (c == 5)
                break;                          // no sys for EW_no
            if (c == 7 && (s == 23 || s == 24)) // no pdf or alphas for STop
                break;
            draw(c, s);
            cout << "finished sys of " << sys_n[s] << " of " << process[c] << endl;
        }
    }
    draw_data();
}
prepare::~prepare()
{
    file->Close();
}