#include "prepare.h"

void prepare::draw(TH1 *h1, TString file, TString tree, TString weight = "")
{
    cout << file << " " << tree << endl;
    tree_draw *mytree = new tree_draw(tree, file, sets, string(weight));
    auto c0 = new TCanvas(weight, "c0", 8, 30, 600, 600);
    c0->cd();
    if (zvar.bins == 0 && yvar.bins == 0)
    {
        TH1D *h = (TH1D *)h1;
        mytree->draw(h, xvar.name);
    }
    else if (zvar.bins == 0)
    {
        TH2D *h = (TH2D *)h1;
        mytree->draw(h, xvar.name, yvar.name);
    }
    else
    {
        TH3D *h = (TH3D *)h1;
        mytree->draw(h, xvar.name, yvar.name, zvar.name);
    }
    delete mytree;
    delete c0;
}
void prepare::write(TH1* hist)
{
    file->cd();
    hist->Write();
    delete hist;
}
void prepare::draw_nom()
{
    TString weight;
    TH1 *hist;
    TString mcdir;
    for (int c = begin; c < min(npro, end); c++)
    {
        if (c < nEW)
            mcdir = Form("~/analysis/weight/output/%d/", sets.year);
        else
            mcdir = MC_dir;
        weight = weight_base;
        create_hist(hist, process[c]);
        if (c < nEW)
            weight = weight + EW[c] + "*nnlo_wt";
        if (c == nEW)
            weight = weight + "*nnlo_wt";
        for (int i = sets.index_pro[fileTitle[c]].first; i <  sets.index_pro[fileTitle[c]].second; i++)
            draw(hist, mcdir + sets.fileName(i), "mytree", weight);
        if (c == nEW - 1 && end == npro + 2)
            create_hist(h_other, "other");
        if (h_other)
        {    
            if (c != nEW)
                h_other->Add(hist);
            if (c == npro)
                use_other = true; 
        }
        write(hist);
        cout << "finished nom of " << process[c] << endl;
    }
    cout << "finished nom" << endl;
}

void prepare::draw_pdf(int p)
{
    TString weight;
    TH1 *hist_pdf;
    TString mcdir;
    for (int c = begin; c < min(npro, end); c++)
    {
        if (p >= npdf_w[c])
            continue;
        if (c < nEW)
            mcdir = Form("~/analysis/weight/output/%d/", sets.year);
        else
            mcdir = MC_dir;
        weight = weight_base + Form("*LHEPdfWeight[%d]", p);
        create_hist(hist_pdf, process[c] + Form("_pdf_w%d", p));
        if (c > nEW)
            hist_pdf->SetName(process[c] + Form("_pdf%d_w%d", c - nEW, p));
        else if (c < nEW)
            weight = weight + EW[c] + "*nnlo_wt";
        else
            weight = weight + "*nnlo_wt";
        for (int i = sets.index_pro[fileTitle[c]].first; i <  sets.index_pro[fileTitle[c]].second; i++)
            draw(hist_pdf, mcdir + sets.fileName(i), "mytree", weight);
        write(hist_pdf);
        cout << "finished pdf_w" << p << " of " << process[c] << endl;
    }
    cout << "finished pdf_w" << endl;
}
void prepare::draw_sys(stype s)
{
    TH1 *hist_up, *hist_dn;
    TString weight;
    TString mcdir;
    for (int c = begin; c < min(npro, end); c++)
    {
        if (!psys->match(s, process[c]))
            continue;
        if (c < nEW)
            mcdir = Form("~/analysis/weight/output/%d/", sets.year);
        else
            mcdir = MC_dir;
        weight = weight_base;
        create_hist(hist_up, process[c] + "_" + s.name + "Up");
        create_hist(hist_dn, process[c] + "_" + s.name + "Down");
        if (c < nEW)
            weight = weight + EW[c] + "*nnlo_wt";
        if (c == nEW)
            weight = weight + "*nnlo_wt";
        for (int i = sets.index_pro[fileTitle[c]].first; i < sets.index_pro[fileTitle[c]].second; i++)
        {
            vector<TString> a = psys->give_sys_name(sets.fileName(i), weight, s, process[c]);
            draw(hist_up, mcdir + a[0], a[2], a[4]);
            draw(hist_dn, mcdir + a[1], a[3], a[5]);
        }
        write(hist_up);
        write(hist_dn);
        cout << "finished sys of " << s.name << " of " << process[c] << endl;
    }
    cout << "finished sys of " << s.name << endl;
}
void prepare::draw_qcd(TString cg, bool isEnriched)
{
    TString ori_cg = sets.cg;
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
    TH1 *hmc;
    create_hist(hmc, "QCD_MC_" + cg + en);
    for (int j = sets.index_pro[qcd_title].first; j < sets.index_pro[qcd_title].second; j++)
        draw(hmc, MC_dir + sets.fileName(j), "mytree", weight_base);
    
    qcd_hists.push_back(hmc);
    sets.set_suf(ori_cg);
}
void prepare::draw_derived(TString cg)
{
    TString ori_cg = sets.cg;
    sets.set_suf(cg);
    TH1 *hdata, *hmc;
    create_hist(hdata, "QCD_prompt_" + cg); // final QCD;
    draw(hdata, data_dir + sets.dataName(), "mytree");
    if (cg == ori_cg && use_other)
        hmc = h_other;
    else
    {
        create_hist(hmc, "other_MC_" + cg);
        for (int j = sets.index_pro["other"].first; j < sets.index_pro["other"].second; j++)
        {
            TString weight = weight_base;
            if (j < 3)
                weight = weight + "*nnlo_wt*weight_ci0000";
            draw(hmc, MC_dir + sets.fileName(j), "mytree", weight);
        }
    }
    hdata->Add(hmc, -1.0);
    qcd_hists.push_back(hdata);
    delete hmc;
    sets.set_suf(ori_cg);
}

void prepare::add_qcd()
{
    TString cgs[] = {"B", "C", "D"};
    bool en[] = {false, true};
    qcd_hists.clear();
    draw_derived(sets.cg);
    for (int j = 0; j < 2; j++)
        draw_qcd(sets.cg, en[j]);
    
    if (for_pre && sets.cg == "A")
    {
        for (int i = 0; i < 3; i++)
        {
            draw_derived(cgs[i]);
            for (int j = 0; j < 2; j++)
                draw_qcd(cgs[i], en[j]);
        }
    }
    for (vector<TH1 *>::iterator it = qcd_hists.begin(); it != qcd_hists.end(); ++it)
        write(*it);
}
void prepare::set_dir()
{
    TString process_s[] = {"ttbar_ci0200", "ttbar_ci0100", "ttbar_ci0010", "ttbar_ci0001", "ttbar_ci0000", "EW_no", "Eta", "DYJets", "STop", "WJets"};
    TString fileTitle_s[] = {"ttbar", "ttbar", "ttbar", "ttbar", "ttbar", "ttbar", "Eta", "DYJets", "STop", "WJets"};
    TString EWs[] = {"*weight_ci0200", "*weight_ci0100", "*weight_ci0010", "*weight_ci0001", "*weight_ci0000"};
    Int_t npdf_ws[] = {103, 103, 103, 103, 103, 0, 0, 103, 101, 103};

    for (int i = 0; i < npro; i++)
    {
        fileTitle[i] = fileTitle_s[i];
        npdf_w[i] = npdf_ws[i];
        process[i] = process_s[i];
    }
    for (int i = 0; i < nEW; i++)
        EW[i] = EWs[i];
    
    begin = 0;
    end = npro + 1;
    category = "ttbar_" + sets.cut_name + Form("_%d", op);
}

void prepare::draw_data()
{
    TString data_name = "data_obs";
    TH1 *h1;
    create_hist(h1, data_name);
    draw(h1, data_dir + sets.dataName(), "mytree");
    write(h1);
}

prepare::prepare(int cut_num, int year, bool is_ttx, int option, TString sg, bool ready = false) : sets(cut_num, year, is_ttx)
{
    sets.set_suf(sg);
    op = option;
    set_dir();
    psys = sys::instance(year, sets.cut_name);
    for_pre = ready;
}
void prepare::set_bins(var xvars, var yvars =  {"", 0, 0, 0}, var zvars = {"", 0, 0, 0})
{
    xvar = xvars;
    yvar = yvars;
    zvar = zvars;
    if (yvar.bins == 0 && zvar.bins == 0)
        category = xvar.name + "_" + category;
    else if (zvar.bins == 0)
        category = xvar.name + "_" + yvar.name + "_" + category;
}

void prepare::create_hist(TH1 *&hist, TString name)
{
    if (zvar.bins == 0 && yvar.bins == 0)
        hist = new TH1D(name, "", xvar.bins, xvar.xlow, xvar.xup);
    else if (zvar.bins == 0 )
        hist = new TH2D(name, "", xvar.bins, xvar.xlow, xvar.xup, yvar.bins, yvar.xlow, yvar.xup);
    else
        hist = new TH3D(name, "", xvar.bins, xvar.xlow, xvar.xup, yvar.bins, yvar.xlow, yvar.xup, zvar.bins, zvar.xlow, zvar.xup);
    hist->Sumw2();
}

void prepare::run()
{
    file = new TFile(outputDir + category + ".root", "recreate");
    if (op > (psys->sys_all).size() + npdf || op < 0)
        return;
    if (op == 0)
    {
        draw_nom();
        if (end > npro)
            draw_data();
        if (end == npro + 2)
            add_qcd();
    }
    else if (sets.cg == "A" && for_pre)
    {    
        if (op <= (psys->sys_all).size())
            draw_sys(psys->sys_all[op - 1]);
        else
        {
            int index = op - (psys->sys_all).size();
            draw_pdf(2 * index - 1);
            draw_pdf(2 * index);
        }
    }
    cout << "finished all the things" << endl;
}
prepare::~prepare()
{
    file->Close();
}