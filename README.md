# Echo Game Engine

Welcome to **Echo**, a basic game engine built with a focus on simplicity and flexibility. Echo is currently in an early development stage with ongoing code refactoring, but as progress continues, this README will help you get started with building, running, and contributing to the engine.

---

## Table of Contents

1. [Overview](#overview)
2. [Requirements](#requirements)
3. [Building Echo](#building-echo)
4. [Project Structure](#project-structure)
5. [TODO List](#todo-list)
6. [License](#license)

---

### Overview

Echo is a work-in-progress game engine designed with modularity and efficiency in mind. The aim is to eventually support a wide array of game development features, starting from foundational graphics rendering up to complex systems like physics and audio.

---

### Requirements

To use or contribute to Echo, you need the following:

- **Premake5** (latest version recommended)
- **Vulkan SDK** (install during setup if needed)
- **C++ Compiler** (compatible with C++latest or higher)

---

### Building Echo

Follow these steps to build the Echo engine from scratch:

1. **Initial Setup**  
   Navigate to the `scripts/` folder and run `Setup.bat`. This script will:

   - Install Vulkan SDK (if not already installed).
   - Configure all required dependencies.
   - Run `premake5 vs2022` to generate Visual Studio solution files.

2. **Build the Project**  
   Open the generated project files (`Echo.sln`) in Visual Studio, set the solution configuration to **Release** or **Debug**, and build the project.

3. **Run the Engine**  
   Once the build is complete, you can execute the engine from your IDE or directly from the output folder.

> **Note:** Additional setup might be necessary as the project evolves.

---

### Project Structure

The Echo game engine is organized as follows:

- **src/** – Contains the core source code for the engine.
- **scripts/** – Contains setup and build scripts for configuring the development environment.
- **vendor/** – Houses external libraries, such as GLFW and ImGui.
- **build/** – Generated files and output of the build process (created after running the setup script).

---

### TODO List

- [ ] Complete code refactoring
- [ ] Set up basic rendering pipeline
- [ ] Implement initial Vulkan setup and testing
- [ ] Add input handling (keyboard/mouse)
- [ ] Integrate ImGui for debug UI
- [ ] Build basic entity-component-system (ECS) architecture
- [ ] Set up basic physics module
- [ ] Add initial audio system support
- [ ] Documentation for core systems and modules

---

### License

Echo is licensed under the MIT License. See the `LICENSE` file for more details.
