import os
import subprocess

os.chdir('test/')
class_files = os.listdir()
class_files = [c for c in class_files if c.endswith('class')]

tests_passed = 0
for class_file in class_files:
    class_name = class_file.split('.')[0]
    java_out = subprocess.run(['java', class_name],
                              capture_output=True).stdout.decode('utf-8')
    tiger_shrimp_process = subprocess.run(
        ['../build/TigerShrimp', class_file], capture_output=True)
    tiger_shrimp_out = tiger_shrimp_process.stdout.decode('utf-8')
    tiger_shrimp_err = tiger_shrimp_process.stderr

    java_lines = [round(float(j), 3) for j in java_out.split('\n') if j]
    tiger_shrimp_lines = [round(float(t), 3)
                          for t in tiger_shrimp_out.split('\n') if t]

    if(java_lines == tiger_shrimp_lines):
        print('\033[92mTest {} passed :D \033[0m'.format(class_name))
        tests_passed += 1
    elif tiger_shrimp_err:
        print('\033[91mTest {} failed with error: {} :(\033[0m\n'.format(
            class_name, tiger_shrimp_err.decode('utf-8')[:-1]), end='')
    else:
        print('\033[91mTest {} failed: \n\tGot: {}\tExpected: {}\033[0m'.format(
            class_name, tiger_shrimp_out, java_out), end='')

print('Tests: {}/{} passed'.format(tests_passed, len(class_files)))
if tests_passed == len(class_files):
    subprocess.run(['say', 'didididididi'])
else:
    subprocess.run(['say', 'didu'])
