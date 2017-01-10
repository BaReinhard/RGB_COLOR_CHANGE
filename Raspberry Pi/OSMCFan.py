import RPi.GPIO as GPIO
import time
import atexit
import subprocess
def exit_handler():
	pwm.stop()
	GPIO.cleanup()
def FanOn(dutyCycle,fanTime,pwm):
	start,exit = time.time()
	pwm.ChangeDutyCycle(dutyCycle)
	while((exit - start) < fanTime):
		exit = time.time()
		time.sleep(10)


GPIO.setmode(GPIO.BCM)
GPIO.setup(25,GPIO.OUT)
pwm = GPIO.PWM(25,50)
pwm.start(0)
atexit.register(exit_handler)
try:
	while 1:
		process = subprocess.Popen('/opt/vc/bin/vcgencmd measure_temp',stdout = subprocess.PIPE,shell=True)
		temp,err = process.communicate()
		temp = str(temp).replace("temp=","")
		temp = str(temp).replace("\'C\n","")
		temp = float(temp)
		if(temp<50):
			FanOn(0,300,pwm)
		elif(temp<55):
			FanOn(75,60,pwm)
		elif(temp<60):
			FanOn(85,90,pwm)
		else:
			FanOn(100,180,pwm)
finally:
	exit_handler()
