import os
import sys
import zipfile
import argparse
import shutil
import io
import subprocess
from urllib.request import urlretrieve




### Set your own paths and skip the script ####
skip = False ## Change this to True and change paths
QT_DIR = "C:/Qt/5.12.1/msvc2017_64"
QWT_INCLUDE_DIR = "C:/Qwt/qwt-6.1.0/src"
QWT_LIBRARY_RELEASE = "C:/Qwt/qwt-6.1.0/x64/Release/qwt.lib"
QWT_LIBRARY_DEBUG = "C:/Qwt/qwt-6.1.0/x64/Debug/qwtd.lib"
DLL_DIR = "C:/Users/Desktop/development/x64/x64/" #


###############################################

QT_FOUND, QWT_FOUND, DLL_FOUND = False, False,False
QT_DIR, QWT_DIR, DLL_DIR = "", "", ""
TOTAL_CHECKS = 3
hard_ignore_dll = False
hard_first_run = False
hard_qt_path = None
hard_qwt_path = None
hard_dll_path = None

QWT_UNZIP_PATH = "source_code/Vendors/Qwt"
DLL_UNZIP_PATH = "source_code/Vendors/RequiredDLLs"

QWT_TEMP_DOWNLOAD = "TEMP_QWT"
DLL_TEMP_DOWNLOAD = "TEMP_DLL"

QT_VER_DEFAULT = "5.12.11"
QT_COMPILER_DEFAULT = "msvc2017_64"

DEFAULT_LOOKUP_FOLDERS_QT = [f"C:/Qt/{QT_VER_DEFAULT}/{QT_COMPILER_DEFAULT}/", f"C:/Qt/Qt{QT_VER_DEFAULT}/{QT_COMPILER_DEFAULT}/"]
DEFAULT_LOOKUP_FOLDERS_QWT = ["./source_code/Vendors/Qwt/qwt-6.1.0/", "C:/Qwt/qwt-6.1.0/"]
DEFAULT_LOOKUP_FOLDERS_DLL = ["./source_code/Vendors/RequiredDLLs/"]

DEFAULT_FILES_TO_CHECK_QT = ["bin/qmake.exe"]
DEFAULT_FILES_TO_CHECK_QWT = ["x64/Release/qwt.dll"]
DEFAULT_FILES_TO_CHECK_DLL = ["Release/Qt5Core.dll"]

QWT_DOWNLOAD_LINK = 'https://www.dl.dropboxusercontent.com/scl/fi/z7xx5ispzhnnleqzcn2cu/Qwt.zip?rlkey=gmigab3avy3w5s713kbw4r2ye&dl=0'
DLL_DOWNLOAD_LINK = 'https://www.dl.dropboxusercontent.com/scl/fi/ajqratxwnodyj713olk0g/RequiredDLLs.zip?rlkey=nsjrr1yui54lqsy8z4sadjks7&dl=0'

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

def show_progress(block_num, block_size, total_size):
    print(str(round(block_num * block_size / total_size *100,2)) + " %", end="\r")
#### ---- ####

def build_using_cmake():
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
            '-D', f'QWT_INCLUDE_DIR={QWT_INCLUDE_DIR}',
            '-D', f'QWT_LIBRARY_RELEASE={QWT_LIBRARY_RELEASE}',
            '-D', f'QWT_LIBRARY_DEBUG={QWT_LIBRARY_DEBUG}',
            '-D', f'DLL_DIR={DLL_DIR}',
            '..'
        ]
        subprocess.run(cmake_args,  shell=False, check=True)
        print("CMake configuration completed successfully.")
    except subprocess.CalledProcessError:
        print("CMake configuration failed.")

    input("Press Enter to exit...")
    exit()


### Downloading and unzipping ###

def download_qwt():
    url = QWT_DOWNLOAD_LINK
    cprint("Downloading qwt...", YELLOW)
    urlretrieve(url, QWT_TEMP_DOWNLOAD+".zip", show_progress)

    cprint("Unzipping qwt...", YELLOW)
    with zipfile.ZipFile(QWT_TEMP_DOWNLOAD+".zip", 'r') as zip_ref:
        zip_ref.extractall(QWT_TEMP_DOWNLOAD)

    shutil.move(QWT_TEMP_DOWNLOAD + "/Qwt", QWT_UNZIP_PATH)

    os.remove(QWT_TEMP_DOWNLOAD+".zip")
    os.rmdir(QWT_TEMP_DOWNLOAD)

def download_dlls():
    url = DLL_DOWNLOAD_LINK
    cprint("Downloading dlls...", YELLOW)
    urlretrieve(url, DLL_TEMP_DOWNLOAD+".zip", show_progress)

    cprint("Unzipping dlls...", YELLOW)
    with zipfile.ZipFile(DLL_TEMP_DOWNLOAD+".zip", 'r') as zip_ref:
        zip_ref.extractall(DLL_TEMP_DOWNLOAD)

    shutil.move(DLL_TEMP_DOWNLOAD + "/RequiredDLLs", DLL_UNZIP_PATH)

    os.remove(DLL_TEMP_DOWNLOAD+".zip")
    os.rmdir(DLL_TEMP_DOWNLOAD)
###



#### File checking ####
def check_folder_n_files(folders_to_check, files_to_check):
    if not isinstance(folders_to_check, list):
        folders_to_check = [folders_to_check]
    for folder in folders_to_check:
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

def get_QWT_from_input():
    qwt_path = cinput("Please enter the path to QWT (ex: C:/Qwt/qwt-6.1.0): ",YELLOW).strip()
    if check_folder_n_files([qwt_path], DEFAULT_FILES_TO_CHECK_QWT):
        cprint(f"QWT path '{qwt_path}' is valid and QWT is installed.", GREEN)
        return qwt_path
    else:
        cprint(f"The provided installation of QWT at '{qwt_path}' is not valid. Please check the path.", RED)
        get_QWT_from_input()

def fix_QWT():
    qwt_installed = cinput("Is QWT downloaded on your system? (Valid answers: Yes/No y/n): ", YELLOW).strip().lower()
    if qwt_installed in ['yes', 'y']:
        qwt_path = get_QWT_from_input()
        return qwt_path
    else:
        cprint("QWT is not installed. Please find it here: \n" + QWT_DOWNLOAD_LINK, YELLOW)
        qwt_script_install = cinput("Should we install it for you? (Valid answers: Yes/No y/n): ", YELLOW).strip().lower()
        if qwt_script_install in ['yes', 'y']:
            download_qwt()
            return QWT_UNZIP_PATH
        else:
            cprint("After download is complete please run this script again", YELLOW)

            input("Press Enter to exit...")
            exit()

def get_DLL_from_input():
    # Ask the user to enter the path to dlls
    dlls_path = cinput("Please enter the path to dlls (ex: C:/Users/Desktop/development/x64/x64/): ",YELLOW).strip()
    if check_folder_n_files([dlls_path], DEFAULT_FILES_TO_CHECK_DLL):
        cprint(f"dlls path '{dlls_path}' is valid", GREEN)
        return dlls_path
    else:
        cprint(f"The provided path of dlls at '{dlls_path}' is not valid. Please check the path.", RED)
        get_DLL_from_input()

def fix_DLL():
    dlls_installed = cinput("Are the required dlls downloaded on your system? (Valid answers: Yes/No y/n): ", YELLOW).strip().lower()

    if dlls_installed in ['yes', 'y']:
        return get_DLL_from_input()
    else:
        cprint("dlls is not installed. Please find them here: \n" + DLL_DOWNLOAD_LINK, YELLOW)
        dlls_script_install = cinput("Should we download them for you? (Valid answers: Yes/No y/n): ", YELLOW).strip().lower()
        if dlls_script_install in ['yes', 'y']:
            download_dlls()
            return DLL_UNZIP_PATH
        else:
            cprint("After download is complete please run this script again", YELLOW)

            input("Press Enter to exit...")
            exit()



def FixMissing():
        global QT_DIR, QWT_DIR, DLL_DIR
        cprint("\n")
        cprint("Not all requirements are were met...", YELLOW)
        cprint("\n")
        if not QT_FOUND:
            QT_DIR = fix_QT()
        if not QWT_FOUND:
            QWT_DIR = fix_QWT()
        if not DLL_FOUND:
            DLL_DIR = fix_DLL()



def BuildIfAllIsGood(hard_qt, hard_qwt,hard_dll,hard_ignore_dll):
    if (QT_FOUND or hard_qt) and (QWT_FOUND or hard_qwt) and (DLL_FOUND or hard_dll or hard_ignore_dll):
        build_using_cmake()
    else:
        FixMissing()
        build_using_cmake()

def defined(var):
    return True if var is not None else False

def print_status():
    if QT_FOUND:
        cprint(f"[1/{TOTAL_CHECKS}] Qt Creator is installed on your Windows system at '{QT_DIR}' .", GREEN)
    else:
        cprint(f"[1/{TOTAL_CHECKS}] Qt Creator is not installed on your Windows system.", RED)

    if QWT_FOUND:
        cprint(f"[2/{TOTAL_CHECKS}] QWT library was found at '{QWT_DIR}' .", GREEN)
    else:
        cprint(f"[2/{TOTAL_CHECKS}] QWT library was not found.", RED)

    if not hard_ignore_dll:
        if DLL_FOUND:
            cprint(f"[3/{TOTAL_CHECKS}] Required Development DLLs were found at '{DLL_DIR}' .", GREEN)
        else:
            cprint(f"[3/{TOTAL_CHECKS}] Required Development DLLs were not found.", RED)
    else:
        cprint("DLL copying is ignored", YELLOW)

def first_run_download():
    download_qwt()
    download_dlls()

def main():
    global QT_FOUND, QWT_FOUND, DLL_FOUND
    global QT_DIR, QWT_DIR, DLL_DIR
    global TOTAL_CHECKS
    global hard_ignore_dll, hard_first_run, hard_qt_path, hard_qwt_path, hard_dll_path

    hard_ignore_dll = args.ignore_dll
    hard_first_run = args.first_run
    hard_qt_path = args.qt
    hard_qwt_path = args.qwt
    hard_dll_path = args.dll

    if skip:
        build_using_cmake()
        exit()

    cprint("ProMaIDeS is built using CMake. This script will check for the the required library and help you download them. For more information please refer to the https://promaides.myjetbrains.com/youtrack/articles/PMID-A-12/Development-Guide \n")
    cprint("Checking Requirement...", GREEN)

    if hard_first_run:
        first_run_download()

    if defined(hard_qt_path):
        QT_FOUND, QT_DIR = True, hard_qt_path
        if not check_folder_n_files(hard_qt_path, DEFAULT_FILES_TO_CHECK_QT)[0]:
            cprint("The Forced QT path is not valid but we will continue anyways",RED)
            QT_FOUND = False
    else:
        QT_FOUND, QT_DIR = check_folder_n_files(DEFAULT_LOOKUP_FOLDERS_QT, DEFAULT_FILES_TO_CHECK_QT)

    if defined(hard_qwt_path):
        QWT_FOUND, QWT_DIR = True, hard_qwt_path
        if not check_folder_n_files(hard_qwt_path, DEFAULT_FILES_TO_CHECK_QWT)[0]:
            cprint("The Forced QWT path is not valid but we will continue anyways",RED)
            QWT_FOUND = False
    else:
        QWT_FOUND, QWT_DIR  = check_folder_n_files(DEFAULT_LOOKUP_FOLDERS_QWT, DEFAULT_FILES_TO_CHECK_QWT)

    if not hard_ignore_dll:
        if defined(hard_dll_path):
            DLL_FOUND, DLL_DIR = True, hard_dll_path
            if not check_folder_n_files(hard_dll_path, DEFAULT_FILES_TO_CHECK_DLL)[0]:
                cprint("The Forced DLLs path is not valid but we will continue anyways",RED)
            DLL_FOUND = False
        else:
            DLL_FOUND, DLL_DIR  = check_folder_n_files(DEFAULT_LOOKUP_FOLDERS_DLL, DEFAULT_FILES_TO_CHECK_DLL)
    else:
        TOTAL_CHECKS = TOTAL_CHECKS - 1

    print_status()

    BuildIfAllIsGood(defined(hard_qt_path), defined(hard_qwt_path), defined(hard_dll_path), hard_ignore_dll)

    input("Press Enter to exit...")
    exit()

if __name__ == '__main__':
    parser  = argparse.ArgumentParser()
    parser = argparse.ArgumentParser(
                    prog='ProMaIDeS CMake Setup',
                    description='Setup the ProMaIDeS dependencies',
                    epilog='For more information please refer to the https://promaides.myjetbrains.com/youtrack/articles/PMID-A-12/Development-Guide')
    parser.add_argument("-g", "--ignore-dll", action='store_true', help="Ignore copying the dlls into the executable's folder (They will need to be copied manually)")
    parser.add_argument("-f", "--first_run", action='store_true', help="Skipping walking through options and directly downloading prerequisites")
    parser.add_argument("-q", "--qt", help="Specify path of QT Framework")
    parser.add_argument("-w", "--qwt", help="Specify path of QWT Library")
    parser.add_argument("-d", "--dll", help="Specify path of the dlls (ignored if -g is set)")
    args = parser.parse_args()

    main()