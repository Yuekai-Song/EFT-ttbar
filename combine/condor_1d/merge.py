import ROOT
import glob
import os
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('--dir-name', '-d', required=True, help='')
args = parser.parse_args()

dir_name = os.path.join('/grid_mnt/data__data.polcms/cms/song/CMSSW_14_1_0_pre4/src/ttbar/combine/datacards', args.dir_name)
os.chdir(dir_name)
output_file = ROOT.TFile("output.root", "RECREATE")
output_tree = None

file_list = sorted(glob.glob("higgsCombinescan_1Dz_fix.POINTS.*.root"))

if not file_list:
    print("No matching ROOT files found.")
    exit(1)

print(f"Found {len(file_list)} files, starting merge...")

chain = ROOT.TChain("limit")
for f in file_list:
    chain.Add(f)

output_file = ROOT.TFile("output.root", "RECREATE")
output_tree = chain.CloneTree(0)
output_tree.SetDirectory(output_file)

n_entries = chain.GetEntries()
print(f"Total entries: {n_entries}")

entry_file_map = []
curr_file = None
for i in range(n_entries):
    chain.GetEntry(i)
    filename = chain.GetTree().GetCurrentFile().GetName()
    if filename != curr_file:
        curr_file = filename
        entry_file_map.append(i)

skip_set = set(entry_file_map[1:])

for i in range(n_entries):
    if i in skip_set:
        continue
    chain.GetEntry(i)
    output_tree.Fill()

output_tree.Write("limit")
output_file.Close()
print("Merge complete: output.root, best-fit point saved from only one file.")