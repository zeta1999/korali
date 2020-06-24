'''
*Usage*                                                                                                                                   #####
*Displays/Raise the Variables Frame and Initiate the Variable key of the main dictionary -> results.                                      #####
                                                                                                                                          #####
*Functions and Classes*                                                                                                                   #####
*1 class: Variables.                                                                                                                      #####
                                                                                                                                          #####
*MAIN DICTIONARY KEY = 2                                                                                                                  #####
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
colorProblem = 'snow'
forbidden = ['Variables','Problem','Solver']



class Variables():
    def __init__(self,master,experiments,selectedtab):
        # master is the frame from the previous class where we want to insert data.
      
        self.variables = tk.Frame(master,bg=selectorColor,width=495,height=925)
##        self.variables.grid(column=0,row=0)
##        self.variables.grid_propagate(0)

        experiments[selectedtab]['variables'] = self.variables
        res_var = {}
        experiments[selectedtab]['labels_var'] = res_var

        efake =tk.Label(self.variables, text='', fg=colorProblem, bg=colorProblem)
##        efake.grid() No grid, to avoid displaying the widget but using its position.
        e =tk.Button(self.variables, text='Variables',activebackground='aliceblue', font="Arial 20", fg='black', bg=colorProblem,borderwidth=2,relief='solid',
                     command = lambda : functions.Clear(self.gs)) #bg = 'darkcyan', fg='white')
        e.grid(row=1, column=0,columnspan=4,pady = 4 ,padx=10, sticky='n')
        edescr =tk.Label(self.variables, text='Click title to clear the variables...', font="Arial 10", fg='navy', bg=colorProblem) #bg = 'darkcyan', fg='white')
        edescr.grid(row=2, column=0,columnspan=4,rowspan=1,pady = 4 ,padx=10, sticky='n')
        efake1 =tk.Label(self.variables, text='', fg=colorProblem, bg=colorProblem)
        efake1.grid(row=3, column=0,columnspan=4,pady = 10 ,padx=5, sticky='nw')

    def Show_frame(experiments,selectedtab):
        
        results = experiments[selectedtab]['results']
        # Problems and Solver must be chosen before creating Variables.        
        if 0 not in results.keys():
            functions.popupmsgwarning('First choose a Problem and a Solver')
            return
        elif 1 not in results.keys():
            functions.popupmsgwarning('Problem already displayed. Choose a Solver')
            return
        variables = experiments[selectedtab]['variables']

        variables.grid(column=0,row=0)
        variables.grid_propagate(0)
        variables.tkraise()


        
        

