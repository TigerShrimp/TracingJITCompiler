import os
from subprocess import Popen, PIPE


def run_experiment(executable, program, runs):
    ex_time = 0
    for i in range(runs):
        print("\r{}: ({}/{})".format(executable, i+1, runs), end="")
        p = Popen([executable, program], stdout=PIPE)
        p.wait()
        ex_time += float(p.stdout.readlines()[-1]) / 10**3
    ex_time /= runs
    print("\rTot: {:.1f}                        ".format(ex_time))
    return ex_time


def run_java_experiment(program, runs):
    ex_time = 0.
    with open(os.devnull, 'wb', 0) as DEV_NULL:
        for i in range(runs):
            print("\rjava: ({}/{})".format(i+1, runs), end="")
            p = Popen(["java", "-Xlog:class+init=info:file=trace.log",
                       program], stdout=DEV_NULL)
            p.wait()
            with open('trace.log', 'r') as java_init:
                java_init_reversed = reversed(java_init.readlines())
                last = next(java_init_reversed)
                exit_time = float(last.split("[")[1].split("]")[
                    0].replace("s", ""))
                for line in java_init_reversed:
                    if program in line:
                        init_time = float(line.split("[")[1].split("]")[
                            0].replace("s", ""))
                        elapsed_time = exit_time - init_time
                        break

            ex_time += elapsed_time * 1000
    ex_time /= runs
    print("\rTot: {:.1f}                        ".format(ex_time))
    return ex_time


def get_trace_info(program):
    p = Popen(["../versions/trace_info",
               "{}.class".format(program)], stdout=PIPE)
    p.wait()
    info = p.stdout.readlines()[-1].decode('utf8').split(",")
    return info[0], info[1][:-1]


def run_experiments(program, runs=1000):
    print("{}:".format(program))
    interpret = run_experiment(
        "../versions/no_trace", "{}.class".format(program), runs)
    tracing = run_experiment("../versions/tracing",
                             "{}.class".format(program), runs)
    java = run_java_experiment(program, runs)
    traces, bytesWritten = get_trace_info(program)
    return interpret, tracing, java, traces, bytesWritten


tests = ["TwoHotSideExits", "EvenMoreLoops", "SingleLoop",
         "ChineseRemainder", "Factorial", "IsPrime", "LongFactorial",
         "FloatFactorial", "DoubleFactorial", "ManyVariables", "ManyVariablesMulTrace"]
timing = []
for test in tests:
    timing.append(run_experiments(test))

with open("data.csv", 'w') as res:
    res.write(
        "Test name, Interpreter, Tracing, Java, Actual Java run time, Traces, Bytes Written\n")
    for test, (inter, trace, java, traces, bytesWritten) in enumerate(timing):
        res.write("{},{},{},{},{},{}\n".format(
            tests[test], inter, trace, java, traces, bytesWritten))
