# Example Output

To demonstrate, I've chosen to use codes from the Sony RMT-CF1A. (I had the radio but no remote.)

They can be found here:

https://github.com/probonopd/irdb/blob/master/codes/Sony/Unknown_RMT-CF1A/26%2C19.csv

In particular, I will be using this ProntoHEX code for the power button:

```
0000 0067 0000 003F 0060 0018 0030 0018 0018 0018 0030 0018 0018 0018 0030 0018 0018 0018 0018 0018 0018 0018 0030 0018 0018 0018 0030 0018 0030 0018 0030 0018 0030 0018 0018 0018 0018 0018 0030 0018 0018 0018 0018 0018 0018 0210 0060 0018 0030 0018 0018 0018 0030 0018 0018 0018 0030 0018 0018 0018 0018 0018 0018 0018 0030 0018 0018 0018 0030 0018 0030 0018 0030 0018 0030 0018 0018 0018 0018 0018 0030 0018 0018 0018 0018 0018 0018 0210 0060 0018 0030 0018 0018 0018 0030 0018 0018 0018 0030 0018 0018 0018 0018 0018 0018 0018 0030 0018 0018 0018 0030 0018 0030 0018 0030 0018 0030 0018 0018 0018 0018 0018 0030 0018 0018 0018 0018 0018 0018 0210
```

This was generated using IRScrutinizer. A preview of the code is generated:

![IRScrutinizer output.](/example_output/irscrutinizer.png)

So starting up Arduino and running the sketch. The code can be sent in the serial terminal. TePIR replies back with:

```
Carrier Period (us): 25
Non-repeating Pairs: 0
Burst Pairs: 63
Done!
```

Of course, you should see the onboard led blink. (The IR led also blinks as well.)

Attaching Pin 4 and Pin 11 to an oscilloscope:

![Oscilloscope capture of whole waveform.](/example_output/everything.png)

See the resemblance? 😎

Closing in on the POWER key code:

![Scope capture on single repetition.](/example_output/power_code.png)

You can see it practically measures 45ms as predicted by IRScrutinizer.

Zooming in on the first pulse:

![First pulse.](/example_output/pulse.png)

2.410ms... 0.5% off... I suppose it's close enough... 

Zooming further into the carrier frequency:

![Carrier wave.](/example_output/carrier_frequency.png)

Dead on 40kHz.

Looking at the header of a pulse:

![Header pulse.](/example_output/header.png)

This corresponds to this section of the code:

```C
  if(decode_state == 2) {
    if(pair_cur == 0) {
      time_sta = time_cur;
    } else {
      time_sta = time_sta + space[pair_cur-1];
    }
    digitalWrite(11, 1);
    Timer1.setPwmDuty(4,512);

    decode_state = 3;
  }
```

It seems like `setPwmDuty` takes a whole 11us to run. This equates around to whopping 176 cycles. (16MHz*11us.) The whole Arduino compile chain ain't that effecient it seems.

Anyways, looking at the trailer:

![Trailer...](/example_output/trailer.png)

```C
  if(decode_state == 3 && (time_cur-time_sta)>=pulse[pair_cur]) {
    Timer1.setPwmDuty(4,0);
    digitalWrite(11, 0);
    //time_sta = time_cur;
    time_sta = time_sta + pulse[pair_cur];
    decode_state = 4;
  }
```

A whole 19us lost. Despite the `digitalWrite` being after in the C code, it gets executed before `setPwmDuty`.

Weird business with compiler "optimizations". Though not critical, it's very annoying if it is. This sometimes leads to weird code in C in an attempt to "fix" the compiler output.

Anyways, it's "good enough" for sending remote codes. Arduino is "good enough". Believe it or not, it only took me a day to code this up.
