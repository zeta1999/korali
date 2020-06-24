'''
*Usage*                                                                                                                                   #####
*Main Window App CLASS                                                                                                                    #####
                                                                                                                                          #####
*Functions and Classes*                                                                                                                   #####
*1 class: KORALI                                                                                                                          #####
*Main Function: crear_tab() - In charge of calling the 3 main classes/FRAMES and displaying and managing the window toolbar               #####                                                                                                                 #####
'''
try:
    import tkinter as tk
    from tkinter import *
    from tkinter import ttk
except ImportError:
    import Tkinter as tk
    from Tkinter import *
    from Tkinter import ttk
import os, sys
import json
from PIL import Image, ImageTk
from tkinter.messagebox import showerror, showwarning, showinfo


import webbrowser

#FILES import:
import classes_FRAMES
import functions
import class_Distributions
import class_GeneralSettings
import MAIN_APP


## Variables
HUGE_FONT = ("Verdana",22)
LARGE_FONT = ("Verdana",10) #Font and size.
NORM_FONT = ("Verdana",6) #Font and size.
SMALL_FONT = ("Verdana",4) #Font and size.
RES_FONT = ('Courier',8)
font = "Helvetica 10"
bgColor = 'azure'
darkColor ='lightseagreen'
lightColor = '#00A3E0'
extraColor = '#183A54'
selectorColor = 'aliceblue'

# Globals
experiments = {}
selectedtab = ''
       

### FUNCTIONS:
def popupmsgwarning(text):
    Tk().withdraw()
    showwarning(title = 'Warning',message=text)
##    showinfo(title = 'Info',message=text)
##    showerror(title = 'Error',message=text)
    

########
####### MAIN CLASS:
########

class KORALI(tk.Tk): #Inherited tk.tk

    # Initializing:
    def __init__(self,*args,**kwargs): # Self is implied, you don't need to pass self, but is a must.
        tk.Tk.__init__(self,*args,**kwargs)
 
##        tk.Tk.wm_title(self,'KORALI')


        ## MAIN TOOLBAR ----------
        # FILE MENU:
        self.menubar = tk.Menu(self) # Menu in the container.
        self.homemenu = tk.Menu(self.menubar, tearoff=0) # Tearoff = if clicking in the dashedline, we can make it its own window.
        self.homemenu.add_separator() # Separator bar.
        self.homemenu.add_command(label='Exit',command = quit)
        self.homemenu.add_command(label = 'Save As...', command = lambda:functions.exportFile())
        self.menubar.add_cascade(label='File',menu = self.homemenu)
        
        # EXPERIMENT SETTINGS MENU:
        self.experimentMenu = tk.Menu(self.menubar, tearoff = 0)
        self.menubar.add_cascade(label = 'Settings', menu = self.experimentMenu)

        self.newMenu = tk.Menu(self.experimentMenu, tearoff = 0)
        self.experimentMenu.add_cascade(label = 'New...', menu = self.newMenu)
        self.experimentMenu.add_separator()
        
        self.deleteMenu = tk.Menu(self.experimentMenu, tearoff = 0)
        self.experimentMenu.add_cascade(label = 'Delete...', menu = self.deleteMenu)
        self.experimentMenu.add_separator()

        self.newMenu.add_command(label = 'Experiment', command = lambda:self.crearTab(self.totalTabs,experiments,selectedtab))
        self.newMenu.add_command(label = 'Distribution', command = lambda: class_Distributions.Distributions)

        self.deleteMenu.add_command(label = 'Experiment', command = lambda:self.deleteTab(self.totalTabs))

        self.experimentMenu.add_command(label = 'Export File As...', command = lambda:functions.exportFile())
        self.experimentMenu.add_command(label = 'Import File...', command = lambda: functions.importFile(self))


        # HELP MENU:
        self.helpmenu = tk.Menu(self.menubar, tearoff = 0)
        self.helpmenu.add_command(label='Tutorial', command = self.tutorial)
        self.menubar.add_cascade(label='Help', menu = self.helpmenu)

        tk.Tk.config(self, menu = self.menubar)
        ## END MAIN TOOLBAR ----------

        ## NOTEBOOK TO SET THE TABS:  # This is the bar where the tabs will be attached.
        s = ttk.Style()
        s.configure('TNotebook.Tab', font=('Arial','14','bold'), foreground='#183A54',background = 'cadetblue')
        self.totalTabs = ttk.Notebook(self)
        self.totalTabs.pack(expand = 1, fill = "both")
               
        ## DOWN-TOOLBAR
        self.toolbar = tk.Frame(self, background='darkcyan')

        self.space = tk.Label(self.toolbar, text='',font='Helvetica 12 bold', fg='darkcyan',bg='darkcyan')
        self.space.pack(side='right',padx=50,pady=1) # Padding options.
        self.space.pack_propagate(0)

        self.symbol = tk.Label(self.toolbar, text='©',font='Helvetica 16 bold', fg='white',bg='darkcyan')
        self.symbol.pack(side='right',padx=2,pady=1) # Padding options.
        self.symbol.pack_propagate(0)
        
        self.CSELab = tk.Label(self.toolbar, text='Copyright 2020 by Mark Martori Lopez',font='Helvetica 13', fg='white',bg='darkcyan')
        self.CSELab.pack(side='right',padx=2,pady=1) # Padding options.
        self.CSELab.pack_propagate(0)
        
        self.toolbar.pack(side='bottom', fill='x')
        ## END DOWN TOOLBAR

        self.crearTab(self.totalTabs,experiments,selectedtab)             
    
    # In Which Experiment are we? selectedtab.
    def on_tab_selected(self,event):
        global selectedtab
        tabb = event.widget.select()
        selectedtab = event.widget.tab(tabb,'text')       

    def getNextExperimentName(self,totalTabs):
        for n in range(1,8): # 7 MAX ALLOWED NUM EXPERIMENTS.
            existe = False
            exp_name = 'Experiment '+str(n)
            for a in range(0,len(totalTabs.tabs())):
                exp = totalTabs.tab(a,"text")
                if exp == exp_name:
                    existe = True
                    break
            if existe == False:
                return n
        return -1

        
    #### START - TUTORIAL
    def tutorial(self):
        def openweb(url):
            webbrowser.open(url,new=1)
        def page2():
            #tut.destroy()
            tut2 = tk.Toplevel()
            tut2.geometry('600x600+650+100')
            tut2.minsize("600","400")
            tut2.maxsize("600","400")
            tut2.wm_title('KORALI - Help - Import')
            frame_tut = tk.Frame(tut2,width=600,height=400, bg='snow')
            frame_tut.pack()

            label = tk.Label(frame_tut, text='How do I import a file?',fg='black', font = 'Arial 25 bold',background='snow')
            label.place(x=115,y=30)
            text = tk.Label(frame_tut, text ="Korali provides an option to import a '.config' file:\n"+
                            "'Settings -> Import File...'.\n\n\n"+
                            "However, the number and the order of the variables \n"+
                            "inside the '.config' file, must follow the same \n"+
                            "structure as displayed on the KORALI App.",font = 'Arial 16',
                            background = 'snow', fg='black')
            text.place(x=55,y=100)
            tut2.mainloop()

        def page3():
##            tut.destroy()
            tut3 = tk.Toplevel()
            tut3.geometry('600x600+350+80')
            tut3.minsize("600","400")
            tut3.maxsize("600","400")
            tut3.wm_title('KORALI - Help - Export')
            frame_tut = tk.Frame(tut3,width=600,height=400, bg='snow')
            frame_tut.pack()
            frame_tut.pack_propagate(0)
            label = tk.Label(frame_tut, text='How do I export the data?',fg='black', font = 'Arial 25 bold',background='snow')
            label.place(x=105,y=30)
            text = tk.Label(frame_tut, text ="Korali provides an option to Export the current data:\n"+
                            "'Settings -> Export File As...'.\n\n\n"+
                            "However, the file can only be saved as:\n"+
                            "'.config', '.txt' or '.log'.",
                font = 'Arial 16', background = 'snow', fg='black')
            text.place(x=55,y=100)
            tut3.mainloop()
            
        tut = tk.Toplevel()
        tut.geometry('1300x900+350+80')
        tut.minsize("1300","900")
        tut.maxsize("1300","900")
        tut.wm_title('KORALI - Help')
        frame_tut = tk.Frame(tut,width=1300,height=900, bg='snow')
        frame_tut.pack()
        
        canvas = tk.Canvas(frame_tut, width=1300, height=900)
        img = ImageTk.PhotoImage(Image.open('images_KORALI/korali_screen.png').resize((1300, 1100), Image.ANTIALIAS))
        canvas.background = img  # Keep a reference in case this code is put in a function.
        bg = canvas.create_image(0, 0, anchor=tk.NW, image=img)
        canvas.pack()
        frame_tut.pack_propagate(0)

        label = tk.Label(canvas, text='What do you need help with?', font = 'Arial 30 bold',background='white')
        label.place(x=350,y=30)

        text = tk.Label(canvas, text = 'Korali is a high-performance framework for uncertainty quantification of computational models. Korali\n provides a scalable engine enables sampling and optimization on large-scale HPC systems, and\n a multi-language interface allows the execution of multiple computational models, either\n sequential or distributed (MPI), C++ or Python, and pre-compiled/legacy applications.',
                        font = 'Arial 16', background = 'white', fg='darkgray')
        text.place(x=150,y=100)
        B1 = tk.Button(canvas, text = ' - Overview of the application',width = 25,fg =extraColor,highlightcolor=selectorColor,
                               borderwidth = 0, background = selectorColor, anchor = 'w',activeforeground='teal',
                               activebackground= selectorColor,font= 'Arial 20 bold', command = lambda: openweb('https://www.cse-lab.ethz.ch/korali/#features'))
        B1.place(x=150,y=300)
        B2 = tk.Button(canvas, text = ' - How do I import a file?',width = 25,fg =extraColor,highlightcolor=selectorColor,
                               borderwidth = 0, background = selectorColor, anchor = 'w',activeforeground='teal',
                               activebackground= selectorColor,font= 'Arial 20 bold', command = page2)
        B2.place(x=750,y=300)
        B3 = tk.Button(canvas, text = ' - How do I export the data?',width = 25,fg =extraColor,highlightcolor=selectorColor,
                               borderwidth = 0, background = selectorColor, anchor = 'w',activeforeground='teal',
                               activebackground= selectorColor,font= 'Arial 20 bold', command = page3)
        B3.place(x=150,y=600)

        B3 = tk.Button(canvas, text = ' - CSE-Lab Main Website',width = 25,fg =extraColor,highlightcolor=selectorColor,
                               borderwidth = 0, background = selectorColor, anchor = 'w',activeforeground='teal',
                               activebackground= selectorColor,font= 'Arial 20 bold', command = lambda: openweb('https://www.cse-lab.ethz.ch/'))
        B3.place(x=750,y=600)

        version = tk.Label(canvas,text = MAIN_APP.version, font='Times 12 bold', fg='black',background='white')
        version.place(x=575,y=850)

        canvas.pack_propagate(0)
        tut.mainloop()
    ##### END - TUTORIAL

    # Call the 3 main frames and displays the main frame attached to the window.
    def crearTab(self,totalTabs,experiments,selectedtab):
        print('Creating a NEW experiment !! \n')
        print('This is the experiment Dictionary : \n')
        for keys in experiments.keys():
            print(experiments[keys])

        general_first_time = True
        self.tab = tk.Frame(totalTabs,bg='gray')
        position = self.getNextExperimentName(totalTabs)
        if position == -1:
            functions.popupmsgwarning('Number of experiments exceeded!')
            return
        selectedtab = 'Experiment '+str(position)
        experiments[selectedtab] = {}

        experiments[selectedtab]['times_var'] = 1
        experiments[selectedtab]['general_first_time'] = True
        experiments[selectedtab]['dist_first_time'] = True
        experiments[selectedtab]['dist_times'] = 1
        experiments[selectedtab]['rows'] = 0
        experiments[selectedtab]['first_time_p'] = True
        experiments[selectedtab]['first_time_s'] = True
        experiments[selectedtab]['variables_accessible'] = True
        experiments[selectedtab]['frame_problem'] = ''
        experiments[selectedtab]['frame_solver'] = ''
        experiments[selectedtab]['problems_ind'] = ''
        experiments[selectedtab]['solvers_ind'] = ''
        experiments[selectedtab]['ro_var'] = 4
        experiments[selectedtab]['rows_counter_var'] = 0
        experiments[selectedtab]['linktoVariables'] = []
        experiments[selectedtab]['linktoVariables0'] = ''
        experiments[selectedtab]['linktoVariables1'] = ''
        
        self.totalTabs.add(self.tab, text = selectedtab)
        self.totalTabs.bind('<<NotebookTabChanged>>', self.on_tab_selected)
        classes_FRAMES.FirstFrame(self.tab,selectedtab,experiments)
        classes_FRAMES.SecondFrame(self.tab,selectedtab,experiments)
        classes_FRAMES.ThirdFrame(self.tab,selectedtab,experiments)
        last = len(totalTabs.tabs())
        self.totalTabs.select(last-1)
       
        
    def deleteTab(self,totalTabs):
        if len(totalTabs.tabs()) >1:
            for item in totalTabs.winfo_children():
                if str(item)==totalTabs.select():
                    item.destroy()
                    return
        else:
            popupmsgwarning('At least 1 Experiment must remain.')

## --------------- END OF CLASSES ------------------------    
########################################################    
