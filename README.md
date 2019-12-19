# PDF Cleaner tool

Author: Alvin Ahmadov (alvin.dev.ahmadov@yandex.com)

## Features

  Removes all found embedded uris that match specific 
  search term from pdf file.
  
### Requires

* libpodofo-dev >=0.9.5
* boost >=1.65
   
# Building

```bash
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build .
```