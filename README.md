# logu

The single header logging utility for C++.

Example:

```cpp
#include "logu/logu.h"

int main()
{
    LOGS_DEBUG << "debug message";
    LOGS_INFO << "info message";
    LOGS_WARN << "warning message";
    LOGS_ERROR << "error message";
    LOGS << "message with no severity specified";

    return 0;
}
```

Result:

```
[2022-04-04 00:10:23.000] [DEBUG] [6793] [int main()@16] debug message
[2022-04-04 00:10:23.000] [INFO ] [6793] [int main()@17] info message
[2022-04-04 00:10:23.000] [WARN ] [6793] [int main()@18] warning message
[2022-04-04 00:10:23.000] [ERROR] [6793] [int main()@19] error message
[2022-04-04 00:10:23.000] [-----] [6793] [int main()@20] message with no severity specified
```

Log format (default):
```
[{date-time}] [{severity}] [{thiread-id}] [{function-name}@{line-no}] [{message}]
```

Please see [example.cpp](/example/example.cpp) for example.

# Setup

1. Place `logu/logu.hpp` in include path of your project.
2. Add `#include "logu/logu.hpp"` into your source code.

# Lisence

MIT License.
