import os
import subprocess
import tempfile

def dispatch_helper():
    link_helper = subprocess.run(['make', 'dispatch_helper'], capture_output=True, text=True)
    if link_helper.returncode != 0:
        print('Fatal error. Failed to link correct helper object. Exiting...')
        exit(-1)

def check_task_1_1():
    dispatch_helper()
    os.system("gcc checker/checker.c checker/helper.o src/task1.c src/task3.c -o checker.exe -Iinclude -lm")
    os.system("chmod +x checker.exe") 
    result = subprocess.run(['./checker.exe', '1'], capture_output=True, text=True)  
    print(result.stdout)
    return result.returncode

def check_task_1_2():
    dispatch_helper()
    os.system("gcc checker/checker.c checker/helper.o src/task1.c src/task3.c -o checker.exe -Iinclude -lm")
    os.system("chmod +x checker.exe") 
    result = subprocess.run(['./checker.exe', '2'], capture_output=True, text=True)  
    print(result.stdout)
    return result.returncode

def check_task_1_3():
    dispatch_helper()
    os.system("gcc checker/checker.c checker/helper.o src/task1.c src/task3.c -o checker.exe -Iinclude -lm")
    os.system("chmod +x checker.exe") 
    
    result = subprocess.run(['./checker.exe', '3'], capture_output=True, text=True)
    if result.returncode != 5:
        print(result.stdout)
        return 0
    
    result = subprocess.run(['valgrind', '--leak-check=full', './checker.exe', '3'], capture_output=True, text=True)
    if "ERROR SUMMARY: 0 errors from 0 contexts" in result.stderr:
        print("✅ Task 1.3 trecut cu succes!")
        return 5
    else:
        print("❌ Memory leaks found")
        print(result.stderr)
    return 0

def check_task_1():
    points = 0
    print("\n🔹 TASK 1")
    print("=" * 60)
    print("\n  📝 Verificare task 1.1")
    points += int(check_task_1_1())
    print("-" * 60)
    print("\n  📝 Verificare task 1.2")
    points += int(check_task_1_2())
    print("-" * 60)
    print("\n  📝 Verificare task 1.3")
    points += int(check_task_1_3())
    print("=" * 60)
    print(f"✨ Numarul total de puncte Task 1: {points}\n")

    os.system("rm checker.exe")
    return points

def check_task_2():
    dispatch_helper()
    result_make = subprocess.run(['make', 'build'], capture_output=True, text=True)
    if result_make.returncode != 0:
        print("❌ Eroare la compilare!")
        print(result_make.stderr)
        return 0
    
    points = 0
    valgrind_pass = True
    os.system("chmod +x tema3")

    print("\n🔹 TASK 2")
    print("=" * 60)

    # Obține și sortează listele de baze de date și teste
    databases = sorted([db for db in os.listdir("tests/db") 
                       if os.path.isfile(os.path.join("tests/db", db)) 
                       and ":Zone.Identifier" not in db])
    
    tests = sorted([test for test in os.listdir("tests/input") 
                   if ":Zone.Identifier" not in test])

    for database in databases:
        print(f"\n📁 Database: {database}")
        print("-" * 60)
        
        for test in tests:
            input_test = "tests/input/" + test
            output_test = "tests/output/" + database.split(".")[0] + "." + test.split(".")[0] + ".out"
            ref_test = "tests/ref/" + database.split(".")[0] + "." + test.split(".")[0] + ".ref"

            print(f"  🧪 Test: {test}", end=" ... ")

            with open(input_test, 'r') as f:
                result = subprocess.run(['./tema3', 'tests/db/' + database], stdin=f, capture_output=True, text=True)
            
            if result.returncode != 0:
                print("❌ Eroare de rulare! (Segmentation fault)")
                continue

            with open(output_test, 'w') as f:
                f.write(result.stdout)
            
            if (os.system(f"diff {output_test} {ref_test} > /dev/null") != 0):
                print("❌ Rezultat incorect!")
            else:
                print("✅ OK", end="")
                points += 3.5
                valgrind_result = check_valgrind("tema3", database, input_test)
                if valgrind_result:
                    print(" | Valgrind: ✅")
                else:
                    print(" | Valgrind: ❌ Memory leaks")
                    valgrind_pass = False
        
        print()
    
    print("=" * 60)
    if not valgrind_pass:
        print("⚠️  Depunctare pentru memory leaks: -10 puncte")
        points -= 10

    print(f"✨ Numarul total de puncte Task 2: {points}\n")        
    return points

def check_task_3():
    if not os.path.exists("tests/output/task3"):
        os.makedirs("tests/output/task3")
    dispatch_helper()
    os.system("gcc checker/checker.c checker/helper.o src/task1.c src/task3.c -o checker.exe -Iinclude -lm")
    os.system("chmod +x checker.exe") 
    
    print("\n🔹 TASK 3")
    print("=" * 60)
    
    result = subprocess.run(['./checker.exe', '4'], capture_output=True, text=True)  
    print(result.stdout)

    points = 0
    
    # Sortează și filtrează testele
    databases = sorted([db for db in os.listdir("tests/db/task3/")
                       if os.path.isfile(os.path.join("tests/db/task3", db))
                       and ":Zone.Identifier" not in db])
    
    print("-" * 60)
    for database in databases:
        output_test = "tests/output/task3/" + database.split(".")[0] + ".db.enc"
        ref_test = "tests/ref/task3/" + database.split(".")[0] + ".db.enc.ref"

        print(f"  🧪 Test: {database}", end=" ... ")
        
        if os.system(f"cmp {output_test} {ref_test} > /dev/null 2>&1") != 0:
            print("❌ Rezultat incorect!")
        else:
            print("✅ OK")
            points += 4
    
    print("=" * 60)
    os.system("rm checker.exe")

    print(f"✨ Numarul total de puncte Task 3: {points}\n")
    return points

def check_valgrind(exec, database, test):
    with open(test, 'r') as f:
        result = subprocess.run(['valgrind', '--leak-check=full', './' + exec, 'tests/db/' + database], 
                              stdin=f, capture_output=True, text=True)
    if "ERROR SUMMARY: 0 errors from 0 contexts" in result.stderr:
        return True
    else:
        return False

def check_readme():
    print("\n📄 README")
    print("=" * 60)
    if not os.path.exists("README.md"):
        print("❌ README.md lipseste! -5 puncte\n")
        return -5
    print("✅ README.md gasit!\n")
    return 0

def print_result(test_name, result):
    print(f"{test_name}: {result}")

def check_coding_style():
    is_style_ok = True
    total_score = 0
    print("\n🎨 CODING STYLE")
    print("=" * 60)
    with tempfile.NamedTemporaryFile(delete=False, mode='w+', encoding='utf-8') as result_temp:
        temp_file_path = result_temp.name
        if os.path.isdir('.venv'):
            activate_script = os.path.join('.venv', 'bin', 'activate')
            subprocess.run(['source', activate_script], shell=True, check=False)

        result_temp.write("\nRun cpplint\n\n")
        try:
            cpplint_command = [
                "cpplint",
                "--filter=-legal/copyright,-readability/casting,-build/include_subdir,-runtime/threadsafe_fn,-build/header_guard,-runtime/int,-build/include_what_you_use",
                "--linelength=120",
                *subprocess.getoutput("find ./src/ -name '*.c'").splitlines()
            ]
            cpplint_result = subprocess.run(cpplint_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
            result_temp.write(cpplint_result.stderr)

            if cpplint_result.returncode != 0:
                is_style_ok = False
        except Exception as e:
            result_temp.write(f"Error running cpplint: {str(e)}\n")
            is_style_ok = False

        if os.path.isdir('.venv'):
            subprocess.run(['deactivate'], shell=True, check=False)

        result_temp.write("\nRun clang-tidy\n\n")
        try:
            clang_tidy_command = [
                "clang-tidy",
                "-checks=-*,cppcoreguidelines*,-cppcoreguidelines-macro-to-enum",
                "--warnings-as-errors=-*,cppcoreguidelines*,-cppcoreguidelines-macro-to-enum",
                *subprocess.getoutput("find ./src -name '*.h' -or -name '*.c'").splitlines(),
                "--"
            ]
            clang_tidy_result = subprocess.run(clang_tidy_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
            result_temp.write(clang_tidy_result.stdout)
            result_temp.write(clang_tidy_result.stderr)

            if clang_tidy_result.returncode != 0:
                is_style_ok = False
        except Exception as e:
            result_temp.write(f"Error running clang-tidy: {str(e)}\n")
            is_style_ok = False

    if not is_style_ok:
        print("❌ Test Coding Style: -20/0p failed")
        total_score -= 20
    else:
        print("✅ Test Coding Style: 0/0p passed")

    try:
        subprocess.run(['awk', '-f', 'checker/parse.awk', temp_file_path], check=False)
    except Exception as e:
        print(f"Error running summary parsing: {str(e)}")

    print("\n" + "=" * 60)
    print("📋 Detailed coding style report")
    print("=" * 60 + "\n")
    with open(temp_file_path, 'r', encoding='utf-8') as result_temp:
        print(result_temp.read())

    os.remove(temp_file_path)
    return total_score

def main():
    points = 0
    print("\n" + "=" * 60)
    print("🚀 RULARE CHECKER")
    print("=" * 60)
    
    points += check_task_1()
    points += check_task_2()
    points += check_task_3()
    points += check_readme()
    points += check_coding_style()
    
    print("\n" + "=" * 60)
    print(f"🏆 PUNCTAJ FINAL: {points}")
    print("=" * 60 + "\n")
    
    os.system("rm -f tema3")

if __name__ == "__main__":
    main()