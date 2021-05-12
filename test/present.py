from matplotlib import pyplot as plt
import numpy as np

TABLE_TEMPLATE = "\\begin{{table}}[h]\n\
    \\begin{{tabular}}{{|l|l|l|l|l|l|}}\n\
        \hline\n\
        \\textbf{{\\footnotesize Test name}} & \\textbf{{\\footnotesize Interpreter}} & \\textbf{{\\footnotesize Tracing}} & \\textbf{{\\footnotesize Java}} & \\textbf{{\\footnotesize \\#Traces}} & \\textbf{{\\footnotesize Traces size}}\\\\ \hline\hline\n\
{}\
    \end{{tabular}}\n\
    \caption{{Table with numbers}}\n\
    \label{{tab:testresults}}\n\
\end{{table}}"


labels = []
interpreter = []
tracing = []
java = []
table_rows = ""

with open('data.csv', 'r') as results:
    next(results)
    for line in results.readlines():
        line = line.split(',')
        l, i, t, j, ts, b = line[0], float(line[1]), float(
            line[2]), float(line[3]), int(line[4]), int(line[5])
        labels.append(l)
        interpreter.append(i)
        tracing.append(t)
        java.append(j)
        table_rows += "\t{{\\footnotesize {}}} & {{\\footnotesize {:.1f}ms}} & {{\\footnotesize {:.1f}ms}} & {{\\footnotesize {:.1f}ms}} & {{\\footnotesize {}}} & {{\\footnotesize {}B}} \\\\ \hline\n".format(l,
                                                                                                                                                                                                                i, t, j, ts, b)


max_time = max(max(tracing), max(java))
x = np.arange(len(labels))
width = 0.25

fig, ax = plt.subplots(figsize=(5.91, 5))
interpreter_bar = ax.bar(x-width, interpreter,
                         width, label='Interpreting')
tracing_bar = ax.bar(x, tracing, width, label='Tracing')
java_bar = ax.bar(x + width, java, width, label='Java')


ax.set_ylabel('Execution time (ms)')
ax.set_ylim(0, max_time*1.5)
ax.set_xticks(x)
ax.set_xticklabels(labels)
plt.xticks(rotation=90)
ax.legend()

fig.tight_layout()

print(TABLE_TEMPLATE.format(table_rows))

plt.show()
