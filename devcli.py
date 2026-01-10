import cmd
import os
import time
import typer
import dload
import shutil
import subprocess
import re

from rich import print
from rich.console import Console

JAR_COMPONENTS = ['core', 'launcher', 'logviewer', 'clingyspider', 'RemoteFileViewer']

# Constants
MAVEN_VERSION   =   "3.9.12"
MAVEN_DIR       =   f"maven\\apache-maven-{MAVEN_VERSION}"
MAVEN_BIN       =   f"{MAVEN_DIR}\\bin"

CMAKE_VERSION   =   "4.1.1"
CMAKE_DIR       =   f"cmake\\cmake-{CMAKE_VERSION}-windows-x86_64"
CMAKE_BIN       =   f"{CMAKE_DIR}\\bin"

BANNER = """[bold green]
##############################################################
#             ETERNALHUSH FRAMEWORK DEVELOPER CLI             #
#                    VERSION 1.0.0.0-ALPHA                   #
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
        subprocess.run(f"{CMAKE_BIN}\\cmake.exe -G \"Visual Studio 16 2019\" -S EternalHushCore -B EternalHushCore\\build", shell=True)

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
            metadesc_file   =   os.path.join(f, "metainfo.xml")
            build_files     =   os.path.join(f, "bin_files")
            bin_out32       =   os.path.join(build_files, "bin32\\Release")
            bin_out64       =   os.path.join(build_files, "bin64\\Release")
            script_files    =   os.path.join(f, "scripts")
            additional_files=   os.path.join(f, "additional")
            prebuiltfiles_files=os.path.join(f, "files")
            script_data=os.path.join(f, "data")
            custom_handlers =   os.path.join(f, "handlers")

            subprocess.run(f'{CMAKE_BIN}\\cmake.exe -G \"Visual Studio 16 2019\" -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=../../bin32 -A Win32 -S {build_files} -B {build_files}\\output32', shell=True)
            subprocess.run(f'{CMAKE_BIN}\\cmake.exe -G \"Visual Studio 16 2019\" -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=../../bin64 -A x64 -S {build_files} -B {build_files}\\output64', shell=True)
            subprocess.run(f'{CMAKE_BIN}\\cmake.exe --build {build_files}\\output32 --config Release', shell=True)
            subprocess.run(f'{CMAKE_BIN}\\cmake.exe --build {build_files}\\output64 --config Release', shell=True)

            subprocess.run(f'XCOPY /e /i /Y {script_files} build\\modules\\{directory}\\scripts', shell=True)
            subprocess.run(f'XCOPY /e /i /Y {script_data} build\\modules\\{directory}\\data', shell=True)
            subprocess.run(f'XCOPY /e /i /Y {additional_files} build\\base\\pylibs\\additional', shell=True)
            subprocess.run(f'XCOPY /e /i /Y {prebuiltfiles_files} build\\modules\\{directory}\\files', shell=True)
            subprocess.run(f'XCOPY /e /i /Y {custom_handlers} build\\base\\pylibs\\handlers', shell=True)
            subprocess.run(f'COPY {desc_file} build\\modules\\{directory}\\ /Y', shell=True)
            subprocess.run(f'COPY {metadesc_file} build\\modules\\{directory}\\ /Y', shell=True)
            subprocess.run(f'MKDIR build\\modules\\{directory}\\files', shell=True)

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
        subprocess.run("MKDIR build\\plugins", shell=True)
        for comp in JAR_COMPONENTS:
            subprocess.run("XCOPY /e /i /Y {}\\target\\libs build\\libs".format(comp), shell=True)
            if comp == 'launcher':
                subprocess.run("COPY {}\\target\\*.jar build\\ /Y".format(comp), shell=True)
            if comp != 'launcher' and comp != 'core':
                subprocess.run("COPY {}\\target\\*.jar build\\plugins\\ /Y".format(comp), shell=True)
        
        

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

    def sync_hotplug_scripts(self):
        """ Синхронизирует Python-скрипты из build в modules (наоборот) """
        console.log('[bold white]\[ [bold green]+ [bold white]] Синхронизация hotplug скриптов (наоборот)')

        for directory in os.listdir("modules"):
            f = os.path.join("modules", directory)
            script_files = os.path.join(f, "scripts")
            script_data = os.path.join(f, "data")
            root_script_dir = os.path.join("build", "modules", directory)
            build_script_dir = os.path.join(root_script_dir, "scripts")
            build_script_data_dir = os.path.join(root_script_dir, "data")

            # Создать директорию scripts в modules, если не существует
            subprocess.run(f'MKDIR "{script_files}"', shell=True)
            
            # Копировать скрипты из build обратно в modules
            subprocess.run(f'XCOPY /e /i /Y "{build_script_dir}" "{script_files}"', shell=True)
            subprocess.run(f'XCOPY /e /i /Y "{build_script_data_dir}" "{script_data}"', shell=True)

            subprocess.run(f'COPY "{root_script_dir}"\\module.xml "{f}"\\ /Y', shell=True)
            subprocess.run(f'COPY "{root_script_dir}"\\metainfo.xml "{f}"\\ /Y', shell=True)

        console.log('[bold white]\[ [bold green]+ [bold white]] Hotplug скрипты успешно скопированы обратно в modules')

eternalhush = EternalHushFrameworkBuilder()

class DevCli(cmd.Cmd):
    current_config = None
    prompt = "devcli> "

    def _get_active_config_name(self):
        """
        Возвращает имя активной конфигурации из prompt, если она задана в виде 'devcli[имя]> ', иначе None.
        """
        if hasattr(self, 'prompt') and self.prompt.startswith('devcli[') and self.prompt.endswith(']> '):
            return self.prompt[7:-3].strip()
        return None

    def do_build(self, line):
        """Собрать фреймворк"""
        eternalhush.download_tools()
        eternalhush.build()

    def do_build_modules(self, line):
        """Собрать модули"""
        eternalhush.download_tools()
        eternalhush.build_modules()

    def do_build_core(self, line):
        """Собрать core"""
        eternalhush.download_tools()
        eternalhush._build_core()

    def do_build_gui(self, line):
        """Собрать GUI"""
        eternalhush.download_tools()
        eternalhush.build_gui()
    
    def do_run_build(self, line):
        try:
            subprocess.run('cd build && "java" -jar Start.jar', shell=True)
        except KeyboardInterrupt:
            return

    def do_rebuild(self, line):
        """Пересобрать фреймворк"""
        eternalhush.download_tools()
        eternalhush.clean()
        eternalhush.build()

    def do_clean(self, line):
        """Очистить директорию проекта"""
        eternalhush.download_tools()
        eternalhush.clean()

    def do_force_clean(self, line):
        """Удалить все build-файлы"""
        eternalhush.download_tools()
        eternalhush.clean()
        eternalhush.remove_tools()
        eternalhush.remove_build()

    def do_sync_hotplug_scripts(self, line):
        """Синхронизировать hotplug-скрипты из modules в build"""
        eternalhush.sync_hotplug_scripts()
    
    def do_exit(self, line):
        return True

    def do_get_build_version(self, line):
        """Показать значение DefinedVersion из активной xml-конфигурации"""
        if hasattr(self, 'current_config') and self.current_config is not None:
            root = self.current_config
            elem = root.find('DefinedVersion')
            if elem is not None and elem.text:
                print(f"DefinedVersion: {elem.text}")
            else:
                print("Элемент DefinedVersion не найден в активной xml-конфигурации.")
        else:
            print("Нет активной xml-конфигурации. Используйте команду use_build_info.")

    def do_get_build_codename(self, line):
        """Показать значение DefinedCodename из активной xml-конфигурации"""
        if hasattr(self, 'current_config') and self.current_config is not None:
            root = self.current_config
            elem = root.find('DefinedCodename')
            if elem is not None and elem.text:
                print(f"DefinedCodename: {elem.text}")
            else:
                print("Элемент DefinedCodename не найден в активной xml-конфигурации.")
        else:
            print("Нет активной xml-конфигурации. Используйте команду use_build_info.")

    def do_get_build_uid(self, line):
        """Показать значение DefinedUid из активной xml-конфигурации"""
        if hasattr(self, 'current_config') and self.current_config is not None:
            root = self.current_config
            elem = root.find('DefinedUid')
            if elem is not None and elem.text:
                print(f"DefinedUid: {elem.text}")
            else:
                print("Элемент DefinedUid не найден в активной xml-конфигурации.")
        else:
            print("Нет активной xml-конфигурации. Используйте команду use_build_info.")

    def do_set_build_version(self, line):
        """Установить значение DefinedVersion только в активной xml-конфигурации. Использование: set_defined_version <новая_версия>"""
        new_version = line.strip()
        if not new_version:
            print("Пожалуйста, укажите новую версию.")
            return
        if hasattr(self, 'current_config') and self.current_config is not None:
            import xml.etree.ElementTree as ET
            root = self.current_config
            elem = root.find('DefinedVersion')
            if elem is None:
                elem = ET.SubElement(root, 'DefinedVersion')
            elem.text = new_version
            config_name = self._get_active_config_name()
            if config_name:
                file_path = f"xml_configs/{config_name}.xml"
                try:
                    tree = ET.ElementTree(root)
                    tree.write(file_path, encoding="utf-8", xml_declaration=True)
                    print(f"DefinedVersion обновлён в xml-конфигурации: {config_name}")
                except Exception as e:
                    print(f"Ошибка при сохранении xml-конфигурации: {e}")
        else:
            print("Нет активной xml-конфигурации. Используйте команду use_build_info.")

    def do_set_build_codename(self, line):
        """Установить значение DefinedCodename только в активной xml-конфигурации. Использование: set_defined_codename <новый_codename>"""
        new_codename = line.strip()
        if not new_codename:
            print("Пожалуйста, укажите новый codename.")
            return
        if hasattr(self, 'current_config') and self.current_config is not None:
            import xml.etree.ElementTree as ET
            root = self.current_config
            elem = root.find('DefinedCodename')
            if elem is None:
                elem = ET.SubElement(root, 'DefinedCodename')
            elem.text = new_codename
            config_name = self._get_active_config_name()
            if config_name:
                file_path = f"xml_configs/{config_name}.xml"
                try:
                    tree = ET.ElementTree(root)
                    tree.write(file_path, encoding="utf-8", xml_declaration=True)
                    print(f"DefinedCodename обновлён в xml-конфигурации: {config_name}")
                except Exception as e:
                    print(f"Ошибка при сохранении xml-конфигурации: {e}")
        else:
            print("Нет активной xml-конфигурации. Используйте команду use_build_info.")

    def do_set_build_uid(self, line):
        """Установить значение DefinedUid только в активной xml-конфигурации. Использование: set_defined_uid <новый_uid>"""
        new_uid = line.strip()
        if not new_uid:
            print("Пожалуйста, укажите новый uid.")
            return
        if hasattr(self, 'current_config') and self.current_config is not None:
            import xml.etree.ElementTree as ET
            root = self.current_config
            elem = root.find('DefinedUid')
            if elem is None:
                elem = ET.SubElement(root, 'DefinedUid')
            elem.text = new_uid
            config_name = self._get_active_config_name()
            if config_name:
                file_path = f"xml_configs/{config_name}.xml"
                try:
                    tree = ET.ElementTree(root)
                    tree.write(file_path, encoding="utf-8", xml_declaration=True)
                    print(f"DefinedUid обновлён в xml-конфигурации: {config_name}")
                except Exception as e:
                    print(f"Ошибка при сохранении xml-конфигурации: {e}")
        else:
            print("Нет активной xml-конфигурации. Используйте команду use_build_info.")

    def do_apply_build_conf(self, line):
        """
        Применить значения DefinedVersion, DefinedCodename и DefinedUid из активной xml-конфигурации в ExtGlobalConfig.java.
        Использование: apply_build_info
        """
        import re
        import os
        path = r'core\src\main\java\eternalhush\main\ExtGlobalConfig.java'
        if not (hasattr(self, 'current_config') and self.current_config is not None):
            print("Нет активной xml-конфигурации. Используйте команду use_build_info.")
            return
        root = self.current_config
        version_elem = root.find('DefinedVersion')
        codename_elem = root.find('DefinedCodename')
        uid_elem = root.find('DefinedUid')
        new_version = version_elem.text if version_elem is not None else None
        new_codename = codename_elem.text if codename_elem is not None else None
        new_uid = uid_elem.text if uid_elem is not None else None

        if not (new_version or new_codename or new_uid):
            print("В xml-конфигурации не найдено ни одной переменной для применения.")
            return

        try:
            with open(path, encoding='utf-8') as f:
                content = f.read()
            changed = False
            if new_version is not None and re.search(r'DefinedVersion\s*=\s*["\']([^"\']+)["\']', content):
                content = re.sub(
                    r'(DefinedVersion\s*=\s*["\'])([^"\']+)(["\'])',
                    r'\g<1>' + new_version + r'\3',
                    content
                )
                print(f"DefinedVersion будет установлен на: {new_version}")
                changed = True
            if new_codename is not None and re.search(r'DefinedCodename\s*=\s*["\']([^"\']+)["\']', content):
                content = re.sub(
                    r'(DefinedCodename\s*=\s*["\'])([^"\']+)(["\'])',
                    r'\g<1>' + new_codename + r'\3',
                    content
                )
                print(f"DefinedCodename будет установлен на: {new_codename}")
                changed = True
            if new_uid is not None and re.search(r'DefinedUid\s*=\s*["\']([^"\']+)["\']', content):
                content = re.sub(
                    r'(DefinedUid\s*=\s*["\'])([^"\']+)(["\'])',
                    r'\g<1>' + new_uid + r'\3',
                    content
                )
                print(f"DefinedUid будет установлен на: {new_uid}")
                changed = True
            if changed:
                with open(path, 'w', encoding='utf-8') as f:
                    f.write(content)
                print("Изменения успешно применены к ExtGlobalConfig.java")
            else:
                print("Ни одна из переменных не найдена в ExtGlobalConfig.java или значения не заданы.")
        except FileNotFoundError:
            print(f"Файл {path} не найден.")

    def do_uidgen(self, line):
        import random
        import string
        print(''.join(random.SystemRandom().choice(string.ascii_uppercase + string.digits) for _ in range(16)))

    def do_create_build_conf(self, line):
        """Создать новый XML-файл в папке xml_configs. Использование: create_build_info <name>"""
        import os
        import xml.etree.ElementTree as ET

        name = line.strip()
        if not name:
            print("Пожалуйста, укажите имя файла.")
            return

        dir_path = "xml_configs"
        if not os.path.exists(dir_path):
            os.makedirs(dir_path)

        file_path = os.path.join(dir_path, f"{name}.xml")

        if os.path.exists(file_path):
            print(f"Файл {file_path} уже существует.")
            return

        root = ET.Element("BuildInfo")
        tree = ET.ElementTree(root)
        try:
            tree.write(file_path, encoding="utf-8", xml_declaration=True)
            print(f"Файл {file_path} успешно создан.")
        except Exception as e:
            print(f"Ошибка при создании файла: {e}")

    def do_use_build_conf(self, line):
        """Использовать конфигурацию сборки из xml_configs. Использование: use_build_info <name>"""
        import os
        import xml.etree.ElementTree as ET

        name = line.strip()
        if not name:
            print("Пожалуйста, укажите имя конфигурации.")
            return

        dir_path = "xml_configs"
        file_path = os.path.join(dir_path, f"{name}.xml")

        if not os.path.exists(file_path):
            print(f"Файл {file_path} не найден.")
            return

        try:
            tree = ET.parse(file_path)
            root = tree.getroot()
            self.current_config = root
            self.prompt = f"devcli[{name}]> "
            print(f"Конфигурация {name} успешно активирована.")
        except Exception as e:
            print(f"Ошибка при загрузке конфигурации: {e}")

    def do_list_build_conf(self, line):
        """Показать список доступных конфигураций build info (xml_configs)"""
        import os

        dir_path = "xml_configs"
        if not os.path.exists(dir_path):
            print("Папка xml_configs не существует.")
            return

        files = [f for f in os.listdir(dir_path) if f.endswith('.xml')]
        if not files:
            print("Нет доступных конфигураций.")
            return

        print("Доступные конфигурации:")
        for f in files:
            print(" -", f[:-4])

if __name__ == '__main__':
    eternalhush.banner()
    DevCli().cmdloop()