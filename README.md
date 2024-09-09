# CBReader
A C++ library which allows reading data from various SCP:CB formats.

Current Support:
| Mesh    | Reader | Writing |
|---------|--------|---------|
| RMesh   | ✔     | ❌      |
| B3D     | ❌    | ❌      |
| RMesh2  | ❌    | ❌      |
| DirectX | ❌    | ❌      |

## Building
### Requirements
- CMake. While not strictly required, as compiling can be done without CMake, it just makes it easier to manage the project and include directories.

### Steps
This project is designed to be added as submodule. When adding please keep in mind that this library has another submodule as dependency, which is [BufferStream](https://github.com/craftablescience/BufferStream).

1. Add the project as a submodule: 
    `git submodule add https://github.com/AWildErin/CBReader.git thirdparty/cbreader`
2. Update submodules to correctly fetch dependencies: 
    `git submodule update --init --recursive`
3. Add the CBReader subdirectory in cmake: 
    `add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cbreader")`
4. Link the library (include directories will be handled automatically): 
    `target_link_libraries(${PROJECT_NAME} PRIVATE CBReader)`

## Usage
Usage can be found on the docs (SOON)
