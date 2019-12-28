# Pulse dialing traffic simulator
Arduino sketch simulating a [pulse dialing]([https://en.wikipedia.org/wiki/Pulse_dialing](https://en.wikipedia.org/wiki/Pulse_dialing)) traffic to a step-by-step telephone exchange (Strowger, Siemens etc.).

A simultaneous traffic on up to 8 different lines can be simulated. The simulation consists of four stages for each line:
- connecting to the line,
- dialing in the given phone number,
- occupying the line for a random interval,
- releasig the line.