# GDBWave

GDB server to debug CPU simulation waveform traces.

See [GDBWave - A Post-Simulation Waveform-Based RISC-V GDB Debugging Server](https://tomverbeure.github.io/2022/02/20/GDBWave-Post-Simulation-RISCV-SW-Debugging.html).

# Usage

**Start gdbwave**

```sh
cd ./src
make run
```

This does the following:

* Read in an FST file.
* Extract all the instruction addresses with associated simulation timestamp.
* Extract all CPU writes to the CPU register file
* Extract all CPU writes to external memory
* Start a GDB server
* Makes GDB believe that it's dealing with an active or simulated CPU

