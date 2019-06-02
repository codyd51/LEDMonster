LED Monster
-------------

Weekend project to make & animate a 3x3x3 LED matrix out of popsicle sticks and gumption. My coworker advised me to throw the code up for posterity. Eat your heart out, Thomas.

This project controls 27 LEDs in total. 14 are controlled by a master Arduino, and 13 by a slave. There are more exotic wirings which would enable 27 LEDs to be controlled by a single Arduino (namely, charlieplexing), but ease-of-construction and time were considerations, so I didn't bother. The master Arduino controls its own LEDs and tells the slave when to flip LEDs on or off. The master has several pre-programmed LED patterns.

I built the physical model with what I had readily available: popsicle sticks, superglue, and hope.

MIT license.
