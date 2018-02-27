# Blitz Meeting Feb 14th

Sensor bus?
- Concentrators
	front and back
	reduces wiring complexity

- UI and obstacle sensors together based on location

general purpose IO board
	UI piece and obstacle pieces locally based and fed (low level bus) to IO board

### 1 common GPIO ethernet port

POE by name won't be used, forces a standard withh RJ45 forbidden
really what we are talking about is power on the same connector 

100 Mbps max

M12 connectors?

same PCB design on front and back

### LEDs

number of LEDs?
1 dozen 
RGB leds (RGBWhite)

LED board talks to IO concentrator

headlights (white front, red back)

touch lights separate from accent lights
all neopixels

each side of LEDs should be controlled separately for turn signals

### Sensor positions

##### forward looking obstruction sensors (front whiskers)
- down and slightly to the side
detecting a cliff, side objects

minimum of 2 - coweled up high

turn on when moving, turn off when stopped

dark room testing with JRT measurement

##### rear rangefinders
- can't back into walls, or people

what is minimum range on ST rangers

positioned on rear belt line
avoid people standing within Gita rear footprint path
right up to the rear face of Gita

### Cameras

3 cameras? 
Camera cluster - close together as possible probably
maybe 1 more camera for forward object detection (structured light)

### Batteries 

2 hr mission time with 80% availability
600-700 Whr - design the smallest possible (upsell for larger batteries)
battery compartment temp/smoke/water detection for safety
UI considerations
backup batteries
needs to deal with regen

buying a BMS

### charger vs power supply

plugs into the metal chassis not plastic body
slow and fast chargers (maybe 1 is a portable version)

### anti water intrusion efforts

IP rating
IP64 at best...? IP54
should be "drip proof" based on use cases
drainage from bins and body
water intrusion detection

### estop

battery fire risk?
1) remove power from the motion
2) removed energy from the device
if we need an "emergency" stop it has to be a red mushroom, with a yellow button
need to do risk analysis

maybe an on/off button for the motors
this is separate from emergency

lets not go to court!!!

"controlled reliable circuits"

