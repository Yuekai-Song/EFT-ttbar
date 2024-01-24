#include <TFile.h>
#include <TTree.h>
#include <iostream>
using namespace std;

vector<Float_t> get_1sigma(TString dir, TString poi_name)
{
    Float_t poi, deltaNLL;
    Float_t best_fit;
    vector<Float_t> result;
    TFile *file = TFile::Open(dir + "limit_scan_" + poi_name + ".root");
    TTree *limit = (TTree *)file->Get("limit");
    limit->SetBranchAddress(poi_name, &poi);
    limit->SetBranchAddress("deltaNLL", &deltaNLL);
    limit->GetEntry(0);
    best_fit = poi;
    float dis = 1000;
    int best_entry = -1, left_entry = -1, right_entry = -1;
    float pois[2], deltaNLLs[2];    
    for (int entry = 1; entry < limit->GetEntries(); entry++)
    {
        limit->GetEntry(entry);
        if (fabs(poi - best_fit) < dis)
        {
            dis = fabs(poi - best_fit);
            if (2 * deltaNLL < 1)
                best_entry = entry;
        }
    }
    if (best_entry == -1)
        cout << "can't find the best fit point" << endl;
    for (int d = 0; d < 2; d++)
    {
        int entry = best_entry;
        while(entry > 0 && entry < limit->GetEntries())
        {
            limit->GetEntry(entry);
            pois[0] = pois[1];
            pois[1] = poi;
            deltaNLLs[0] =  deltaNLLs[1];
            deltaNLLs[1] = 2 * deltaNLL;
            if (d == 0)
                entry--;
            else
                entry++;
            if (2 * deltaNLL > 1)
                break;
        }
        //cout << pois[0] << " " << deltaNLLs[0] << ", " << pois[1] << " " << deltaNLLs[1] << endl;
        //cout << (pois[0] - pois[1] + pois[1] * deltaNLLs[0] - pois[0] * deltaNLLs[1]) / (deltaNLLs[0] - deltaNLLs[1]) << endl;
        result.push_back((pois[0] - pois[1] + pois[1] * deltaNLLs[0] - pois[0] * deltaNLLs[1]) / (deltaNLLs[0] - deltaNLLs[1]));
    }
    return result;
} 