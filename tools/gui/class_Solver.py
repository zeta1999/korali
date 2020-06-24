'''
*Usage*                                                                                                                                   #####
*Displays/Raises the Solver Frame and Initiate the Solver key of the main dictionary -> results.                                          #####
                                                                                                                                          #####
*Functions and Classes*                                                                                                                   #####
*1 class: Solvers.                                                                                                                        #####
*MAIN DICTIONARY KEY = 1                                                                                                                  #####
'''
try:
    import tkinter as tk
    from tkinter import *
    from tkinter import ttk
except ImportError:
    import Tkinter as tk
    from Tkinter import *
    from Tkinter import ttk
import json

#FILES import
import classes_FRAMES
import class_KORALI
import functions

# Frame Variables:
selectorColor = 'snow'
forbidden = ['Variables','Problem','Solver']

class Solvers():
    def __init__(self,master,experiments,directorio,nombre,DB,cont,selectedtab):
        # master is the frame from the previous class where we want to insert data.

        self.solver = tk.Frame(master,bg=selectorColor,width=495,height=925)
        self.solver.grid(column=0,row=0)
        self.solver.grid_propagate(0)

        # STORE THIS FRAME IN THE experiments dictionary.
        experiments[selectedtab]['solver'] = self.solver

        results = experiments[selectedtab]['results']
        # Every time this class is called, new Solver in the results:
        results[cont] = {}

        functions.printConfig(self.solver,experiments,selectedtab,directorio,nombre,DB,cont)

    def Show_frame(experiments,selectedtab,directorio,cont,tf):
        solver = experiments[selectedtab]['solver']
        solver.tkraise()
        functions.time.sleep(1)
        # Call the function that embeds the HTML
        functions.website(directorio,cont,tf)
