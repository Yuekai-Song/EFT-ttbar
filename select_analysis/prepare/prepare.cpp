#include "prepare.h"

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

void prepare::renew_weight(TString *weight, TString dir, TString file)
{ // global weight

    auto c0 = new TCanvas("c0", "c0", 8, 30, 600, 600);
    TChain *tree0 = new TChain("rawtree");
    // cout << dir << " " << file << endl;
    tree0->Add(dir + file);
    TH1D *nmc = new TH1D("nmc", "", 50, 0, 100);
    // nmc->Sumw2();
    c0->cd();
    tree0->Draw("nJet>>nmc", "Generator_weight");
    // cout<<nmc->GetSumOfWeights()<<endl;
    double cross_section = sets.xs(file).first;
    double K_Factor = sets.xs(file).second;
    double weight1 = cross_section * sets.lumi / (nmc->GetSumOfWeights()) * K_Factor * 1000;
    // cout << weight1 << endl;
    *weight = Form("%lf*", weight1) + (*weight);
    delete tree0;
    delete nmc;
    delete c0;
}
void prepare::draw(TH1 *h1, TString file, TString tree, TString weight)
{
    auto c0 = new TCanvas("c0", "c0", 8, 30, 600, 600);
    TChain chain(tree);
    chain.Add(file);
    c0->cd();
    TString h1Name = h1->GetName();
    // h1->Sumw2();
    if (type == 0)
    {
        if (is_corr)
            chain.Draw("likelihood:fabs(rapidity_tt):mass_tt >>" + h1Name, weight + "*" + sets.cut);
        else
            chain.Draw("likelihood:fabs(rapidity_tt):mass_tt_uncorr >>" + h1Name, weight + "*" + sets.cut);
    }
    else
        chain.Draw(xvar + ">>" + h1Name, weight + "*" + sets.cut);
    delete c0;
}
void prepare::draw_nom(int c)
{
    TString weight = weight_base;
    TString weight_nom;
    TH1 *hist;
    create_hist(hist, process[c]);
    if (c < 5)
        weight = weight + EW[c] + "*nnlo_wt";
    if (c == 5)
        weight = weight + "*nnlo_wt";
    for (int i = sets.index_pro[fileTitle[c]].first; i <  sets.index_pro[fileTitle[c]].second; i++)
    {
        weight_nom = weight;
        renew_weight(&weight_nom, MC_dir, sets.fileName(i));
        TH1 *h1;
        create_hist(h1, "h1");
        draw(h1, MC_dir + sets.fileName(i), "mytree", weight_nom);
        hist->Add(h1);
        delete h1;
    }
    file->cd();
    hist->Write();
    delete hist;
}
void prepare::draw_pdf(int c, int p)
{
    TString weight = weight_base + Form("*LHEPdfWeight[%d]", p);
    TString weight_nom;
    TH1 *hist_pdf;
    create_hist(hist_pdf, process[c] + Form("_pdf_w%d", p));
    if (c > 5)
        hist_pdf->SetName(process[c] + Form("_pdf%d_w%d", c - 5, p));
    else if (c < 5)
        weight = weight + EW[c] + "*nnlo_wt";
    else
        weight = weight + "*nnlo_wt";
    for (int i = sets.index_pro[fileTitle[c]].first; i <  sets.index_pro[fileTitle[c]].second; i++)
    {
        weight_nom = weight;
        renew_weight(&weight_nom, MC_dir, sets.fileName(i));
        TH1 *h1; 
        create_hist(h1, "h1");
        draw(h1, MC_dir + sets.fileName(i), "mytree", weight_nom);
        hist_pdf->Add(h1);
        delete h1;
    }
    file->cd();
    hist_pdf->Write();
    delete hist_pdf;
}
void prepare::draw_sys(int c, int s)
{
    TString weight = weight_base;
    TH1 *hist_up, *hist_dn;
    create_hist(hist_up, "hist_up");
    create_hist(hist_dn, "hist_dn");
    if (c > 5 && (sys_n[s].Contains("muR") || sys_n[s].Contains("muF")))
    {
        hist_up->SetName(process[c] + "_" + sys_n[s] + Form("%dUp", c - 5));
        hist_dn->SetName(process[c] + "_" + sys_n[s] + Form("%dDown", c - 5));
    }
    else if (sys_n[s].Contains("SF_lepton") && sets.cut_name.Contains("E_"))
    {
        hist_up->SetName(process[c] + "_SF_ElecUp");
        hist_dn->SetName(process[c] + "_SF_ElecDown");
    }
    else if (sys_n[s].Contains("SF_lepton") && sets.cut_name.Contains("M_"))
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
    for (int i = sets.index_pro[fileTitle[c]].first; i < sets.index_pro[fileTitle[c]].second; i++)
    {
        give_sys_name(sets.fileName(i), weight, s, c);
        renew_weight(&weight_up, MC_dir, file_up);
        renew_weight(&weight_dn, MC_dir, file_dn);
        TH1 *h1_up, *h1_dn;
        create_hist(h1_up, "h1_up");
        create_hist(h1_dn, "h1_dn");
        draw(h1_up, MC_dir + file_up, tree_up, weight_up);
        draw(h1_dn, MC_dir + file_dn, tree_dn, weight_dn);
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
void prepare::draw_qcd(TString cg, bool isEnriched)
{
    sets.set_suf(cg);
    TString en = "", qcd_title = "QCD_HT";
    if (isEnriched)
    {
        en = "_En";
        if (sets.cut_name.Contains("M"))
            qcd_title = "QCD_MuEn";
        else
            qcd_title = "QCD_EMEn";
    }

    TH1 *hdata, *hmc[2];
    create_hist(hmc[1], "QCD_MC_" + cg + en);
    if (cg != "A" && !isEnriched)
    {
        create_hist(hmc[0], "other_MC_" + cg);
        create_hist(hdata, "QCD_prompt_" + cg); // final QCD;
        draw(hdata, QCD_dir + "data/" + sets.dataName(), "mytree", Form("%.2f", sets.pre_scale));
    }
    TString titles[] = {"other", qcd_title};
    for (int i = 0; i < 2; i++)
    {
        if (i == 0 && (cg == "A" || isEnriched))
            continue;
        for (int j = sets.index_pro[titles[i]].first; j < sets.index_pro[titles[i]].second; j++)
        {
            TString weight = weight_base;
            if (j < 3)
                weight = weight + "*nnlo_wt";
            renew_weight(&weight, QCD_dir + "MC/", sets.fileName(j));
            TH1 *h1;
            create_hist(h1, "h1");
            draw(h1, QCD_dir + "MC/" + sets.fileName(j), "mytree", weight);
            hmc[i]->Add(h1);
            delete h1;
        }
    }
    if (cg != "A" && !isEnriched)
    {
        hdata->Add(hmc[0], -1.0);
        qcd_hists[hdata->GetName()] = hdata;
        delete hmc[0];
    }
    
    qcd_hists[hmc[1]->GetName()] = hmc[1];
}
void prepare::add_qcd()
{
    TString cgs[] = {"A", "B", "C", "D"};
    bool en[] = {false, true};
    qcd_hists.clear();
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            draw_qcd(cgs[i], en[j]);
        }
    }
    file->cd();
    if (type == 0)
    {
        for (map<TString, TH1 *>::iterator it = qcd_hists.begin(); it != qcd_hists.end(); ++it)
        {
            it->second->Write();
        }
    }
    else
    {
        set0(qcd_hists["QCD_prompt_C"]);
        set0(qcd_hists["QCD_prompt_B"]);
        set0(qcd_hists["QCD_prompt_D"]);
        double norm = qcd_hists["QCD_MC_A"]->GetSumOfWeights() / qcd_hists["QCD_MC_C"]->GetSumOfWeights();
        double ns = qcd_hists["QCD_MC_B"]->GetSumOfWeights() / qcd_hists["QCD_MC_D"]->GetSumOfWeights();
        ns /= norm;
        qcd_hists["QCD_prompt_C"]->Scale(norm);
        qcd_hists["QCD_prompt_C"]->SetName("QCD");
        TH1 *hist_normup = (TH1 *)qcd_hists["QCD_prompt_C"]->Clone();
        TH1 *hist_normdn = (TH1 *)qcd_hists["QCD_prompt_C"]->Clone();
        hist_normup->SetName("QCD_normUp");
        hist_normdn->SetName("QCD_normDown");
        hist_normup->Scale(ns);
        hist_normdn->Scale(1.0 / ns);

        qcd_hists["QCD_prompt_B"]->Scale(1.0 / qcd_hists["QCD_prompt_B"]->GetSumOfWeights());
        qcd_hists["QCD_prompt_D"]->Scale(1.0 / qcd_hists["QCD_prompt_D"]->GetSumOfWeights());
        TH1 *temp = (TH1 *)qcd_hists["QCD_prompt_B"]->Clone();
        temp->SetName("temp");
        qcd_hists["QCD_prompt_B"]->Divide(qcd_hists["QCD_prompt_D"]);
        qcd_hists["QCD_prompt_D"]->Divide(temp);
        qcd_hists["QCD_prompt_B"]->Multiply(qcd_hists["QCD_prompt_C"]);
        qcd_hists["QCD_prompt_D"]->Multiply(qcd_hists["QCD_prompt_C"]);
        qcd_hists["QCD_prompt_B"]->SetName("QCD_shapeUp");
        qcd_hists["QCD_prompt_D"]->SetName("QCD_shapeDown");

        qcd_hists["QCD_prompt_C"]->Write();
        qcd_hists["QCD_prompt_B"]->Write();
        qcd_hists["QCD_prompt_D"]->Write();
        hist_normup->Write();
        hist_normdn->Write();

    }
    for (map<TString, TH1 *>::iterator it = qcd_hists.begin(); it != qcd_hists.end(); ++it)
    {
        delete it->second;
    }
    sets.set_suf("");
}
void prepare::set_dir(int option)
{
    TString sys_s[] = {"jer", "unclus", "L1PreFiringWeight_Nom", "SF_btag", "SF_btag", "SF_btag", "SF_btag", "SF_btag", "SF_btag", "SF_lepton", "pu_wt", "muR", "muF", "ISR", "FSR", "mtop", "mtop", "hdamp", "TuneCP5", "nnlo_wt", "EW_un"};
    TString sys_ns[] = {"jer", "unclus", Form("L1PF_%d", year), "SF_btag_jes", "SF_btag_type3", "SF_btag_pileup", Form("SF_btag_stat%d", year), "SF_ltag", Form("SF_ltag%d", year), Form("SF_lepton%d", year), "PU", "muR", "muF", "ISR", "FSR", "mtop", "mtop3", "hdamp", "TuneCP5", "nnlo_wt", "EW_un"};
    TString sys_up_s[] = {"jerUp", "unclusUp", "L1PreFiringWeight_Up", "SF_btag_jesup", "SF_btag_type3up", "SF_btag_pileupup", "SF_btag_statisticup", "SF_ltag_up_co", "SF_ltag_up", "SF_lepton_up", "pu_wt_up", "muR_up", "muF_up", "ISR_up", "FSR_up", "mtop173p5", "mtop175p5", "hdampUP", "TuneCP5up", "nnlo_wt_up", "EW_un_up"};
    TString sys_down_s[] = {"jerDown", "unclusDown", "L1PreFiringWeight_Dn", "SF_btag_jesdn", "SF_btag_type3dn", "SF_btag_pileupdn", "SF_btag_statisticdn", "SF_ltag_down_co", "SF_ltag_down", "SF_lepton_down", "pu_wt_dn", "muR_down", "muF_down", "ISR_down", "FSR_down", "mtop171p5", "mtop169p5", "hdampDOWN", "TuneCP5down", "nnlo_wt_down", "EW_un_down"};

    TString jes_source[] = {"Absolute", Form("Absolute_%d", year), "FlavorQCD", "BBEC1", "EC2", "HF", Form("BBEC1_%d", year), Form("EC2_%d", year), "RelativeBal", Form("RelativeSample_%d", year)};
    TString sf_bl[] = {"SF_btag_co", "SF_ltag_co", "SF_btag_un", "SF_btag_un"};


    TString process_s[] = {"ttbar_ci0200", "ttbar_ci0100", "ttbar_ci0010", "ttbar_ci0001", "ttbar_ci0000", "EW_no", "Eta", "DYJets", "STop", "WJets"};
    TString fileTitle_s[] = {"ttbar", "ttbar", "ttbar", "ttbar", "ttbar", "ttbar", "Eta", "DYJets", "STop", "WJets"};
    TString EWs[5] = {"*weight_ci0200", "*weight_ci0100", "*weight_ci0010", "*weight_ci0001", "*weight_ci0000"};
    Int_t npdf_ws[] = {103, 103, 103, 103, 103, 0, 0, 103, 101, 103};
    
    for (int i = 0; i < 10; i++)
    {
        fileTitle[i] = fileTitle_s[i];
        npdf_w[i] = npdf_ws[i];
        process[i] = process_s[i];
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
    for (int i = 0; i < 5; i++)
        EW[i] = EWs[i];

    int begins[] = {0, 1, 2, 3, 4, 6, 4, 10};
    int ends[] = {1, 2, 3, 4, 5, 10, 11, 11};
    TString categorys[] = {"ttbar200", "ttbar100", "ttbar010", "ttbar001", "ttbar000", "bg", "ttbar", "QCD"};
    begin = begins[option];
    end = ends[option];
    category = categorys[option] + "_" + sets.cut_name;
}

void prepare::draw_data()
{
    TString data_name = "data_obs";
    TH1 *h1;
    create_hist(h1, data_name);
    draw(h1, data_dir + sets.dataName(), "mytree", "1");
    file->cd();
    h1->Write();
    delete h1;
}

prepare::prepare(int cut_num, int year_s, bool is_ttx, int option) : sets(cut_num, year_s, is_ttx)
{
    sets.set_suf("");
    year = year_s;
    set_dir(option);
}
void prepare::set_bins(bool is_corrs)
{
    type = 0;
    is_corr = is_corrs;
    xbins = sets.xyz_bins[0];
    ybins = sets.xyz_bins[1];
    zbins = sets.xyz_bins[2];
    xlow = sets.xyz_range[0];
    xup = sets.xyz_range[1];
    ylow = sets.xyz_range[2];
    yup = sets.xyz_range[3];
    zlow = sets.xyz_range[4];
    zup = sets.xyz_range[5];
}
void prepare::set_bins(TString xvar_s, TString title, int nbin, double *xbin_s)
{
    xvar = xvar_s;
    category = title + "_" + category;
    xbins = nbin;
    type = 2;
    xbin = new double[nbin + 1];
    for (int i = 0; i < nbin + 1; i++)
        xbin[i] = xbin_s[i];
}
void prepare::set_bins(TString xvar_s, TString title, int nbin, double xlow_s, double xup_s)
{
    xvar = xvar_s;
    category = title + "_" + category;
    xbins = nbin;
    type = 1;
    xlow = xlow_s;
    xup = xup_s;
}
void prepare::create_hist(TH1 *&hist, TString name)
{
    switch (type)
    {
    case 0:
        hist = new TH3D(name, "", xbins, xlow, xup, ybins, ylow, yup, zbins, zlow, zup);
        break;
    case 1:
        hist = new TH1D(name, "", xbins, xlow, xup);
        break;
    case 2:
        hist = new TH1D(name, "", xbins, xbin);
        break;
    }
    hist->Sumw2();
}
void prepare::run()
{
    file = new TFile(outputDir + category + ".root", "recreate");
    for (int c = begin; c < min(10, end); c++)
    {
        draw_nom(c);
        cout << "finished nom of " << process[c] << endl;
    }
    for (int s = 0; s < 31; s++)
    {
        for (int c = begin; c < min(10, end); c++)
        {
            if (c > 4 && s > 24) // sys only for signal
                break;
            if (c == 5 || c == 6)
                continue; // no sys for EW_no and Eta
            draw_sys(c, s);
            cout << "finished sys of " << sys_n[s] << " of " << process[c] << endl;
        }
    }
    for (int c = begin; c < min(10, end); c++)
    {
        for (int p = 0; p < npdf_w[c]; p++)
        {
            draw_pdf(c, p);
            cout << "finished pdf_w" << p << " of " << process[c] << endl;
        }
    }
    if (end == 11)
    {
        draw_data();
        add_qcd();
    }
    cout << "finished all the things" << endl;
}
prepare::~prepare()
{
    file->Close();
}