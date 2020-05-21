import tkinter as tk
from tkinter import *
from tkinter.messagebox import showerror, showwarning, showinfo

#FILES import:
import classes_FRAMES


## Main Variables:
HUGE_FONT = ("Verdana",22)
LARGE_FONT = ("Verdana",10) #Font and size.
NORM_FONT = ("Verdana",6) #Font and size.
SMALL_FONT = ("Verdana",4) #Font and size.
RES_FONT = ('Courier',8)
font = "Helvetica 10"


contador = 1



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

        ## MAIN TOOLBAR ----------
        # FILE MENU:
        self.menubar = tk.Menu(self) # Menu in the container.
        self.homemenu = tk.Menu(self.menubar, tearoff=0) # Tearoff = if clicking in the dashedline, we can make it its own window.
        self.homemenu.add_separator() # Separator bar.
        self.homemenu.add_command(label='Exit',command = quit)
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

        self.newMenu.add_command(label = 'Experiment', command = lambda:self.crearTab(self.totalTabs))
        self.newMenu.add_command(label = 'Variable', command = print('New Variable have not been done yet'))
        self.newMenu.add_command(label = 'Distribution', command = print('New Distribution have not been done yet'))

        self.deleteMenu.add_command(label = 'Experiment', command = lambda:self.deleteTab(self.totalTabs))

        # HELP MENU:
        self.helpmenu = tk.Menu(self.menubar, tearoff = 0)
        self.helpmenu.add_command(label='Tutorial', command = self.tutorial)
        self.menubar.add_cascade(label='Help', menu = self.helpmenu)

        tk.Tk.config(self, menu = self.menubar)
        ## END MAIN TOOLBAR ----------

        ## NOTEBOOK TO SET THE TABS:  # This is the bar where the tabs will be attached.
        s = ttk.Style()
        s.configure('TNotebook.Tab', font=('Arial','14','bold'), foreground='navy',background = 'cadetblue')#'slategray')
        self.totalTabs = ttk.Notebook(self)
        self.totalTabs.pack(expand = 1, fill = "both")
               
        ## DOWN-TOOLBAR
        self.toolbar = tk.Frame(self, background='darkcyan')
        self.insertButt = tk.Button(self.toolbar, text='Close KORALI', command=quit)
        self.insertButt.config(bd = 2)
        self.insertButt.pack(side='right',padx=2,pady=2) # Padding options.
        self.insertButt.pack_propagate(0)
        self.toolbar.pack(side='bottom', fill='x')
        ## END DOWN TOOLBAR

        self.crearTab(self.totalTabs)
        
    def on_tab_selected(self,event):
        global selectedtab
        tabb = event.widget.select()
        selectedtab = event.widget.tab(tabb,'text')
            
    def tutorial(self):
        def page2():
            tut.destroy()
            tut2 = tk.Tk()

            def page3():
                tut2.destroy() # Destroys page 2 and opens page 3:
                tut3 = tk.Tk()

                tut3.wm_title('Part 3!')

                label = ttk.Label(tut3, text = 'Part 3', font = NORM_FONT)
                label.pack(side='top', fill='x', pady=10)
                B1 = ttk.Button(tut3, text='Done!', command=tut3.destroy)
                B1.pack()
                tut3.mainloop()

            tut2.wm_title('Part 2!')
            label = ttk.Label(tut2, text = 'Part 2', font = NORM_FONT)
            label.pack(side='top', fill='x', pady=10)
            B1 = ttk.Button(tut2, text='Done!', command= page3)
            B1.pack()
            tut2.mainloop()

        tut = tk.Tk()
        tut.wm_title('Tutorial')
        label = ttk.Label(tut, text='What do you need help with?', font = NORM_FONT)
        label.pack(side='top', fill='x', pady=10)

        B1 = ttk.Button(tut, text = 'Overview of the application', command = page2)
        B1.pack()
        B2 = ttk.Button(tut, text = 'How do I get Days-timed data?', command = lambda:popupmsgwarning('Not yet complited')) # How do I trade???
        B2.pack()
        B3 = ttk.Button(tut, text = 'Graph Question/Help', command = lambda: popupmsgwarning('Not yet complited'))
        B3.pack()

        tut.mainloop()

    def crearTab(self,totalTabs):
        global contador
        global selectedtab

        if contador < 7:
            self.tab = tk.Frame(totalTabs,bg='gray')
            selectedtab = 'Experiment '+str(contador)
            self.totalTabs.add(self.tab, text = selectedtab)
            self.totalTabs.bind('<<NotebookTabChanged>>', self.on_tab_selected)
            self.selector_frame = classes_FRAMES.FirstFrame(self.tab)
            self.display_frame = classes_FRAMES.SecondFrame(self.tab)
            self.info_frame = classes_FRAMES.ThirdFrame(self.tab)
            
        else:
                    popupmsgwarning('Number of experiments exceeded!')
        contador += 1
        
    def deleteTab(self,totalTabs):
        global contador
        if contador >2:
            for item in totalTabs.winfo_children():
                if str(item)==totalTabs.select():
                    print(str(item))
                    item.destroy()
                    contador -=1
                    return
        else:
            popupmsgwarning('At least 2 experiments to delete 1')
       

## --------------- END OF CLASSES ------------------------    
########################################################    
