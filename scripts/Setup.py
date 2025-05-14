import os
import sys
import subprocess
import argparse
import shutil

from SetupPython import PythonConfiguration as PythonRequirements

def setup_and_build():
    PythonRequirements.Validate()

    from SetupCMake import CMakeConfiguration as CMakeRequirements
    from SetupVulkan import VulkanConfiguration as VulkanRequirements

    # Parse command line arguments
    parser = argparse.ArgumentParser(description="Setup and build Echo Engine")
    parser.add_argument("--config", choices=["Debug", "Release", "Dist", "All"], 
                        help="Build configuration (Debug, Release, Dist, or All)")
    parser.add_argument("--no-build", action="store_true", 
                        help="Only configure, don't build")
    parser.add_argument("--clean", action="store_true", 
                        help="Clean build (wipes build directory)")
    args = parser.parse_args()

    # Change to project root directory
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.abspath(os.path.join(script_dir, ".."))
    os.chdir(project_root)

    # Validate requirements
    cmakeInstalled = CMakeRequirements.Validate()
    VulkanRequirements.Validate()

    print("\nUpdating submodules...")
    subprocess.call(["git", "submodule", "update", "--init", "--recursive"])

    # Interactive configuration if not provided via command line
    if args.config is None:
        print("\nSelect build configuration:")
        print("1. Debug")
        print("2. Release")
        print("3. Dist")
        print("4. All configurations")
        choice = input("Enter your choice (1-4) [1]: ").strip()
        
        if choice == "2":
            args.config = "Release"
        elif choice == "3":
            args.config = "Dist"
        elif choice == "4":
            args.config = "All"
        else:
            args.config = "Debug"  # Default to Debug

    # Ask about cleaning
    if not args.clean:
        clean_choice = input("\nPerform a clean build? (y/n) [n]: ").strip().lower()
        args.clean = clean_choice == 'y'
    
    # Ask about building
    if not args.no_build:
        build_choice = input("\nBuild after configuration? (y/n) [y]: ").strip().lower()
        args.no_build = build_choice == 'n'

    # Define build directory and clean if requested
    build_dir = os.path.join(project_root, "build")
    if args.clean and os.path.exists(build_dir):
        print("\nCleaning build directory...")
        try:
            # Safer than rmtree - first remove files, then directory
            for root, dirs, files in os.walk(build_dir, topdown=False):
                for name in files:
                    os.remove(os.path.join(root, name))
                for name in dirs:
                    os.rmdir(os.path.join(root, name))
            # Remove the directory itself
            if os.path.exists(build_dir):
                os.rmdir(build_dir)
        except Exception as e:
            print(f"Warning: Failed to clean build directory: {e}")
            print("You may need to close Visual Studio or other programs using files in the build directory.")
            input("Press Enter to continue anyway...")

    # Create the build directory if it doesn't exist
    os.makedirs(build_dir, exist_ok=True)

    # Configure the project - this creates a global solution
    print("\nConfiguring project with CMake...")
    configure_cmd = [
        "cmake", 
        "-G", "Visual Studio 17 2022", 
        "-A", "x64",
        "-B", build_dir,
        "-S", project_root
    ]
    
    configure_result = subprocess.call(configure_cmd)
    
    if configure_result != 0:
        print(f"\nCMake configuration failed with error code: {configure_result}")
        sys.exit(configure_result)
    
    # Build the project if requested
    if not args.no_build:
        configs_to_build = []
        if args.config == "All":
            configs_to_build = ["Debug", "Release", "Dist"]
        else:
            configs_to_build = [args.config]
            
        for config in configs_to_build:
            print(f"\nBuilding {config} configuration...")
            build_cmd = [
                "cmake", 
                "--build", build_dir, 
                "--config", config
            ]
            
            build_result = subprocess.call(build_cmd)
            
            if build_result == 0:
                print(f"\n{config} build completed successfully!")
            else:
                print(f"\n{config} build failed with error code: {build_result}")
                if len(configs_to_build) > 1:
                    continue_choice = input("\nContinue with other configurations? (y/n) [n]: ").strip().lower()
                    if continue_choice != 'y':
                        sys.exit(build_result)
                else:
                    sys.exit(build_result)
    
    # Print completion message with information on how to use the result
    print("\nSetup complete!")
    print(f"\nYou can open the solution at: {os.path.join(build_dir, 'EchoEngine.sln')}")
    print("Visual Studio will edit your source files directly while keeping build artifacts in the build directory.")

if __name__ == "__main__":
    setup_and_build()