from tkinter import Tk, StringVar, Button, Label, Entry, Scale, Canvas
from serial import Serial, SerialException
import serial.tools.list_ports
from time import time
from threading import Thread
import serial.tools.list_ports
import colorama
import sys

comportEstablishedFlag = False

ser = 0
freq = 1
freq_prev = 1
pulsewdith = 50
pulsewidth_prev = 50
amp = 50
amp_prev = 50
enState = 0
enState_prev = 0
amp_set_flag = False
def EstablishConnection():   
    ports = serial.tools.list_ports.comports()
    
    global comportEstablishedFlag
    global ser
    ser = 0
    if len(ports) == 0:
        print("No serial ports found.")
        input("Press Any Key to exit program.")
    for port, desc, hwid in sorted(ports):
        t = time()
        print('Trying '+port)
        timeoutErrorFlag = False
        try: 
            ser = Serial(port, baudrate=9600, timeout=0) # try to establish serial connection with com port
            ser.write("Python Connection Established!\n".encode()) # send string
            
            response = ''
            numLines = 0
            while True:
                response += ser.read().decode("utf-8")
                if ('\n' in response):
                    numLines = numLines + 1
                if (numLines >= 1):
                    break
                if time() >= t + 1:
                    print("No Response. Trying another port, or exiting.")
                    timeoutErrorFlag = True
                    t = time()
                    while time() <= t+1:
                        a=2
                    break

            if response == 'Arduino Connection Established!\r\n':
                comportEstablishedFlag = True
                print('Connection Established ('+port+').')
            if response != 'Arduino Connection Established!\r\n' and not timeoutErrorFlag:
                comportEstablishedFlag = False
                print('Incorrect Response. Tring another port, or exiting.')

            if comportEstablishedFlag == True:
                break
        except: 
            
            continue
    
    return ser

def writeReadSerial(string):
    global ser
    ser.write((string+'\n').encode())
    t = time()
    print(readSerialLine(ser, time()))

def readSerialLine(ser):
    response = ser.readline().decode("utf-8")
    response = response[:-2]
    
    return response

def SendSerialCommand(a):
    ser.write((a+'\r\n').encode())

def SendAmplitudeCommand(event):
    ser.write(("A"+str(AMPLITUDE.get())+"\r\n").encode())

def SendFrequencyCommand(event):
    ser.write(("F"+str(f.get())+"\r\n").encode())  
    if (1000/float(f.get()) < float(pw.get())):
        sys.stdout.write("\nCannot set frequency > "+str(round(1000/float(pw.get()), 1))+" Hz at selected pulse width.")
        f.set(str(freq))

def SendPulseWidthCommand(event):
    ser.write(("P"+str(pw.get())+"\r\n").encode())
    if float(pw.get()) > (1000/float(f.get())):
        sys.stdout.write("\nCannot set pulse width > "+str(1000/float(f.get()))+" ms at selected frequency.")
        pw.set(str(int(pulsewidth)))
    elif float(pw.get()) < 10:
        sys.stdout.write("\nCannot set pulse width < 10 ms.")
        pw.set(str(int(pulsewidth)))
    
def SetAmplitudeFlag(event):
    print('\n', end='')


# comport = "COM3"
# ser = Serial(comport, 9600, timeout=0)              
# print(ser.name)
ser = EstablishConnection()

top = Tk()
top.geometry("425x200")

pw = StringVar(top, 50) # ms
f = StringVar(top, 1) # Hz
a = StringVar(top, 50) # %

color = ['white','#00c0ff']
toggleOptions=['raised','sunken']
enableOptions = ['Enable','Disable']
PULSEWIDTH = Entry(top, textvariable=pw, justify='right')
PW = Button(top, text='UPDATE PULSE WIDTH', command=lambda: SendSerialCommand("P"+str(pw.get())))
FREQUENCY = Entry(top, textvariable=f, justify='right')
F = Button(top, text='UPDATE FREQUENCY', command=lambda: SendSerialCommand("F"+str(f.get())))
AMPLITUDE = Scale(top, from_=100, to=0, variable=a, orient='vertical', length=150, command=SendAmplitudeCommand)

ENABLE = Button(top, text='Enable', command=lambda: SendSerialCommand("E"))

canvas = Canvas(top, width=60, height=60)
INDICATOR = Canvas.create_oval(canvas, 5,5  , 50, 50)

Label(top, text='Pulse Width (ms):', justify='right').place(x=10, y=10, width=100)
PULSEWIDTH.place(x=120, y=10, width=50)
Label(top, text='-> Press ENTER to set').place(x=180, y=10)
PULSEWIDTH.bind('<Return>', SendPulseWidthCommand)

Label(top, text='   Frequency (Hz):', justify='right').place(x=10, y=40, width=100)
FREQUENCY.place(x=120, y=40, width=50)
FREQUENCY.bind('<Return>', SendFrequencyCommand)
Label(top, text='-> Press ENTER to set').place(x=180, y=40)


Label(top, text='Amplitude (%):').place(x=325, y=10)
AMPLITUDE.place(x=325, y=40)
#AMPLITUDE.bind('<B1-Motion>', SendAmplitudeCommand)
AMPLITUDE.bind('<ButtonPress-1>', SetAmplitudeFlag)
# AMPLITUDE.bind('<ButtonRelease-1>', SetAmplitudeFlag)

ENABLE.place(x=50, y=100, width=100, height=50)
canvas.place(x=175, y=100)

t_start = time()
def update():
    t = time()
    global freq, freq_prev
    global pulsewidth, pulsewidth_prev
    global amp, amp_prev
    global enState, enState_prev
    global amp_set_flag
    response = ''
    numLines = 0
    while True:
        try: 
            response += ser.read().decode("utf-8")
        except SerialException as e:
            top.destroy()
            SendSerialCommand('S')
            ser.close()
        if ('\n' in response):
            numLines = numLines + 1
        if (numLines >= 1):
            break
    
    if response[:7] == "VALUES:":   
        values = response[7:-2].split(',')
    
        enState = int(values[0])
        ENABLE.config(relief=toggleOptions[enState])
        ENABLE.config(text=enableOptions[enState])
        freq = float(values[1])
        pulsewidth = float(values[2])/1000
        amp = int(values[3])
        ledstate = int(values[4])
        canvas.itemconfig(INDICATOR, fill=color[int(ledstate>0)])
        if (enState_prev != enState):
            if enState == 1:
                sys.stdout.write("\nLEDs Enabled.")
            else:
                sys.stdout.write("\nLEDs Disabled.")
        
        if (freq_prev != freq):
            sys.stdout.write("\nFrequency changed: "+str(freq)+" Hz")
        if (pulsewidth_prev != pulsewidth):
            sys.stdout.write("\nPulse width changed: "+str(pulsewidth)+" ms")
        
        if (amp_prev != amp):
            sys.stdout.write("\rAmplitude set: "+str(AMPLITUDE.get())+"%     ")
            sys.stdout.flush()
        
        enState_prev = enState
        freq_prev = freq
        pulsewidth_prev = pulsewidth
        amp_prev = amp
        

    top.after(1, update)

Thread(target=update()).start()


SendSerialCommand("S")
top.after(1)
SendSerialCommand("P50")
top.after(1)
SendSerialCommand("A50")
top.after(1)
SendSerialCommand("F1")
top.after(1)
print(' ')
top.mainloop()
SendSerialCommand('S')
ser.close()

