### Extended documentation at https://xy-kao.com/projects/reverse-engineering-a-serial-scoreboard-protocol-nevco-mpc-5/

### Folders
- *`atmega328_MPC_5_signal_generator`* - replicates a signal from the MPC-5
- *`attiny85_multivibrator`* - microcontroller based multivibrator (1MHz)
- *`logic_analyzer_vcd_dumps`* - dumps from the logic analyzer, used to find correct bit positions in bitstream
- *`MPC-5_decoder_schematic`* - schematic diagram for the circuitry
- *`oscilloscope_screenshots`* - screenshots from the oscilloscope
- *`teensy_decoder`* - primary microcontroller to interpret bitstream and send over serial to computer


I have decoded the [Daktronics scoreboard signal](https://xy-kao.com/projects/decoding-daktronics-omnisport-2000/) in the past for a swim meet, but this time it's a Nevco MPC-5 multipurpose controller for a basketball. This was considerably more difficult to reverse engineer for several reasons:

- The Daktronics is standard RS232, the MPC-5 runs a non-standard signal
- The MPC-5 sends all scoreboard information over a single coaxial cable
- The clock rate is significantly higher. Daktronics is at 19200 baud (19.2 KHz), the MPC-5 at ~260 KHz
- I had less time with the actual controller to play around with

## Physical layer
The raw signal from the NPC-5 on the oscilloscope looks like this:
<img class='mobile-max-width img-center' src="{% asset_path source.png %}" style='width:500px'>

So the signal is ~5Vpp ~262KHz, which is a little off from the standard 256KHz for RS232. There also isn't a stop bit. There aren't many serial protocols at this frequency that run on coaxial cable, and the ones I have used before (DMX, RS232/485, CAN bus, I2C, SPI) do not match the signal or cabling, so some more research needed to be done.

After some research, it looks like this signal is a variation of the [1-Wire protocol](https://en.wikipedia.org/wiki/1-Wire), possibly inverted. Some more analysis with the logic analyzer:
<img class='mobile-max-width img-center' src="{% asset_path pulse.png %}" style=''>

I have added annotations to where I deduced the bits are. I have made the assumption that all bits start on the rising-edge. 1-Wire specification says it starts on the falling-edge, but either way, the same bitstream can be read. With the logic analyzer, the following deductions can be made:

- *Start bit* - the start bit is a ~8 microsecond high pulse, followed by a ~0.7 microsecond low pulse
- *1* - a ~3.1 microsecond high pulse is followed by a ~0.7 microsecond low pulse
- *0* - a ~0.7 second high pulse is followed by a ~3.1 microsecond low pulse

The ~3.8 microsecond pulse length gives the signal a ~262KHz data rate, disregarding the start bit. Total of 256 bits are sent (32 bytes) between start pulses.
