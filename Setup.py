import os
import sys
import argparse
import shutil
import io
import subprocess
from urllib.request import urlretrieve

QT_FOUND = False
QT_DIR = ""
TOTAL_CHECKS = 1

##Windows
if os.name == 'nt':
    #QT_VER_DEFAULT = "6.5.3"
    QT_VER_DEFAULT = "5.15.2"
    QT_COMPILER_DEFAULT = "msvc2019_64"
    DEFAULT_LOOKUP_FOLDERS_QT = [f"C:\\Qt\\{QT_VER_DEFAULT}\\{QT_COMPILER_DEFAULT}\\", f"C:\\Qt\\Qt{QT_VER_DEFAULT}\\{QT_COMPILER_DEFAULT}\\"]
    DEFAULT_FILES_TO_CHECK_QT = ["bin\\qmake.exe"]
    QWT_INCLUDE_DIR     = os.getcwd() + "\\source_code\\Vendors\\QWT\\src\\"
    QWT_LIBRARY_RELEASE = os.getcwd() + "\\source_code\\Vendors\\QWT\\lib\\qwt.lib"
    QWT_LIBRARY_DEBUG   = os.getcwd() + "\\source_code\\Vendors\\QWT\\lib\\qwtd.lib"
    STATIC_LIBS_DIR     = os.getcwd() + "\\source_code\\Vendors\\StaticLibs\\Win64\\Release\\"
#Linux
if os.name == 'posix':
    QWT_INCLUDE_DIR     = os.getcwd() + "\\source_code\\Vendors\\QWT\\src\\"
    QWT_LIBRARY_RELEASE = os.getcwd() + "\\source_code\\Vendors\\QWT\\lib\\libqwt.so"
    QWT_LIBRARY_DEBUG   = os.getcwd() + "\\source_code\\Vendors\\QWT\\lib\\libqwt.so"
    STATIC_LIBS_DIR     = os.getcwd() + "\\source_code\\Vendors\\StaticLibs\\linux\\Release\\" ###Change this



CMAKE_BUILD_DIR = "build"   
#### Util ####
RED = 1
YELLOW = 2
GREEN = 3
def cprint(text, color=-1):
    if color == RED:
        print("\033[31m" + text + "\033[0m")
    elif  color == YELLOW:
        print("\033[33m" + text + "\033[0m")
    elif  color == GREEN:
        print("\033[32m" + text + "\033[0m")
    else:
        print(text)
def cinput(text, color=-1):
    if color == RED:
        return input("\033[31m" + text + "\033[0m")
    elif  color == YELLOW:
        return input("\033[33m" + text + "\033[0m")
    elif  color == GREEN:
        return input("\033[32m" + text + "\033[0m")
    else:
        return input(text)

#### ---- ####

def build_using_cmake():
    global QT_DIR
    
    cprint("\n")
    cprint("All Requirementes are met...🎉", GREEN)
    cprint("Running cmake", GREEN)
    cprint("\n")

    if not os.path.exists(CMAKE_BUILD_DIR):
        os.makedirs(CMAKE_BUILD_DIR)
    os.chdir(CMAKE_BUILD_DIR)

    try:
        cmake_args = [
            'cmake',
            '-D', f'QT_DIR={QT_DIR}',
            '..'
        ]
        subprocess.run(cmake_args,  shell=False, check=True)
        cprint("CMake configuration completed successfully.", GREEN)
    except subprocess.CalledProcessError:
        cprint("CMake configuration failed.", RED)

    input("Press Enter to exit...")
    exit()

#### File checking ####
def check_folder_n_files(folders_to_check, files_to_check):
    if not isinstance(folders_to_check, list):
        folders_to_check = [folders_to_check]
    for folder in folders_to_check:
        folder = folder.replace("/","\\")
        if not folder[-1] == "\\":
            folder += "\\"
        allFilesThere=True
        for file in files_to_check:
            if not os.path.exists(folder + file):
                allFilesThere = False
                break
        if not allFilesThere:
            continue
        return [True, folder]
    return [False, ""]
#### ---- ####


#### Inputing paths ####

def get_QT_from_input():
        # Ask the user to enter the path to Qt
        qt_path = cinput("Please enter the path to Qt (ex: C:/Qt/5.12.11/msvc2017_64): ",YELLOW).strip()
        # Check if the provided path exists
        if check_folder_n_files([qt_path], DEFAULT_FILES_TO_CHECK_QT):
            if QT_VER_DEFAULT in qt_path and QT_COMPILER_DEFAULT in qt_path:
                cprint(f"Qt path '{qt_path}' is valid and Qt is installed.", GREEN)
            else:
                cprint(f"Qt path '{qt_path}' is valid but uses a different version than the recommended one. Please use " + QT_VER_DEFAULT + " " + QT_COMPILER_DEFAULT + " if you face problems later on.", YELLOW)
            return qt_path
        else:
            cprint(f"The provided installation of QT at '{qt_path}' is not valid. Please check the path.", RED)
            get_QT_from_input()


def fix_QT():
    qt_installed = cinput("Is Qt " + QT_VER_DEFAULT + " " + QT_COMPILER_DEFAULT +" installed on your system? (Valid answers: Yes/No y/n): ", YELLOW).strip().lower()

    if qt_installed in ['yes', 'y']:
        return get_QT_from_input()
    else:
        url = 'https://download.qt.io/archive/qt/5.12/5.12.11/qt-opensource-windows-x86-5.12.11.exe' 
        cprint("QT is not installed, Please download and install from: \n" + url, YELLOW)
        cprint("Create a QT account if required, and walk through the setup", YELLOW)
        cprint("If link is broken, search for \"Qt for open source use\" and install QT v" + QT_VER_DEFAULT + " " + QT_COMPILER_DEFAULT + "", YELLOW)
        cprint("\n")
        cprint("After setup is complete please run this script again", YELLOW)

        input("Press Enter to exit...")
        exit()

def FixMissing():
        global QT_DIR
        cprint("\n")
        cprint("Not all requirements are were met...", YELLOW)
        cprint("\n")
        if not QT_FOUND:
            QT_DIR = fix_QT()

def print_status_QT(FoldersChecked):
    if QT_FOUND:
        cprint(f"[1/{TOTAL_CHECKS}] Qt Creator is installed on your Windows system at '{QT_DIR}' .", GREEN)
    else:
        cprint(f"[1/{TOTAL_CHECKS}] Qt Creator is not installed on your Windows system. Looked in {FoldersChecked}", RED)

def main():
    global QT_FOUND, QT_DIR

    cprint("ProMaIDeS is built using CMake. This script will check for the the required library and help you download them. For more information please refer to the https://promaides.myjetbrains.com/youtrack/articles/PMID-A-12/Development-Guide \n")
    cprint("Checking Requirement...", GREEN)

    QT_FOUND, QT_DIR = check_folder_n_files(DEFAULT_LOOKUP_FOLDERS_QT, DEFAULT_FILES_TO_CHECK_QT)
    print_status_QT(DEFAULT_LOOKUP_FOLDERS_QT)

    if QT_FOUND:
        build_using_cmake()
    else:
        FixMissing()
        build_using_cmake()

    input("Press Enter to exit...")
    exit()

if __name__ == '__main__':
    parser  = argparse.ArgumentParser()
    parser = argparse.ArgumentParser(
                    prog='ProMaIDeS CMake Setup',
                    description='Setup the ProMaIDeS dependencies',
                    epilog='For more information please refer to the https://promaides.myjetbrains.com/youtrack/articles/PMID-A-12/Development-Guide')
    args = parser.parse_args()

    main()