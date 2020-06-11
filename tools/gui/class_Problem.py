import tkinter as tk
from tkinter import *
import json

#FILES import
import classes_FRAMES
import class_KORALI
import functions

# Frame Variables:
selectorColor = 'snow'
forbidden = ['Variables','Problem','Solver']


class Problems():
    def __init__(self,master,experiments,directorio,nombre,DB,cont,selectedtab):
        # master is the frame from the previous class where we want to insert data.

        self.problem = tk.Frame(master,bg=selectorColor,width=717,height=925)
        self.problem.grid(column=0,row=0)
        self.problem.grid_propagate(0)

        # STORE THIS FRAME IN THE experiments dictionary.
        experiments[selectedtab]['problem'] = self.problem

        results = experiments[selectedtab]['results']

        # Every time this class is called, new Problem in the results:
        results[cont] = {}

        functions.printConfig(self.problem,experiments,selectedtab,directorio,nombre,DB,cont)

    def Show_frame(experiments,selectedtab,directorio,cont,tf):
        problem = experiments[selectedtab]['problem']
        problem.tkraise()
        functions.time.sleep(1)
        functions.website(directorio,cont,tf)
        
        
        

