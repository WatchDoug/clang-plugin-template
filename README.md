# clang-plugin-template
The template plugin implements a struct field name filter. 

It prints out all struct field names that contain one of the given substrings.

Put the folder in clang\_dir/examples and add subdirectory in CMakeLists.txt in the examples directory.

Usage:
```bash
clang -fplugin=path/to/your/plugin.so <source file>
```
