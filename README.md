# Pulse dialing tools

Various Arduino sketches to work with a [pulse dialing](https://en.wikipedia.org/wiki/Pulse_dialing) devices (telephones, exchanges).

## Pulse dialing traffic simulator
See `sketches/pulse_dialing_traffic.ino`

Arduino sketch simulating a pulse dialling traffic to a step-by-step telephone exchange (Strowger, Siemens etc.).

A simultaneous traffic on up to 8 different lines can be simulated. The simulation consists of four stages for each line:
- connecting to the line,
- dialing in the given phone number,
- occupying the line for a random interval,
- releasig the line.

Sketch contains a ready configuration for different pulse rates (Strowger, Siemens, Ericsson) and decadic coding (standard, Swedish, New Zealand). Phone numbers are randomly selected from a given set.

Arduino can be connected to an actual (inactive!) excahnge via relay shield/module. For research purposes only.