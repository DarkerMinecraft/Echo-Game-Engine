import os
import sys
import subprocess
from pathlib import Path
import urllib.request
import zipfile
import tarfile
import platform

class CMakeConfiguration:
    requiredCMakeVersion = "4.0.2"
    installCMakeVersion = "4.0.2"
    
    @classmethod
    def Validate(cls):
        if not cls.CheckCMake():
            print("CMake not installed correctly!")
            return False
        return True

    @classmethod
    def CheckCMake(cls):
        try:
            result = subprocess.run(["cmake", "--version"], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            version_output = result.stdout.decode("utf-8").split("\n")[0]
            print(f"\n{version_output}")
            if cls.requiredCMakeVersion not in version_output:
                print("\nCMake version is not correct!")
                cls.__InstallCMake()
                return False
            print("Correct CMake version located.")
            return True
        except (subprocess.CalledProcessError, FileNotFoundError):
            print("\nCMake not found!")
            cls.__InstallCMake()
            return False

    @classmethod
    def __InstallCMake(cls):
        permissionGranted = False
        while not permissionGranted:
            reply = str(input(f"Would you like to install CMake {cls.installCMakeVersion}? [Y/N]: ")).lower().strip()[:1]
            if reply == 'n':
                return
            permissionGranted = (reply == 'y')

        system = platform.system()
        if system == "Windows":
            cmake_url = f"https://github.com/Kitware/CMake/releases/download/v{cls.installCMakeVersion}/cmake-{cls.installCMakeVersion}-windows-x86_64.zip"
            install_path = os.path.expandvars("%ProgramFiles%/CMake")
        elif system == "Linux":
            cmake_url = f"https://github.com/Kitware/CMake/releases/download/v{cls.installCMakeVersion}/cmake-{cls.installCMakeVersion}-linux-x86_64.tar.gz"
            install_path = "/usr/local"
        elif system == "Darwin":
            cmake_url = f"https://github.com/Kitware/CMake/releases/download/v{cls.installCMakeVersion}/cmake-{cls.installCMakeVersion}-macos-universal.tar.gz"
            install_path = "/Applications/CMake.app/Contents"
        else:
            print(f"Automatic installation not supported for {system}. Please download CMake manually.")
            return

        cmake_path = f"{install_path}/cmake-{cls.installCMakeVersion}.{'zip' if 'Windows' in system else 'tar.gz'}"
        print(f"Downloading {cmake_url} to {cmake_path}")
        urllib.request.urlretrieve(cmake_url, cmake_path)
        print("Download complete.")

        cls.__ExtractCMake(cmake_path, install_path)
        print("Restart the script after installing CMake.")
        quit()

    @classmethod
    def __ExtractCMake(cls, archive_path, extract_path):
        print("Extracting CMake...")
        if archive_path.endswith(".zip"):
            with zipfile.ZipFile(archive_path, 'r') as zip_ref:
                zip_ref.extractall(extract_path)
        elif archive_path.endswith(".tar.gz"):
            with tarfile.open(archive_path, 'r:gz') as tar_ref:
                tar_ref.extractall(extract_path)
        print("Extraction complete.")
        os.remove(archive_path)

if __name__ == "__main__":
    CMakeConfiguration.Validate()
