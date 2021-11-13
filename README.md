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



* Info about FST:

    * [fstminer.c](https://github.com/gtkwave/gtkwave/blob/master/gtkwave3-gtk3/src/helpers/fstminer.c)
    * [fst.c](https://github.com/gtkwave/gtkwave/blob/master/gtkwave3-gtk3/src/fst.c)

# FST questions

* No support for vector with LSB that is not 0?

* What does "fac" stand for?

    facility?

    `num_facs = fstReaderGetVarCount(ctx);`

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


