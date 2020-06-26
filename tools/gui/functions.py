'''
*Usage*                                                                                                                                   #####
*Creates all the Frames and stores all the MAIN Functions                                                                                 #####
                                                                                                                                          #####
*Functions and Classes*                                                                                                                   #####
*0 classes                                                                                                                                #####
*Main Function          #####
                                                                                                                                          #####
'''
try:
    import tkinter as tk
    from tkinter import *
    from tkinter import ttk
except ImportError:
    import Tkinter as tk
    from Tkinter import *
    from Tkinter import ttk
from tkinter.messagebox import showerror, showwarning, showinfo, askyesno
import webview
import json
import os
from tkinter.filedialog import asksaveasfile, askopenfilename
import time
from cefpython3 import cefpython as cef

# Add different colors to messages in console.
import sys

try: color = sys.stdout.shell
except AttributeError: raise RuntimeError("Use IDLE")

# FILES import
import class_GeneralSettings
import class_Problem
import class_Solver
import classes_FRAMES
import MAIN_APP
import class_Variables
import class_KORALI
import class_Distributions
import MAIN_APP
import class_BrowserFrame


# Variables
frameWidth=1150
frameHeight=900
selectorColor = 'aliceblue'
colorDescription = 'snow'
colorProblem = 'snow'
lightColor = '#00A3E0'
extraColor = '#183A54'


# Globals
files = [('Config Files', '*.config'),('Log Document','*.log'), 
             ('Text Document', '*.txt')] 
import_dist_done = True




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
            if len(default)>0:
                e.delete(0,tk.END)
                e.insert(END,values[numofentry])
            else:
                e.insert(END,values[numofentry])
        numofentry+=1
        
def copiarDiccionario(d):
    d2 = {}
    for key in d.keys():
        valor = d[key]
        if isinstance(valor,dict):
            valor = copiarDiccionario(valor)
        d2[key] = valor     
    return d2

def importFile(self):
    global files
    global import_dist_done

    configTreeDB = classes_FRAMES.configTreeDB
    solverDB = classes_FRAMES.solverDB
    experiments = class_KORALI.experiments
    selectedtab = class_KORALI.selectedtab

    experiments[selectedtab]['variables_accessible'] = False  # To control how to print the variables when importing.

    sf = experiments[selectedtab]['secondFrame']
    tf = experiments[selectedtab]['thirdFrame']

    filename = askopenfilename(filetypes=files)
    parsedFile = ''
    try:
        file = open(filename,'r')
        parsedFile = json.load(file)
    except:
        popupmsgwarning('File is not readable')
    finally:
        file.close()
    
    if parsedFile != '': # If file is readable
        valuesProblem = []
        valuesSolver = []
        valuesVariables = {}

        results = experiments[selectedtab]['results']
        
        for directorio in parsedFile.keys(): # Loop through the keys of the config file.
            if directorio == 'Problem': # PROBLEM IMPORT                                
                cont = 0
                texto = parsedFile[directorio]['Type']
                x_pos = 0
                y_pos = 215
                nombre = configTreeDB[texto]['names']
                # Check which Problem it is and display it on the screen:
                checkFormulario(sf,experiments,selectedtab, texto,nombre,configTreeDB, cont,x_pos,y_pos,tf)

                # Fill in its variables by using the file ones.
                for key2 in parsedFile[directorio]:
                    valuesProblem.append(parsedFile[directorio][key2])
                insertValues(results,cont,valuesProblem)
                experiments[selectedtab]['linktoVariables0'] = configTreeDB[texto]['config']
                
            elif directorio == 'Solver':     # SOLVER IMPORT
                cont = 1
                texto = parsedFile[directorio]['Type']
                x_pos = 0
                y_pos = 335
                nombre = solverDB[texto]['names']
                checkFormulario(sf,experiments,selectedtab, texto,nombre,solverDB, cont,x_pos,y_pos,tf)                
                for key2 in parsedFile[directorio]:
                    valuesSolver.append(parsedFile[directorio][key2])
                insertValues(results,cont,valuesSolver)
                experiments[selectedtab]['linktoVariables1'] = solverDB[texto]['config']
                   
            elif 'ariable' in directorio:  # VARIABLES IMPORT avoiding v in case there is also a V, same with ending (-s).
                
                valuesVariables = []
                cont = 2
                numofentry = 0
                results = experiments[selectedtab]['results']
                experiments[selectedtab]['ro_var'] = 4
                experiments[selectedtab]['rows_counter_var'] = 0
                sf = experiments[selectedtab]['secondFrame']                
                if 2 in results.keys():
                    del results[2]
                class_Variables.Variables(sf,experiments,selectedtab)
                v = 0 # If there are several Variables Tabs in the imported file...
                for dictionary in parsedFile[directorio]:
                    if v>0:
                        experiments[selectedtab]['times_var'] += 1
                        results = experiments[selectedtab]['results']
                        res_var = experiments[selectedtab]['labels_var']
                        results[cont][experiments[selectedtab]['times_var']] = {}
                        res_var[cont][experiments[selectedtab]['times_var']] = {}
##                        ro = 7
                        experiments[selectedtab]['ro_var'] += 1
                        ##            photo = PhotoImage(file = "trash1.ico")   image = photo,  -> añadir a button.
                        var = res_var[2][experiments[selectedtab]['times_var']]            
                        var['a'] =tk.Label(selfV, text='', fg=selectorColor, bg=selectorColor)
                        var['a'].grid(row=experiments[selectedtab]['ro_var'], column=0,columnspan=4,pady = 20 ,padx=10, sticky='nw')
                        experiments[selectedtab]['ro_var'] += 1
                        var['b'] =tk.Button(selfV, text='Variable '+str(experiments[selectedtab]['times_var']),activebackground=selectorColor, font="Arial 16", fg='black', bg='snow',
                                          borderwidth=1,relief='solid',command = lambda: delVar(selfV,experiments[selectedtab]['times_var'],experiments,selectedtab))
                        var['b'].grid(row=experiments[selectedtab]['ro_var'], column=0,columnspan=2,pady = 4 ,padx=10, sticky='n')
                        
                        printVariables2(selfV,experiments,selectedtab,cont)
                        for key in dictionary.keys():
                            valuesVariables.append(dictionary[key])
                        v+=1
                    else:    
                        # CREATING FRAME VARIABLES FOR FIRST TIME WHEN IMPORTING TO AVOID MISSUNDERSTANDINGS WITH VARIABLES:
                        res_var = experiments[selectedtab]['labels_var']
                        selfV = experiments[selectedtab]['variables']
                        # Crear Frame Variables
                        results[2] = {}
                        res_var[2] = {}
                        experiments[selectedtab]['times_var'] = 1
                        experiments[selectedtab]['linktoVariables'] = []
                        experiments[selectedtab]['linktoVariables'].append(experiments[selectedtab]['linktoVariables0'])
                        experiments[selectedtab]['linktoVariables'].append(experiments[selectedtab]['linktoVariables1'])
                        results[2][experiments[selectedtab]['times_var']] = {}
                        res_var[2][experiments[selectedtab]['times_var']] = {}
                        button = tk.Button(selfV,text='Add +',highlightcolor='snow', fg='dimgray',
                                               relief = 'flat', background = 'snow', anchor = 'w',activeforeground='teal',
                                               activebackground= 'snow',font= 'Arial 16',command = lambda:printVariables(sf,directorio,cont,experiments,selectedtab))
                        button.config(highlightbackground='snow')
                        button.grid(row=0,column=0,columnspan = 4)
                        printVariables2(selfV,experiments,selectedtab,cont)
#                        experiments[selectedtab]['ro_var'] += 1 # CHECK THIS ONE.
                        v+=1
                        results = experiments[selectedtab]['results']
                        for key in dictionary.keys():
                            valuesVariables.append(dictionary[key])

                numofentry = 0
                for key in results[cont].keys():
                    for key2 in results[cont][key]:
                        e = results[cont][key][key2]
                        e.delete(0,tk.END)
                        e.insert(0,valuesVariables[numofentry])
                        numofentry+=1
             
            else:                                       ## expFrame
                if directorio == 'Distributions':      ## MAIN CONFIGURATION AND DISTRIBUTION IMPORT
                    import_dist_done = False
                    r = 11
                    c = 0
                    valuesDistr = []
                    experiments[selectedtab]['dist_times'] = 1
                    experiments[selectedtab]['dist_first_time']= True
                    for distribution in parsedFile[directorio]:
                        class_Distributions.Distributions(sf,selectedtab,experiments)
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
                    import_dist_done = True
 
                elif directorio == 'Random Seed':
                    experiments[selectedtab]['general_first_time'] = True
                    class_GeneralSettings.GeneralSettings(sf,selectedtab,experiments)
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


def exportFile():
    global files # Types of files.

    experiments = class_KORALI.experiments
    selectedtab = class_KORALI.selectedtab

    results = experiments[selectedtab]['results']
    ### Avoiding that the user clicks create.config without choosing a problem/solver
    if 2 not in results.keys():
        popupmsgwarning('Variables Missing. First choose a Solver')
        return

    if 0 not in results.keys() or 1 not in results.keys():
        popupmsgwarning('None Problem chosen. First choose a Problem')
        return
    
    bottomleftFr = experiments[selectedtab]['solver']
    rightFr = experiments[selectedtab]['problem']

    # Copy results dictionary into results2
    results2 = {}
    results2 = copiarDiccionario(results)
         
    results2['Problem'] = results2.pop(0)
    results2['Solver'] = results2.pop(1)
    results2['Variables'] = results2.pop(2)
    if 4 in results2.keys():
        results2['Distributions'] = results2.pop(4)
       
    for frame in results2.keys():
        if frame == 'Distributions' or frame == 'Variables':
            res = []
            for distr in results2[frame]:
                for texto in results2[frame][distr]:
                    w = results2[frame][distr][texto]
                    if isinstance(w,str):
                        pass
                    else:                        
                        val = w.get()
                        results2[frame][distr][texto] = val
                distribution = results2[frame][distr]
                res.append(distribution)
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
    return

def validarstring(string_Var):
    r = False
    if string_Var.isalpha() or string_Var == '' or ' ' in string_Var:
        r = True
    return r

def validardigit(num):
    # Function that validates >0, <0 and -+Inf.
    r = False
    if num in 'Inf' or num in '-Inf' or num in 'inf' or num in '-inf':
        r = True
    elif num == '' or num == '-':
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
        
def website(directorio,cont,tf):
    global frameWidth
    global frameHeight

    if directorio in MAIN_APP.links_website.keys():
        link = MAIN_APP.links_website[directorio]
    else:
        popupmsgwarning('There is not a CSE-Lab link reference to the chosen directory')
        return

    frame = class_BrowserFrame.createHTMLFrame(tf,frameWidth,frameHeight,directorio,link)    
    frame.grid(row=0, column=0)
    
    

def checkFormulario(sf,exp,whichtab, directorio,nombre,DB, cont,x_pos,y_pos,tf):

    global experiments
    global selectedtab

    experiments = exp
    selectedtab = whichtab

    ff = experiments[selectedtab]['firstFrame']
    results = experiments[selectedtab]['results']

    under_name = nombre
    if len(nombre)>30:
        nombre = nombre[0:30]+'...'     

    if cont == 0:
        if experiments[selectedtab]['first_time_p'] == False:
            if directorio == experiments[selectedtab]['frame_problem']: # If we click on the same Problem as before...
                class_Problem.Problems.Show_frame(experiments,selectedtab,directorio,cont,tf) # We only show the frame again.
            else: # If we are choosing a different problem and not for first time:
                ANSWER = messagebox.askyesno("Different Problem","Are you sure you want to proceed? Previous variables will vanish.")
                if ANSWER == False:
                    return
                else: # If 'Yes':
                    if 'variables' in experiments[selectedtab].keys(): # Remove VARIABLES and compute it again.
                        selfV = experiments[selectedtab]['variables']
                        # Restart the variables selection
                        experiments[selectedtab]['linktoVariables0'] = ''
                        experiments[selectedtab]['linktoVariables1'] = ''
                        for widget in selfV.winfo_children():
                            widget.destroy()
                        if 2 in results.keys():
                            del results[2]

                if experiments[selectedtab]['problems_ind'] != '': # If problem selected is not the same as the previously chosen and we can access it...
                    for widget in ff.winfo_children():
                        if not isinstance(widget,tk.Canvas):
                            if widget['text'] == experiments[selectedtab]['problems_ind']:
                                widget.destroy()
                else:
                    popupmsgwarning('No file named '+directorio+'.rst')
                if experiments[selectedtab]['solvers_ind'] != '':
                    for widget in ff.winfo_children():
                        if not isinstance(widget,tk.Canvas):
                            if widget['text'] == experiments[selectedtab]['solvers_ind']:
                                widget.destroy()
                            
                experiments[selectedtab]['first_time_s'] = True
                if 1 in results.keys():
                    del results[1]
                    
                if 'solver' in experiments[selectedtab].keys():
                    solver_frame = experiments[selectedtab]['solver']
                    for widget in solver_frame.winfo_children():
                        widget.destroy()
                    popupmsginfo('When choosing a new Problem, the previous chosen Solver is removed.')
                
                which = tk.Button(ff,text = nombre,width=29, font = 'Arial 11 bold',fg =extraColor,highlightcolor=selectorColor,borderwidth = 0, background = selectorColor,command = lambda: class_Problem.Problems.Show_frame(experiments,selectedtab,directorio,cont,tf))
                which.config(cursor = 'watch')
                which.place(x=x_pos,y=y_pos+40)
                experiments[selectedtab]['problems_ind'] = nombre
                experiments[selectedtab]['frame_problem'] = directorio # IF USERS ANSWERS YES...
                experiments[selectedtab]['times_var'] = 1
                class_Problem.Problems(sf,experiments,directorio,under_name,DB,cont,selectedtab)

        else: # IF ITS THE FIRST TIME WE CHOOSE A PROBLEM:         
            experiments[selectedtab]['first_time_p'] = False
            class_Problem.Problems(sf,experiments,directorio,under_name,DB,cont,selectedtab)
            experiments[selectedtab]['frame_problem'] = directorio
            which = tk.Button(ff,text = nombre,width=29,font = 'Arial 11 bold',fg =extraColor,highlightcolor=selectorColor,borderwidth = 0,
                              background = selectorColor,command = lambda:class_Problem.Problems.Show_frame(experiments,selectedtab,directorio,cont,tf))
            which.config(cursor = 'watch')
            which.place(x=x_pos,y=y_pos+40)
            experiments[selectedtab]['problems_ind'] = nombre
            class_Problem.Problems.Show_frame(experiments,selectedtab,directorio,cont,tf)
            
        
    elif cont == 1:
        experiments[selectedtab]['times_var'] = 1
        if experiments[selectedtab]['first_time_s'] == False:
            if directorio == experiments[selectedtab]['frame_solver']:
                class_Solver.Solvers.Show_frame(experiments,selectedtab,directorio,cont,tf)
            else:
                ANSWER = messagebox.askyesno("Different Solver","Are you sure you want to proceed? Previous variables will vanish.")
                if ANSWER == False:
                    return
                for widget in ff.winfo_children():
                    if not isinstance(widget,tk.Canvas):
                        if widget['text'] == experiments[selectedtab]['solvers_ind']:
                            widget.destroy()
                # First thing to do, recalculate Variables. Delete previous ones in frame.
                if 'variables' in experiments[selectedtab].keys(): # Remove VARIABLES and compute it again.
                    selfV = experiments[selectedtab]['variables']
                    results = experiments[selectedtab]['results']
                    experiments[selectedtab]['linktoVariables1'] = DB[directorio]['config']
                    for widget in selfV.winfo_children():
                        widget.destroy()
                    if 2 in results.keys():
                        del results[2]

                if experiments[selectedtab]['solvers_ind'] != '':
                    for widget in ff.winfo_children():
                        if not isinstance(widget,str):
                            pass #Avoiding an error: canvas widget doesnt have ['text'] option.
                        elif widget['text'] == experiments[selectedtab]['solvers_ind']:
                            widget.destroy()
                else:
                    popupmsgwarning('Ups! Something went wrong... please restart the App')
                if 1 in results.keys():
                    del results[1]
                    
                which2 = tk.Button(ff,text = nombre,width=29,font = 'Arial 11 bold',fg =extraColor,highlightcolor=selectorColor,borderwidth = 0, background = selectorColor,
                                   command = lambda: class_Solver.Solvers.Show_frame(experiments,selectedtab,directorio,cont,tf))
                which2.config(cursor = 'watch')
                which2.place(x=x_pos,y=y_pos+40)
                experiments[selectedtab]['solvers_ind'] = nombre
                experiments[selectedtab]['frame_solver'] = directorio # IF USERS ANSWERS YES...
                class_Solver.Solvers(sf,experiments,directorio,under_name,DB,cont,selectedtab)
        else:
            experiments[selectedtab]['first_time_s'] = False
            class_Solver.Solvers(sf,experiments,directorio,under_name,DB,cont,selectedtab)
            experiments[selectedtab]['frame_solver'] = directorio
            which2 = tk.Button(ff,text = nombre,width=29,font = 'Arial 11 bold',fg =extraColor,highlightcolor=selectorColor,borderwidth = 0,
                               background = selectorColor,command = lambda: class_Solver.Solvers.Show_frame(experiments,selectedtab,directorio,cont,tf))
            which2.config(cursor = 'watch')
            which2.place(x=x_pos,y=y_pos+40)
            experiments[selectedtab]['solvers_ind'] = nombre
                     
def printdata(self,experiments,selectedtab,line, texto, r, c,cont,options,dict_defaults):
    
    results = experiments[selectedtab]['results']

    if cont == 0 or cont == 1:
        color = 'snow'
        res = results[cont]
        res2 = {}
    elif cont == 2:
        res_var = experiments[selectedtab]['labels_var']
        res2 = res_var[cont][experiments[selectedtab]['times_var']]
        res = results[cont][experiments[selectedtab]['times_var']]
        color = 'snow'
    else:
        color = 'aliceblue'
        res = results[cont]
        res2 = {}

    
    double_Var = tk.DoubleVar()
    num = tk.IntVar()
    boolean = tk.BooleanVar()
    experiments[selectedtab]['results'] = results
    stringVar = {}
    self.grid_propagate(0)
    
    if 'vector' not in line:
        
        if 'string' in line or 'korali' in line: # == 'std::string
            res2[r]=tk.Label(self, text=texto, width = 25,justify='left', bg=color, anchor="w", font="Arial 15")
            res2[r].grid(row=r, column=c,pady = 2,padx=6, sticky='w')
            if options == 'None':
                stringVar[texto] = tk.StringVar()
                res[texto] = tk.Entry(self,width=18,font='Arial 11',textvariable = stringVar[texto])
                res[texto].grid(row=r, column=c+1, pady= 2,padx=6, sticky = 'w')
                self.corrector = self.register(validarstring)
                res[texto].config(validate = 'focus',validatecommand = (self.corrector,'%P')) # %P represents the parameter we want to pass to validate.
            else:
                stringVar[texto] = tk.StringVar()
                if isinstance(dict_defaults,dict):
                    for key in dict_defaults.keys():
                        if key == texto:
                            default_variable = dict_defaults[key]
                            stringVar[texto].set(default_variable) # default value
                else:
                    stringVar[texto].set("default") # default value
                listoptions = []
                for option in options:
                    listoptions.append(option['Value'])
                res2[r] = OptionMenu(self,stringVar[texto],*listoptions)
                res2[r].config(font='Arial 13 bold',bg=color,activebackground = color,
                              width=11, foreground = 'darkcyan',disabledforeground = 'darkcyan',
                              relief = 'flat', cursor = 'fleur', highlightthickness = 0)
                res2[r].grid(row=r, column = c+1, pady=2, padx=6)
                res[texto] = stringVar[texto]
            
        elif line == 'double':
            res2[r]=tk.Label(self, text=texto, width = 25,justify='left', bg=color, anchor="w", font="Arial 15")
            res2[r].grid(row=r, column=c,pady = 2,padx=6, sticky='w')
            res[texto] = Spinbox(self, width=18, from_='-Infinity',font='Arial 11', to='Infinity', wrap=True,
                                 textvariable=num, state='normal',buttondownrelief = 'flat',
                                 buttonup = 'flat',highlightthickness=0)
            res[texto].grid(row=r, column=c+1, pady= 2,padx=6)
            self.corrector = self.register(validardigit)
            res[texto].config(validate = 'key',validatecommand = (self.corrector,'%P')) # %P represents the parameter we want to pass to validate.

        elif line == 'bool':
            res2[r]=tk.Label(self, text=texto, width = 25,justify='left',bg=color, anchor="w", font="Arial 15")
            res2[r].grid(row=r, column=c,pady = 2,padx=6, sticky='w')
            stringVar[texto] = tk.StringVar()
            
            stringVar[texto].set("No") # default value
            res2[r] = OptionMenu(self, stringVar[texto],"Yes", "No")
            res2[r].config(font='Arial 13 bold',bg=color,activebackground = color,
                              width=11, foreground = 'darkcyan',disabledforeground = 'darkcyan',
                              relief = 'flat', cursor = 'fleur', highlightthickness = 0)
            res2[r].grid(row=r, column = c+1, pady=2, padx=6)
            res[texto] = stringVar[texto]
             
        elif line == 'size_t':
            res2[r]=tk.Label(self, text=texto, width = 25,justify='left', bg=color, anchor="w", font="Arial 15")
            res2[r].grid(row=r, column=c,pady = 2,padx=6, sticky='w')
            stringVar[texto] = tk.StringVar()
            res[texto] = tk.Entry(self,width=18,textvariable = stringVar[texto], font = 'Arial 11')
            res[texto].grid(row=r, column=c+1, pady=2,padx=6, sticky = 'w')
            self.corrector = self.register(validarstring)
            res[texto].config(validate = 'focus',validatecommand = (self.corrector,'%P')) # %P represents the parameter we want to pass to validate.    

    else:
        if line =="std::vector<size_t>":
            
            res2[r]=tk.Label(self, text=texto, width = 25,justify='left', bg=color, anchor="w", font="Arial 15")
            res2[r].grid(row=r, column=c, sticky='w', pady = 2,padx=6)
            stringVar[texto] = tk.StringVar()
            res[texto] = tk.Entry(self,width=18,textvariable = stringVar[texto], font = 'Arial 11')
            res[texto].grid(row=r, column=c+1,sticky = 'w', pady = 2,padx=6)
            self.corrector = self.register(validarstring)
            res[texto].config(validate = 'focus',validatecommand = (self.corrector,'%P')) # %P represents the parameter we want to pass to validate.
            
        if 'double' in line:
            res2[r]=tk.Label(self, text=texto, width = 25,justify='left', bg=color, anchor="w", font="Arial 15")
            res2[r].grid(row=r, column=c,pady = 2,padx=6,sticky='w')
            stringVar[texto] = tk.StringVar()
            res[texto] = tk.Entry(self,width=18,font='Arial 11',textvariable = stringVar[texto])
            res[texto].grid(row=r, column=c+1, pady= 2,padx=6, sticky = 'w')
            self.corrector = self.register(validardigit)

def printVariables(sf,directorio,cont,experiments,selectedtab):

    experiments[selectedtab]['linktoVariables'] = []
    results = experiments[selectedtab]['results']
    
    cont = 2
    if experiments[selectedtab]['times_var'] == 1:
        class_Variables.Variables(sf,experiments,selectedtab)
        res_var = experiments[selectedtab]['labels_var']
        selfV = experiments[selectedtab]['variables']
        # Crear Frame Variables
        results[2] = {}
        res_var[2] = {}
        
        if experiments[selectedtab]['linktoVariables0'] !='':
            experiments[selectedtab]['linktoVariables'].append(experiments[selectedtab]['linktoVariables0'])    # Añadimos las variables que vienen del Problem.
        else:
            popupmsgwarning('This Problem does not have Variables, choose another one.')
            return        
        if experiments[selectedtab]['linktoVariables1'] !='':
            experiments[selectedtab]['linktoVariables'].append(experiments[selectedtab]['linktoVariables1'])    # Añadimos las variables que vienen del Solver.
        else:
            popupmsgwarning('Solver does not have Variables, proceeding without them...')
    elif experiments[selectedtab]['times_var'] >3:
        popupmsgwarning('The maximum number of Variables allowed is 3.')
        return
    else: # Every time we entry this function we create a new linktoVariables.
        experiments[selectedtab]['linktoVariables'].append(experiments[selectedtab]['linktoVariables0'])
        experiments[selectedtab]['linktoVariables'].append(experiments[selectedtab]['linktoVariables1'])

    selfV = experiments[selectedtab]['variables']
    res_var = experiments[selectedtab]['labels_var']

    if experiments[selectedtab]['times_var'] < 2:
        results[2][experiments[selectedtab]['times_var']]={}
        res_var[2][experiments[selectedtab]['times_var']] = {}                       
        button = tk.Button(selfV,text='Add +',highlightcolor='snow', fg='dimgray',
                               relief = 'flat', background = 'snow', anchor = 'w',activeforeground='teal',
                               activebackground= 'snow',font= 'Arial 16',command = lambda:printVariables(sf,directorio,cont,experiments,selectedtab))
        button.config(highlightbackground='snow')
        button.grid(row=0,column=0,columnspan = 4)
        printVariables2(selfV,experiments,selectedtab,cont)
        experiments[selectedtab]['ro_var'] += 1 # CHECK THIS ONE.
        experiments[selectedtab]['times_var'] += 1

    else:
        results[2][experiments[selectedtab]['times_var']] = {}
        res_var[2][experiments[selectedtab]['times_var']] = {}
        ##            photo = PhotoImage(file = "trash1.ico")   image = photo,  -> Add to button to show a trash image next to it.
        var = res_var[2][experiments[selectedtab]['times_var']]
        times_var = experiments[selectedtab]['times_var']
        var['a'] =tk.Label(selfV, text='', fg=selectorColor, bg=selectorColor)
        var['a'].grid(row=experiments[selectedtab]['ro_var'], column=0,columnspan=4,pady = 20 ,padx=10, sticky='nw')
        experiments[selectedtab]['ro_var'] += 1
        experiments[selectedtab]['rows_counter_var'] += 1
        var['b'] =tk.Button(selfV, text='Variable '+str(times_var),activebackground=selectorColor, font="Arial 16", fg='black', bg='snow',
                          borderwidth=1,relief='solid',command = lambda: delVar(selfV,times_var,experiments,selectedtab)) #bg = 'darkcyan', fg='white')
        var['b'].grid(row=experiments[selectedtab]['ro_var'], column=0,columnspan=2,pady = 4 ,padx=10, sticky='n')
        experiments[selectedtab]['ro_var'] += 1
        experiments[selectedtab]['rows_counter_var'] += 1
        printVariables2(selfV,experiments,selectedtab,cont)
        experiments[selectedtab]['times_var'] += 1
        
    
def delVar(self,which,experiments,selectedtab):    
    res_var = experiments[selectedtab]['labels_var']
    results = experiments[selectedtab]['results']

    for var_key in results[2].keys():
        if var_key > which:
            popupmsgwarning('First delete the last variable')
            return
    # Key value '4' is for Distributions
    var = results[2][which]
    var2 = res_var[2][which]
     
    for entry in var.values():
        entry.destroy()
    for entry in var2.values():
        entry.destroy()

    del res_var[2][which]
    del results[2][which]

    experiments[selectedtab]['times_var'] -= 1
    experiments[selectedtab]['ro_var'] -= experiments[selectedtab]['rows_counter_var'] # Adding 2 due to the title of the variables.
    experiments[selectedtab]['ro_var'] -= 2

def printVariables2(selfV,experiments,selectedtab,cont):
    dict_defaults_var = ''
    experiments[selectedtab]['avoid_repetition'] = []
    experiments[selectedtab]['rows_counter_var'] = 0
    experiments[selectedtab]['ro_var'] += 1
    results = experiments[selectedtab]['results']
    co = 0
    variables = classes_FRAMES.variables
    for part in experiments[selectedtab]['linktoVariables']:
        for llave in variables[part]:
            for var in llave.keys():
                if var == 'Type':
                    line = llave['Type']
                    # llave son las llaves del diccionario, por ejemplo: Type, Function, Description, Produced By...
                    texto = llave['Name']
                    texto = texto[0] # Remove the '{}' from the label name.
                    try:
                        options = llave['Options'] # options = list of diccionaries.
                    except:
                        options = 'None'
                    if texto in experiments[selectedtab]['avoid_repetition']: # AVOID PROBLEM AND SOLVER ADDING SAME VARIABLE.
                        pass
                    else:
                        experiments[selectedtab]['avoid_repetition'].append(texto)
                        printdata(selfV,experiments,selectedtab,line, texto, experiments[selectedtab]['ro_var'], co, cont,options,dict_defaults_var)
                        experiments[selectedtab]['ro_var']+=1
                        experiments[selectedtab]['rows_counter_var'] += 1
        

    
def printConfig(self,experiments,selectedtab,directorio,nombre,DB, cont):
    
    if cont == 0:
        experiments[selectedtab]['linktoVariables0'] = DB[directorio]['config']
        experiments[selectedtab]['times_var'] = 1 # If we choose a new problem, delete variables and reset times_var
    elif cont == 1:
        tf = experiments[selectedtab]['thirdFrame']
        class_Solver.Solvers.Show_frame(experiments,selectedtab,directorio,cont,tf)
        experiments[selectedtab]['linktoVariables1'] = DB[directorio]['config']
        
    if experiments[selectedtab]['linktoVariables0'] != '' and experiments[selectedtab]['linktoVariables1'] != '' and experiments[selectedtab]['variables_accessible'] == True:
        sf = experiments[selectedtab]['secondFrame']
        printVariables(sf,directorio,2,experiments,selectedtab)
        
    ## DELETE ANY WIDGET THAT WAS THERE BEFORE:
    for widget in self.winfo_children():
        widget.destroy()
    results = experiments[selectedtab]['results']
    results[cont] = {}

    under_name = nombre
    if len(nombre) > 25:
        nombre = nombre[0:25]+'...'

    efake =tk.Label(self, text='', fg=colorProblem, bg=colorProblem)
    efake.grid(row=0, column=0,columnspan=4,pady = 20 ,padx=10, sticky='nw')
    e =tk.Button(self, text=nombre,activebackground='aliceblue', font="Arial 20", fg='black', bg=colorProblem,borderwidth=2,relief='solid')
    e.grid(row=1, column=0,columnspan=4,pady = 4 ,padx=10, sticky='n')
    edescr =tk.Label(self, text= under_name, font="Arial 10", fg='navy', bg=colorProblem)
    edescr.grid(row=2, column=0,columnspan=4,rowspan=1,pady = 4 ,padx=10, sticky='n')
    efake =tk.Label(self, text='', fg=colorProblem, bg=colorProblem)
    efake.grid(row=3, column=0,columnspan=4,pady = 10 ,padx=10, sticky='nw')

    results[cont]['Type']= directorio #chosen

    config = DB[directorio]['herencia']
    conf_sett = 'Configuration Settings'

    r = 4
    c = 0

    dict_defaults = {}
    ## Get Default Values:
    if 'Module Defaults' in config.keys():
        experiments[selectedtab]['dict_defaults'] = config['Module Defaults']
    else:
        experiments[selectedtab]['dict_defaults'] = ''
    
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
                                    # key2 are dictionary keys, ex: Type, Function, Description, Produced By...
                                    texto = dicc['Name']
                                    texto = texto[0] # Remove the {} from the label name.
                                    description = dicc['Description']
                                    try:
                                        options = dicc['Options'] # options = list of diccionaries.
                                    except:
                                        options = 'None'

                                    printdata(self,experiments,selectedtab,dicc[key2], texto, r, c,cont,options,experiments[selectedtab]['dict_defaults'])                                    
                                r+=1
    else:
        popupmsgwarning('No Configuration Settings found')


def popupmsgwarning(text):
    Tk().withdraw()
    showwarning(title = 'Warning',message=text)

def popupmsginfo(text):
    Tk().withdraw()
    showinfo(title = 'Info', message = text)

def explainDescription(description):
    tut = tk.Tk()
    tut.configure(background=colorDescription)
    half = len(description)/2
    tut.wm_title('Description')
    label = ttk.Label(tut, text= description[0:int(half)]+'\n'+description[int(half)::], font = 'Arial 14',background=colorDescription)
    label.pack(side='top', fill='x', pady=10)
    tut.maxsize('950','600')

def Clear(frame):
    for item in frame.winfo_children():
        item.destroy()
