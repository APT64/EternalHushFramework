MAVEN_VERSION = "3.9.4"
MAVEN_DIR = "maven\\apache-maven-" + MAVEN_VERSION
MAVEN_BIN = MAVEN_DIR + "\\bin"

CMAKE_VERSION = "3.27.1"
CMAKE_DIR = "cmake\\cmake-" + CMAKE_VERSION + "-windows-x86_64"
CMAKE_BIN = CMAKE_DIR + "\\bin"

BUILD_DIR = "final"

import os
from colorama import *
import time
import dload
import argparse

def print_good(text):
    os.system("cls")
    print("[" +Fore.GREEN + "+" + Fore.RESET + "] " + text)
def print_bad(text):
    os.system("cls")
    print("[" +Fore.RED + "-" + Fore.RESET + "] " + text)
def print_warn(text):
    os.system("cls")
    print("[" +Fore.YELLOW + "!" + Fore.RESET + "] " + text)

def main():
    parser = argparse.ArgumentParser(prog='compile.py', description='A script that automatically builds the EternalHush Framework')
    parser.add_argument("-b", "--build", action="store_true", help="build the framework")
    parser.add_argument("-c", "--clean", action="store_true", help="clean build files")
    parser.add_argument("-r", "--rebuild", action="store_true", help="clean build files and build the framework")
    parser.add_argument("-f", "--force_clean", action="store_true", help="remove all build files")
    parser.add_argument("--build_core", action="store_true", help="build core")
    parser.add_argument("--build_gui", action="store_true", help="build gui")
    args = parser.parse_args()

    just_fix_windows_console()
    if args.build:
        download_tools()
        build()
    elif args.clean:
        download_tools()
        clean()
    elif args.build_core:
        download_tools()
        build_core()
    elif args.build_gui:
        download_tools()
        build_gui()
    elif args.rebuild:
        download_tools()
        clean()
        build()
    elif args.force_clean:
        download_tools()
        clean()
        remove_tools()
        remove_build()

def build_core():
    print_good('Configuring core module')
    os.system(CMAKE_BIN+"\\cmake.exe -S EternalHushCore -B EternalHushCore\\build")
    
    print_good('Building core module')
    os.system(CMAKE_BIN+"\\cmake.exe --build EternalHushCore\\build --config Release")
    
    print_good('Copying core module')
    os.system("COPY EternalHushCore\\build\\Release\\EternalHushCore.dll build\\ /Y")

def build_gui():
    print_good('Compiling EternalHush project')
    os.system(MAVEN_BIN +"\\mvn.cmd compile")
    print_good('Packaging EternalHush project')
    os.system(MAVEN_BIN +"\\mvn.cmd package")
    
    print_good('Copying jar and libs')
    os.system("COPY target\\EternalHushFramework* build\\ /Y")
    os.system("XCOPY /e /i /Y target\\libs build\\libs")

def remove_tools():
    print_good('Removing downloads')
    os.system('DEL *.zip')
    os.system('RMDIR /s /q maven')
    os.system('RMDIR /s /q cmake')
    
def remove_build():
    print_good('Removing build files')
    os.system('RMDIR /s /q target')
    os.system('RMDIR /s /q build')

def download_tools():
    print_good('Downloading Maven ' + MAVEN_VERSION)
    dload.save_unzip("https://dlcdn.apache.org/maven/maven-3/"+MAVEN_VERSION+"/binaries/apache-maven-"+MAVEN_VERSION+"-bin.zip", "maven")
    
    print_good('Downloading Cmake ' + CMAKE_VERSION)
    dload.save_unzip("https://github.com/Kitware/CMake/releases/download/v"+CMAKE_VERSION+"/cmake-"+CMAKE_VERSION+"-windows-x86_64.zip", "cmake")

def clean():
    print_good('Cleaning project directory')
    os.system(MAVEN_BIN + "\\mvn.cmd clean")
    os.system("RMDIR /s /q EternalHushCore\\build")

def build():
    print_good('Building "ETERNALHUSH FRAMEWORK"')
    time.sleep(0.5)
    
    os.system("MKDIR build")
    
    build_gui()
    
    print_good('Copying prebuilt content')
    os.system("XCOPY /e /Y prebuilt\\ build\\")
    
    build_core()

if __name__=="__main__":
    main()