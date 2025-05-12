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