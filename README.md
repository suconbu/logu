# clog

This is logging utility for C++.

# Setup

1. Place `include/clog.hpp` file in your project.
2. Add `#include "include/clog.hpp"` into your source code.

# Example to use

Please see [example.cpp](/example/example.cpp).

Output example:

```
[2022-02-06 18:44:09.206] [debug] [example.cpp] [example::severities_example@9] debug debug debug
[2022-02-06 18:44:09.218] [info ] [example.cpp] [example::severities_example@10] info info info
[2022-02-06 18:44:09.224] [warn ] [example.cpp] [example::severities_example@11] warning warning warning
[2022-02-06 18:44:09.227] [error] [example.cpp] [example::severities_example@12] error error error
[2022-02-06 18:44:09.230] [-----] [example.cpp] [example::severities_example@14] none none none
[2022-02-06 18:44:09.230] [-----] [example.cpp] [example::datatypes_example@19] 1
[2022-02-06 18:44:09.231] [-----] [example.cpp] [example::datatypes_example@20] 65535
[2022-02-06 18:44:09.232] [-----] [example.cpp] [example::datatypes_example@21] 3.14159
...
```

# Option definitions

You can change the behavior by enabling some definitions before `#include "clog/clog.hpp"`.

Example:

```cpp
#define CLOG_OPTION_MIN_SEVERITY_WARN
#define CLOG_OPTION_OUTPUT_STREAM std::cerr
#include "clog/clog.hpp"
...
```

Definitions:

| Name                           | Description                                   | Default         |
| ------------------------------ | --------------------------------------------- | --------------- |
| CLOG_OPTION_MIN_SEVERITY_INFO  | Limits the output severity to INFO/WARN/ERROR | (No definition) |
| CLOG_OPTION_MIN_SEVERITY_WARN  | Limits the output severity to WARN/ERROR      | (No definition) |
| CLOG_OPTION_MIN_SEVERITY_ERROR | Limits the output severity to ERROR           | (No definition) |
| CLOG_OPTION_DISABLE_LOGGING    | Turn off the logging                          | (No definition) |
| CLOG_OPTION_OUTPUT_STREAM      | Specifies the output stream                   | std::cout       |

# Lisence

MIT License.
