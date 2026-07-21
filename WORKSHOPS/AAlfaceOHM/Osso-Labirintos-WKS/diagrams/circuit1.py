from skidl import *

# Define custom parts
def Battery():
    part = Part()
    part.add_pins(Pin('p', 'pos', 'output'), Pin('n', 'neg', 'output'))
    part.name = 'Battery'
    part.description = 'Custom Battery'
    return part

def Resistor(value):
    part = Part()
    part.add_pins(Pin(1), Pin(2))
    part.name = 'Resistor'
    part.value = value
    part.description = f'Custom Resistor ({value})'
    return part

def LED():
    part = Part()
    part.add_pins(Pin('A', 'anode', 'input'), Pin('K', 'cathode', 'input'))
    part.name = 'LED'
    part.description = 'Custom LED'
    return part

# Create parts
battery = Battery()
resistor = Resistor("220")
led = LED()

# Create nets
vcc = Net('VCC')
gnd = Net('GND')
led_anode = Net('LED_ANODE')

# Connect the parts
battery['p'] += vcc
battery['n'] += gnd
vcc += resistor[1]
resistor[2] += led_anode
led_anode += led['A']
led['K'] += gnd

# Run ERC check
ERC()

# Generate netlist
generate_netlist()

# Show the schematic (if you want a visual representation)
show()