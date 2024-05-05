#include "prepare_3D.h"

void prepare_3D::give_sys_name(TString file, TString weight, int s, int c)
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
    else if (s < 21 || s == 29)
    { // SF_btag*6, SF_lepton, L1_PreFiring, Pileup, and NNLO_QCD
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
    { // muR, muF, ISR, FSR
        tree_up = "mytree";
        tree_dn = "mytree";
        file_up = file;
        file_dn = file;
        weight_up = weight + "*" + sys_up[s];
        weight_dn = weight + "*" + sys_down[s];
    }
    else if (s < 28)
    { // mtop, mtop3, hdamp
        tree_up = "mytree";
        tree_dn = "mytree";
        file_up = file;
        file_up.ReplaceAll("TuneCP5", sys_up[s] + "_TuneCP5");
        file_dn = file;
        file_dn.ReplaceAll("TuneCP5", sys_down[s] + "_TuneCP5");
        weight_up = weight;
        weight_dn = weight;
    }
    else if (s < 30)
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
    else if (s < 31)
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
    // cout << tree_up << " " <<tree_dn << endl;
}

void prepare_3D::renew_weight(TString *weight, TString file)
{ // global weight
    double lumi_s[4] = {19.5, 16.8, 41.48, 59.83};
    double lumi = lumi_s[year - 2015];
    auto c0 = new TCanvas("c0", "c0", 8, 30, 600, 600);
    TChain *tree0 = new TChain("rawtree");
    if (is_ttx)
        tree0->Add(dir + "/MC_ttx/" + file);
    else
        tree0->Add(dir + "/MC/" + file);
    TH1D *nmc = new TH1D("nmc", "", 50, 0, 100);
    // nmc->Sumw2();
    c0->cd();
    tree0->Draw("nJet>>nmc", "Generator_weight");
    // cout<<nmc->GetSumOfWeights()<<endl;
    double cross_section = xsection[file].first;
    double K_Factor = xsection[file].second;
    double weight1 = cross_section * lumi / (nmc->GetSumOfWeights()) * K_Factor * 1000;
    // cout<<weight1<<endl;
    *weight = Form("%lf*", weight1) + (*weight);
    delete tree0;
    delete nmc;
    delete c0;
}
void prepare_3D::draw(TH3D *h1, TString file, TString tree, TString weight)
{
    auto c0 = new TCanvas("c0", "c0", 8, 30, 600, 600);
    TChain chain(tree);
    if(is_ttx)
        chain.Add(dir + "/MC_ttx/" + file);
    else
        chain.Add(dir + "/MC/" + file);
    c0->cd();
    TString h1Name = h1->GetName();
    // h1->Sumw2();
    if (is_corr)
        chain.Draw("likelihood:fabs(rapidity_tt):mass_tt>>" + h1Name, weight + "*" + cut);
    else
        chain.Draw("likelihood:fabs(rapidity_tt):mass_tt_uncorr>>" + h1Name, weight + "*" + cut);
    c0->cd();
    delete c0;
}
void prepare_3D::draw(int c)
{
    TString weight = "Generator_weight*SF_btag*SF_lepton*pu_wt*L1PreFiringWeight_Nom";
    TString weight_nom;
    TH3D *hist = new TH3D(process[c] + "_sub", "", xbins, xlow, xup, ybins, ylow, yup, zbins, zlow, zup);
    hist->Sumw2();
    if (c < 5)
        weight = weight + EW[c] + "*nnlo_wt";
    if (c == 5)
        weight = weight + "*nnlo_wt";
    for (int i = edge_i[c]; i < edge_f[c]; i++)
    {
        weight_nom = weight;
        renew_weight(&weight_nom, fileNames[i]);
        TH3D *h1 = new TH3D("h1", "", xbins, xlow, xup, ybins, ylow, yup, zbins, zlow, zup);
        h1->Sumw2();
        draw(h1, fileNames[i], "mytree", weight_nom);
        hist->Add(h1);
        delete h1;
    }
    file->cd();
    hist->Write();
    delete hist;
}
void prepare_3D::draw_pdf(int c, int p)
{
    TString weight = Form("Generator_weight*SF_btag*SF_lepton*pu_wt*L1PreFiringWeight_Nom*LHEPdfWeight[%d]", p);
    TString weight_nom;
    TH3D *hist_pdf = new TH3D(process[c] + Form("_pdf_w%d_sub", p), "", xbins, xlow, xup, ybins, ylow, yup, zbins, zlow, zup);
    if (c > 5)
        hist_pdf->SetName(process[c] + Form("_pdf%d_w%d_sub", c - 5, p));
    hist_pdf->Sumw2();
    if (c < 5)
        weight = weight + EW[c] + "*nnlo_wt";
    if (c == 5)
        weight = weight + "*nnlo_wt";
    for (int i = edge_i[c]; i < edge_f[c]; i++)
    {
        weight_nom = weight;
        renew_weight(&weight_nom, fileNames[i]);
        TH3D *h1 = new TH3D("h1", "", xbins, xlow, xup, ybins, ylow, yup, zbins, zlow, zup);
        h1->Sumw2();
        draw(h1, fileNames[i], "mytree", weight_nom);
        hist_pdf->Add(h1);
        delete h1;
    }
    file->cd();
    hist_pdf->Write();
    delete hist_pdf;
}
void prepare_3D::draw_sys(int c, int s)
{
    TString weight = "Generator_weight*SF_btag*SF_lepton*pu_wt*L1PreFiringWeight_Nom";
    TH3D *hist_up = new TH3D("hist_up", "", xbins, xlow, xup, ybins, ylow, yup, zbins, zlow, zup);
    TH3D *hist_dn = new TH3D("hist_dn", "", xbins, xlow, xup, ybins, ylow, yup, zbins, zlow, zup);
    if (c > 5 && (sys_n[s].Contains("muR") || sys_n[s].Contains("muF")))
    {
        hist_up->SetName(process[c] + "_" + sys_n[s] + Form("%dUp_sub", c - 5));
        hist_dn->SetName(process[c] + "_" + sys_n[s] + Form("%dDown_sub", c - 5));
    }
    else if (sys_n[s].Contains("SF_lepton") && cut_name.Contains("E_"))
    {
        hist_up->SetName(process[c] + "_SF_ElecUp_sub");
        hist_dn->SetName(process[c] + "_SF_ElecDown_sub");
    }
    else if (sys_n[s].Contains("SF_lepton") && cut_name.Contains("M_"))
    {
        hist_up->SetName(process[c] + "_SF_MuonUp_sub");
        hist_dn->SetName(process[c] + "_SF_MuonDown_sub");
    }
    else
    {
        hist_up->SetName(process[c] + "_" + sys_n[s] + "Up_sub");
        hist_dn->SetName(process[c] + "_" + sys_n[s] + "Down_sub");
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
        TH3D *h1_up = new TH3D("h1_up", "", xbins, xlow, xup, ybins, ylow, yup, zbins, zlow, zup);
        TH3D *h1_dn = new TH3D("h1_dn", "", xbins, xlow, xup, ybins, ylow, yup, zbins, zlow, zup);
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

void prepare_3D::set_dir(int option)
{
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
                                      365.34, 88.29, 377.96, 365.29, 88.28, 377.90,
                                      365.34, 88.29, 377.96, 365.34, 88.29, 377.96,};
    Float_t K_Factor[nsample] = {1.0, 1.0, 1.0,
                                 1.23, 1.23, 1.23, 1.23, 1.23, 1.23, 1.23, 1.23,
                                 1.0, 1.0, 1.0, 1.0, 1.0,
                                 1.21, 1.21, 1.21, 1.21,
                                 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                                 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                                 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                                 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
    TString sys_s[] = {"jer", "unclus", "L1PreFiringWeight_Nom", "SF_btag", "SF_btag", "SF_btag", "SF_btag", "SF_btag", "SF_btag", "SF_lepton", "pu_wt", "muR", "muF", "ISR", "FSR", "mtop", "mtop", "hdamp", "TuneCP5", "nnlo_wt", "EW_un"};
    TString sys_ns[] = {"jer", "unclus", Form("L1PF_%d", year), "SF_btag_jes", "SF_btag_type3", "SF_btag_pileup", Form("SF_btag_stat%d", year), "SF_ltag", Form("SF_ltag%d", year), Form("SF_lepton%d", year), "PU", "muR", "muF", "ISR", "FSR", "mtop", "mtop3", "hdamp", "TuneCP5", "nnlo_wt", "EW_un"};
    TString sys_up_s[] = {"jerUp", "unclusUp", "L1PreFiringWeight_Up", "SF_btag_jesup", "SF_btag_type3up", "SF_btag_pileupup", "SF_btag_statisticup", "SF_ltag_up_co", "SF_ltag_up", "SF_lepton_up", "pu_wt_up", "muR_up", "muF_up", "ISR_up", "FSR_up", "mtop173p5", "mtop175p5", "hdampUP", "TuneCP5up", "nnlo_wt_up", "EW_un_up"};
    TString sys_down_s[] = {"jerDown", "unclusDown", "L1PreFiringWeight_Dn", "SF_btag_jesdn", "SF_btag_type3dn", "SF_btag_pileupdn", "SF_btag_statisticdn", "SF_ltag_down_co", "SF_ltag_down", "SF_lepton_down", "pu_wt_dn", "muR_down", "muF_down", "ISR_down", "FSR_down", "mtop171p5", "mtop169p5", "hdampDOWN", "TuneCP5down", "nnlo_wt_down", "EW_un_down"};

    TString jes_source[] = {"Absolute", Form("Absolute_%d", year), "FlavorQCD", "BBEC1", "EC2", "HF", Form("BBEC1_%d", year), Form("EC2_%d", year), "RelativeBal", Form("RelativeSample_%d", year)};
    TString sf_bl[] = {"SF_btag_co", "SF_ltag_co", "SF_btag_un", "SF_btag_un"};

    for (int i = 0; i < nsample; i++)
    {
        fileName[i].ReplaceAll(".root", "_*.root");
        if (i < 20)
            fileNames[i] = fileName[i];
        xsection[fileName[i]] = pair<double, double>(cross_section[i], K_Factor[i]);
    }

    TString process_s[] = {"ttbar_ci0000", "ttbar_ci0100", "ttbar_ci0010", "ttbar_ci0001", "ttbar_ci0200", "EW_no", "DYJets", "STop", "WJets"};
    TString EWs[5] = {"*weight_ci0200", "*weight_ci0100", "*weight_ci0010", "*weight_ci0001", "*weight_ci0000"};
    Int_t npdf_ws[] = {103, 103, 103, 103, 103, 0, 103, 101, 103};
    int edge_is[] = {0, 0, 0, 0, 0, 0, 3, 11, 16}; // 23,31};
    int edge_fs[] = {3, 3, 3, 3, 3, 3, 11, 16, 20};
    for (int i = 0; i < 9; i++)
    {
        edge_i[i] = edge_is[i];
        edge_f[i] = edge_fs[i];
        npdf_w[i] = npdf_ws[i];
    }
    for (int i = 0; i < 10; i++)
    {
        sys[i] = "jes";
        sys_n[i] = "jes_" + jes_source[i];
        sys_up[i] = "jes_" + jes_source[i] + "Up";
        sys_down[i] = "jes_" + jes_source[i] + "Down";
    }
    for (int i = 0; i < 21; i++)
    {
        sys[i + 10] = sys_s[i];
        sys_n[i + 10] = sys_ns[i];
        sys_up[i + 10] = sys_up_s[i];
        sys_down[i + 10] = sys_down_s[i];
    }
    for (int i = 0; i < 9; i++)
    {
        process[i] = process_s[i];
    }
    for (int i = 0; i < 5; i++)
    {
        EW[i] = EWs[i];
    }

    int begins[] = {0, 1, 2, 3, 4, 5, 4};
    int ends[] = {1, 2, 3, 4, 5, 9, 9};
    TString categorys[] = {"ttbar200", "ttbar100", "ttbar010", "ttbar001", "ttbar000", "bg", "ttbar"};
    begin = begins[option];
    end = ends[option];
    category = categorys[option] + "_" + cut_name;
}

void prepare_3D::draw_data()
{
    // TChain chain_data("mytree");
    // chain_data.Add(dir+"/data/new_data*.root"); //fake data now
    TString data_name = "data_obs_sub";
    // auto c0 = new TCanvas("c0", "c0", 8, 30, 600, 600); // temporary canvas
    TH3D *h1 = new TH3D(data_name, "", xbins, xlow, xup, ybins, ylow, yup, zbins, zlow, zup);
    for (int i = 0; i < h1->GetNbinsX(); i++)
    {
        for (int j = 0; j < h1->GetNbinsY(); j++)
        {
            for (int k = 0; k < h1->GetNbinsZ(); k++)
                h1->SetBinContent(i + 1, j + 1, k + 1, 1.0);
        }
    }
    // c0->cd();
    // chain_data.Draw("mass_tt>>data_name", cut);
    file->cd();
    h1->Write();
    delete h1;
    // delete c0;
}

prepare_3D::prepare_3D(TString cut_s, TString cut_name_s, int year_s, vector<int> xyz_bins, vector<double> xyz_range, int option, bool is_ttxs, bool is_corrs)
{
    is_ttx = is_ttxs;
    is_corr = is_corrs;
    year = year_s;
    cut = cut_s;
    cut_name = cut_name_s;
    set_dir(option);
    xbins = xyz_bins[0];
    ybins = xyz_bins[1];
    zbins = xyz_bins[2];
    xlow = xyz_range[0];
    xup = xyz_range[1];
    ylow = xyz_range[2];
    yup = xyz_range[3];
    zlow = xyz_range[4];
    zup = xyz_range[5];
}
void prepare_3D::prepare()
{
    file = new TFile(outputDir + "/" + category + ".root", "recreate");
    for (int c = begin; c < end; c++)
    {
        draw(c);
        cout << "finished nom of " << process[c] << endl;
    }

    // add_qcd();
    for (int s = 0; s < 31; s++)
    {
        for (int c = begin; c < end; c++)
        {
            if (c > 4 && s > 24) // sys only for signal
                break;
            if (c == 5)
                continue; // no sys for EW_no
            draw_sys(c, s);
            cout << "finished sys of " << sys_n[s] << " of " << process[c] << endl;
        }
    }
    for (int c = begin; c < end; c++)
    {
        for (int p = 0; p < npdf_w[c]; p++)
        {
            draw_pdf(c, p);
            cout << "finished pdf_w" << p << " of " << process[c] << endl;
        }
    }
    draw_data();
    cout << "finished all the things" << endl;
}
prepare_3D::~prepare_3D()
{
    file->Close();
}