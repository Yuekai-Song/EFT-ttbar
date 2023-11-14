#include "draw_sys_pre.cpp"
void draw_sys(TString cutname, int year, TString name)
{
    vector<double> ycuts;
    vector<vector<double>> xbins;
    ycuts = {0.0, 0.4, 1.0, 2.0};
    xbins = {{0,340,380,420,460,500,600,3000}, {0,350,400,450,500,550,600,700,800,3000}, 
                    {0,450,500,550,600,650,700,800,1000,3000}, {0,650,700,750,800,900,1000,1200,3000}};
    if (xbins.size() != ycuts.size())
    {
        cout << "the number of cuts on deltay is not the same" << endl;
        return;
    }
    TString inpath = "../../combine/";
    TString filename = "ttbar_" + cutname + Form("_%d.root", year);
    TFile *file_ori = TFile::Open(inpath + "datacard/original/" + filename);
    TFile *file = TFile::Open(inpath + "datacard/" + name + "/" + filename);
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
    map<TString, double> sys_range = {{"sb_co", 0.25}, {"mtop3", 0.15}};
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
            if (sys_range.find(it_sys->first) != sys_range.end())
                range = 0;
            else
                range = sys_range[it_sys->first];
            hsm = &hist_map[*it_nom];
            hmc[0] = &hist_map[*it_nom + "_" + it_sys->first + "Up"];
            hmc[1] = &hist_map[*it_nom + "_" + it_sys->first + "Down"];
            hmc[2] = &hist_map[*it_nom + "_" + it_sys->first + "Up_ori"];
            hmc[3] = &hist_map[*it_nom + "_" + it_sys->first + "Down_ori"];
            draw_pre(hsm, hmc, it_sys->first, name + "/" + cutname + Form("_%d/", year) + it_sys->first + "_" + *it_nom, range, xbins, ycuts);
        }
    }
    file->Close();
    file_ori->Close();
}