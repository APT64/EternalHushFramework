import os
import time
import typer
import dload
import shutil
import subprocess

from rich import print
from rich.console import Console

# Constants
MAVEN_VERSION   =   "3.9.4"
MAVEN_DIR       =   f"maven\\apache-maven-{MAVEN_VERSION}"
MAVEN_BIN       =   f"{MAVEN_DIR}\\bin"

CMAKE_VERSION   =   "3.27.1"
CMAKE_DIR       =   f"cmake\\cmake-{CMAKE_VERSION}-windows-x86_64"
CMAKE_BIN       =   f"{CMAKE_DIR}\\bin"

BANNER = """[bold green]
##############################################################
#                    ETERNALHUSH FRAMEWORK                   #
#                    VERSION 1.0.0.0-ALPHA                   #
#                Author: https://github.com/APT64            #
##############################################################
[bold white]
"""

# Init console
console = Console()

# Helpers
class EternalHushFrameworkBuilder(object):
    """ EternalHuashFramework builder functions class """
    def __init__(self) -> None:
        pass

    def banner(self):
        """ Shows off the banner """
        print(BANNER)

    def build(self):
        """ Builds the EternalHushFramework """
        console.log('[bold white]\[ [bold green]+ [bold white]] Building [bold green]"ETERNALHUSH FRAMEWORK"[bold white]')
        time.sleep(0.5)

        subprocess.run("MKDIR build", shell=True)

        self.build_gui()

        console.log('[bold white]\[ [bold green]+ [bold white]] Copying prebuilt content')
        subprocess.run("XCOPY /e /Y prebuilt\\ build\\", shell=True)

        self._build_core()
        self.build_modules()

    def _build_core(self):
        """ Builds only the core of EternalHushFramework """
        console.log('[bold white]\[ [bold green]+ [bold white]] Configuring core module')
        subprocess.run(f"{CMAKE_BIN}\\cmake.exe -S EternalHushCore -B EternalHushCore\\build", shell=True)

        console.log('[bold white]\[ [bold green]+ [bold white]] Building core module')
        subprocess.run(f"{CMAKE_BIN}\\cmake.exe --build EternalHushCore\\build --config Release", shell=True)

        console.log('[bold white]\[ [bold green]+ [bold white]] Copying core module')
        subprocess.run("COPY EternalHushCore\\build\\Release\\EternalHushCore.dll build\\ /Y", shell=True)

    def build_modules(self):
        """ Builds only the modules of EternalHushFramework """
        console.log('[bold white]\[ [bold green]+ [bold white]] Building additional modules')

        for directory in os.listdir("modules"):
            f               =   os.path.join("modules", directory)
            desc_file       =   os.path.join(f, "module.xml")
            build_files     =   os.path.join(f, "bin_files")
            bin_out32       =   os.path.join(build_files, "bin32\\Release")
            bin_out64       =   os.path.join(build_files, "bin64\\Release")
            script_files    =   os.path.join(f, "scripts")

            subprocess.run(f'{CMAKE_BIN}\\cmake.exe -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=../../bin32 -A Win32 -S {build_files} -B {build_files}\\output32', shell=True)
            subprocess.run(f'{CMAKE_BIN}\\cmake.exe -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=../../bin64 -A x64 -S {build_files} -B {build_files}\\output64', shell=True)
            subprocess.run(f'{CMAKE_BIN}\\cmake.exe --build {build_files}\\output32 --config Release', shell=True)
            subprocess.run(f'{CMAKE_BIN}\\cmake.exe --build {build_files}\\output64 --config Release', shell=True)

            subprocess.run(f'MKDIR build\\modules\\{directory}\\files', shell=True)
            subprocess.run(f'XCOPY /e /i /Y {script_files} build\\modules\\{directory}\\scripts', shell=True)
            subprocess.run(f'COPY {desc_file} build\\modules\\{directory}\\ /Y', shell=True)

            for root, dirs, files in os.walk(bin_out32):
                for file in files:
                    shutil.copyfile(
                        os.path.join(root, file),
                        f"build\\modules\\{directory}\\files\\X32_{os.path.basename(file)}"
                    )

            for root, dirs, files in os.walk(bin_out64):
                for file in files:
                    shutil.copyfile(
                        os.path.join(root, file),
                        f"build\\modules\\{directory}\\files\\X64_{os.path.basename(file)}"
                    )

    def build_gui(self):
        """ Builds the EternalHushFramework's GUI """
        console.log('[bold white]\[ [bold green]+ [bold white]] Compiling EternalHush project')
        subprocess.run(f"{MAVEN_BIN}\\mvn.cmd compile", shell=True)
        console.log('Packaging EternalHush project')
        subprocess.run(f"{MAVEN_BIN}\\mvn.cmd package", shell=True)

        console.log('[bold white]\[ [bold green]+ [bold white]]Copying jar and libs')

        subprocess.run("COPY target\\EternalHushFramework* build\\ /Y", shell=True)
        subprocess.run("XCOPY /e /i /Y target\\libs build\\libs", shell=True)

    def remove_tools(self):
        """ Removes the installed tools """
        console.log('[bold white]\[ [bold green]+ [bold white]] Removing downloads')
        subprocess.run('DEL *.zip', shell=True)
        subprocess.run('RMDIR /s /q maven', shell=True)
        subprocess.run('RMDIR /s /q cmake', shell=True)

    def remove_build(self):
        """ Removes the build files """
        console.log('[bold white]\[ [bold green]+ [bold white]] Removing build files')
        subprocess.run('RMDIR /s /q target', shell=True)
        subprocess.run('RMDIR /s /q build', shell=True)

    def download_tools(self):
        """ Downloades externel tools """
        console.log(f'[bold white]\[ [bold green]+ [bold white]] Downloading Maven [bold green]{MAVEN_VERSION}')
        dload.save_unzip(f"https://dlcdn.apache.org/maven/maven-3/{MAVEN_VERSION}/binaries/apache-maven-{MAVEN_VERSION}-bin.zip", "maven")

        console.log(f'[bold white]\[ [bold green]+ [bold white]] Downloading Cmake [bold green]{CMAKE_VERSION}')
        dload.save_unzip(f"https://github.com/Kitware/CMake/releases/download/v{CMAKE_VERSION}/cmake-{CMAKE_VERSION}-windows-x86_64.zip", "cmake")

    def clean(self):
        """ Cleans up the project directory """
        console.log('[bold white]\[ [bold green]+ [bold white]] Cleaning project directory')

        subprocess.run(f"{MAVEN_BIN}\\mvn.cmd clean", shell=True)
        subprocess.run("RMDIR /s /q EternalHushCore\\build", shell=True)

# Init cli
cli = typer.Typer()

# Init EternalHushFrameworkBuilder
eternal_hush = EternalHushFrameworkBuilder()

@cli.command()
def build():
    """ Build the framework """
    eternal_hush.download_tools()
    eternal_hush.build()

@cli.command()
def build_modules():
    """ Build modules """
    eternal_hush.download_tools()
    eternal_hush.build_modules()

@cli.command()
def build_core():
    """ Build the core"""
    eternal_hush.download_tools()
    eternal_hush._build_core()

@cli.command()
def build_gui():
    """ Build the GUI """
    eternal_hush.download_tools()
    eternal_hush.build_gui()

@cli.command()
def rebuild():
    """ Rebuild the framework """
    eternal_hush.download_tools()
    eternal_hush.clean()
    eternal_hush.build()

@cli.command()
def clean():
    """ Clean up the project directory """
    eternal_hush.download_tools()
    eternal_hush.clean()

@cli.command()
def force_clean():
    """ Remove all build files """
    eternal_hush.download_tools()
    eternal_hush.clean()
    eternal_hush.remove_tools()
    eternal_hush.remove_build()

run = cli

if __name__ == "__main__":
    eternal_hush.banner()
    run()
