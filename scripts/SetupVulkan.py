import os 
import sys 
import subprocess
from pathlib import Path

import Utils

from io import BytesIO
from urllib.request import urlopen

class VulkanConfiguration:
    requiredVulkanVersion = "1.3."
    installVulkanVersion = "1.3.290.0"
    vulkanDirectory = "./Echo/vendor/Vulkan"

    @classmethod
    def Validate(cls):
        if (not cls.CheckVulkanSDK()):
            print("Vulkan SDK not install correctly!")

            if (not cls.CheckVulkanSDKDebugLibs()):
                print("\nNo Vulkan SDK debug libs found. Install Vulkan SDK with debug libs!")
                print("Debug Configuration disabled!")

    @classmethod
    def CheckVulkanSDK(cls):
        vulkanSDK = os.environ.get("VULKAN_SDK")
        if (vulkanSDK is None):
            print("\nYou don't have Vulkan SDK installed!")
            cls.__InstallVulkanSDK()
            return False
        else: 
            print("\nVulkan SDK found: " + vulkanSDK)
        
        if(cls.requiredVulkanVersion not in vulkanSDK):
            print("\nVulkan SDK version is not correct!")
            cls.__InstallVulkanSDK()
            return False
        
        print(f"Correct Vulkan SDK located at {vulkanSDK}")
        return True
    
    @classmethod
    def __InstallVulkanSDK(cls):
        permissionGranted = False
        while not permissionGranted:
            reply = str(input("Would you like to install Vulkan SDK {0:s}? [Y/N]: ".format(cls.installVulkanVersion))).lower().strip()[:1]
            if reply == 'n':
                return
            permissionGranted = (reply == 'y')

        vulkanInstallURL = f"https://sdk.lunarg.com/sdk/download/{cls.installVulkanVersion}/windows/VulkanSDK-{cls.installVulkanVersion}-Installer.exe"
        vulkanPath = f"{cls.vulkanDirectory}/VulkanSDK-{cls.installVulkanVersion}-Installer.exe"
        print("Downloading {0:s} to {1:s}".format(vulkanInstallURL, vulkanPath))
        Utils.DownloadFile(vulkanInstallURL, vulkanPath)
        print("Running Vulkan SDK installer...")
        os.startfile(os.path.abspath(vulkanPath))
        print("Restart the script after installing the Vulkan SDK.")
        quit()

    @classmethod
    def CheckVulkanSDKDebugLibs(cls):
        vulkanSDK = os.environ.get("VULKAN_SDK")
        shadercdLib = Path(f"{vulkanSDK}/Lib/shaderc_sharedd.lib")
        
        return shadercdLib.exists()
    
if __name__ == "__main__":
    VulkanConfiguration.Validate()