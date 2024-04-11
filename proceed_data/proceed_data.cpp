#include "read_gJSON.cpp"
typedef pair<pair<UInt_t, UInt_t>, ULong64_t> id;

// int main(){
void proceed_data(TString dir, int year, TString file, TString out)
{
    map<id, bool> event_map;
    pair<id, bool> event_id;
    TChain oldtree("mytree");
    oldtree.Add(dir + "new_" + file + ".root");
    UInt_t luminosityBlock, run;
    ULong64_t event;
    oldtree.SetBranchAddress("luminosityBlock", &luminosityBlock);
    oldtree.SetBranchAddress("event", &event);
    oldtree.SetBranchAddress("run", &run);
    map<UInt_t, std::vector<UInt_t>> json_map = read_gJSON(year);
    int num_f = 0;
    pair<map<id, bool>::iterator, bool> isexsit;
    map<UInt_t, std::vector<UInt_t>>::iterator run_lumi;
    bool injson;
    TFile *outFile;
    TTree *mytree;
    cout << oldtree.GetEntries() << endl;
    for (int entry = 0; entry < oldtree.GetEntries(); entry++)
    {
        if (entry % 500000 == 0)
        {
            cout << num_f << endl;
            outFile = new TFile(dir + Form("new_data_%d", num_f) + out + ".root", "RECREATE");
            mytree = (TTree *)oldtree.CloneTree(0);
            num_f++;
        }
        oldtree.GetEntry(entry);
        event_id.first.first.first = run;
        event_id.first.first.second = luminosityBlock;
        event_id.first.second = event;
        event_id.second = true;
        isexsit = event_map.insert(event_id);
        run_lumi = json_map.find(run);
        injson = false;
        if (run_lumi != json_map.end())
        {
            for (int i = 0; i < run_lumi->second.size(); i = i + 2)
            {
                if (luminosityBlock >= run_lumi->second[i] && luminosityBlock <= run_lumi->second[i + 1])
                {
                    injson = true;
                    break;
                }
            }
        }
        if (isexsit.second && injson)
            mytree->Fill();
        if ((entry + 1) % 500000 == 0 || entry == oldtree.GetEntries() - 1)
        {
            outFile->cd();
            mytree->Write();
            delete mytree;
            outFile->Close();
            // delete outFile;
        }
    }
}
