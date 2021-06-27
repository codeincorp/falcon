# Falcon
Falcon project has the goal to develope the world-fastest analytical engine that can analyze data from diverse sources. But it's not there yet. :)

# Development Envionment
- Language: C++20
- Build: CMake
- Test: GTest & CTest
- IDE: Visual Studio Code
- Platform: WSL Ubuntu 20.04
  - Theoretically, it can be built and run on any Linux-flavors but not tested except WSL Ubuntu 20.04

# Status
- It supports CSV file source, project, limit, filter, sequence and hash aggregation
- It supports constants, variables, logical expressions, comparisons, 4 arithmetic expression, conditional tenary expression, and conversion expression
- It supports bool, int, uint, float, double, and string types in expressions
- It's in very early stage and active development
- It dost not have any client drivers or user-facling query languages
  - The team has a plan to define a query language using antlr
- It can be used as a library like embedded analytical engine for now
