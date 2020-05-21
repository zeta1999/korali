import tkinter as tk
from tkinter import *
from tkinter.messagebox import showerror, showwarning, showinfo, askyesno

# FILES import
import class_GeneralSettings
import class_Problem
import class_Solver

# Variables:
colorDescription = 'snow'
colorProblem = 'snow'
frame_problem = ''
frame_solver =''
first_time_p = True
first_time_s = True


def checkFormulario(sf,experiments,selectedtab, directorio,nombre,DB, cont):
    global frame_problem
    global frame_solver
    global first_time_p
    global first_time_s

    if cont == 0:
        if directorio == frame_problem:
            print('Make the Problem-frame visible with the data from before')
        else:
            if first_time_p == False:
                messagebox.askyesno("Different Problem","Are you sure you want to proceed? Previous variables will vanish.")
                frame_problem = directorio # IF USERS ANSWERS YES...
            first_time_p = False
            class_Problem.Problems(sf,experiments,directorio,nombre,DB,cont)
            #printConfig(experiments,directorio,DB, cont)            
            #printVariables(sf.tab2,directorio,DB,cont)
    elif cont == 1:
        if directorio == frame_solver:
            print('Make the Solver-frame visible with the data from before')
        else:
            if first_time_s == False:
                messagebox.askyesno("Different Solver","Are you sure you want to proceed? Previous variables will vanish.")
                frame_solver = directorio
            first_time_s = False
            class_Solver.Solvers(sf,experiments,directorio,nombre,DB,cont)

            #printConfig(directorio,DB, cont)
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



def printdata(self,experiments,selectedtab,line, texto, description, fakedescription, r, c,cont,options):
    
##    global selectedtab2

##    if cont == 2:
##        color = 'floralwhite'
##        res = results[cont][selectedtab2]
    
    if cont ==0 or cont == 1:
        color = 'snow'
        results = experiments[selectedtab]['results']
        res = results[cont]
    else:
        results = experiments[selectedtab]['results']
        color = 'aliceblue'
        res = results[cont]

    
    double_Var = tk.DoubleVar()
    num = tk.IntVar()
    boolean = tk.BooleanVar()
    experiments[selectedtab]['results'] = results
    stringVar = {}
    self.grid_propagate(0)
    
    if 'vector' not in line:
        
        if 'string' in line or 'korali' in line: # == 'std::string
            self.label=tk.Label(self, text=texto, width = 25,justify='left', bg=color, anchor="w", font="Arial 15")
            self.label.grid(row=r, column=c,pady = 2,padx=6, sticky='w')
            if options == 'None':
                stringVar[texto] = tk.StringVar()
                res[texto] = tk.Entry(self,width=15,font='Arial 11',textvariable = stringVar[texto])
                res[texto].grid(row=r, column=c+1, pady= 2,padx=6, sticky = 'w')
                self.corrector = self.register(validarstring)
                res[texto].config(validate = 'focus',validatecommand = (self.corrector,'%P')) # %P represents the parameter we want to pass to validate.
                self.buttonLabel = tk.Button(self, text = fakedescription,font='Arial 11', width = 25, activeforeground = 'darkcyan', bg = color ,activebackground = 'white',
                                                command = lambda:explainDescription(description))
                self.buttonLabel.grid(row=r, column=c+3,sticky='w',pady=2,padx=6)
            else:
                stringVar[texto] = tk.StringVar()
                stringVar[texto].set("default") # default value
                listoptions = []
                for option in options:
                    listoptions.append(option['Value'])
                optionMenu = OptionMenu(self,stringVar[texto],*listoptions)#,command = lambda _:getValOptionMenu(stringVar[texto]))
                optionMenu.grid(row=r, column = c+1, pady=2, padx=6)
                res[texto] = stringVar[texto]
                self.buttonLabel = tk.Button(self, text = fakedescription,font='Arial 11', width = 25, activeforeground = 'darkcyan', bg = color ,activebackground = 'white',
                                                command = lambda:explainDescription(description))
                self.buttonLabel.grid(row=r, column=c+3,sticky='w',pady=2,padx=6)

            
        elif line == 'double':
            self.label=tk.Label(self, text=texto, width = 25,justify='left', bg=color, anchor="w", font="Arial 15")
            self.label.grid(row=r, column=c,pady = 2,padx=6, sticky='w')
            res[texto] = Spinbox(self, width=15, from_=0,font='Arial 11', to=9999, wrap=True, textvariable=num, state='normal')
            res[texto].grid(row=r, column=c+1, pady= 2,padx=6)
            self.corrector = self.register(validardigit)
            res[texto].config(validate = 'key',validatecommand = (self.corrector,'%P')) # %P represents the parameter we want to pass to validate.
            #spinbox_value = spinbox.get()
            self.buttonLabel = tk.Button(self, text = fakedescription,font='Arial 11', width = 25, activeforeground = 'darkcyan', bg = color ,activebackground = 'white',
                                                command = lambda:explainDescription(description))
            self.buttonLabel.grid(row=r, column=c+3,sticky='w',pady=2,padx=6)
            
        elif line == 'bool':
            self.label=tk.Label(self, text=texto, width = 25,justify='left',bg=color, anchor="w", font="Arial 15")
            self.label.grid(row=r, column=c,pady = 2,padx=6, sticky='w')
            stringVar[texto] = tk.StringVar()
            stringVar[texto].set("default") # default value
            om = OptionMenu(self, stringVar[texto],"Yes", "No")#,command = lambda _:getValOptionMenu(stringVar[texto]))
            om.grid(row=r, column = c+1, pady=2, padx=6)
            res[texto] = stringVar[texto]
            self.buttonLabel = tk.Button(self, text = fakedescription,font='Arial 11', width = 25, activeforeground = 'darkcyan', bg = color ,activebackground = 'white',
                                                command = lambda:explainDescription(description))
            self.buttonLabel.grid(row=r, column=c+3,sticky='w',pady=2,padx=6)
             
        elif line == 'size_t':
            self.label=tk.Label(self, text=texto, width = 25,justify='left', bg=color, anchor="w", font="Arial 15")
            self.label.grid(row=r, column=c,pady = 2,padx=6, sticky='w')
            stringVar[texto] = tk.StringVar()
            res[texto] = tk.Entry(self,width=15,textvariable = stringVar[texto], font = 'Arial 11')
            res[texto].grid(row=r, column=c+1, pady=2,padx=6, sticky = 'w')
            self.corrector = self.register(validarstring)
            res[texto].config(validate = 'focus',validatecommand = (self.corrector,'%P')) # %P represents the parameter we want to pass to validate.
            self.buttonLabel = tk.Button(self, text = fakedescription, font = 'Arial 11', width = 25, activeforeground = 'darkcyan', bg = color,activebackground = 'white',
                                            command = lambda:explainDescription(description))
            self.buttonLabel.grid(row=r, column=c+3,sticky='w',pady=2,padx=6)
            
            
    else:
        if line =="std::vector<size_t>":
            
            self.label=tk.Label(self, text=texto, width = 25,justify='left', bg=color, anchor="w", font="Arial 15")
            self.label.grid(row=r, column=c, sticky='w', pady = 2,padx=6)
            stringVar[texto] = tk.StringVar()
            res[texto] = tk.Entry(self,width=15,textvariable = stringVar[texto], font = 'Arial 11')
            res[texto].grid(row=r, column=c+1,sticky = 'w', pady = 2,padx=6)
            self.corrector = self.register(validarstring)
            res[texto].config(validate = 'focus',validatecommand = (self.corrector,'%P')) # %P represents the parameter we want to pass to validate.
            self.buttonLabel = tk.Button(self, text = fakedescription,font = 'Arial 11', width = 25, activeforeground = 'darkcyan', bg = color,activebackground = 'white',
                                            command = lambda:explainDescription(description))
            self.buttonLabel.grid(row=r, column=c+3,sticky='w' , pady=2,padx=6)
        
        if 'double' in line:
            self.label=tk.Label(self, text=texto, width = 25,justify='left', bg=color, anchor="w", font="Arial 15")
            self.label.grid(row=r, column=c,pady = 2,padx=6,sticky='w')
            stringVar[texto] = tk.StringVar()
            res[texto] = tk.Entry(self,width=15,font='Arial 11',textvariable = stringVar[texto])
            res[texto].grid(row=r, column=c+1, pady= 2,padx=6, sticky = 'w')
            self.corrector = self.register(validardigit)
            self.buttonLabel = tk.Button(self, text = fakedescription, width = 25,font='Arial 11', activeforeground = 'darkcyan', bg = color,activebackground = 'white',
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
    
def printConfig(problem,experiments,selectedtab,directorio,nombre,DB, cont):
    


    ## DELETE ANY WIDGET THAT WAS THERE BEFORE:
    for widget in problem.winfo_children():
        widget.destroy()
    results = experiments[selectedtab]['results']
    results[cont] = {}

    if len(nombre) > 25:
        nombre = nombre[0:25]+'...'
    # Start reading and printing on screen: # DON'T DELETE THIS COMMENT -> TITULO FRAMES : list(DB.keys())[0]
##    self.titulo=tk.Label(self, text=directorio,justify='left', anchor="w", font="Arial 18", bg = 'darkcyan', fg='white')
##    self.titulo.grid(row=r, column=c,pady = 10 ,padx=20, sticky='w', columnspan = 2)
    efake =tk.Label(problem, text='', fg=colorProblem, bg=colorProblem)
    efake.grid(row=0, column=0,columnspan=4,pady = 20 ,padx=10, sticky='nw')
    e =tk.Button(problem, text=nombre,activebackground='aliceblue', font="Arial 20", fg='black', bg=colorProblem,borderwidth=2,relief='solid', command = lambda : functions.Clear(self.gs)) #bg = 'darkcyan', fg='white')
    e.grid(row=1, column=0,columnspan=4,pady = 4 ,padx=10, sticky='n')
    edescr =tk.Label(problem, text='Click title to clear the variables...', font="Arial 10", fg='navy', bg=colorProblem) #bg = 'darkcyan', fg='white')
    edescr.grid(row=2, column=0,columnspan=4,rowspan=1,pady = 4 ,padx=10, sticky='n')
    efake =tk.Label(problem, text='', fg=colorProblem, bg=colorProblem)
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

                                    fakedescription = description
                                    printdata(problem,experiments,selectedtab,dicc[key2], texto, description,fakedescription, r, c,cont,options)                                    
                                r+=1
    else:
        popupmsgwarning('No Configuration Settings found')


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
