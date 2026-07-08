import tkinter as tk #import the necessary goods
import threading 

window = tk.Tk()


class app(threading.Thread):
    def __int__(self, tk_window):
        
        self.window = tk_window
        self.window.bind("<Key>", self.exitcase)
        threading.Thread.__init__(self, group=None)
        self.start()
    
    def run(self):
        loop_active = True
        while loop_active:
            print("hello loop")
    

    def exitcase(self, event):
        if event.keysym == "q":
            self.window.quit()
            
    
def setup():
    
    window.title("Tube Timer")
    window.geometry('300x100')

    

    #setup items in window
    button1 = tk.Button(window, text="Button 1", )


    #run code
    APP = app(window)

    window.mainloop()

if __name__ == "__main__":
    setup()