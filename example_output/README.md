# Example Output

To demonstrate, I've chosen to use codes from the Sony RMT-CF1A. (I had the radio but no remote.)

They can be found here:

https://github.com/probonopd/irdb/blob/master/codes/Sony/Unknown_RMT-CF1A/26%2C19.csv

In particular, I will be using this ProntoHEX code for the power button:

```
0000 0067 0000 003F 0060 0018 0030 0018 0018 0018 0030 0018 0018 0018 0030 0018 0018 0018 0018 0018 0018 0018 0030 0018 0018 0018 0030 0018 0030 0018 0030 0018 0030 0018 0018 0018 0018 0018 0030 0018 0018 0018 0018 0018 0018 0210 0060 0018 0030 0018 0018 0018 0030 0018 0018 0018 0030 0018 0018 0018 0018 0018 0018 0018 0030 0018 0018 0018 0030 0018 0030 0018 0030 0018 0030 0018 0018 0018 0018 0018 0030 0018 0018 0018 0018 0018 0018 0210 0060 0018 0030 0018 0018 0018 0030 0018 0018 0018 0030 0018 0018 0018 0018 0018 0018 0018 0030 0018 0018 0018 0030 0018 0030 0018 0030 0018 0030 0018 0018 0018 0018 0018 0030 0018 0018 0018 0018 0018 0018 0210
```

So starting up Arduino and running the sketch. The code can be sent in the serial terminal. TePIR replies back with:

```
Carrier Period (us): 25
Non-repeating Pairs: 0
Burst Pairs: 63
Done!
```

Of course, you should see the onboard led blink. (The IR led also blinks as well.)

Attaching Pin 4 and Pin 11 to an oscilloscope:

