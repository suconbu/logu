# logu

The single header logging utility for C++.

Example:

```cpp
#include "logu/logu.h"

int main()
{
    // Stream style

    LOGU_DEBUG << "debug message";
    LOGU_INFO << "info message";
    LOGU_WARN << "warning message";
    LOGU_ERROR << "error message";
    LOGU << "message with no severity specified";

    // Print variable name and value

    int32_t age = 3;
    std::string name = "taro";
    LOGU_DEBUG << LOGU_VARS(name, age);

    return 0;
}
```

Result:

```
2022-04-04 00:10:23.000 | DEBUG | 6793 | example.cpp@7 | debug message
2022-04-04 00:10:23.000 | INFO  | 6793 | example.cpp@8 | info message
2022-04-04 00:10:23.000 | WARN  | 6793 | example.cpp@9 | warning message
2022-04-04 00:10:23.000 | ERROR | 6793 | example.cpp@10 | error message
2022-04-04 00:10:23.000 | ----- | 6793 | example.cpp@11 | message with no severity specified
2022-04-04 00:10:23.000 | DEBUG | 6793 | example.cpp@17 | (name, age) -> (taro, 3)
```

Log format (default):
```
{date-time} | {severity} | {thiread-id} | {file-name}@{line-no} | {message}
```

Please see [example.cpp](/example/example.cpp) for example.

# Setup

1. Place `logu/logu.hpp` in include path of your project.
2. Add `#include "logu/logu.hpp"` into your source code.

# Lisence

MIT License.
