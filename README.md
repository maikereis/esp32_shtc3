# shtc3
Simplest SHTC3 driver to measure Temperature and Humidity

# CONFIGURATION
I2C FREQ 100kHz

SDA = GPIO_NUM_21 (pull-up)

SCL = GPIO_NUM_22 (pull-up)

# STEP-BY-STEP

 ## Wake-up
  Write 0xE0
  
  Write 0x3517
  
 ## Configure to read Temperature First
  Write 0xE0
  
  Write 0x7866
  
 ## Read out data
  Write 0xE1
  
  Read BUFFER
  
 
