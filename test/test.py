import os
import subprocess

os.chdir('test/')
class_files = os.listdir()
class_files = [c for c in class_files if c.endswith('class')]

tests_passed = 0
for class_file in class_files:
    java_out = subprocess.run(['java', class_file.split(
        '.')[0]], capture_output=True).stdout.decode('utf-8')
    tiger_shrimp_process = subprocess.run(
        ['../build/TigerShrimp', class_file], capture_output=True)
    tiger_shrimp_out = tiger_shrimp_process.stdout.decode('utf-8')
    tiger_shrimp_err = tiger_shrimp_process.stderr
    if(java_out == tiger_shrimp_out):
        print('\033[92mTest {} passed: \033[0m'.format(class_file))
        tests_passed += 1
    elif tiger_shrimp_err:
        print('\033[91mTest {} failed with error: {}\033[0m'.format(
            class_file, tiger_shrimp_err.decode('utf-8')), end='')
    else:
        print('\033[91mTest {} failed: \n\tGot: {}\tExpected: {}\033[0m'.format(
            class_file, tiger_shrimp_out, java_out), end='')

print('Tests: {}/{} passed'.format(tests_passed, len(class_files)))