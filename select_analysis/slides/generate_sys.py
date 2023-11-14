import os
# To list the files in a specific folders

def sys_and_nom(h1_sys_name: str) -> tuple:
    h1_sys_name_new = h1_sys_name.replace(".pdf", "")
    #names of sys with 201* and M/E need to be same for all years and all channels
    years = ["2015", "2016", "2017", "2018"]
    for year in years:
        h1_sys_name_new = h1_sys_name_new.replace("_" + year, "unco")
        h1_sys_name_new = h1_sys_name_new.replace(year, "unco")

    leps = ["Muon", "Elec"]
    for lep in leps:
        h1_sys_name_new = h1_sys_name_new.replace(lep, "lep")
    pos = 0
    while h1_sys_name_new[pos] != '_':
        pos += 1
    #pdf_w, jes_, SF_
    odds = ["pdf", "jes", "SF", "nnlo", "EW"]
    for odd in odds:
        if odd in h1_sys_name_new:
            pos += 1
            break
    while h1_sys_name_new[pos] != '_':
        pos += 1

    sys = h1_sys_name_new[:pos]
    nom = h1_sys_name_new[pos + 1: ]
    sys = sys.replace("_", "-")
    nom = nom.replace("_ci0000", "")
    return sys, nom

def get_channel_name(path_name: str) -> str:
    pos = len(path_name) - 1
    while path_name[pos] != '/':
        pos = pos - 1
    return path_name[pos + 1 :]

def list_files(dir: str) -> tuple:
    pdfdict = dict()
    sys_nom = dict()
    for root, dirs, files in os.walk(dir):
        if dirs == []:
            for file in files:
                if file.endswith('.pdf'):
                    sys, nom = sys_and_nom(file)
                    if sys not in sys_nom.keys():
                        sys_nom[sys] = set()
                    sys_nom[sys].add(nom)
                    channel = get_channel_name(root)
                    pdfdict[sys + "_" + channel + "_" + nom] = os.path.join(root, file)
    return pdfdict, sys_nom

# To obtain the str needed in latex file


def obtain_initial(filename):
    lines = []
    with open(filename, 'r', encoding='utf-8') as f:
        for line in f.readlines():
            if line != '\\end{document}':
                lines.append(line)
    f.close()
    return lines

# To generate latex files



def gentex_sb(filename: str, filedict: dict, sys: str,
              processes: list) -> None:
    channels = ["E_3jets", "M_3jets", "E_4jets", "M_4jets"]
    years = ["2015", "2016", "2017", "2018"]

    with open(filename, 'a', encoding='utf-8') as f:
        f.write('\n')
        if "pdf" not in sys:
            f.write('\\subsection{%s}\n' % sys)
        elif "pdf_0" in sys:
            f.write('\\subsection{pdf}}\n')

        for process in processes:
            #f.write('\\subsection{%s}\n' % process)
            for t1 in range(0, 2):
                f.write('\n')
                f.write('\\begin{frame}\n')

                f.write('\\frametitle{%s}\n' % (process))
                f.write('\\fontsize{5}{1}\selectfont\n')
                for t2 in range(0, 2):
                    i = 2 * t1 + t2
                    f.write(years[i] + ":\n")
                    f.write('\\begin{figure}\n')
                    f.write('\\centering\n')

                    for j in range(4):
                        f.write('\\begin{subfigure}[b]{0.24\\textwidth}\n')
                        f.write('\\includegraphics[width=\\textwidth]{%s}\n' %
                                filedict[sys + "_" + channels[j] + "_" +
                                         years[i] + "_" + process])
                        if t2 == 1:
                            f.write('\\captionsetup{font=tiny}\n')
                            f.write('\\caption{%s}\n' %
                                    channels[j].replace("_", ", "))
                        f.write('\\end{subfigure}\n')
                    f.write('\\end{figure}\n')
                f.write('\\end{frame}\n')
                f.write('\n')
        #f.write('\\end{document}\n')'''
    f.close()

def position(process: str) -> int:
    processes = ["ttbar", "DYJets", "STop", "WJets"]
    if process not in processes:
        return len(processes)
    else:
        return processes.index(process)

def sort_dict_by_key(input_dict):
    sorted_items = sorted(input_dict.items())
    sorted_dict = {k: v for k, v in sorted_items}
    return sorted_dict

if __name__ == "__main__":
    dir = "/home/yksong/code/EFT-ttbar/test/combine_test/test2/sys_pdf/processed_bg_flat"
    modelfile = obtain_initial('./temp/temp.tex')
    pdfdict, sys_nom = list_files(dir)
    #print("jes_M_4jets_2015_Absolute_2018_WJets" in pdfdict.keys())
    filename = "./ttbar/ttbar.tex"

    with open(filename, 'w', encoding='utf-8') as f:
        for line in modelfile:
            f.write('%s' % line)
    f.close()

    sys_nom_sorted = sort_dict_by_key(sys_nom)
    for sys, noms in sys_nom_sorted.items():
        nom_list = list(noms)
        nom_list.sort(key=position)
        if "pdf" not in sys:
            gentex_sb(filename, pdfdict, sys, nom_list)


    with open(filename, 'a', encoding='utf-8') as f:
        f.write('\\end{document}\n')
    f.close()
    print('END')
