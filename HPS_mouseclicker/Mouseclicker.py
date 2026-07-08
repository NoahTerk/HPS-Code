import serial #py -m pip install pyserial
import pyautogui
import time

#constants 
Defualt_X_Click_Value = 1174 #X pixles for mouse click
Defualt_Y_Click_Value =  581 #y pixles for mouse click
Click_Image_Ref = "NMR_Go_Icon.png"
Find_Image_Max_Tries = 3

Image_Ref_Found = False
Current_X_Click_Value = -1
Current_Y_Click_Value = -1
Current_Image_Tires = 0

Turn_On_Clicker = True  # change to false to turn off clicking event | Default Value - True


PORT = "COM9"
BAUD = 115200

ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)


def main():
    while True:
        state = SerialWatch() 

        if (state ==-1): #error accoured
            break
        elif (state == 1):
            # MouseClick(Defualt_X_Click_Value, Defualt_Y_Click_Value, Click_Image_Ref)
            MouseClick(Defualt_X_Click_Value, Defualt_Y_Click_Value) 


def MouseClick(x, y, image_ref=None):
    global Image_Ref_Found
    global Current_X_Click_Value
    global Current_Y_Click_Value
    global Current_Image_Tires
    global Find_Image_Max_Tries
    global Click_Image_Ref

    if (Image_Ref_Found):
        print("Image found clicked at X:" + str(Current_X_Click_Value) + " Y:" + str(Current_Y_Click_Value))
        pyautogui.click(Current_X_Click_Value, Current_Y_Click_Value , button="left")
    else:
        if (Current_Image_Tires < Find_Image_Max_Tries):
            try:
                centerIcon = pyautogui.locateCenterOnScreen(Click_Image_Ref) #confidence=0.8
            except:
                centerIcon = None
                
            if centerIcon:
                print("Image found clicked at " + str(centerIcon))
                pyautogui.click(centerIcon, button="left")
                Current_X_Click_Value = centerIcon.x
                Current_Y_Click_Value = centerIcon.y
                Image_Ref_Found = True
                return
            else:
                Current_Image_Tires = Current_Image_Tires + 1
                print("Image not found; Try: " + str(Current_Image_Tires))
                print("Defualt Clicked at X:" + str(Defualt_X_Click_Value) + " Y:" + str(Defualt_Y_Click_Value))
                pyautogui.click(Defualt_X_Click_Value,Defualt_Y_Click_Value,button="left")    
        else:
            print("Defualt Clicked at X:" + str(Defualt_X_Click_Value) + " Y:" + str(Defualt_Y_Click_Value))
            pyautogui.click(Defualt_X_Click_Value,Defualt_Y_Click_Value,button="left")
    

def SerialWatch():
    if ser.in_waiting > 0: #message reviced from aduino
        line = ser.readline().decode('utf-8', errors="ignore").strip()

        if not line: #error recived in reding serial return false and carry on
            return 0 

        if line.startswith("Click:"):
            print("Click: Started NMR")
            return 1
        elif line.startswith("Error:"):
            print("Error Occoured: " + line[6:].strip())
            return 0
        elif line.startswith("Crit Error:"):
            print("Crit Error Occoured: " + line[11:].strip())
            print("Exiting Program")
            return -1
        else:
            LogMessage(line)
            return 0
            
                
def LogMessage(msg): #for GUI Log
    print("Log:" + msg.removeprefix("Log:"))


if __name__ == "__main__":
    main()