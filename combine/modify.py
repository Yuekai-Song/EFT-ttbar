import ROOT as rt
cates = ['E_3jets', 'E_4jets', 'M_3jets', 'M_4jets']
years = ['2015', '2016', '2017', '2018']
process = ['ttbar_ci0000', 'ttbar_ci0010', 'ttbar_ci0100', 'ttbar_ci0200', 'ttbar_ci0001']
for cate in cates:
    for year in years:
        hists = []
        input_file = rt.TFile.Open("backup/ttbar_{}_{}.root".format(cate, year), "READ")
        for key in input_file.GetListOfKeys():    
            hist = key.ReadObj()
            hist_copy = hist.Clone()
            hist_copy.SetDirectory(0)
            if 'ttbar_ci0010' in hist.GetName():
                hist_clone = hist.Clone()
                hist_clone.SetDirectory(0)
                hist_clone.SetName(hist.GetName().replace('ttbar_ci0010', 'ttbar_add'))
                print(hist_clone.GetName())
                hists.append(hist_clone)
            if hist.GetName() in process:
            # if 'ttbar' in hist.GetName():    
                for i in range(0, hist_copy.GetNbinsX() + 2):
                    hist_copy.SetBinError(i, 0.0)
            hists.append(hist_copy)
        input_file.Close()
        output_file = rt.TFile.Open("datacards/ttbar_{}_{}.root".format(cate, year), "RECREATE")
        for hist in hists:
            hist.Write()
        output_file.Close()
