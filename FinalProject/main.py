#!/usr/bin/python
from time import sleep
import board
import busio
import adafruit_bme280
import sys
import termios
import tty
from os import path
import subprocess
import re
from display import display_initialize, to_print
# A defined value of measurements to take for getting breath data
calibration_measurements = 100


def main():


    # Check if BME280 exists on address 77 and can be found!
    check_BME()

    # Check if HT16K33 exists on address 70 and can be found!
    check_display()

    # Create termios struct for detecting user input
    orig_settings = termios.tcgetattr(sys.stdin)


    # Check if file exists
    if path.exists('results.csv'):
        fd = open('results.csv', 'a')
    
    # If it does not exist, create it, and write the appropriate heading to the file.
    else:
        fd = open('results.csv', 'w')
        fd.write("Participant, Room Humidity(RH), Mask on measurement (%RH), mask off measurement(%RH), Overall measured mask effectivity(%)\n")
        fd.flush()

    # Variables designated for creating the BME object, as well as reading in data from BME280
    i2c = busio.I2C(board.SCL, board.SDA)

    display = display_initialize(i2c)


    bme280 = adafruit_bme280.Adafruit_BME280_I2C(i2c)
    baseline_humidity = bme280.humidity
    temperature = bme280.temperature
    
    # Get users name
    name = input('Please enter the name of the current test subject\n')

    # See if the device has settled down; if measurements are the roughly the same over 100 polls
    check_readiness(bme280)

    # Now that we are ready, please begin the first procedure of the device
    print(f'{name}, please proceed to remove your mask')
    sleep(2)
    mask_off = read_humidity(bme280, baseline_humidity)
    to_print(round(mask_off,2), display)
    
    # Check to calibrate device once more
    check_readiness(bme280)

    # Now prompt the user to breathe with the mask on
    print(f'{name}, now proceed to put your mask on once more')
    mask_on = read_humidity(bme280, baseline_humidity)
    to_print(round(mask_on,2), display)
    sleep(2)


    # Take a direct ratio with mask on vs off
    on_vs_off = mask_on / mask_off

    # Get a scaled ratio between 1 and 0 of mask effectivity
    mask_efficiency = 1 - on_vs_off

    if mask_efficiency < 0:
        print("Error reading subjects mask effectivity, please try again!\n")
        exit()

    # Report user mask effectivity to terminal
    print(f'\n\n{name}, your mask was approximately {mask_efficiency * 100}% effective')
    to_print(round(mask_efficiency * 100, 2), display)




    # Write the data from the user to our results file only if device owner deems output is correct
    print("Please select if you would like to save the following results.\nSelect yes(Y) or no(N).")

    # Force linux to be character break, not line break (not require hitting enter for results)
    tty.setcbreak(sys.stdin)

    key = 0

    # Until user gives input
    while True:
        
        # Check the key they pressed
        key = sys.stdin.read(1)[0]
        # If answer is yes, save and leave
        if key == 'y' or key == "Y":
            # Revert the terminal back how i found it (fix that character break mode)
            termios.tcsetattr(sys.stdin, termios.TCSADRAIN, orig_settings)
            print(f'File for {name} has been saved.\n')
            # fd.write("Participant, room humidity, mask on measurement, mask off measurement\n")
            fd.write(str(name) + ', ' + str(baseline_humidity) + ', ' + str(mask_on) + ', ' + str(mask_off) + ', ' + str(mask_efficiency * 100) + "%\n")
            fd.close()
            exit()

        # If answer is no, close files and leave
        if key == "n" or key == "N":
            # Revert the terminal and leave
            termios.tcsetattr(sys.stdin, termios.TCSADRAIN, orig_settings)
            fd.close()
            exit()

    # Close our results file
    fd.close()

    exit()


def check_BME():
   
    # Taking advantage of built in I2Cdetect utility, return all addresses from RPI
    I2C_connections = subprocess.Popen(['i2cdetect', '-y', '1'], stdout=subprocess.PIPE)
   
    # Read the return data from subprocess using STDIO
    resulting_connections = I2C_connections.stdout.readlines()
    
    # Find if address 77 is used
    resulting_connections = str(resulting_connections)

    # If address 77 has a connection, return successful
    if resulting_connections.find('77') > 0:
        print('Device found on default I2C address')

    # Otherwise, exit the program
    else:
        print('No BME280 device connected')
        exit()


def check_display():

    # Taking advantage of built in I2Cdetect utility, return all addresses from RPI
    I2C_connections = subprocess.Popen(['i2cdetect', '-y', '1'], stdout=subprocess.PIPE)
   
    # Read the return data from subprocess using STDIO
    resulting_connections = I2C_connections.stdout.readlines()
    
    # Find if address 70 is used (70 is default for HT16K33)
    resulting_connections = str(resulting_connections)

    # If address 70 has a connection, return successful
    if resulting_connections.find('77') > 0:
        print('Device found on default I2C address')

    # Otherwise, exit the program
    else:
        print('No HT16K33 device connected')
        exit()
    

def check_readiness(bme):
    
    # Variable to count the BME280 humidity sensor calibration . When the difference between two consecutive measurements falls within the value, the within_tolerance counter is incremented is set.
    within_tolerance = 1
    
    # Place holder for previous relative humidity value
    past_humidity = 0
    
    # For each measurement taken in the amount of specified calibration measurement samples
    for x in range(calibration_measurements):
        
        # Record the currently measured humidity
        current_humidity = bme.humidity
        
        # If the current humidity and past humidity difference is less than 1
        if abs(past_humidity - current_humidity) < 1:
            within_tolerance += 1
        # Set the past humidity as the current humidity
        past_humidity = current_humidity
   
    if within_tolerance == calibration_measurements:
        print('Device ready, begin sample')
        return 
    else:
        check_readiness(bme)


    
def read_humidity(bme280, baseline_humidity):
    
    humidity_flag = 0
    
    humidities = [0]

    print('Upon countdown hitting 0, please breathe into device\n')

    for seconds in range(3):
        print(3-seconds)
        sleep(1)
    print('0')
    while len(humidities) < 100:
        humidity = bme280.humidity - baseline_humidity
        temperature = bme280.temperature
        pressure = bme280.pressure
        humidities.append(humidity)
        
    return max(humidities)


if __name__ == '__main__':
    main()
    exit()
