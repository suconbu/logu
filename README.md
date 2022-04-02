# clog

The single header logging utility for C++.

Example:

```cpp
#include "clog/clog.h"

int main()
{
    CLOG_DEBUG << "debug message";
    CLOG_INFO << "info message";
    CLOG_WARN << "warning message";
    CLOG_ERROR << "error message";
    CLOG << "message with no severity specified";

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

1. Place `clog/clog.hpp` in include path of your project.
2. Add `#include "clog/clog.hpp"` into your source code.

# Lisence

MIT License.
