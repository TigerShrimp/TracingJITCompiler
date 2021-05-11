from matplotlib import pyplot as plt
import numpy as np

TABLE_TEMPLATE = "\\begin{{table}}[h]\n\
    \\begin{{tabular}}{{|l|l|l|l|l|l|}}\n\
        \hline\n\
        \\textbf{{\\footnotesize Test name}} & \\textbf{{\\footnotesize Interpreter}} & \\textbf{{\\footnotesize Tracing}} & \\textbf{{\\footnotesize Java}} & \\textbf{{\\footnotesize \\#Traces}} & \\textbf{{\\footnotesize Traces size}}\\\\ \hline\hline\n\
{}\
    \end{{tabular}}\n\
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
        l, i, t, fj, j, ts, b = line[0], int(line[1]), int(
            line[2]), int(line[3]), int(line[4]), int(line[5]), int(line[6])
        labels.append(l)
        interpreter.append(i)
        tracing.append(t)
        java.append(j)
        table_rows += "\t{{\\footnotesize {}}} & {{\\footnotesize {}ms}} & {{\\footnotesize {}ms}} & {{\\footnotesize {}ms}} ({{\\footnotesize {}ms}}) & {{\\footnotesize {}}} & {{\\footnotesize {}B}} \\\\ \hline\n".format(l,
                                                                                                                                                                                                                              i, t, j, fj, ts, b)


max_time = max(max(tracing), max(java))
x = np.arange(len(labels))
width = 0.25

fig, ax = plt.subplots()
interpreter_bar = ax.bar(x-width, interpreter,
                         width, label='Interpreting')
tracing_bar = ax.bar(x, tracing, width, label='Tracing')
java_bar = ax.bar(x + width, java, width, label='Java')


print("💣")
ax.set_ylabel('Times')
ax.set_title('Wow')
ax.set_ylim(0, max_time*2)
ax.set_xticks(x)
ax.set_xticklabels(labels)
plt.xticks(fontsize=8, rotation=90)
ax.legend()

fig.tight_layout()

print(TABLE_TEMPLATE.format(table_rows))

plt.show()
