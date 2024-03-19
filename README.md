# MAVCam

## Basic architecture
```mermaid
---
title: architecture
---
flowchart LR
    subgraph Client
    C1[mavsdk]
    end
    
    subgraph Middleware
    M1[mavsdk]
    M2[local]
    M3[rpc]
    M1 <--> M2
    M1 <--> M3
    end

    subgraph Backend
    B1[rpc]
    end

    C1 <-->|mavlink| M1
    M3 <--> |grpc| B1
```

### Format code request

First need install clang-format-15 for format code, in **ubuntu** you can use the following command to install clang-format-15

```shell
sudo apt install clang-format-15
```

then use the tools/fix_style.sh to format the source dir

```
./tools/fix_style.sh ./src
```

