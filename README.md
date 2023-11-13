# TePIR
ProntoHex implemented on Teensy2

## Intro
TePIR simply takes in a ProntoHex code and flashes an IR LED accordingly.

Though developed for Teensy2, the code should be portable with few tweaks for other Arduino devices. The most major change would involve using specific code for the timer. Using a Teensy2, the code is robust enough to take in multiple ("well formated") commands... Should simply work out of the box.

(LIRC is a nightmare to deal with. In fact, it doesn't really work with FTDI serial devices for transmitting.)

Everything runs entirely using the main loop. This means not using interrupts.

ProntoIR codes can be generated via IRScrutinizer and various remote codes can be found in IRDB.

Here is a demo of TePIR's performance: https://github.com/stickteo/TePIR/blob/main/example_output/README.md

## Circuit
The circuit simply involves connecting a 100ohm resistor and an IR led in series to Pin 4. Teensy2 outputs 5V. The measure voltage and current across the IR led was ~1.4V@36mA.

Pin 11 is used to indicate the pulse without the carrier frequency. (Pin 11 is the onboard LED.)

![It's reassuring if the circuit is built and a photo is taken? Hire me!](/example_output/circuit50.jpg)

## Overview
The main loop is a basic state machine.

Priority is taken to clear the HW serial buffer. A circular buffer is used here. Not much to comment but it seems robust enough.

State 0 parses the preamble. This generates the carrier period and the amount of burst pairs expected. The parser expects well-formatted ProntoHex codes as outputed by IRScrutinizer. If an error is encountered, error will be print out and the parser and circular buffer will reset. (I have not exhaustively check against intentional bad codes meant to exploit...) Basic error checking is meant to make usage convenient instead of softlocking and reseting the device.

State 1 parses every burst pair into an array. The initial decision was to parse each pair as needed. However, this created too much overhead during transmission of the IR led. The overhead simply causes too much timing issues. When done parsing, the timer is initialized and turned "off" by setting the pulsewidth to 0.

State 2 starts pulsing the IR led.

State 3 waits until pulsing is done then turns off the led.

State 4 checks if all pairs are transmitted. If not done go to State 2 else "Done!" and go back to State 0.

## Performance & Tidbits
Using a timer to generate a PWM signal gives a really clean carrier wave.

Initially, the start time was updated using the current time. Of course, this generates a lot of jitter since the current time is polled. Instead, the start time is updated with the burst pair times instead. Jitter will still exist but on average the timing is by definition accurate. (As accurate as the `micros()` function will be.)

Essentially though, the main loop is just one big busy loop.

On creating an actual remote, all of the serial stuff can be removed and the ProntoIR codes can be stored into a `uint16_t` array. Perhaps flash usage can be reduced with more compact code formats and ProntoIR codes can be generated from that.

Instead of using `disablePwm`, `setPwmDuty` is used instead. Using `setPwmDuty` seems to incur significantly less overhead than `disablePwm` and `enablePwm`. (`Timer1.setPwmDuty(PIN, 0)` effectively turns off PWM.)

`us_min_repeat` was added so the code can be repetively output instead of just outputting once. This helps with certain remotes that can "learn"/programmed by other remotes.

## ProntoHex
The parser assumes the following preamble: `0000 pppp 0000 bbbb`

`pppp` is the carrier period in 0.241246us units. `bbbb` is the amount of burst pairs.

(carrier frequency = 1 / (`pppp` * 0.241246us) )

For each burst pair: `nnnn ffff`

`nnnn` is the pulse period (on time) in terms of the period. `ffff` is the space period (off time) in terms of the period.

(i.e. pulse period = nnnn * pppp * 0.241246us.)

The intput characters are ASCII hex.

## Resources
These resources were used for the development of TePIR.

* https://github.com/probonopd/irdb
* https://github.com/bengtmartensson/IrScrutinizer
* https://www.remotecentral.com/features/irdisp2.htm
