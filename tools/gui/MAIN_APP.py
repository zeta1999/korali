import tkinter as tk
from tkinter import *
from tkinter import ttk # if python.7 Tkinter. ttk is like the CSS.
# Files import:
import class_KORALI
import classes_FRAMES

# ************ VARIABLES *****************


mainPath = '../../source/modules/problem'
mainPath2 = '../../source/modules/solver'

contador = 1


#############################################################
## ******************* CLASSES ******************************


if __name__ == "__main__":
    app = class_KORALI.KORALI()
    app.geometry("1500x1000") # Size of our application.
    app.minsize("1500","1000")
    app.mainloop()
