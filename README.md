# gdbwave

GDB server to debug CPU simulation waveform traces

General plan:

* extract PC from FST file

    * valid PC when `clk && lastStagePcValid`

* Create list with all PC value and timestamp
* starting from a given timestamp, return the PC value
    * call addr2line to convert PC value to line nr
* Create memory object with initial value
* extract all memory read and write operations with timestamp

# Usage

**Start gdbwave**

```sh
cd ./src
make run
```

This does the following:

* Read in an FST file.
* Extract all the instruction addresses with associated simulation timestamp.
* Start a GDB server.

**Start gdb**

```sh
cd ./test_data/sw_semihosting
make gdb_only
```

# FST

* Info about FST:

    * [fstminer.c](https://github.com/gtkwave/gtkwave/blob/master/gtkwave3-gtk3/src/helpers/fstminer.c)
    * [fst.c](https://github.com/gtkwave/gtkwave/blob/master/gtkwave3-gtk3/src/fst.c)

# FST questions

* No support for vector with LSB that is not 0?

* What does "fac" stand for?

    facility?

    `num_facs = fstReaderGetVarCount(ctx);`

* what is `doubleEndianMatchState`? 

* what is `valudChangeSectionCount`? 

    How do I use it?

* what is a dumpActivityChange?

    Does it have to do with times where values are not recorded?

* `fstReaderIterateHier` goes through the whole signal hierarchy?

    Why is GTKWave going through the whole hierarchy for each signal?

* What is an alias?

    Signals in a design that have identical values. Signals that are aliases have the same
    fstHandle.

    For example, for the following design, `io_occupancy`, `full`, `when_Stream_l1017`, and 
    `IBusSimplePlugin_rspJoin_rspBuffer_c_io_occupancy` are all aliases.

    ```verilog
module StreamFifoLowLatency (
  ...
  output     [0:0]    io_occupancy,
  ...
);

...
    assign ptrMatch = 1'b1;
    assign io_occupancy = (risingOccupancy && ptrMatch);
...
    assign full = (ptrMatch && risingOccupancy);
    assign empty = (ptrMatch && (! risingOccupancy));
    assign when_Stream_l1017 = (! empty);
endmodule

...

  StreamFifoLowLatency IBusSimplePlugin_rspJoin_rspBuffer_c (
    ...
    .io_occupancy             (IBusSimplePlugin_rspJoin_rspBuffer_c_io_occupancy          ), //o
    ...
  );

    ```

* GTKWave code scans for '[' in the signal names, but signal names only have this  
  when the signal is an array of values. IOW: the signal name does not contain an individual bit selector.


* `fstReaderSetFacProcessMask`, `fstReaderClrFacProcessMask`

    What do they do?

    `fstReaderClrFacProcessMask(fstCtx, fstHandle);`  : remove signal from those that are returned by fstReaderIterBlocks2?

    ```
void            fstReaderSetFacProcessMaskAll(void *ctx);
void            fstReaderClrFacProcessMaskAll(void *ctx);

void            fstReaderSetFacProcessMask(void *ctx, fstHandle facidx);
void            fstReaderClrFacProcessMask(void *ctx, fstHandle facidx);
int             fstReaderGetFacProcessMask(void *ctx, fstHandle facidx);
    ```



* difference between `value_change_callback` and `value_change_callback_varlen` ?

    I never see a callback with a len that is not 0?


* Probing VexRiscv PC:

    > Here is how the verilator testbench track the commited state of the CPU :
    > https://github.com/SpinalHDL/VexRiscv/blob/master/src/test/cpp/regression/main.cpp#L1778
    > https://github.com/SpinalHDL/VexRiscv/blob/master/src/test/cpp/regression/main.cpp#L1793
    > https://github.com/SpinalHDL/VexRiscv/blob/master/src/test/cpp/regression/main.cpp#L1803
    > lastStagexxxxx is the way ^^
    > It was made for that purpose

* Hazard3 OpenOCD remote_bitbang server

    https://github.com/Wren6991/Hazard3/blob/c1f17b0b23d7e1a52241663bfb53568c89440f2d/test/sim/openocd/tb.cpp#L91g

* GDB packets: https://sourceware.org/gdb/current/onlinedocs/gdb/Packets.html#Packets

* RISC-V GDB register description: 

    * https://github.com/bminor/binutils-gdb/blob/master/gdb/features/riscv/32bit-cpu.xml
    * (derived) https://github.com/bminor/binutils-gdb/blob/master/gdb/features/riscv/32bit-cpu.c


* gdbstub

```
    demo.c:_start
        demo.c:dbg_start()
            arch_x86/gdbstub_sys: dbg_start(void)
                - setup interrupt handlers to dbg_int_handlers(1 and 3)
                - issue interrupt to start debugging
            arch_x86/gdbstub_int.nasm:dbg_int_handlers:
                -> dbg_int_handler_common:
                    - save registers
                    - call dbg_int_handler()

                    arch_x86/gdbstub_sys.c: void dbg_int_handler(struct dbg_interrupt_state *istate)
                        arch_x86/gdbstub_sys.c: void dbg_interrupt(struct dbg_interrupt_state *istate)
                            - memset dbg_state.registers
                            - set correct signum
                            - copy istate to dbg_state.registers
                            - dbg_main(dbg_state)
                            gdbstub.cc: int dbg_main(struct dbg_state *state)
                                dbg_send_signal_packet(pkt_buf.., state->signum)
                                    This sends a "S AA" response (See E.3 Stop Reply Packets), where AA is the
                                    signum.
                                wihle(1){
                                    receive packet from client
                                    process packet
                                }
                            - restore istate from dbg_state.registers


```

* [`radare2`](https://github.com/radareorg/radare2) reverse engineering tool

     * See also: https://rada.re/n/radare2.html

**TODO/Limitations**

* Improved error messages for config file
* Fix 'next' issue 
* Link with GTKWave
* Support superscalar CPUs
* Load ELF files directly
* Support complex memory maps
* Decode register file and memory reads
* Support for RISC-V instruction tracing decompression
* 'R' should work.

