import os
import subprocess
import platform

from SetupPython import PythonConfiguration as PythonRequirements

PythonRequirements.Validate()

from SetupCMake import CMakeConfiguration as CMakeRequirements
from SetupVulkan import VulkanConfiguration as VulkanRequirements
os.chdir('./../')

cmakeInstalled = CMakeRequirements.Validate()
VulkanRequirements.Validate()

print("\nUpdating submodules...")
subprocess.call(["git", "submodule", "update", "--init", "--recursive"])

if (cmakeInstalled):
    print("\nRunning Cmake")
    
    system = platform.system()
    build_dir = "build"
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)
    os.chdir(build_dir)

    if system == "Windows":
        subprocess.call(["cmake", "-G", "Visual Studio 17 2022", ".."])
    elif system == "Linux":
        subprocess.call(["cmake", "-G", "Unix Makefiles", ".."])
    elif system == "Darwin":
        subprocess.call(["cmake", "-G", "Xcode", ".."])
    else:
        print(f"Unsupported platform: {system}")

    subprocess.call(["cmake", "--install", "build"])

    print("\nSetup completed!")
else:
    print("Echo requires CMake to generate project files.")