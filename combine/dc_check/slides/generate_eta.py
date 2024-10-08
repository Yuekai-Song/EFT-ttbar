import os
# To list the files in a specific folders


def get_channel_name(path_name: str) -> str:
    pos = len(path_name) - 1
    while path_name[pos] != '/':
        pos = pos - 1
    return path_name[pos + 1 :]

def list_files(dir: str) -> dict:
    pdfdict = dict()
    pois = set()
    for root, dirs, files in os.walk(dir):
        if dirs == []:
            for file in files:
                if file.endswith('.pdf'):
                    eta = file.replace(".pdf", "")
                    channel = get_channel_name(root)
                    pdfdict[eta + "_" + channel] = os.path.join(root, file)
                    pois.add(eta)
    return pdfdict, pois


# To generate latex files

def gentex(filename: str, filedict: dict, poi: str) -> None:
    channels = ["E_3jets", "M_3jets", "E_4jets", "M_4jets"]
    years = ["2015", "2016", "2017", "2018"]

    with open(filename, 'a', encoding='utf-8') as f:
        f.write('\n')
        f.write('\\subsection{%s}\n' % poi)
        for t1 in range(0, 2):
            f.write('\n')
            f.write('\\begin{frame}\n')

            #f.write(years[t1] + ":\n")
            for t2 in range(0, 2):
                i = 2 * t1 + t2
                f.write(years[i] + ":\n")
                f.write('\\begin{figure}\n')
                f.write('\\centering\n')
                
                for j in range(4):
                    f.write('\\begin{subfigure}[b]{0.24\\textwidth}\n')
                    f.write('\\includegraphics[width=\\textwidth]{%s}\n' %
                            filedict[poi + "_" + channels[j] + "_" + years[t1]])
                    if t2 == 1:
                        f.write('\\captionsetup{font=tiny}\n')
                        f.write('\\caption{%s}\n' % channels[j].replace("_", ", "))
                    f.write('\\end{subfigure}\n')
                f.write('\\end{figure}\n')
            f.write('\\end{frame}\n')
            f.write('\n')
        #f.write('\\end{document}\n')'''
    f.close()


def eta_add(dir: str, filename: str):
    with open(filename, 'a', encoding='utf-8') as f:
        f.write('\n')
        f.write('\\section{Distributions of Toponium}\n')
    f.close()
    pdfdict, pois = list_files(dir)
    for poi in pois:
        gentex(filename, pdfdict, poi)