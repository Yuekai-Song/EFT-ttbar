#include "format.h"
void sys_and_nom(TString hist_name, TString &sys_name, TString &nom_name)
{
    int pos = 0;
    hist_name.ReplaceAll("Up", "");
    hist_name.ReplaceAll("Down", "");
    char *s = const_cast<char *>(hist_name.Data());
    while (s[pos] != '_')
        pos++;
    if (hist_name.Contains("ttbar"))
    {
        pos++;
        while (s[pos] != '_')
            pos++;
    }
    s[pos] = 0;
    nom_name = TString(s);
    sys_name = TString(s + pos + 1);
}
double get_range_pad2(TH1D *h1[4])
{
    float up = 1, down = 1;
    for(int i = 0; i < 4; i++)
    {
        if (up < h1[i]->GetMaximum())
            up = h1[i]->GetMaximum();
        if (down > h1[i]->GetMinimum())
            down = h1[i]->GetMinimum();
    }
    up = fabs(up - 1);
    down = fabs(down - 1);
    if (up > down)
        return 1.3 * up;
    else
        return 1.3 * down;
}
void draw(TString file_n, TString file_ori_n, bool option, map<TString, double> &sys_range, TString lumi = "", TString out_path = "", vector<TString> ycuts = {}, vector<TString> zcuts = {}, vector<vector<double>> xbins = {}, TString xtitle = "", TString cut = "")
{
    map<TString, TH1D> hist_map;
    static TString classname("TH1D");
    TH1D *hsm;
    TH1D *hmc[4];
    map<TString, std::vector<TString>> sys_nom;
    TString sys_name, nom_name;
    TKey *key, *key_ori;
    TFile *file = TFile::Open(file_n);
    TList *list = file->GetListOfKeys();
    TIter iter(list);
    while ((key = (TKey *)iter()))
    {
        if (key->GetClassName() == classname)
        {
            TH1D *hist = (TH1D *)key->ReadObj();
            if (hist)
            {
                TString hist_name = TString(hist->GetName());
                hist_map[hist_name] = *hist;
                if (hist_name.Contains("Up"))
                {
                    sys_and_nom(hist_name, sys_name, nom_name);
                    if((nom_name.Contains("ttbar") && nom_name.Contains("ci0000")) || (!nom_name.Contains("ttbar")))
                        sys_nom[sys_name].push_back(nom_name);
                }
                hist_map[hist_name].SetDirectory(0);
                delete hist;
            }
        }
    }
    file->Close();
    TFile *file_ori = TFile::Open(file_ori_n);
    TList *list_ori = file_ori->GetListOfKeys();
    TIter iter_ori(list_ori);
    while ((key_ori = (TKey *)iter_ori()))
    {
        if (key_ori->GetClassName() == classname)
        {
            TH1D *hist = (TH1D *)key_ori->ReadObj();
            if (hist)
            {
                TString hist_name = TString(hist->GetName()) + "_ori";
                hist_map[hist_name] = *hist;
                hist_map[hist_name].SetDirectory(0);
                delete hist;
            }
        }
    }
    file_ori->Close();
    for (map<TString, std::vector<TString>>::iterator it_sys = sys_nom.begin(); it_sys != sys_nom.end(); it_sys++)
    {
        for (vector<TString>::iterator it_nom = it_sys->second.begin(); it_nom != it_sys->second.end(); it_nom++)
        {
            hsm = &hist_map[*it_nom];
            TString name = *it_nom + "_" + it_sys->first;
            hmc[0] = &hist_map[name + "Up"];
            hmc[1] = &hist_map[name + "Down"];
            hmc[2] = &hist_map[name + "Up_ori"];
            hmc[3] = &hist_map[name + "Down_ori"];
            if (option)
            {
                int color[] = {2, 4};
                TH1D *hd[4];
                TString legend[] = {"#theta = +1#sigma", "#theta = -1#sigma"};
                DiCanvas *dc = new DiCanvas(xtitle, xbins, "./sys_pdf/" + out_path + it_sys->first + "_" + *it_nom);
                dc->draw_up(hsm, 1, "SM case", lumi, ycuts, zcuts, cut, it_sys->first);
                for (int i = 0; i < 2; i++)
                    dc->draw_up(hmc[i], color[i], legend[i]);
                for (int i = 0; i < 4; i++)
                {
                    hd[i] = (TH1D *)hmc[i]->Clone(Form("hd_%d", i));
                    hd[i]->Add(hsm, -1);
                    hd[i]->Divide(hsm);
                }
                for (int i = 0; i < 2; i++)
                {
                    dc->draw_dn(hd[i], "Ph", sys_range[name], color[i], 2);
                }
                for (int i = 0; i < 2; i++)
                    dc->draw_dn(hd[2 + i], "h", sys_range[name], color[i], 1);
                delete dc;
                for (int i = 0; i < 4; i++)
                    delete hd[i];
            }
            else
            {
                for (int i = 0; i < 4; i++)
                    hmc[i]->Divide(hsm);
                double range = get_range_pad2(hmc);
                if (sys_range.find(name) == sys_range.end() || sys_range[name] < range)
                    sys_range[name] = range;
            }
        }
    }
}
void draw_sys()
{
    TString datacard_name = "mtt_dyt_cost";
    TString type_name = "processed_bg_flat";
    vector<vector<double>> xbins;
    vector<TString> ycut, zcut;
    vector<var> x;
    TString xtitle;
    binning(datacard_name, x, xbins, ycut, zcut, xtitle);
    // cut[0] = Form("|cos(#theta^{*})| < %.1f", ycuts[1]);
    // cut[ncuts - 1] = Form("|cos(#theta^{*})| > %.1f", ycuts[ncuts - 1]);
    // for (int i = 1; i < ncuts - 1; i++)
    //     cut[i] = Form("%.1f < |cos(#theta^{*})| < %.1f", ycuts[i], ycuts[i + 1]);

    TString cutname[4] = {"E_3jets", "E_4jets", "M_3jets", "M_4jets"};
    TString cut[4] = {"e, 3jets", "e, #geq4jets", "#mu, 3jets", "#mu, #geq4jets"};
    int year[4] = {2015, 2016, 2017, 2018};
    TString lumi[] = {"19.5", "16.8", "41.48", "59.83"};
    map<TString, double> sys_range;
    TString inpath = "../datacards/";
    TString file_ori[4][4], file[4][4];
    for (int c = 0; c < 4; c++)
    {
        for (int y = 0; y < 4; y++)
        {
            TString filename = "ttbar_" + cutname[c] + Form("_%d.root", year[y]);
            file_ori[c][y] = inpath + datacard_name + "/original/" + filename;
            file[c][y] = inpath + datacard_name + "/" + type_name + "/" + filename;
            draw(file[c][y], file_ori[c][y], 0, sys_range);
        }
    }
    for (int c = 0; c < 4; c++)
    {
        for (int y = 0; y < 4; y++)
        {
            TString out_path = datacard_name + "/" + cutname[c] + Form("_%d/", year[y]);
            draw(file[c][y], file_ori[c][y], 1, sys_range, lumi[y], out_path, ycut, zcut, xbins, xtitle, cut[c]);
        }
    }
}