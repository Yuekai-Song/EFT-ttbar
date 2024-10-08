#include <vector>
#include <iostream>
#include <TROOT.h>
#include <TString.h>
#include "../analysis/prepare/settings.h"
using namespace std;
TString deg(double a)
{
    double b = a / 0.1;
    if (fabs(b - round(b)) > 1e-6)
        return Form("%.2f",  a);
    else
        return  Form("%.1f", a);
}

TString get_cut(vector<double> cut, var vars, TString name)
{
    TString cutn;
    if (cut[1] > vars.xup)
        cutn = name + " > " + deg(cut[0]);
    else if (cut[0] < vars.xlow)
        cutn = name + " < " + deg(cut[1]);
    else
        cutn = deg(cut[0]) + " < "  + name + " < " + deg(cut[1]);
    return cutn;
}
vector<vector<vector<double>>> binning(TString dc_name, vector<var> &x, vector<vector<double>> &xbins_in, vector<TString> &ycuts, vector<TString> &zcuts, TString &xtitle)
{
    vector<double> ycut;
    vector<vector<double>> zcut;
    vector<vector<double>> xbins;
    vector<vector<vector<double>>> xbins_user;
    TString xn, yn, zn;
    if (dc_name.Contains("2cuts")) {
        zcut = {{0.0, 1.0}};
        ycut = {0.0, 1.4, 4.1};
        xbins = {{300,340,360,380,400,420,440,460,480,500,520,540,570,600,640,700,3000},
                                   {300,450,500,570,630,700,820,3000}};
        xn = "M_{t#bar{t}} [GeV]";
        yn = "|#Deltay|";
        zn = "|cos(#theta)|";
        x = {abs_cost, abs_deltay, Mtt_fine};
    }
    else if (dc_name.Contains("mtt_dyt_cost_4")) {
        zcut = {{0.0, 0.15, 0.3, 0.6, 1.0}, {0.0, 0.5, 0.7, 0.85, 1.0}, {0, 1.0}, {0, 1.0}};
        ycut = {0.0, 0.4, 1.0, 2.0, 4.1};
        xbins = {{0,340,380,420,460,500,600,3000},{0,340,380,420,460,500,600,3000},{0,340,380,420,460,500,600,3000},{0,340,380,420,460,500,600,3000},
                 {0,350,400,450,500,550,600,700,800,3000},{0,350,400,450,500,550,600,700,800,3000},{0,350,400,450,500,550,600,700,800,3000},{0,350,400,450,500,550,600,700,800,3000},
                 {0,450,500,550,600,650,700,800,1000,3000},
                 {0,650,700,750,800,900,1000,1200,3000}};
        xn = "M_{t#bar{t}} [GeV]";
        yn = "|#Deltay|";
        zn = "|cos(#theta)|";
        x = {abs_cost, abs_deltay, Mtt_fine};
    }
    else if (dc_name.Contains("mtt_dyt_cost_1")) {
        zcut = {{0.0, 0.15, 0.3, 0.6, 1.0}, {0.0, 0.65, 0.8, 0.9, 1.0}};
        ycut = {0.0, 1.0, 4.1};
        xbins = {{300,350,400,450,500,550,600,700,800,1000,2000},{300,350,400,450,500,550,600,700,800,1000,2000},
                 {300,350,400,450,500,550,600,700,800,1000,2000},{300,350,400,450,500,550,600,700,800,1000,2000},
                 {300,450,500,550,600,700,800,1000,2000},{300,450,500,550,600,700,800,1000,2000},
                 {300,450,500,550,600,700,800,1000,2000},{300,450,500,550,600,700,800,1000,2000}};
        xn = "M_{t#bar{t}} [GeV]";
        yn = "|#Deltay|";
        zn = "|cos(#theta)|";
        x = {abs_cost, abs_deltay, Mtt_fine};
    }
    else if (dc_name.Contains("mtt_dyt_cost_2")) {
        zcut = {{0.0, 0.2, 0.4, 1.0}, {0.0, 0.55, 0.75, 1.0}, {0.0, 0.75, 0.9, 1.0}};
        ycut = {0.0, 0.6, 1.4, 4.1};
        xbins = {{300,350,400,450,500,550,600,700,800,1000,2000},{300,350,400,450,500,550,600,700,800,1000,2000},{300,350,400,450,500,550,600,700,800,1000,2000},
                 {300,400,450,500,550,600,700,800,1000,2000},{300,400,450,500,550,600,700,800,1000,2000},{300,400,450,500,550,600,700,800,1000,2000},
                 {300,450,500,550,600,700,800,1000,2000},  {300,450,500,550,600,700,800,1000,2000},    {300,450,500,550,600,700,800,1000,2000}};
        xn = "M_{t#bar{t}} [GeV]";
        yn = "|#Deltay|";
        zn = "|cos(#theta)|";
        x = {abs_cost, abs_deltay, Mtt_fine};
    }
    else if (dc_name.Contains("mtt_dyt_cost_3")) {
        zcut = {{0.0, 0.4, 0.6, 0.75, 0.9, 1.0}};
        ycut = {0.0, 2.0};
        xbins = {{300,350,400,450,500,550,600,650,700,800,1000,2000},{300,350,400,450,500,550,600,650,700,800,1000,2000},
                 {300,350,400,450,500,550,600,650,700,800,1000,2000},{300,350,400,450,500,550,600,650,700,800,1000,2000},
                 {300,350,400,450,500,550,600,650,700,800,1000,2000}};
        xn = "M_{t#bar{t}} [GeV]";
        yn = "|#Deltay|";
        zn = "|cos(#theta)|";
        x = {abs_cost, abs_deltay, Mtt_fine};
    }
    else if (dc_name.Contains("mtt_dyt_cost")) {
        zcut = {{0.0, 0.3, 1.0}, {0.0, 0.5, 1.0}, {0.0, 0.8, 1.0}, {0.0, 1.0}};
        ycut = {0.0, 0.4, 1.0, 2.0, 4.1};
        xbins = {{0,340,380,420,460,500,600,3000},{0,340,380,420,460,500,600,3000},
                 {0,350,400,450,500,550,600,700,800,3000},{0,350,400,450,500,550,600,700,800,3000},
                 {0,450,500,550,600,650,700,800,1000,3000},{0,450,500,550,600,650,700,800,1000,3000},
                 {0,650,700,750,800,900,1000,1200,3000}};
        xn = "M_{t#bar{t}} [GeV]";
        yn = "|#Deltay|";
        zn = "|cos(#theta)|";
        x = {abs_cost, abs_deltay, Mtt_fine};
    }
    else if (dc_name.Contains("mtt_dyt_new")) {
        zcut = {{0.0, 1.0}};
        ycut = {0.0, 0.4, 1.0, 2.0, 4.1};
        xbins = {{300,350,400,450,500,550,600,700,800,2000}, {300,350,400,450,500,550,600,700,800,2000}, 
                              {300,350,400,450,500,550,600,700,800,2000}, {300,350,400,450,500,550,600,700,800,2000}};
        xn = "M_{t#bar{t}} [GeV]";
        yn = "|#Deltay|";
        zn = "|cos(#theta)|";
        x = {abs_cost, abs_deltay, Mtt_fine};
    }
    else if (dc_name.Contains("mtt_dyt")) {
        zcut = {{0.0, 1.0}};
        ycut = {0.0, 0.4, 1.0, 2.0, 4.1};
        xbins = {{0,340,380,420,460,500,600,3000}, {0,350,400,450,500,550,600,700,800,3000}, 
                               {0,450,500,550,600,650,700,800,1000,3000}, {0,650,700,750,800,900,1000,1200,3000}};
        xn = "M_{t#bar{t}} [GeV]";
        yn = "|#Deltay|";
        zn = "|cos(#theta)|";
        x = {abs_cost, abs_deltay, Mtt_fine};
    }
    else if (dc_name.Contains("mtt_cost")) {
        zcut = {{0.0, 0.4, 0.6, 0.75, 0.9, 1.0}};
        ycut = {0.0, 4.1};
        xbins = {{300,350,400,450,500,550,600,650,700,800,1000,2000,}, {300,350,400,450,500,550,600,650,700,800,1000,2000}, 
                 {300,350,400,450,500,550,600,650,700,800,1000,2000}, {300,350,400,450,500,550,600,650,700,800,1000,2000},
                 {300,350,400,450,500,550,600,650,700,800,1000,2000}};
        xn = "M_{t#bar{t}} [GeV]";
        yn = "|#Deltay|";
        zn = "|cos(#theta)|";
        x = {abs_cost, abs_deltay, Mtt_fine};
    }

    else if (dc_name.Contains("pt_dyt")) {
        zcut = {{0.0, 1.0}};
        ycut = {0.0, 0.4, 1.0, 2.0, 4.1};
        xbins = {{0,50,100,150,200,250,300,400,500,600,800,1000}, {0,50,100,150,200,250,300,400,500,600,800,1000}, 
                               {0,50,100,150,200,250,300,400,500,600,800,1000}, {0,50,100,150,200,250,300,400,500,600,800,1000}};
        xn = "p_{T}(t) [GeV]";
        yn = "|#Deltay|";
        zn = "|cos(#theta)|";
        x = {abs_cost, abs_deltay, top_pt_fine};
    }
    else if (dc_name.Contains("pt_cost")) {
        zcut = {{0.0, 0.4, 0.6, 0.75, 0.9, 1.0}};
        ycut = {0.0, 4.1};
        xbins = {{0,50,100,150,200,250,300,400,500,600,800,1000}, {0,50,100,150,200,250,300,400,500,600,800,1000}, 
                 {0,50,100,150,200,250,300,400,500,600,800,1000}, {0,50,100,150,200,250,300,400,500,600,800,1000},
                 {0,50,100,150,200,250,300,400,500,600,800,1000}};
        xn = "p_{T}(t) [GeV]";
        yn = "|#Deltay|";
        zn = "|cos(#theta)|";
        x = {abs_cost, abs_deltay, top_pt_fine};
    }
    int num = 0;
    for (int i = 0 ; i < ycut.size() - 1; i++)
    {
        int index = i;
        if (zcut.size() < ycut.size() - 1)
            index = 0;
        for (int j = 0 ; j < zcut[index].size() - 1; j++)
        {
            xbins_user.push_back({{zcut[index][j], zcut[index][j + 1]}, {ycut[i], ycut[i + 1]}, xbins[num]});
            num++;
        }
    }
    xtitle = xn;
    xbins_in.clear();
    ycuts.clear();
    zcuts.clear();
    TString zcutn, ycutn;
    for (int i = 0; i < xbins_user.size(); i++)
    {
        xbins_in.push_back(xbins_user[i][2]);
        if (zcut[0].size() != 2 && ycut.size() != 2)
        {
            ycuts.push_back(get_cut(xbins_user[i][1], x[1], yn));
            zcuts.push_back(get_cut(xbins_user[i][0], x[0], zn));
        }
        else if(zcut[0].size() != 2)
            zcuts.push_back(get_cut(xbins_user[i][0], x[0], zn));
        else if(ycut.size() != 2)
            ycuts.push_back(get_cut(xbins_user[i][1], x[1], yn));
    }
    return xbins_user;
}