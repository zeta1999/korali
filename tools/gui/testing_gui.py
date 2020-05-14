import tkinter as tk
from tkinter import *
from tkinter import ttk # if python.7 Tkinter. ttk is like the CSS.
from tkinter.messagebox import showerror, showwarning, showinfo
import os, sys
import json
import copy                     # recursiveUpdate
from functools import partial # Callback to retrieve values from entries
from tkinter.filedialog import asksaveasfile, askopenfilename


# ************ VARIABLES *****************
HUGE_FONT = ("Verdana",22)
LARGE_FONT = ("Verdana",10) #Font and size.
NORM_FONT = ("Verdana",6) #Font and size.
SMALL_FONT = ("Verdana",4) #Font and size.
RES_FONT = ('Courier',8)
font = "Helvetica 10"

darkColor ='lightseagreen'
lightColor = '#00A3E0'
extraColor = '#183A54'

questions = ["Optimization","Sampling","Bayesian Inference","Hierarchical Bayesian Modeling"]
forbidden = ['Variables','Problem','Solver']

default = False # readirs function.

contador = 1 # Counts the number of experiments.
cont = 0     # Which frame are we in. Problem, Solver or Variables.
decision = '' # Yes or No in MenuOption
times_distr = 1 # Number of Distribution num1
configuration = "Configuration Settings"  # crearMainConfiguration parameter.

contadorVariables = 1 # Counts the number of variables.
linktoVariables = []  # Stores the information coming from Problem and Solver to update the Variable Frame.

menus = [] # Store which directories have already been read.
listofproblems = [] # Which directories are part of problem.
listofsolvers = []


experiments = {} # Global dictionary for the frames and experiments.
selectedtab = ''
selectedtab2 = ''
selectedtab3 = ''

on = 0  #

files = [('Config Files', '*.config'),('Log Document','*.log'), 
             ('Text Document', '*.txt')] 

mainPath = '../../source/modules/problem'
mainPath2 = '../../source/modules/solver'

printVariable = 'Configuration Settings'

#############################################################
## ******************* FUNCTIONS ****************************


# ******************* TUTORIAL ******************
def tutorial():
    
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
## -------------------------END OF TUTORIAL FUNCTION ----------------
     

############################### QUICK FUNCTIONS:
def popupmsgwarning(text):
    Tk().withdraw()
    showwarning(title = 'Error',message=text)

def getValOptionMenu(menu):
    global decision
    decision = menu.get()
    print(decision)

def validardigit(num):
    # Función para validar POSITIVOS Y NEGATIVOS.
    r = False
    if num == '' or num == '-':
        r = True
    elif num[0] == '-':
        if num[1::].isdigit():
            r= True
    else:
        if num.isdigit():
            r = True
        else:
            r = False
    return r

def validarstring(string_Var):
    r = False
    if string_Var.isalpha() or string_Var == '' or ' ' in string_Var:
        r = True
    return r

def on_tab_selected(event):
    global selectedtab
    tabb = event.widget.select()
    selectedtab = event.widget.tab(tabb,'text')
    

def explainDescription(description):
    tut = tk.Tk()
    half = len(description)/2
    tut.wm_title('Description')
    label = ttk.Label(tut, text= description[0:int(half)]+'\n'+description[int(half)::], font = LARGE_FONT)
    label.pack(side='top', fill='x', pady=10)
    tut.maxsize('950','600')

################
################ DELETE FUNCTIONS:
def Clear(frame):
    for item in frame.winfo_children():
        item.destroy()

def clearFrame():
    global experiments
    global selectedtab
    global contadorVariables

    
    frame = experiments[selectedtab]['canvas']
    frame2 = experiments[selectedtab]['rightFrame']
    frame3 = experiments[selectedtab]['bottomrightFrame']

    Clear(frame)
    Clear(frame2)
    for widget in frame3.winfo_children():
        Clear(widget)

def deleteTab(totalTabs):
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

def deleteVariable(self):
    global experiments
    global selectedtab
    
    totalTabs2 = experiments[selectedtab]['totalTabs2']
    #frame = experiments[selectedtab]['bottomrightFrame']
    results =  experiments[selectedtab]['results']
    ## Delete Tab and frame inside:
    if len(totalTabs2.tabs()) > 1:
        a = totalTabs2.select()
        selectedtab2Deleted = totalTabs2.tab(a, "text")
        if results[2][selectedtab2Deleted]:
             del results[2][selectedtab2Deleted]
        totalTabs2.forget(a)
        
    else:
        popupmsgwarning('At least 2 variables to delete 1')

def deleteDistribution(self):
    global experiments
    global selectedtab
    global times_distr
    
    totalTabs3 = experiments[selectedtab]['totalTabs3']
    #frame = experiments[selectedtab]['bottomrightFrame']
    results =  experiments[selectedtab]['results']
    ## Delete Tab and frame inside:
    if len(totalTabs3.tabs()) > 1:
        a = totalTabs3.select()
##        selectedtab3Deleted = totalTabs3.tab(a, "text")
        if results[4][times_distr-1]:
             del results[4][times_distr-1]
        totalTabs3.forget(a)
        
    else:
        popupmsgwarning('At least 2 distributions to delete 1')

#################
################# PRINTING FUNCTIONS:

def printdata(self,line, texto, description, fakedescription, r, c,cont,options):
    
    global experiments
    global selectedtab
    global selectedtab2

    results = experiments[selectedtab]['results']
    if cont == 2:
        color = 'floralwhite'
        res = results[cont][selectedtab2]
    elif cont ==0 or cont == 1:
        color = 'azure'
        res = results[cont]
    else:
        color = 'white'
        res = results[cont]

    
    double_Var = tk.DoubleVar()
    num = tk.IntVar()
    boolean = tk.BooleanVar()
    experiments[selectedtab]['results'] = results
    stringVar = {}
    self.grid_propagate(0)
    
    if 'vector' not in line:
        
        if 'string' in line or 'korali' in line: # == 'std::string
            self.label=tk.Label(self, text=texto, width = 17,justify='left', bg=color, anchor="w", font="Arial 12")
            self.label.grid(row=r, column=c,pady = 2,padx=6, sticky='w')
            if options == 'None':
                stringVar[texto] = tk.StringVar()
                res[texto] = tk.Entry(self,width=10,font='Arial 12',textvariable = stringVar[texto])
                res[texto].grid(row=r, column=c+1, pady= 2,padx=6, sticky = 'w')
                self.corrector = self.register(validarstring)
                res[texto].config(validate = 'focus',validatecommand = (self.corrector,'%P')) # %P represents the parameter we want to pass to validate.
            else:
                stringVar[texto] = tk.StringVar()
                stringVar[texto].set("default") # default value
                listoptions = []
                for option in options:
                    listoptions.append(option['Value'])
                optionMenu = OptionMenu(self,stringVar[texto],*listoptions)#,command = lambda _:getValOptionMenu(stringVar[texto]))
                optionMenu.grid(row=r, column = c+1, pady=2, padx=6)
                res[texto] = stringVar[texto]
                self.buttonLabel = tk.Button(self, text = fakedescription,font='Arial 11', width = 20, activeforeground = 'darkcyan', bg = color ,activebackground = 'white',
                                                command = lambda:explainDescription(description))
                self.buttonLabel.grid(row=r, column=c+3,sticky='w',pady=2,padx=6)

            
        elif line == 'double':
            self.label=tk.Label(self, text=texto, width = 17,justify='left', bg=color, anchor="w", font="Arial 12")
            self.label.grid(row=r, column=c,pady = 2,padx=6, sticky='w')
            res[texto] = Spinbox(self, width=10, from_=0,font='Arial 12', to=9999, wrap=True, textvariable=num, state='normal')
            res[texto].grid(row=r, column=c+1, pady= 2,padx=6)
            self.corrector = self.register(validardigit)
            res[texto].config(validate = 'key',validatecommand = (self.corrector,'%P')) # %P represents the parameter we want to pass to validate.
            #spinbox_value = spinbox.get()
            
        elif line == 'bool':
            self.label=tk.Label(self, text=texto, width = 17,justify='left',bg=color, anchor="w", font="Arial 12")
            self.label.grid(row=r, column=c,pady = 2,padx=6, sticky='w')
            stringVar[texto] = tk.StringVar()
            stringVar[texto].set("default") # default value
            om = OptionMenu(self, stringVar[texto],"Yes", "No")#,command = lambda _:getValOptionMenu(stringVar[texto]))
            om.grid(row=r, column = c+1, pady=2, padx=6)
            res[texto] = stringVar[texto]

##            self.label=tk.Label(self, text=texto, width = 17,justify='left',bg=color, anchor="w", font="Arial 12")
##            self.label.grid(row=r, column=c,pady = 2,padx=4, sticky='w')
##            radiobutton = tk.Radiobutton(self, width=10,text='Yes' , font='Arial 12',highlightbackground = color,activeforeground ='white', bg=color,
##                                        activebackground='darkcyan',borderwidth = 0,variable = boolean, value=True)
##            radiobutton.grid(row=r, column=c+1, columnspan = 1,pady= 2, sticky = 'w')
##            
##            radiobutton = tk.Radiobutton(self, width=10,text='No' , font = 'Arial 12', highlightbackground = color,activeforeground ='white', bg=color,
##                                        activebackground='darkcyan',borderwidth = 0,variable = boolean, value=False)
##            radiobutton.grid(row=r, column=c+2, columnspan = 1,pady= 2, sticky = 'w')
##            res[texto] = boolean
             
        elif line == 'size_t':
            self.label=tk.Label(self, text=texto, width = 17,justify='left', bg=color, anchor="w", font="Arial 12")
            self.label.grid(row=r, column=c,pady = 2,padx=6, sticky='w')
            stringVar[texto] = tk.StringVar()
            res[texto] = tk.Entry(self,width=10,textvariable = stringVar[texto], font = 'Arial 12')
            res[texto].grid(row=r, column=c+1, pady=2,padx=6, sticky = 'w')
            self.corrector = self.register(validarstring)
            res[texto].config(validate = 'focus',validatecommand = (self.corrector,'%P')) # %P represents the parameter we want to pass to validate.
            self.buttonLabel = tk.Button(self, text = fakedescription, font = 'Arial 11', width = 20, activeforeground = 'darkcyan', bg = color,activebackground = 'white',
                                            command = lambda:explainDescription(description))
            self.buttonLabel.grid(row=r, column=c+3,sticky='w',pady=2,padx=6)
            
            
    else:
        if line =="std::vector<size_t>":
            
            self.label=tk.Label(self, text=texto, width = 17,justify='left', bg=color, anchor="w", font="Arial 12")
            self.label.grid(row=r, column=c, sticky='w', pady = 2,padx=6)
            stringVar[texto] = tk.StringVar()
            res[texto] = tk.Entry(self,width=10,textvariable = stringVar[texto], font = 'Arial 12')
            res[texto].grid(row=r, column=c+1,sticky = 'w', pady = 2,padx=6)
            self.corrector = self.register(validarstring)
            res[texto].config(validate = 'focus',validatecommand = (self.corrector,'%P')) # %P represents the parameter we want to pass to validate.
            self.buttonLabel = tk.Button(self, text = fakedescription,font = 'Arial 11', width = 20, activeforeground = 'darkcyan', bg = color,activebackground = 'white',
                                            command = lambda:explainDescription(description))
            self.buttonLabel.grid(row=r, column=c+3,sticky='w' , pady=2,padx=6)
        
        if 'double' in line:
            self.label=tk.Label(self, text=texto, width = 17,justify='left', bg=color, anchor="w", font="Arial 12")
            self.label.grid(row=r, column=c,pady = 2,padx=6,sticky='w')
            stringVar[texto] = tk.StringVar()
            res[texto] = tk.Entry(self,width=10,font='Arial 12',textvariable = stringVar[texto])
            res[texto].grid(row=r, column=c+1, pady= 2,padx=6, sticky = 'w')
            self.corrector = self.register(validardigit)
            self.buttonLabel = tk.Button(self, text = fakedescription, width = 20,font='Arial 11', activeforeground = 'darkcyan', bg = color,activebackground = 'white',
                                            command = lambda:explainDescription(description))
            self.buttonLabel.grid(row=r, column=c+3,sticky='w',pady=2,padx=6)

def printVariables(self,directorio,DB,cont):
    '''
    linktoVariables refers to the .config path of each element of the cascade. This path also works
    as a key in 'variables' dictionary, and as a value, we can find their variables configuration.
    '''
    global on
    global variables
    global linktoVariables
    global linktoVariables0
    global linktoVariables1
    global experiments
    global selectedtab2
    global selectedtab

    self = experiments[selectedtab]['bottomrightFrame']
    self = self.tab2
    
    # IF AND ONLY IF WE HAVE A PROBLEM AND A SOLVER SET...
    if (cont == 0 and on == 1) or (cont == 0 and on == 0):
        linktoVariables0 = DB[directorio]['config']
        on += -1
    elif cont == 0 and on == -1:
        linktoVariables0 = DB[directorio]['config']
    elif (cont == 1 and on == -1) or (cont == 1 and on == 0):
        linktoVariables1 = DB[directorio]['config']
        on += 1 
    elif cont == 1 and on == 1:
        linktoVariables1 = DB[directorio]['config']

    linktoVariables = []
    if on == 0:
        if linktoVariables0:
            linktoVariables.append(linktoVariables0)    # Añadimos las variables que vienen del Problem.
        if linktoVariables1:
            linktoVariables.append(linktoVariables1)    # Añadimos las variables que vienen del Solver.
        cont = 2
        # Clean the frame:
        for widget in self.winfo_children():
            widget.destroy()
        results = experiments[selectedtab]['results']
        # Coger nombre variable:
        results[cont] = {}
        results[cont][selectedtab2] = {}          
        ro = 0
        co = 0
        for part in linktoVariables:
            for llave in variables[part]:
                for var in llave.keys():
                    if var == 'Type':
                        line = llave['Type']
                        # llave son las llaves del diccionario, por ejemplo: Type, Function, Description, Produced By...
                        texto = llave['Name']
                        texto = texto[0] # Remove the '{}' from the label name.
                        description = llave['Description']
                        if len(description)>45:
                            fakedescription = str(description[0:42])+'...'
                        else:
                            fakedescription = description
                        options = 'None'
                        printdata(self,line, texto, description,fakedescription, ro, co, cont,options)
                        ro+=1
    '''
        for part in linktoVariables:
            for llave in variables[part].keys():
                #print(llave)
                if llave == 'Type':
                # llave son las llaves del diccionario, por ejemplo: Type, Function, Description, Produced By...
                    texto = variables[part]['Name']
                    texto = texto[0] # Remove the '{}' from the label name.
                    description = variables[part]['Description']
                    if len(description)>45:
                        fakedescription = str(description[0:42])+'...'
                    else:
                        fakedescription = description

                    for widget in self.winfo_children():
                        print(widget)
                        widget.destroy()
                    printdata(self,variables[part][llave], texto, description,fakedescription, ro, co, cont)
                ro+=1
        '''
    

def printConfig(directorio,DB, cont):
    
    global selectedtab
    global experiments

    print('REACHED')
    # From which cascade do we come from? Problem, Solver or Variable?
    if cont == 0:
        self = experiments[selectedtab]['rightFrame']
    elif cont == 1:
        self = experiments[selectedtab]['canvas']
    #elif cont == 2:
    #    self = experiments[selectedtab]['bottomrightFrame']
    #####

    ## DELETE ANY WIDGET THAT WAS THERE BEFORE:
    for widget in self.winfo_children():
        widget.destroy()
    results = experiments[selectedtab]['results']
    results[cont] = {}
    # Set variables:
    r = 0
    c = 0


    # Start reading and printing on screen: # DON'T DELETE THIS COMMENT -> TITULO FRAMES : list(DB.keys())[0]
    self.titulo=tk.Label(self, text=directorio,justify='left', anchor="w", font="Arial 18", bg = 'darkcyan', fg='white')
    self.titulo.grid(row=r, column=c,pady = 10 ,padx=20, sticky='w', columnspan = 2)


    chosen = self.titulo.cget('text')
    results[cont]['Type']= chosen

    config = DB[directorio]['herencia']
    conf_sett = 'Configuration Settings'
    
    if conf_sett in config.keys():
    # Loop through herencia.
        for key in config.keys():
            if key == conf_sett:
                lista = config[key]
                for dicc in lista:
                    if type(dicc) == dict:
                        if 'Type' not in dicc.keys():
                            popupmsgwarning("No 'Type' field found")
                            break
                        else:  
                            for key2 in dicc.keys():
                                if key2 == 'Type':
                                    # key2 son las llaves del diccionario, por ejemplo: Type, Function, Description, Produced By...
                                    texto = dicc['Name']
                                    texto = texto[0] # Remove the {} from the label name.
                                    description = dicc['Description']
                                    try:
                                        options = dicc['Options'] # options = list of diccionaries.
                                    except:
                                        options = 'None'

                                    fakedescription = description
                                    printdata(self,dicc[key2], texto, description,fakedescription, r, c,cont,options)                                    
                                r+=1
    else:
        popupmsgwarning('No Configuration Settings found')
    '''
    if cont == 1:
                bottomleftFrame = experiments[selectedtab]['bottomleftFrame']
                canvas = experiments[selectedtab]['canvas']
                
                hbar=Scrollbar(bottomleftFrame,orient=HORIZONTAL)
                hbar.pack(side=BOTTOM,fill=X)
                hbar.config(command=canvas.xview)
                vbar=Scrollbar(bottomleftFrame,orient=VERTICAL)
                vbar.pack(side=RIGHT,fill=Y)
                vbar.config(command=canvas.yview)
                canvas.config(width=950,height=850)
                canvas.config(xscrollcommand=hbar.set, yscrollcommand=vbar.set)
                canvas.pack(side=LEFT,expand=True,fill=BOTH)
                canvas.pack_propagate(0)
    '''
##########################
########################## MAIN FUNCTIONS:
def insertValues(results,cont,values):
    
    numofentry = 0
    for key in results[cont].keys():
        e = results[cont][key]
        if isinstance(e,str):
            del values[numofentry]
            continue
        if isinstance(e,BooleanVar):
            value = values[numofentry]
            e.set(value)
        elif isinstance(e,StringVar):
            value = values[numofentry]
            e.set(value)
        else:
            default = e.get()
            if default == '0':
                e.delete(0,tk.END)
                e.insert(END,values[numofentry])
            else:
                e.insert(END,values[numofentry])
        numofentry+=1
                 
    
def importFile():
    global experiments
    global configTreeDB
    global solverDB
    global files
    global times_distr

    filename = askopenfilename(filetypes=files)
    parsedFile = ''
    try:
        file = open(filename,'r')
        parsedFile = json.load(file)
    except:
        popupmsgwarning('File is not readable')
    finally:
        file.close()
    bottomrightFrame = experiments[selectedtab]['bottomrightFrame']
    
    if parsedFile != '': # If file is readable
        valuesProblem = []
        valuesSolver = []
        valuesVariables = {}
        rightFrame = experiments[selectedtab]['rightFrame']
        results = experiments[selectedtab]['results']
        
        for directorio in parsedFile.keys(): # Loop through the keys of the config file.
            if directorio == 'Problem': # PROBLEM IMPORT
                cont = 0
                texto = parsedFile[directorio]['Type']
                printConfig(texto,configTreeDB,cont)
                printVariables(bottomrightFrame.tab2,texto,configTreeDB,cont)
                for key2 in parsedFile[directorio]:
                    valuesProblem.append(parsedFile[directorio][key2])
                insertValues(results,cont,valuesProblem)
                
            elif directorio == 'Solver':     # SOLVER IMPORT
                cont = 1
                texto = parsedFile[directorio]['Type']
                printConfig(texto,solverDB,cont)
                printVariables(bottomrightFrame.tab2,texto,solverDB,cont)
                for key2 in parsedFile[directorio]:
                    valuesSolver.append(parsedFile[directorio][key2])
                insertValues(results,cont,valuesSolver)
                   
            elif 'ariable' in directorio:  # VARIABLES IMPORT avoiding v in case there is also a V, same with ending (-s).
                valuesVariables = []
                cont = 2
                numofentry = 0
                v = 0 # If there are several Variables Tabs in the imported file...
                for dictionary in parsedFile[directorio]:
                    if v>0:
                        crearFrameVariables() # Create as many Variable Frames as necessary.
                        v+=1
                    v+=1
                    for key in dictionary.keys():
                        valuesVariables.append(dictionary[key])
                numofentry = 0       
                for key in results[cont].keys():
                    for key2 in results[cont][key]:
                        e = results[cont][key][key2]
                        e.insert(0,valuesVariables[numofentry])
                        numofentry+=1
             
            else:                                       ## expFrame
                if directorio == 'Distributions':       ## MAIN CONFIGURATION AND DISTRIBUTION IMPORT
                    r= 40
                    valuesDistr = []
                    times_distr = 1
                    for distribution in parsedFile[directorio]:
                        crearDistribution(r)
                        r+=6
                        for key in distribution.keys():
                            valuesDistr.append(distribution[key])
                    cont = 4
                    numofentry = 0
                    for key2 in results[cont].keys():
                        for key3 in results[cont][key2]:
                            e = results[cont][key2][key3]
                            default = e.get()
                            if default == '0':
                                e.delete(0,tk.END)
                            e.insert(0,valuesDistr[numofentry])
                            numofentry+=1
 
                elif directorio == 'Random Seed':
                    valuesRS = ''
                    valueRS = parsedFile[directorio]
                    cont = directorio
                    e = results[cont][directorio]
                    e.insert(0,valueRS)
                
                elif directorio == 'Console Output' or directorio == 'File Output':
                    valuesCO_FO = []
                    for key in parsedFile[directorio]:
                        valuesCO_FO.append(parsedFile[directorio][key])
                    numofentry = 0
                    cont = directorio
                    for key2 in results[cont].keys():
                        e = results[cont][key2]
                        if isinstance(e,StringVar):
                            value = valuesCO_FO[numofentry]
                            e.set(value)
                            numofentry+=1
                        else:
                            e.insert(0,valuesCO_FO[numofentry])
                            numofentry+=1
               
    else:
        popupmsgwarning('Could not import the file')

        '''   
            elif 'ariable' in directorio:  # VARIABLES IMPORT avoiding v in case there is also a V, same with ending (-s).
                cont = 2
                numofentry = 0
                if len(parsedFile[directorio]) >1:
                    v = 0 # If there are several Variables Tabs in the imported file...
                    for variableTab in parsedFile[directorio].keys():
                        if v>0:
                            crearFrameVariables()
                            v+=1
                        valuesVariables[variableTab] = []
                        line = parsedFile[directorio][variableTab]
                        v+=1
                        for dictionary in line:
                            for key in dictionary:
                                valuesVariables[variableTab].append(dictionary[key])
            
                else:
                    for variableTab in parsedFile[directorio].keys():
                        valuesVariables[variableTab] = []
                        line = parsedFile[directorio][variableTab]
                        for dictionary in line:
                            for key in dictionary:
                                valuesVariables[variableTab].append(dictionary[key])
                                
                for key in results[cont].keys():
                    for varkey in valuesVariables.keys():
                            if key == varkey:
                                numofentry = 0
                                for key2 in results[cont][key]:
                                    e = results[cont][key][key2]
                                    e.insert(0,valuesVariables[varkey][numofentry])
                                    numofentry+=1
        '''          
         
        
        '''
        for frame in parsedFile.keys():
            for texto in parsedFile[frame]:
                w = parsedFile[frame][texto]
                val = w.get()
                results2[frame][texto] = valprint(valuesProblem, valuesSolver)
        '''
           
def createConfig():
    global selectedtab # To know at which experiment we are.
    global experiments
    global files # Types of files.

    results = experiments[selectedtab]['results']
    ### Avoiding that the user clicks create.config without choosing a problem/solver
    if 2 not in results.keys():
        popupmsgwarning('First choose a problem and a solver')
        return

    bottomleftFr = experiments[selectedtab]['canvas']
    rightFr = experiments[selectedtab]['rightFrame']
    variableFr = experiments[selectedtab]['totalTabs2']

    # Copy results dictionary into results2
    results2 = {}
    for key in results:
         results2[key] = results[key].copy()
         
    results2['Problem'] = results2.pop(0)
    results2['Solver'] = results2.pop(1)
    results2['Variables'] = results2.pop(2)
    results2['Distributions'] = results2.pop(4)
    
    for frame in results2.keys():
        if frame == 'Distributions' or frame == 'Variables':
            res = []
            for distr in results2[frame]:
                for texto in results2[frame][distr]:
                    w = results2[frame][distr][texto]
                    val = w.get()
                    results2[frame][distr][texto] =val
                distribution = results2[frame][distr]
                res.append(distribution)
                
##                if results2[frame] == distr:
##                    results2[frame] == di
            results2[frame] = res
        else:
            for texto in results2[frame]:
                w = results2[frame][texto]
                if texto == 'Random Seed':
                    val = w.get()
                    results2[frame] = val
                elif isinstance(w,str):
                    results2[frame][texto] = w
                else:
                    val = w.get()
                    results2[frame][texto] = val

    #### ASK TO DOWNLOAD AND SAVE THE FILE:
    filesaved = asksaveasfile(filetypes = files, defaultextension = files)
    json.dump(results2, filesaved, indent = 3)
    filesaved.close()

def recursiveUpdate(dest, defaults):
 if (isinstance(defaults, dict)):               # Si default es un diccionario...
  for k, x in defaults.items():                 # Para cada key y valor.
   if (not k in dest): dest[k] = copy.deepcopy(defaults[k])
   else: 
     recursiveUpdate(dest[k], defaults[k])
 if (isinstance(defaults, list)):
  for k in range(len(defaults)):
   if (not k in dest): dest.append(copy.deepcopy(defaults[k]))
   else: 
     recursiveUpdate(dest[k], defaults[k])


     
def splitPath(s):
    dirs = []
    dirName = ''
    for i in s:
        if i == os.path.sep:
            dirs += [dirName]
            dirName = ''
        else:
            dirName += i
    if len(dirName) > 0:
        dirs += [dirName]
    return dirs

def readDirs(filePath,DB,default):
    dirs = splitPath(filePath)
    levels=len(dirs)
    sublevel = levels + 1
    dirInfoDic={}
    childrenList=[]
    dest = False
    for (dirPath,dirNames,fileNames) in os.walk(filePath):
        if dirPath == filePath:
            for fileName in fileNames:
                if fileName.endswith('.config'):
                    dirName = dirs[levels-1]
                    DB[dirName] = dirInfoDic
                    dirInfoDic['config'] = filePath + os.path.sep + fileName
                    dirInfoDic['children'] = childrenList
                    try:
                        archivo = open(filePath+'/'+fileName,'r')
                        file = archivo.read()
                        dest = json.loads(file)
                        if default:
                            recursiveUpdate(dest,default)
                        dirInfoDic['herencia'] = dest
                        #print(dirInfoDic['config'])
                        #print(dirInfoDic['herencia'],'\n')
                    finally:
                        archivo.close()
                elif fileName.endswith('.rst'): # Dame el nombre dentro del README:
                    try:
                        file = open(filePath+'/'+fileName,"r")
                        lines = file.readlines()
                        for i in range(0,len(lines)):
                            line = lines[i]
                            if '****\n' in line or '===\n' in line:
                                name = lines[i+1]
                                dirInfoDic['names'] = name.replace('\n','') # Quitar '\n'
                                break
                            else:
                                pass
                    finally:
                        file.close() ## Cierra el archivo README.
            continue
        dirs=splitPath(dirPath)
        levels=len(dirs)
        if levels > sublevel:
            del dirNames[:]
            continue
        readDirs(dirPath, DB, dest)
        dirName = dirs[levels - 1]
        childrenList += [dirName]
    crearVariables(DB)
    
### CREAR EL DICCIONARIO DE VARIABLES COMBINANDO LOS DOS DICCIONARIOS ANTERIORES
def crearVariables(DB):
    for key in DB.keys():
        if 'Variables Configuration' in DB[key]['herencia'].keys():
            if DB[key]['herencia']['Variables Configuration']:
                variables[DB[key]['config']] = DB[key]['herencia']['Variables Configuration']
            else:
                variables[DB[key]['config']] = {}
        else:
            variables[DB[key]['config']] = {}            
             ######################   
                    
def crearMenu(padre,directorio,DB,cont):
    global menus
    global experiments
    global listofproblems
    global listofsolvers
    
    bottomrightFrame = experiments[selectedtab]['bottomrightFrame']
    nombre = DB[directorio]['names']
    if directorio not in menus: # Checkeamos si el directorio está en la lista de menus.
        subMenu = Menu(padre,activeforeground = 'teal')
        dirInfo=DB[directorio]
        if len(dirInfo['children']) == 0:
            padre.add_command(label = nombre, command = lambda : [printConfig(directorio,DB, cont),printVariables(bottomrightFrame.tab2,directorio,DB,cont)])
        else:
            padre.add_cascade(label=nombre, menu = subMenu)
            configPath=dirInfo['config']
            if configPath != "NULL" and len(dirInfo['children']) == 0: # SI ES UN LEAF:
                dirs=splitPath(configPath)
                subMenu.add_command(label = dirs[len(dirs)-1], command = lambda : [printConfig(directorio,DB, cont),printVariables(bottomrightFrame.tab2,directorio,DB,cont)])
            children=dirInfo['children']
            for child in children:
                crearMenu(subMenu,child,DB,cont)
    menus.append(directorio)
    if cont == 0:
        if directorio not in listofproblems:
            listofproblems.append(directorio)
    elif cont == 1:
        if directorio not in listofsolvers:
            listofsolvers.append(directorio)
            
def select_item():
    pass
def remove_item():
    pass
def add_item():
    pass

def cascade(mainPath,DB,cont):
    global experiments
    dirs = splitPath(mainPath)
    for directorio in DB.keys():
        if directorio in dirs:
            titulo = directorio
            break
    leftFrame = experiments[selectedtab]['leftFrame']
                
    menuButton = tk.Menubutton(leftFrame, text = DB[titulo]['names'], indicatoron=True, borderwidth = 1, relief='raised', width=20, border=3)
    menuPadre = tk.Menu(menuButton, tearoff=False,activeforeground = 'teal')
    menuButton.configure(menu=menuPadre)
    menuButton.grid(row=0, column = cont, pady=10, padx=10, rowspan = 3)
        
    menus.append(titulo) # Añadimos el título del boton, para no repetirlo en los desplegables.
    
      
    # Empezar los menus recursivamente llamando a la funcion crearMenu:
    for directorio in DB.keys():
        crearMenu(menuPadre,directorio,DB,cont)
    menus.clear() # Allows creating different experiments by emptying the menus list.

def crearFrameVariables():
    
    global contadorVariables
    global experiments
    global selectedtab
    global linktoVariables
    global variables
    global selectedtab2

##    print('first:',selectedtab2)
    
    bottomrightFrame = experiments[selectedtab]['bottomrightFrame']
    totalTabs2 = experiments[selectedtab]['totalTabs2']
    if len(totalTabs2.tabs()) == 0:
        bottomrightFrame.tab2 = tk.Frame(bottomrightFrame, height = 450, width = 300, background = 'floralwhite')#florawhite
        selectedtab2 = 'Variable '+str(contadorVariables)
        totalTabs2.add(bottomrightFrame.tab2, text = selectedtab2)
        contadorVariables  += 1
    #for item in self.totalTabs
    else:
        if linktoVariables == []:
            popupmsgwarning('First choose a Problem and a Solver')
        else:
            if contadorVariables < 15 :
                    ## Create Tab and frames inside:
                    bottomrightFrame.tab2 = tk.Frame(bottomrightFrame, height = 450, width = 300, background = 'floralwhite')#florawhite
                    selectedtab2 = 'Variable '+str(contadorVariables)
                    totalTabs2.add(bottomrightFrame.tab2, text = selectedtab2)
                    self = bottomrightFrame.tab2
                    cont = 2
                    results = experiments[selectedtab]['results']
                    # Coger nombre variable:
                    #variableFr = experiments[selectedtab]['totalTabs2']
                    results[cont][selectedtab2] = {}
                    ro = 0
                    co = 0
                    for widget in self.winfo_children():
                        widget.destroy()
                    for part in linktoVariables:
##                        print('This is the part:',part,'\n')
                        for llave in variables[part]:
##                            print('This is the llave:',llave,'\n')
                            for var in llave.keys():
##                                print('This is the var:',var,'\n')
                                if var == 'Type':
                                    getname=llave['Name']
                                    getdesc=llave['Description']
                                    line = llave['Type']
                                    # llave son las llaves del diccionario, por ejemplo: Type, Function, Description, Produced By...
                                    texto = getname
                                    texto = texto[0] # Remove the '{}' from the label name.
                                    description = getdesc
                                    if len(description)>45:
                                        fakedescription = str(description[0:42])+'...'
                                    else:
                                        fakedescription = description
                                    options = 'None'
                                    printdata(self,line, texto, description,fakedescription, ro, co, cont,options)
                            ro+=1
                    ## IT ALREADY ADDS THE NEW VARIABLE TO THE DICTIONARY OF RESULTS BY USING THE FUNCTION PRNTDATA.
            else:
                popupmsgwarning('Number of Variables exceeded!')
            contadorVariables+=1

    
def crearMainConfiguration(configuration,r,c,cont):
    global selectedtab
    
    mainConf = experiments[selectedtab]['mainConf']
    expFrame = experiments[selectedtab]['expFrame']
    results = experiments[selectedtab]['results']

    titulos = []
    for key in mainConf.keys():
        if key == configuration:
            lista = mainConf[key]
            for dicc in lista:
                if type(dicc) == dict:
                    if 'Type' not in dicc.keys():
                        popupmsgwarning("No 'Type' field found")
                        break
                    else:
                        for key2 in dicc.keys():
                            texto = dicc['Name']
                            if texto == 'Random Seed':
                                cont == 'Random Seed'
                            if len(texto)>1: 
                                titulo = texto[0]
                                texto = texto[1]
                                if titulo not in titulos:
                                    e2 =tk.Label(expFrame, text=titulo,justify='left', anchor="w", font="Arial 17", fg='black', bg='white') #bg = 'darkcyan', fg='white')
                                    e2.grid(row=r, column=c,columnspan= 1,pady = 25 ,padx=4, sticky='w')
                                    titulos.append(titulo)
                                    cont=titulo
                                    results[cont] = {}
                            else:
                                texto = texto[0] # Get the element inside list.
                            if texto in forbidden: # IFF Problem, Solver or Variables... skip
                                break
                            else:
                                if key2 == 'Type':
                                    # key2 son las llaves del diccionario, por ejemplo: Type, Function, Description, Produced By...
                                    description = dicc['Description']
                                    try:
                                        options = dicc['Options'] # options = list of diccionaries.
                                    except:
                                        options = 'None'
                                    fakedescription = description
                                    printdata(expFrame,dicc[key2], texto, description,fakedescription, r, c,cont,options)                                    
                                r+=1
def crearDistribution():
    global selectedtab
    global experiments
    global times_distr

    c=0
    r=0

    expFram = experiments[selectedtab]['distribution']
    expFrame = expFram.tab3
    results = experiments[selectedtab]['results']
    results[4][times_distr]={}
    res = results[4][times_distr]
    stringVar = {}

##    texto = 'Distribution '+str(times_distr)
##    l1 =tk.Label(expFrame, text=texto,justify='left', anchor="w", font="Arial 17", fg='black', bg='white') #bg = 'darkcyan', fg='white')
##    l1.grid(row=r, column=c,columnspan= 1,pady = 24 ,padx=4, sticky='w')
    
    texto1 = 'Name'
    l2 = tk.Label(expFrame, text = texto1,  width = 17,justify='left', bg='floralwhite', anchor="w", font="Arial 12")
    l2.grid(row=r+1, column=c,columnspan= 1,pady = 2 ,padx=4, sticky='w')
    stringVar[texto1] = tk.StringVar()
    res[texto1] = tk.Entry(expFrame,width=10,font='Arial 12',textvariable = stringVar[texto1])
    res[texto1].grid(row=r+1, column=c+1, pady= 2,padx=4, sticky = 'w')
    
    texto2 = 'Type'
    l3 = tk.Label(expFrame, text = texto2, width = 17,justify='left', bg='floralwhite', anchor="w", font="Arial 12")
    l3.grid(row=r+2, column=c,columnspan= 1,pady = 2 ,padx=4, sticky='w')
    stringVar[texto2] = tk.StringVar()
    res[texto2] = tk.Entry(expFrame,width=10,font='Arial 12',textvariable = stringVar[texto2])
    res[texto2].grid(row=r+2, column=c+1, pady= 2,padx=4, sticky = 'w')
    
    texto3 = 'Maximum'
    l4=tk.Label(expFrame, text=texto3, width = 17,justify='left', bg='floralwhite', anchor="w", font="Arial 12")
    l4.grid(row=r+3, column=c,pady = 2,padx=4, sticky='w')
    num3 = tk.IntVar()
    res[texto3] = Spinbox(expFrame, width=10, from_=0,font='Arial 12', to=9999, wrap=True, textvariable=num3, state='normal')
    res[texto3].grid(row=r+3, column=c+1, pady= 2,padx=6)
    corrector = expFrame.register(validardigit)
    res[texto3].config(validate = 'key',validatecommand = (corrector,'%P')) # %P represents the parameter we want to pass to validate.

    texto4 = 'Minimum'
    l5=tk.Label(expFrame, text=texto4, width = 17,justify='left', bg='floralwhite', anchor="w", font="Arial 12")
    l5.grid(row=r+4, column=c,pady = 2,padx=4, sticky='w')
    num4 = tk.IntVar()
    res[texto4] = Spinbox(expFrame, width=10, from_=0,font='Arial 12', to=9999, wrap=True, textvariable=num4, state='normal')
    res[texto4].grid(row=r+4, column=c+1, pady= 2,padx=6)
    corrector = expFrame.register(validardigit)
    res[texto4].config(validate = 'key',validatecommand = (corrector,'%P')) # %P represents the parameter we want to pass to validate.
    
    texto5 = 'Random Seed'
    l6=tk.Label(expFrame, text=texto5, width = 17,justify='left', bg='floralwhite', anchor="w", font="Arial 12")
    l6.grid(row=r+5, column=c,pady = 2,padx=4, sticky='w')
    num5 = tk.IntVar()
    res[texto5] = Spinbox(expFrame, width=10, from_=0,font='Arial 12', to=9999, wrap=True, textvariable=num5, state='normal')
    res[texto5].grid(row=r+5, column=c+1, pady= 2,padx=6)
    corrector = expFrame.register(validardigit)
    res[texto5].config(validate = 'key',validatecommand = (corrector,'%P')) # %P represents the parameter we want to pass to validate.

    times_distr +=1

def crearDistributionFrame():
    global experiments
    global selectedtab
    global selectedtab3
    global times_distr

    bottomleftFrame3 = experiments[selectedtab]['distribution']
    totalTabs3 = experiments[selectedtab]['totalTabs3']
    
    if times_distr < 5:
        bottomleftFrame3.tab3 = tk.Frame(bottomleftFrame3, height = 300, width = 520, background = 'floralwhite')#florawhite
        selectedtab3 = 'Distribution '+str(times_distr)
        totalTabs3.add(bottomleftFrame3.tab3, text = selectedtab3)
        crearDistribution()
        results = experiments[selectedtab]['results']
        print(results[4].keys())
    else:
        popupmsgwarning('Exceeded limit of distributions')


                         
    ## Crea los tabs de uno en uno
def crearTab(self,totalTabs):
    global contador
    global selectedtab
    global experiments
    global linktoVariables
    global forbidden
    global times_distr
        
    linktoVariables = []

    if contador < 7:
                ## Create Tab and frames inside:
                self.tab = tk.Frame(self.totalTabs)
##                self.tab.grid(row=0,column=0,rowspan=6,columnspan=3)
                self.buttonTab = tk.Button(self.tab,text = 'xxxx')
                self.buttonTab.grid(row=0,column=0, sticky='e')
                selectedtab = 'Experiment '+str(contador)
                self.totalTabs.add(self.tab, text = selectedtab)
                self.totalTabs.bind('<<NotebookTabChanged>>', on_tab_selected)

                ## LEFT FRAME:
                self.leftFrame = tk.Frame(self.tab, width = 1200, height = 50, background ='azure', borderwidth = 3, relief = 'solid')
                self.leftFrame.grid(column = 0, row = 0, sticky = 'nsew',rowspan = 1, columnspan = 2)
                self.button_done = tk.Button(self.leftFrame, text = 'Create .config file',command = lambda: createConfig(),
                                             activebackground = 'darkcyan',activeforeground='azure', bd = 3, bg = 'teal',fg = 'azure', font = 'Arial 12',
                                             width = 20)

                self.button_done.grid(row=0, column=3, pady= 20, padx=20, sticky = 'E')
                
                self.button_Import = tk.Button(self.leftFrame, text = 'Import .config file',command = lambda: importFile(),
                                             activebackground = 'darkcyan',activeforeground='azure', bd = 3, bg = 'teal',fg = 'azure', font = 'Arial 12',
                                             width = 20)

                self.button_Import.grid(row=0, column=4, pady= 20, padx=20, sticky = 'E')
                self.update_idletasks
                
                ## RIGHT FRAME:
                self.rightFrame = tk.Frame(self.tab, width = 650, height = 450,background ='azure', borderwidth = 1,relief = 'groove')
                self.rightFrame.grid(row = 0, column = 2, stick = 'nsew',rowspan = 3)

                ## BOTTOM LEFT FRAME:

                self.bottomleftFrame1 = tk.Frame(self.tab, background = 'azure', width = 480, height = 850, borderwidth = 1,relief = 'groove')#width = 950, height = 850
                self.bottomleftFrame1.grid(column = 0, row = 1, sticky = 'nsew',rowspan = 5,columnspan=1)
                self.bottomleftFrame1.grid_propagate(0)
                
                self.bottomleftFrame2 = tk.Frame(self.tab, background = 'white', width = 520, height = 550, borderwidth = 3,relief = 'groove')#width = 950, height = 850
                self.bottomleftFrame2.grid(column = 1, row = 1, sticky = 'nsew',rowspan = 4, columnspan = 1) 
                self.bottomleftFrame2.grid_propagate(0)

                self.bottomleftFrame3 = tk.Frame(self.tab, background = 'white', width = 520, height = 300, borderwidth = 3,relief = 'groove')#width = 950, height = 850
                self.bottomleftFrame3.grid(column = 1, row = 5, sticky = 'nsew',rowspan = 1, columnspan = 1) 
                self.bottomleftFrame3.grid_propagate(0)

                
                self.e =tk.Label(self.bottomleftFrame2, text='Main Configuration',justify='left', anchor="w", font="Arial 15", fg='black', bg='white') #bg = 'darkcyan', fg='white')
                self.e.grid(row=0, column=0,columnspan= 1,pady = 40 ,padx=4, sticky='w')
                

                ## BOTTOM RIGHT FRAME:
                self.bottomrightFrame = tk.Frame(self.tab, width = 650, height = 450, background ='azure', borderwidth = 1,relief = 'ridge')
                self.bottomrightFrame.grid(row = 3,column = 2,sticky = 'nsew', rowspan = 3)

                
                # STORE A DICTIONARY WITH ALL THE FRAMES ON IT TO BE CALLED ON ANY FUNCTION AND AVOID PASSING FRAMES.
                expElements = {}
                experiments[selectedtab] = expElements
                experiments[selectedtab]['results'] = {}
                
                expElements['leftFrame'] = self.leftFrame
                expElements['rightFrame'] = self.rightFrame
                expElements['bottomrightFrame'] = self.bottomrightFrame
                expElements['bottomleftFrame'] = self.bottomleftFrame1
                expElements['canvas'] = self.bottomleftFrame1
                expElements['expFrame'] = self.bottomleftFrame2
                expElements['distribution'] = self.bottomleftFrame3


                self.totalTabs2 = ttk.Notebook(self.bottomrightFrame)
                self.totalTabs2.pack(expand = 1, fill = "both")
               
                expElements['totalTabs2'] = self.totalTabs2

                crearFrameVariables()
                

                self.leftFrame.grid_propagate(0)
                self.rightFrame.grid_propagate(0)
                #self.bottomleftFrame.grid_propagate(0)
                self.bottomrightFrame.grid_propagate(0)
                self.bottomleftFrame1.grid_propagate(0)
                
                #############################################################

                try: # Read experiment.config 
                    file = open('../../source/modules/experiment/experiment.config','r')
                    mainConf = json.load(file)
                    cont = 'Random Seed'
                    results = experiments[selectedtab]['results']
                    results[cont] = {}
                except:
                    popupmsgwarning("There is not an 'Experiment.config' file !")
                finally:
                    file.close()

                expElements['mainConf'] = mainConf

                self.e =tk.Label(self.bottomleftFrame2, text='Main Configuration',justify='left', anchor="w", font="Arial 17", fg='black', bg='white') #bg = 'darkcyan', fg='white')
                self.e.grid(row=0, column=0,columnspan= 1,pady = 25 ,padx=4, sticky='w')

                c = 0
                r = 0
                crearMainConfiguration(configuration,r,c,cont)

                ######  DISTRIBUTIONS ######
                self.totalTabs3 = ttk.Notebook(self.bottomleftFrame3)
                self.totalTabs3.pack(expand = 1, fill = "both")
                expElements['totalTabs3'] = self.totalTabs3
                
                r=40
                results[4]={}
                times_distr = 1
                crearDistributionFrame()
                
##                self.distr_button = tk.Button(self.bottomleftFrame3, text = 'Add distribution', command = lambda:crearDistribution(r))
##                self.distr_button.grid(row=100,column=0, pady = 6, padx = 2)

                #############################################################
               # LEFT SIDE OF TAB1:
                
                ## CASCADE:
                # Elegir el título de los Menús:
                cont= 0
                cascade(mainPath,configTreeDB,cont)
                cont+=1
                cascade(mainPath2,solverDB,cont)
                contador += 1 # Controlar el número de experimentos que hay. Máximo 6 -> crearTab()
                
    else:
                popupmsgwarning('Number of experiments exceeded!')

        
## ****************** END OF FUNCTIONS **********************
        
#############################################################
## ******************* CLASSES ******************************

 
class KORALI(tk.Tk): #Inherited tk.tk

    # Initializing:
    def __init__(self,*args,**kwargs): # Self is implied, you don't need to pass self, but is a must.
        tk.Tk.__init__(self,*args,**kwargs)
 
        tk.Tk.wm_title(self,'KORALI')

        global selectedtab


        container = tk.Frame(self) # Edge of the window.
        container.pack(side = "top", fill = "both", expand = True) # Fill the entire space. Expand = If there is white space, you can expand it.


        
        # Barra de arriba:
        self.menubar = tk.Menu(self) # Menu in the container.
        self.homemenu = tk.Menu(self.menubar, tearoff=0) # Tearoff = if clicking in the dashedline, we can make it its own window.
        self.homemenu.add_command(label='Experiments',command = lambda: self.show_frame(KORALI_Page))
        self.homemenu.add_command(label='Second Part',command = lambda: self.show_frame(KORALI_Graph))
        self.homemenu.add_separator()
        self.homemenu.add_command(label='Save',command = lambda: popupmsgwarning('SAVE'))
        self.homemenu.add_command(label='Save as...',command = lambda: createConfig())
        self.homemenu.add_separator() # Separator baselr.
        self.homemenu.add_command(label='Import File...',command = lambda: importFile())
        self.homemenu.add_command(label='Export as...',command = lambda: createConfig())
        self.homemenu.add_separator() # Separator bar.
        self.homemenu.add_command(label='Exit',command = quit)
        self.menubar.add_cascade(label='Home',menu = self.homemenu)
        
        # EXPERIMENT MENU:
        self.experimentmenu = tk.Menu(self.menubar, tearoff = 1)
        self.experimentmenu.add_command(label = 'New Experiment...', command = lambda:crearTab(self,self.totalTabs))
        self.experimentmenu.add_command(label = 'New Variable...', command = lambda: crearFrameVariables())#,self.totalTabs2))
        self.experimentmenu.add_command(label = 'New Distribution...', command = lambda: crearDistributionFrame())#,self.totalTabs2))
        self.experimentmenu.add_separator() # Separator baselr.
        self.experimentmenu.add_command(label = 'Delete Experiment...', command = lambda: deleteTab(self.totalTabs))#self.totalTabs.forget(self.tab))#lambda:deletetab(self,self.totalTabs,contador))
        self.experimentmenu.add_command(label = 'Delete Variable...', command = lambda:deleteVariable(self))
        self.experimentmenu.add_command(label = 'Delete Distribution...', command = lambda:deleteDistribution(self))
        self.experimentmenu.add_separator() # Separator baselr.
        self.experimentmenu.add_command(label = 'Clear Experiment...', command = lambda:clearFrame())
        self.menubar.add_cascade(label = 'Experiments Configuration', menu = self.experimentmenu)

        ##### HELP MENU:
        self.helpmenu = tk.Menu(self.menubar, tearoff = 0)
        self.helpmenu.add_command(label='Tutorial', command = tutorial)
        self.menubar.add_cascade(label='Help', menu = self.helpmenu)
        

        tk.Tk.config(self, menu = self.menubar)


        self.totalTabs = ttk.Notebook(self)
        self.totalTabs.pack(expand = 1, fill = "both")
        
        # Call the function crearTab to create as many tabs as the user wants:       
        crearTab(self,self.totalTabs)

        ###################################       
        # DOWN-TOOLBAR
        self.toolbar = tk.Frame(self, background='darkcyan')
        
        self.insertButt = tk.Button(self.toolbar, text='Close KORALI', command=quit)
        self.insertButt.config(bd = 2)
        self.insertButt.pack(side='right',padx=2,pady=2) # Padding options.
        self.insertButt.pack_propagate(0)
        
        self.toolbar.pack(side='bottom', fill='x')
        #self.toolbar.pack_propagate(0)
'''
        ####################################
        self.frames = {} # Dictionary.

        for F in (StartPage,KORALI_Page, KORALI_Graph): # List of Pages.

            frame = F(container,self) #Pass through container, and self.
            self.frames[F] = frame 
            frame.grid(row=0, column=0, sticky='nsew') # Sticky = Strech everything to north,south,e,w..

        self.show_frame(StartPage)

    def show_frame(self,cont): # Cont = controler.
        frame = self.frames[cont]
        frame.tkraise() # Raise it to the front!


class KORALI_Page(tk.Frame): # Page with Graphs.
    def __init__(self,parent,controller):
        tk.Frame.__init__(self,parent)
        # Create the TABS BAR
        self.totalTabs = ttk.Notebook(self)
        self.totalTabs.pack(expand = 1, fill = "both")

        # Call the function crearTab to create as many tabs as the user wants:       
        crearTab(self,self.totalTabs)

class KORALI_Graph(tk.Frame): # Page with Graphs.
    def __init__(self,parent,controller):
        tk.Frame.__init__(self,parent)
        button2 = tk.Label(self, text = "This is the frame for the next part", width = 150, height = 10, relief=tk.RAISED)
        button2.pack(padx = 1, pady = 50 )


class StartPage(tk.Frame): # inherit everything from tk.Frame
    def __init__(self,parent,controller):
        tk.Frame.__init__(self,parent) #Parent = KORALI
        # Button to allow the user to navigate through windows:
        button1 = tk.Button(self, text = "Welcome to KORALI", font = HUGE_FONT,
                                command = lambda: controller.show_frame(KORALI_Page)) # lambda creates a quick thronaway function.
        button1.config(borderwidth = 2,activeforeground = 'darkcyan',relief = 'groove')
        button1.pack(side = 'top', pady=400)

 '''   

## --------------- END OF CLASSES ------------------------    
########################################################    

configTreeDB = {}
solverDB = {}
variables = {}

readDirs(mainPath, configTreeDB,default)
readDirs(mainPath2,solverDB, default)
#print(configTreeDB)
       
app = KORALI()
app.geometry("1500x1000") # Size of our application.
app.minsize("1500","1000")
app.mainloop()

