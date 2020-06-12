import tkinter as tk
from tkinter import *
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


## Main Variables:
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
##general_first_time = True

contador = 0
##times_dist = 1
times_var = 1
general_first_time = True
dist_first_time = True
dist_times = 1
rows = 0
first_time_p = True
first_time_s = True

variables_accessible = True


experiments = {}
selectedtab = ''

### MAIN FUNCTIONS:
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
 
        tk.Tk.wm_title(self,'KORALI')

##        global times_dist
        global times_var
        global experiments
        global selectedtab
        

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
        self.newMenu.add_command(label = 'Variable', command = lambda: print('New Variable have not been done yet'))
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
        
    def on_tab_selected(self,event):
        global selectedtab
        tabb = event.widget.select()
        selectedtab = event.widget.tab(tabb,'text')

    def tutorial(self):
        def openweb(url):
            webbrowser.open(url,new=1)
        def page2():
##            tut.destroy()
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
            text = tk.Label(frame_tut, text ="Korali provides an option to Export the data provided:\n"+
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
        img = ImageTk.PhotoImage(Image.open('images_KORALI/korali_screen.png').resize((1300, 1100), Image.ANTIALIAS)) # resize((1083, 930) 
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

        version = tk.Label(canvas,text = MAIN_APP.version, font='Times 14 bold', fg='black',background='white')
        version.place(x=575,y=850)

        canvas.pack_propagate(0)
        tut.mainloop()

    def crearTab(self,totalTabs,experiments,selectedtab):
        global contador
        contador+=1

        if contador < 7:
            general_first_time = True
            self.tab = tk.Frame(totalTabs,bg='gray')
            selectedtab = 'Experiment '+str(contador)
            experiments[selectedtab] = {}
            self.totalTabs.add(self.tab, text = selectedtab)
            self.totalTabs.bind('<<NotebookTabChanged>>', self.on_tab_selected)
            classes_FRAMES.FirstFrame(self.tab,selectedtab,experiments)
            classes_FRAMES.SecondFrame(self.tab,selectedtab,experiments)
            classes_FRAMES.ThirdFrame(self.tab,selectedtab,experiments)
            
        else:
            functions.popupmsgwarning('Number of experiments exceeded!')
            contador += 1
        
    def deleteTab(self,totalTabs):
        global contador
        if contador >2:
            for item in totalTabs.winfo_children():
                if str(item)==totalTabs.select():
                    item.destroy()
                    contador -=1
                    return
        else:
            popupmsgwarning('At least 2 experiments to delete 1')

    def hide(self):
        """
        hides main frame
        """
        print('HIDING')


class Information(tk.Toplevel):
    #----------------------------------------------------------------------
    def __init__(self,directorio,link):
        """Constructor"""
        tk.Toplevel.__init__(self)
        self.geometry("400x300")
        self.title("Information")
##        self.grab_set()
        print('hello')
        # create the button
        webview.create_window(directorio, link)
        webview.start()


##    def onClose(self):
##        """
##        closes the frame and sends a message to the main frame
##        """
##        self.destroy()
##        pub.sendMessage("otherFrameClosed", arg1="data")

## --------------- END OF CLASSES ------------------------    
########################################################    
