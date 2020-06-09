import tkinter as tk
from tkinter import *
from tkinter.messagebox import showerror, showwarning, showinfo, askyesno
import webview
import json
import os
from tkinter.filedialog import asksaveasfile, askopenfilename


##import QWebView

# FILES import
import class_GeneralSettings
import class_Problem
import class_Solver
import classes_FRAMES
import MAIN_APP
import class_Variables
import class_KORALI
import class_Distributions


# Variables:
selectorColor = 'aliceblue'
colorDescription = 'snow'
colorProblem = 'snow'
lightColor = '#00A3E0'
extraColor = '#183A54'

files = [('Config Files', '*.config'),('Log Document','*.log'), 
             ('Text Document', '*.txt')] 
frame_problem = ''
frame_solver =''
first_time_p = True
first_time_s = True 
import_dist_done = True

# Tell us which one are we using under the cascade option.
problems_ind = ''
solvers_ind = ''

links = {'sampling':'../../docs/manual/modules/problem/sampling/sampling.rst'}

linktoVariables0 = ''
linktoVariables1 = ''
contadorVariables = 1

ro=4 # Variables rows.


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
    global times_distr
    global rows
    global import_dist_done
    global contadorVariables

    configTreeDB = classes_FRAMES.configTreeDB
    solverDB = classes_FRAMES.solverDB
    experiments = class_KORALI.experiments
    selectedtab = class_KORALI.selectedtab

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
    ##    bottomrightFrame = experiments[selectedtab]['variables']
    
    if parsedFile != '': # If file is readable
        valuesProblem = []
        valuesSolver = []
        valuesVariables = {}
        linktoVariables_imported = []

        results = experiments[selectedtab]['results']
        
        for directorio in parsedFile.keys(): # Loop through the keys of the config file.
            if directorio == 'Problem': # PROBLEM IMPORT                                
                cont = 0
                texto = parsedFile[directorio]['Type']
                x_pos = 0
                y_pos = 185
                nombre = configTreeDB[texto]['names']
                checkFormulario(sf,experiments,selectedtab, texto,nombre,configTreeDB, cont,x_pos,y_pos,tf)
                
                for key2 in parsedFile[directorio]:
                    valuesProblem.append(parsedFile[directorio][key2])
                insertValues(results,cont,valuesProblem)
                linktoVariables_imported.append(configTreeDB[texto]['config'])
                
            elif directorio == 'Solver':     # SOLVER IMPORT
                cont = 1
                texto = parsedFile[directorio]['Type']
                x_pos = 0
                y_pos = 305
                nombre = solverDB[texto]['names']
                checkFormulario(sf,experiments,selectedtab, texto,nombre,solverDB, cont,x_pos,y_pos,tf)                
                for key2 in parsedFile[directorio]:
                    valuesSolver.append(parsedFile[directorio][key2])
                insertValues(results,cont,valuesSolver)
                linktoVariables_imported.append(solverDB[texto]['config'])
                   
            elif 'ariable' in directorio:  # VARIABLES IMPORT avoiding v in case there is also a V, same with ending (-s).
                
                valuesVariables = []
                cont = 2
                numofentry = 0
                selfV = experiments[selectedtab]['variables']
                v = 0 # If there are several Variables Tabs in the imported file...
                for dictionary in parsedFile[directorio]:
                    if v>0:
                        print('QUE ESTA PASANDOOO = ',results)
                        #ro += 4
                        results = experiments[selectedtab]['results']
                        res_var = experiments[selectedtab]['labels_var']
                        results[cont][class_KORALI.times_var] = {}
                        res_var[cont][class_KORALI.times_var] = {}
                        ro = 7
                        ##            photo = PhotoImage(file = "trash1.ico")   image = photo,  -> añadir a button.
                        var = res_var[2][class_KORALI.times_var]            
                        var['a'] =tk.Label(selfV, text='', fg=selectorColor, bg=selectorColor)
                        var['a'].grid(row=ro, column=0,columnspan=4,pady = 20 ,padx=10, sticky='nw')
                        var['b'] =tk.Button(selfV, text='Variable '+str(class_KORALI.times_var),activebackground=selectorColor, font="Arial 16", fg='black', bg='snow',
                                          borderwidth=1,relief='solid',command = lambda: delVar(selfV,class_KORALI.times_var,experiments,selectedtab)) #bg = 'darkcyan', fg='white')
                        var['b'].grid(row=ro+1, column=0,columnspan=2,pady = 4 ,padx=10, sticky='n')
                        ro += 2
                        printVariables2(selfV,experiments,selectedtab,cont,linktoVariables)
                        class_KORALI.times_var += 1
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
                        
                                        
                        
                    '''
                        if len(linktoVariables_imported) != 0:
                            printVariables2(selfV,experiments,selectedtab,cont,linktoVariables_imported)
                        else:
                            popupmsgwarning("Either problem or solver missing. Variables can't be filled in.")
##                        crearFrameVariables() # Create as many Variable Frames as necessary.
                        v+=1
                    v+=1
                    for key in dictionary.keys():
                        valuesVariables.append(dictionary[key])
                ########
                        
                numofentry = 0       
                for key in results[cont].keys():
                    for key2 in results[cont][key]:
                        e = results[cont][key][key2]
                        e.insert(0,valuesVariables[numofentry])
                        numofentry+=1
                '''
             
            else:                                       ## expFrame
                if directorio == 'Distributions':      ## MAIN CONFIGURATION AND DISTRIBUTION IMPORT
                    import_dist_done = False
                    r = 11
                    c = 0
                    valuesDistr = []
                    times_distr = 1
                    class_KORALI.dist_first_time = True
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
                    class_KORALI.general_first_time = True
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
##    global selectedtab # To know at which experiment we are.
##    global experiments
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
##    variableFr = experiments[selectedtab]['totalTabs2']

    # Copy results dictionary into results2
    results2 = {}
##    for key in results:
##         results2[key] = results[key].copy()
##    results2 = {k : v for k,v in results}
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
##                        results2[frame][texto] = w
                    else:                        
                        val = w.get()
                        results2[frame][distr][texto] = val
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
    return
        
def printt():
    pass

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


    
def website(tf,directorio,cont):
    global links
    
    if cont == 0:
        link = '../../docs/manual/modules/problem/'+directorio+'/'+directorio+'.rst'
    elif cont == 1:
        link = '../../docs/manual/modules/solver/'+directorio+'/'+directorio+'.rst'

    file = open(link,'r')
    first_line = file.readline()
    Label = tk.Label(tf, text = first_line, font = 'Arial 18')
    Label.pack(side='top')
    tf.pack_propagate(0)
    file.close()

def checkFormulario(sf,exp,whichtab, directorio,nombre,DB, cont,x_pos,y_pos,tf):
    global frame_problem
    global frame_solver
    global first_time_p
    global first_time_s
    global problems_ind
    global solvers_ind
    global experiments
    global selectedtab

    experiments = exp
    selectedtab = whichtab

    ff = experiments[selectedtab]['firstFrame']
              
    y_pos = y_pos +40
    x_pos = x_pos
    if len(nombre)>30:
        nombre = nombre[0:30]+'...'       

    if cont == 0:
        if first_time_p == False:
            if directorio == frame_problem:
                class_Problem.Problems.Show_frame(experiments,selectedtab)
                website(tf,directorio,cont)
            else:
                ANSWER = messagebox.askyesno("Different Problem","Are you sure you want to proceed? Previous variables will vanish.")
                if ANSWER == False:
                    return
                if problems_ind != '':
                   for widget in ff.winfo_children():
                        if widget['text'] == problems_ind:
                            widget.destroy()
                website(tf,directorio,cont)
                which = tk.Button(ff,text = nombre,width=28, font = 'Arial 11 bold',fg =extraColor,highlightcolor=selectorColor,borderwidth = 0, background = selectorColor,command = lambda: [class_Problem.Problems.Show_frame(experiments,selectedtab),website(tf,directorio,cont)])
                which.config(cursor = 'watch')
                which.place(x=x_pos,y=y_pos)
                problems_ind = nombre
                frame_problem = directorio # IF USERS ANSWERS YES...
                class_Problem.Problems(sf,experiments,directorio,nombre,DB,cont,selectedtab)
                for widget in ff.winfo_children():
                        if widget['text'] == solvers_ind:
                            first_time_s = True
                            widget.destroy()
        else:
            website(tf,directorio,cont)
            first_time_p = False
            class_Problem.Problems(sf,experiments,directorio,nombre,DB,cont,selectedtab)
            frame_problem = directorio
            which = tk.Button(ff,text = nombre,width=28,font = 'Arial 11 bold',fg =extraColor,highlightcolor=selectorColor,borderwidth = 0, background = selectorColor,command = lambda: [class_Problem.Problems.Show_frame(experiments,selectedtab),website(tf,directorio,cont)])
            which.config(cursor = 'watch')
            which.place(x=x_pos,y=y_pos)
            problems_ind = nombre
        
    elif cont == 1:
        if first_time_s == False:
            if directorio == frame_solver:
                class_Solver.Solvers.Show_frame(experiments,selectedtab)
                website(tf,directorio,cont)
            else:
                ANSWER = messagebox.askyesno("Different Solver","Are you sure you want to proceed? Previous variables will vanish.")
                if ANSWER == False:
                    return
                if solvers_ind != '':
                    for widget in ff.winfo_children():
                        if widget['text'] == solvers_ind:
                            widget.destroy()
                website(tf,directorio,cont)
                which2 = tk.Button(ff,text = nombre,width=28,font = 'Arial 11 bold',fg =extraColor,highlightcolor=selectorColor,borderwidth = 0, background = selectorColor,command = lambda: [class_Solver.Solvers.Show_frame(experiments,selectedtab),website(tf,directorio,cont)])
                which2.config(cursor = 'watch')
                which2.place(x=x_pos,y=y_pos)
                solvers_ind = nombre
                frame_solver = directorio # IF USERS ANSWERS YES...
                class_Solver.Solvers(sf,experiments,directorio,nombre,DB,cont,selectedtab)
        else:
            website(tf,directorio,cont)
            first_time_s = False
            class_Solver.Solvers(sf,experiments,directorio,nombre,DB,cont,selectedtab)
            frame_solver = directorio
            which2 = tk.Button(ff,text = nombre,width=28,font = 'Arial 11 bold',fg =extraColor,highlightcolor=selectorColor,borderwidth = 0,
                               background = selectorColor,command = lambda: [class_Solver.Solvers.Show_frame(experiments,selectedtab),website(tf,directorio,cont)])
            which2.config(cursor = 'watch')
            which2.place(x=x_pos,y=y_pos)
            solvers_ind = nombre
            #printConfig(experiments,directorio,DB, cont)            
            #printVariables(sf.tab2,directorio,DB,cont)
            
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



def printdata(self,experiments,selectedtab,line, texto, r, c,cont,options):
    
    results = experiments[selectedtab]['results']

    if cont == 0 or cont == 1:
        color = 'snow'
        res = results[cont]
        res2 = {}
    elif cont == 2:
        res_var = experiments[selectedtab]['labels_var']
        res2 = res_var[cont][class_KORALI.times_var]
        res = results[cont][class_KORALI.times_var]
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
                stringVar[texto].set("default") # default value
                listoptions = []
                for option in options:
                    listoptions.append(option['Value'])
                res2[r] = OptionMenu(self,stringVar[texto],*listoptions)#,command = lambda _:getValOptionMenu(stringVar[texto]))
                res2[r].config(font='Arial 13 bold',bg=color,activebackground = color,
                              width=11, foreground = 'darkcyan',disabledforeground = 'darkcyan',
                              relief = 'flat', cursor = 'fleur', highlightthickness = 0)
                res2[r].grid(row=r, column = c+1, pady=2, padx=6)
                res[texto] = stringVar[texto]
            
        elif line == 'double':
            res2[r]=tk.Label(self, text=texto, width = 25,justify='left', bg=color, anchor="w", font="Arial 15")
            res2[r].grid(row=r, column=c,pady = 2,padx=6, sticky='w')
            res[texto] = Spinbox(self, width=18, from_=0,font='Arial 11', to=9999, wrap=True,
                                 textvariable=num, state='normal',buttondownrelief = 'flat',
                                 buttonup = 'flat',highlightthickness=0)
            res[texto].grid(row=r, column=c+1, pady= 2,padx=6)
            self.corrector = self.register(validardigit)
            res[texto].config(validate = 'key',validatecommand = (self.corrector,'%P')) # %P represents the parameter we want to pass to validate.

        elif line == 'bool':
            res2[r]=tk.Label(self, text=texto, width = 25,justify='left',bg=color, anchor="w", font="Arial 15")
            res2[r].grid(row=r, column=c,pady = 2,padx=6, sticky='w')
            stringVar[texto] = tk.StringVar()
            stringVar[texto].set("default") # default value
            res2[r] = OptionMenu(self, stringVar[texto],"Yes", "No")#,command = lambda _:getValOptionMenu(stringVar[texto]))
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

def printVariables(sf,directorio,DB,cont,linktoVariables0,linktoVariables1,contadorVariables,experiments,selectedtab):
    
    global ro

    linktoVariables = []
    results = experiments[selectedtab]['results']
    
    cont = 2
    if class_KORALI.times_var == 1:
        class_Variables.Variables(sf,experiments,selectedtab)
        res_var = experiments[selectedtab]['labels_var']
        selfV = experiments[selectedtab]['variables']
        # Crear Frame Variables
        results[2] = {}
        res_var[2] = {}
        
        if linktoVariables0 !='':
            linktoVariables.append(linktoVariables0)    # Añadimos las variables que vienen del Problem.
        else:
            popupmsgwarning('This Problem does not have Variables, choose another one.')
            return        
        if linktoVariables1 !='':
            linktoVariables.append(linktoVariables1)    # Añadimos las variables que vienen del Solver.
        else:
            popupmsgwarning('Solver does not have Variables, proceeding without them...')
    elif class_KORALI.times_var >3:
        popupmsgwarning('The maximum number of Variables allowed is 3.')
        return
    else:
        linktoVariables.append(linktoVariables0)
        linktoVariables.append(linktoVariables1)

    selfV = experiments[selectedtab]['variables']
    res_var = experiments[selectedtab]['labels_var']

    if contadorVariables < 2:
        results[2][class_KORALI.times_var]={}
        res_var[2][class_KORALI.times_var] = {}
        contadorVariables += 1                               
        button = tk.Button(selfV,text='Add +',highlightcolor='snow', fg='dimgray',
                               relief = 'flat', background = 'snow', anchor = 'w',activeforeground='teal',
                               activebackground= 'snow',font= 'Arial 16',command = lambda:printVariables(sf,directorio,DB,cont,linktoVariables0,linktoVariables1,contadorVariables,experiments,selectedtab))
        button.config(highlightbackground='snow')
        button.grid(row=0,column=0,columnspan = 4)
        printVariables2(selfV,experiments,selectedtab,cont,linktoVariables)
        ro += 1
        class_KORALI.times_var += 1
        contadorVariables+=1

    else:
        ro += 4
        results[2][class_KORALI.times_var]={}
        res_var[2][class_KORALI.times_var] = {}
        ##            photo = PhotoImage(file = "trash1.ico")   image = photo,  -> añadir a button.
        var = res_var[2][times_var]            
        var['a'] =tk.Label(selfV, text='', fg=selectorColor, bg=selectorColor)
        var['a'].grid(row=ro, column=0,columnspan=4,pady = 20 ,padx=10, sticky='nw')
        var['b'] =tk.Button(selfV, text='Variable '+str(times_var),activebackground=selectorColor, font="Arial 16", fg='black', bg='snow',
                          borderwidth=1,relief='solid',command = lambda: delVar(selfV,times_var,experiments,selectedtab)) #bg = 'darkcyan', fg='white')
        var['b'].grid(row=ro+1, column=0,columnspan=2,pady = 4 ,padx=10, sticky='n')
        ro += 2
        printVariables2(selfV,experiments,selectedtab,cont,linktoVariables)
        class_KORALI.times_var += 1
        
    
def delVar(self,which,experiments,selectedtab):

    res_var = experiments[selectedtab]['labels_var']
    results = experiments[selectedtab]['results']
    # Key value '4' is for Distributions
    var = res_var[2][which-1]
     
    for entry in var.values():
        entry.destroy()

    del res_var[2][which]
    del results[2][which]

    class_KORALI.times_var -= 1
    
    # Clean the frame:
##    for widget in selfV.winfo_children():
##            widget.destroy()
def printVariables2(selfV,experiments,selectedtab,cont,linktoVariables):
    global ro
    
    results = experiments[selectedtab]['results']
    print(results[2].keys())
    co = 0
    variables = classes_FRAMES.variables
    for part in linktoVariables:
        for llave in variables[part]:
            for var in llave.keys():
                if var == 'Type':
                    line = llave['Type']
                    # llave son las llaves del diccionario, por ejemplo: Type, Function, Description, Produced By...
                    texto = llave['Name']
                    texto = texto[0] # Remove the '{}' from the label name.
                    options = 'None'
                    printdata(selfV,experiments,selectedtab,line, texto, ro, co, cont,options)
                    ro+=1
    

    
def printConfig(self,experiments,selectedtab,directorio,nombre,DB, cont):

    global linktoVariables0
    global linktoVariables1
    
    if cont == 0:
        linktoVariables0 = DB[directorio]['config']
        class_KORALI.times_var = 1 # If we choose a new problem, delete variables and reset times_var
    elif cont == 1:
        linktoVariables1 = DB[directorio]['config']
        
    if linktoVariables0 != '' and linktoVariables1 != '':
        sf = experiments[selectedtab]['secondFrame']
        contadorVariables = 0
        printVariables(sf,directorio,DB,2,linktoVariables0,linktoVariables1,contadorVariables,experiments,selectedtab)
        
    ## DELETE ANY WIDGET THAT WAS THERE BEFORE:
    for widget in self.winfo_children():
        widget.destroy()
    results = experiments[selectedtab]['results']
    results[cont] = {}

    if len(nombre) > 25:
        nombre = nombre[0:25]+'...'
    # Start reading and printing on screen: # DON'T DELETE THIS COMMENT -> TITULO FRAMES : list(DB.keys())[0]
##    self.titulo=tk.Label(self, text=directorio,justify='left', anchor="w", font="Arial 18", bg = 'darkcyan', fg='white')
##    self.titulo.grid(row=r, column=c,pady = 10 ,padx=20, sticky='w', columnspan = 2)
    efake =tk.Label(self, text='', fg=colorProblem, bg=colorProblem)
    efake.grid(row=0, column=0,columnspan=4,pady = 20 ,padx=10, sticky='nw')
    e =tk.Button(self, text=nombre,activebackground='aliceblue', font="Arial 20", fg='black', bg=colorProblem,borderwidth=2,relief='solid', command = lambda : functions.Clear(self.gs)) #bg = 'darkcyan', fg='white')
    e.grid(row=1, column=0,columnspan=4,pady = 4 ,padx=10, sticky='n')
    edescr =tk.Label(self, text='Click title to clear the variables...', font="Arial 10", fg='navy', bg=colorProblem) #bg = 'darkcyan', fg='white')
    edescr.grid(row=2, column=0,columnspan=4,rowspan=1,pady = 4 ,padx=10, sticky='n')
    efake =tk.Label(self, text='', fg=colorProblem, bg=colorProblem)
    efake.grid(row=3, column=0,columnspan=4,pady = 10 ,padx=10, sticky='nw')

##    chosen = titulo.cget('text')
    results[cont]['Type']= directorio #chosen

    config = DB[directorio]['herencia']
    conf_sett = 'Configuration Settings'

    r = 4
    c = 0
    
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

                                    printdata(self,experiments,selectedtab,dicc[key2], texto, r, c,cont,options)                                    
                                r+=1
    else:
        popupmsgwarning('No Configuration Settings found')
    if cont == 1:
        class_Solver.Solvers.Show_frame(experiments,selectedtab)


def popupmsgwarning(text):
    Tk().withdraw()
    showwarning(title = 'Warning',message=text)
##    showinfo(title = 'Info',message=text)
##    showerror(title = 'Error',message=text)

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
