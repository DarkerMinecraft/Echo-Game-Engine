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

- **CMake** (latest version recommended)
- **Vulkan SDK** (install during setup if needed)
- **C++ Compiler** (compatible with C++20 or higher)

---

### Building Echo

Follow these steps to build the Echo engine from scratch.

1. **Initial Setup**  
   Navigate to the `scripts/` folder and run `Setup.bat`. This will configure initial dependencies and install Vulkan if necessary.

2. **Generate Projects**  
   After setup, run `GenerateProjects.bat` (located in the `scripts/` folder). This will configure project files for your preferred IDE.

3. **Build the Project**  
   Open the generated project files in your IDE and build the project. Ensure you have installed all required dependencies, especially Vulkan, for successful compilation.

> **Note:** This is a basic setup for development. Additional configuration might be necessary as the project evolves.

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

Echo is licensed under the MIT License. See `LICENSE` file for more details.
