import ROOT as rt
import Acorn.Analysis.analysis as ana
rt.gROOT.SetBatch(rt.kTRUE)
rt.EnableImplicitMT(32)
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('--lo', action='store_true', default=False)
args = parser.parse_args()

selections_cfg = {
    'm3': 'lep_flavour && jet_num == 3',
    'm4': 'lep_flavour && jet_num > 3',
    'e3': '!lep_flavour && jet_num == 3',
    'e4': '!lep_flavour && jet_num > 3'
}
input = '/grid_mnt/data__data.polcms/cms/song/CMSSW_14_1_0_pre4/src/ttbar/select/output/TTJets_TuneCP5_13TeV-madgraphMLM.root'

top_pt = ana.var_model('top_pt', 'p_{T}^{t} [GeV]', bin_edges=[0, 50, 100, 150, 200, 250, 300, 400, 1000])
top_pt.defname = 'top_pt'
deltay = ana.var_model('deltay', '|#Deltay|', bin_edges=[0, 0.4, 1.0, 2.0, 6.0])
deltay.redefine = 'abs(delta_rapidity_gen)'
cost = ana.var_model('cost', 'p_{T}^{t} [GeV]', bin_edges=[0, 0.3, 0.5, 0.8, 1])
cost.redefine = 'abs(ctstar_gen)'

rdf_raw = rt.RDataFrame('rawtree', input)
rdf_raw = rdf_raw.Define("wt_tot", "Generator_weight")
rdf_raw = rdf_raw.Define("ctg1_lin", f"Generator_weight * (1 + A_ctgre*1)")
rdf_raw = rdf_raw.Define("ctg2_lin", f"Generator_weight * (1 + A_ctgre*2)")
rdf_raw = rdf_raw.Define("ctg1_quad", f"Generator_weight * (1 + A_ctgre*1 + B_ctgre*1)")
rdf_raw = rdf_raw.Define("ctg2_quad", f"Generator_weight * (1 + A_ctgre*2 + B_ctgre*4)")
if args.lo:
    rdf_raw = rdf_raw.Filter("n_ajet == 0")

rdf = rt.RDataFrame('mytree', input)
rdf = rdf.Define("wt_tot", "Generator_weight")
rdf = rdf.Define("ctg1_lin", f"Generator_weight * (1 + A_ctgre*1)")
rdf = rdf.Define("ctg2_lin", f"Generator_weight * (1 + A_ctgre*2)")
rdf = rdf.Define("ctg1_quad", f"Generator_weight * (1 + A_ctgre*1 + B_ctgre*1)")
rdf = rdf.Define("ctg2_quad", f"Generator_weight * (1 + A_ctgre*2 + B_ctgre*4)")
if args.lo:
    rdf = rdf.Filter("n_ajet == 0")

hist_node = ana.Node()

hist_node['raw']['SM'] = ana.NDHistFromIndex(rdf_raw, [top_pt, deltay, cost], 0, "wt_tot", name='top_pt__deltay__cost', title='3D', single=True)
hist_node['raw']['ctg1_lin'] = ana.NDHistFromIndex(rdf_raw, [top_pt, deltay, cost], 0, "ctg1_lin", name='top_pt__deltay__cost', title='3D', single=True)
hist_node['raw']['ctg2_lin'] = ana.NDHistFromIndex(rdf_raw, [top_pt, deltay, cost], 0, "ctg2_lin", name='top_pt__deltay__cost', title='3D', single=True)
hist_node['raw']['ctg1_quad'] = ana.NDHistFromIndex(rdf_raw, [top_pt, deltay, cost], 0, "ctg1_quad", name='top_pt__deltay__cost', title='3D', single=True)
hist_node['raw']['ctg2_quad'] = ana.NDHistFromIndex(rdf_raw, [top_pt, deltay, cost], 0, "ctg2_quad", name='top_pt__deltay__cost', title='3D', single=True)

for sel_label in selections_cfg:
    rdf_del = rdf.Filter(selections_cfg[sel_label])
    hist_node[sel_label]['SM'] = ana.NDHistFromIndex(rdf_del, [top_pt, deltay, cost], 0, "wt_tot", name='top_pt__deltay__cost', title='3D', single=True)
    hist_node[sel_label]['ctg1_lin'] = ana.NDHistFromIndex(rdf_del, [top_pt, deltay, cost], 0, "ctg1_lin", name='top_pt__deltay__cost', title='3D', single=True)
    hist_node[sel_label]['ctg2_lin'] = ana.NDHistFromIndex(rdf_del, [top_pt, deltay, cost], 0, "ctg2_lin", name='top_pt__deltay__cost', title='3D', single=True)
    hist_node[sel_label]['ctg1_quad'] = ana.NDHistFromIndex(rdf_del, [top_pt, deltay, cost], 0, "ctg1_quad", name='top_pt__deltay__cost', title='3D', single=True)
    hist_node[sel_label]['ctg2_quad'] = ana.NDHistFromIndex(rdf_del, [top_pt, deltay, cost], 0, "ctg2_quad", name='top_pt__deltay__cost', title='3D', single=True)

allhists = []
for path, key, obj in hist_node.ListObjects():
    allhists.append(obj)
rt.RDF.RunGraphs(allhists)
for path, node in hist_node.ListNodes(withObjects=True):
    for _, key, obj in node.ListObjects():
        node[key] = obj.GetValue()

path_cut = {
    'deltay_0': [{'cut': {'deltay': (0, 0.4), 'cost': (0, 0.3)}, 'rebin': None}, {'cut': {'deltay': (0, 0.4), 'cost': (0.3, 1)}, 'rebin': [0, 50, 100, 150, 200, 250, 300, 1000]}],
    'deltay_1': [{'cut': {'deltay': (0.4, 1.0), 'cost': (0, 0.5)}, 'rebin': None}, {'cut':{'deltay': (0.4, 1.0), 'cost': (0.5, 1)}, 'rebin': [0, 50, 100, 150, 200, 250, 300, 1000]}],
    'deltay_2': [{'cut': {'deltay': (1.0, 2.0), 'cost': (0, 0.8)}, 'rebin': None}, {'cut': {'deltay': (0.4, 1.0), 'cost': (0.8, 1)}, 'rebin': [0, 50, 100, 150, 200, 250, 300, 1000]}],
    'deltay_3': [{'cut': {'deltay': (2.0, 6.0), 'cost': (0, 1.0)}, 'rebin': [0, 50, 100, 150, 200, 250, 300, 1000]}]
}

def project(hist_node, path_cut, target='m_fj'):
    for n, subnode in hist_node.ListNodes(withObjects=True):
        for _, name, obj in subnode.ListObjects(depth=0):
            for path, cut_rebin in path_cut.items():
                subpath = path.split('/')
                temp = subnode
                for k in subpath:
                    temp = temp[k]
                hnd =  ana.HND(obj)
                if not isinstance(cut_rebin, list):
                    temp[name] = hnd.Projection(target, rebinning=cut_rebin['rebin'], cuts=cut_rebin['cut'])
                else:
                    temp[name] = hnd.join(projections=[(target, icut['rebin'], icut['cut']) for icut in cut_rebin])
            del subnode.d[name]
project(hist_node, path_cut, target='top_pt')

file = rt.TFile('./output.root' if not args.lo else './output_LO.root', 'recreate')
ana.NodeToTDir(file, hist_node)
file.Close()