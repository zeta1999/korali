import tkinter as tk
from tkinter import *
import json

#FILES import
import classes_FRAMES
import class_KORALI
import functions

# Frame Variables:
selectorColor = 'snow'
colorProblem = 'snow'
forbidden = ['Variables','Problem','Solver']
##dist_first_time = True


class Variables():
    def __init__(self,master,experiments,selectedtab):
        # master is the frame from the previous class where we want to insert data.
      
        self.variables = tk.Frame(master,bg=selectorColor,width=717,height=925)
        self.variables.grid(column=0,row=0)
        self.variables.grid_propagate(0)

        experiments[selectedtab]['variables'] = self.variables
        res_var = {}
        experiments[selectedtab]['labels_var'] = res_var

        efake =tk.Label(self.variables, text='', fg=colorProblem, bg=colorProblem)
##        efake.grid(row=0, column=0,columnspan=4,pady = 20 ,padx=5, sticky='nw')
        e =tk.Button(self.variables, text='Variables',activebackground='aliceblue', font="Arial 20", fg='black', bg=colorProblem,borderwidth=2,relief='solid', command = lambda : functions.Clear(self.gs)) #bg = 'darkcyan', fg='white')
        e.grid(row=1, column=0,columnspan=4,pady = 4 ,padx=10, sticky='n')
        edescr =tk.Label(self.variables, text='Click title to clear the variables...', font="Arial 10", fg='navy', bg=colorProblem) #bg = 'darkcyan', fg='white')
        edescr.grid(row=2, column=0,columnspan=4,rowspan=1,pady = 4 ,padx=10, sticky='n')
        efake1 =tk.Label(self.variables, text='', fg=colorProblem, bg=colorProblem)
        efake1.grid(row=3, column=0,columnspan=4,pady = 10 ,padx=5, sticky='nw')

##        self.Show_frame(experiments,selectedtab)

    def Show_frame(experiments,selectedtab):
        
        results = experiments[selectedtab]['results']

        if 0 not in results.keys() or 1 not in results.keys():
            functions.popupmsgwarning('First choose a problem and a solver')
            return
        variables = experiments[selectedtab]['variables']
        variables.tkraise()


        
        

