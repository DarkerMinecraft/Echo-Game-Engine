import os
import subprocess

from SetupPython import PythonConfiguration as PythonRequirements

PythonRequirements.Validate()

from SetupVulkan import VulkanConfiguration as VulkanRequirements
os.chdir('./../')

VulkanRequirements.Validate()

print("\nUpdating submodules...")
subprocess.call(["git", "submodule", "update", "--init", "--recursive"])


print("\nRunning cmake...")
subprocess.call([os.path.abspath("./scripts/GenerateProjects.bat"), "nopause"])

print("\nSetup completed!")
