//need cmsenv, run on lxplus
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include "draw_pre.cpp"
bool contains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

void get_ch_sys(TString datacard, std::map<std::string, TString> &channelToTag, std::vector<TString> &nuis) {
    std::ifstream file(datacard);
    std::string line;
    int line_num = 10000;
    int line_rate;
    
    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (contains(line, "shapes"))
            {
                std::istringstream iss(line);
                std::string shapes, asterisk, channel, rootFile, process1, process2;
                if (!(iss >> shapes >> asterisk >> channel >> rootFile >> process1 >> process2)) { 
                    break;
                }
                size_t pos = rootFile.find("_");
                std::string tag = rootFile.substr(pos + 1);
                pos = tag.find(".");
                tag = tag.substr(0, pos);
                channelToTag[channel] = tag;
            }
            if (contains(line, "rate"))
                line_rate = line_num;
            if (line_num >= line_rate + 2 && line != "" && !contains(line, "shapeU"))
            {
                std::string sys;
                size_t pos = line.find(" ");
                sys = line.substr(0, pos);
                nuis.push_back(sys);
            }
            line_num++;
        }
        file.close();
    } else {
        std::cout << "Unable to open file" << std::endl;
    }

    /*
    for (auto const& pair : channelToTag) {
        std::cout << pair.first << " corresponds to " << pair.second << std::endl;
    }
    for (auto const& s : nuis) {
        std::cout << s << std::endl;
    }
    */
}
void draw_nuis(TString dir)
{
    std::map<std::string, TString> channelToTag;
    std::vector<TString> nuis;
    gSystem->Load("libHiggsAnalysisCombinedLimit.so");//cmsenv needed!
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    vector<double> ycuts;
    vector<vector<double>> xbins;
    ycuts = {0.0, 1.4};
    xbins = {{300,340,360,380,400,420,440,460,480,500,520,540,570,600,640,700,3000},
                                   {300,450,500,570,630,700,820,3000}};
    if (xbins.size() != ycuts.size())
    {
        cout << "the number of cuts on deltay is not the same" << endl;
        return;
    }
    map<TString, double> sys_range = {{"sb_co", 0.25}, {"mtop3", 0.15}};
    double range;

    TH1D *h1[5];
    double vals[5] = {0, -1, -0.5, 0.5, 1};
    TString ws_name = dir + "/workspace_ttbar.root";
    TString datacard = dir + "/ttbar.txt";
    get_ch_sys(datacard, channelToTag, nuis);
    TFile *ws_file = TFile::Open(ws_name);

    for (int ch_index = 0; ch_index < channelToTag.size(); ch_index++)
    {
        for (auto const& sys_name : nuis)
        {
            for (int i = 0; i < 5; i++)
                get_th(h1[i], ws_file, vector<TString> {sys_name}, vector<double> {vals[i]}, ch_index);
            TString legend[4];
            for (int i = 0; i < 4; i++)
                legend[i] = sys_name + " = " + Form("%.1f", vals[i + 1]);
            if (sys_range.find(sys_name) != sys_range.end())
                range = 0;
            else
                range = sys_range[sys_name];
            draw_pre(h1[0], h1 + 1, legend, dir + "/nuis/ttbar_" + sys_name + "_" + channelToTag[Form("ch%d", ch_index + 1)], range, xbins, ycuts);
            for (int i = 0; i < 5; i++)
                delete h1[i];
        }
    }
    ws_file->Close();
}