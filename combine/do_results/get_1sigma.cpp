#include <TFile.h>
#include <TTree.h>
#include <iostream>
using namespace std;

double midx(double x1, double x2, double y1, double y2, double y0)
{
    return (x1 * y0 - x2 * y0 + x2 * y1 - x1 * y2) / (y1 - y2);
}
void get_1sigma(TString file_name, TString poi_name)
{
    Float_t poi, deltaNLL;
    Float_t best_fit;
    vector<Float_t> result1, result2;
    TFile *file = TFile::Open(file_name);
    TTree *limit = (TTree *)file->Get("limit");
    limit->SetBranchAddress(poi_name, &poi);
    limit->SetBranchAddress("deltaNLL", &deltaNLL);
    limit->GetEntry(0);
    best_fit = poi;
    float dis = 1000;
    int best_entry = -1;
    float pois[2], deltaNLLs[2];
    bool not_found_68 = true;
    float poi2s[2], deltaNLL2s[2];  
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
    {
        cout << "can't find the best fit point" << endl;
        return;
    }
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
            if (2 * deltaNLL >= 1 && not_found_68)
            {
                result1.push_back(midx(pois[0], pois[1], deltaNLLs[0], deltaNLLs[1], 1));
                not_found_68 = false;
            }
            if (2 * deltaNLL >= 3.84)
            {
                result2.push_back(midx(pois[0], pois[1], deltaNLLs[0], deltaNLLs[1], 3.84));
                break;
            }
        }
        not_found_68 = true;
        //cout << pois[0] << " " << deltaNLLs[0] << ", " << pois[1] << " " << deltaNLLs[1] << endl;
        //cout << (pois[0] - pois[1] + pois[1] * deltaNLLs[0] - pois[0] * deltaNLLs[1]) / (deltaNLLs[0] - deltaNLLs[1]) << endl;
    }
    if (result1.size() == 2)
        cout << "under 68%: (" << result1[0] << ", " << result1[1] << ")" <<endl;
    else if (result1.size() == 1)
    {
        if (result1[0] < best_fit)
            cout << "under 68%: (" << result1[0] << ", +inf" << ")" <<endl;
        else
            cout << "under 68%: (" << "-inf, " << result1[0] << ")" <<endl;
    }
    else
        cout << "68% not limited" << endl;

    if (result2.size() == 2)
        cout << "under 95%: (" << result2[0] << ", " << result2[1] << ")" <<endl;
    else if (result2.size() == 1)
    {
        if (result2[0] < best_fit)
            cout << "under 95%: (" << result2[0] << ", +inf" << ")" <<endl;
        else
            cout << "under 95%: (" << "-inf, " << result2[0] << ")" <<endl;
    }
    else
        cout << "95% not limited" << endl;
    //return result;
} 