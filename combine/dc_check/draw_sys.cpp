#include "draw_sys_pre.cpp"
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
void draw(TString file_n, TString file_ori_n, bool option, map<TString, double> &sys_range, TString out_path = "", vector<double> ycuts = {}, vector<vector<double>> xbins = {})
{
    TFile *file = TFile::Open(file_n);
    TFile *file_ori = TFile::Open(file_ori_n);
    static TString classname("TH1D");
    TH1D *hsm;
    TH1D *hmc[4];
    map<TString, TH1D> hist_map;
    map<TString, std::vector<TString>> sys_nom;
    TString sys_name, nom_name;
    TList *list = file->GetListOfKeys();
    TList *list_ori = file_ori->GetListOfKeys();
    TKey *key, *key_ori;
    TIter iter(list), iter_ori(list_ori);
    double range;
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
            }
            delete hist;
        }
    }
    while ((key_ori = (TKey *)iter_ori()))
    {
        if (key_ori->GetClassName() == classname)
        {
            TH1D *hist = (TH1D *)key_ori->ReadObj();
            if (hist)
            {
                TString hist_name = TString(hist->GetName()) + "_ori";
                hist_map[hist_name] = *hist;
            }
            delete hist;
        }
    }
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
                draw_pre(hsm, hmc, it_sys->first,  out_path + it_sys->first + "_" + *it_nom, sys_range[name], xbins, ycuts);
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
    file->Close();
    file_ori->Close();
}
void draw_sys(TString datacard_name, TString type_name)
{
    vector<double> ycuts;
    vector<vector<double>> xbins;
    if (!datacard_name.Contains("2cuts"))
    {
        ycuts = {0.0, 0.4, 1.0, 2.0};
        xbins = {{0,340,380,420,460,500,600,3000}, {0,350,400,450,500,550,600,700,800,3000}, 
                               {0,450,500,550,600,650,700,800,1000,3000}, {0,650,700,750,800,900,1000,1200,3000}};
    }
    else {
        ycuts = {0.0, 1.4};
        xbins = {{300,340,360,380,400,420,440,460,480,500,520,540,570,600,640,700,3000},
                                   {300,450,500,570,630,700,820,3000}};
    }
    if (xbins.size() != ycuts.size())
    {
        cout << "the number of cuts on deltay is not the same" << endl;
        return;
    }

    TString cutname[4] = {"E_3jets", "E_4jets", "M_3jets", "M_4jets"};
    int year[4] = {2015, 2016, 2017, 2018};
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
            draw(file[c][y], file_ori[c][y], 1, sys_range, out_path, ycuts, xbins);
        }
    }
}