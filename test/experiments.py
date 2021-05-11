import os
from subprocess import Popen, PIPE


def run_experiment(executable, program, runs):
    user, system, max_rss = 0, 0, 0
    with open(os.devnull, 'wb', 0) as DEV_NULL:
        for i in range(runs):
            print("\r{}: ({}/{})".format(executable, i+1, runs), end="")
            p = Popen([executable, program], stdout=DEV_NULL)
            ru = os.wait4(p.pid, 0)[2]
            user += ru.ru_utime
            system += ru.ru_stime
            max_rss += ru.ru_maxrss
    user /= runs
    system /= runs
    max_rss /= runs
    print("\rTot: {:.4f}, user: {:.4f}, system: {:.4f}, max rss: {}".format(
        (user+system), user, system, max_rss))
    return int((user+system)*1000)


def get_trace_info(program):
    p = Popen(["../versions/trace_info",
               "{}.class".format(program)], stdout=PIPE)
    p.wait()
    info = p.stdout.readlines()[-1].decode('utf8').split(",")
    return info[0], info[1][:-1]


def run_experiments(program, runs=1):
    print("{}:".format(program))
    interpret = run_experiment(
        "../versions/no_trace", "{}.class".format(program), runs)
    tracing = run_experiment("../versions/tracing",
                             "{}.class".format(program), runs)
    java = run_experiment("java", program, runs)
    traces, bytesWritten = get_trace_info(program)
    return interpret, tracing, java, traces, bytesWritten


tests = ["TwoHotSideExits", "EvenMoreLoops", "SingleLoop",
         "ChineseRemainder", "Factorial", "IsPrime", "LongFactorial",
         "FloatFactorial", "DoubleFactorial", "ManyVariables", "ManyVariablesMulTrace"]
startup_ts, startup_tst, startup_java, traces, bytesWritten, = run_experiments(
    "Empty", 1)
java_down_time = startup_java - startup_ts
timing = []
for test in tests:
    timing.append(run_experiments(test))

with open("data.csv", 'w') as res:
    res.write(
        "Test name, Interpreter, Tracing, Java, Actual Java run time, Traces, Bytes Written\n")
    res.write("{},{},{},{},{},{},{}\n".format(
        "Empty", startup_ts, startup_tst, startup_java, startup_java-java_down_time, traces, bytesWritten))
    for test, (inter, trace, java, traces, bytesWritten) in enumerate(timing):
        res.write("{},{},{},{},{},{},{}\n".format(
            tests[test], inter, trace, java, java-java_down_time, traces, bytesWritten))
