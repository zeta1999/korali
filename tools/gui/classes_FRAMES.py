import tkinter as tk
from tkinter import *
import os, sys
import json
import copy

# FILES import
import class_GeneralSettings
import functions
import class_Variables
import class_Distributions
import class_BrowserFrame


## Frame Variables:
clickables = ['General Settings', 'Problem', 'Solver', 'Variables', 'Distributions']
bgColor = 'azure'
darkColor ='lightseagreen'
lightColor = '#00A3E0'
extraColor = '#183A54'
selectorColor = 'aliceblue'

##experiments = {} # Global dictionary for the frames and experiments.
menus = [] # Store which directories have already been read.
listofproblems=[]
listofsolvers=[]

## Global Variables:
contador = 1    # Which is the num of the experiment where we are?
##selectedtab = '' # Which is the name of the experiment where we are?
configTreeDB = {}
solverDB = {}
variables = {}
mainPath = '../../source/modules/problem'
mainPath2 = '../../source/modules/solver'

default = False # readirs function.

##experiments = {} # Global dictionary for the frames and experiments.


### ------- Creating DICTIONARIES before starting:
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
######## END OF CREATING DICTIONARIES before starting ------------
    
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

def crearMenu(padre,directorio,DB,cont,x_pos,y_pos,selectedtab,experiments):
    global menus
##    global experiments
    global listofproblems
    global listofsolvers
    global variables

    tf = experiments[selectedtab]['thirdFrame']
    sf = experiments[selectedtab]['secondFrame']
    ff = experiments[selectedtab]['firstFrame']
    nombre = DB[directorio]['names']
    if directorio not in menus: # Checkeamos si el directorio está en la lista de menus.
        subMenu = Menu(padre,activeforeground = 'teal')
        dirInfo=DB[directorio]
        if len(dirInfo['children']) == 0:
            padre.add_command(label = nombre, command = lambda :functions.checkFormulario(sf,experiments,selectedtab, directorio,nombre, DB, cont,x_pos,y_pos,tf))#[printConfig(directorio,DB, cont),printVariables(sf.tab2,directorio,DB,cont)])
        else:
            padre.add_cascade(label=nombre, menu = subMenu)
            configPath=dirInfo['config']
            if configPath != "NULL" and len(dirInfo['children']) == 0: # SI ES UN LEAF:
                dirs=splitPath(configPath)
                subMenu.add_command(label = dirs[len(dirs)-1], command = lambda : functions.checkFormulario(sf,experiments,selectedtab, directorio,nombre, DB, cont,x_pos,y_pos,tf))#command = lambda : [printConfig(directorio,DB, cont),printVariables(bottomrightFrame.tab2,directorio,DB,cont)])
            children=dirInfo['children']
            for child in children:
                crearMenu(subMenu,child,DB,cont,x_pos,y_pos,selectedtab,experiments)
    menus.append(directorio)
    if cont == 0:
        if directorio not in listofproblems:
            listofproblems.append(directorio)
    elif cont == 1:
        if directorio not in listofsolvers:
            listofsolvers.append(directorio)

def cascade(mainPath,DB,cont,selectedtab,experiments):
##    global experiments
    
    dirs = splitPath(mainPath)
    
    for directorio in DB.keys():
        if directorio in dirs:
            titulo = directorio
            break

    ff = experiments[selectedtab]['firstFrame']

    menuButton = tk.Menubutton(ff, text = ' +   '+DB[titulo]['names'],fg =extraColor,highlightcolor=selectorColor,
                               borderwidth = 0, background = selectorColor, anchor = 'w',activeforeground='teal',
                               activebackground= selectorColor,font= 'Arial 16')
    menuButton.config(cursor = 'fleur')
    menuPadre = tk.Menu(menuButton, tearoff=False,activeforeground = 'teal')
    menuButton.configure(menu=menuPadre)
    y_pos = (185+(int(cont)*120))
    x_pos = 0
    menuButton.place(x=x_pos,y=y_pos)
        
    menus.append(titulo) # Añadimos el título del boton, para no repetirlo en los desplegables.
    
      
    # Empezar los menus recursivamente llamando a la funcion crearMenu:
    for directorio in DB.keys():
        crearMenu(menuPadre,directorio,DB,cont,x_pos,y_pos,selectedtab,experiments)
    menus.clear() # Allows creating different experiments by emptying the menus list.

    


class FirstFrame():
    def __init__(self,master,whichtab,exp):
        global experiments
        global selectedtab

        selectedtab = whichtab
        experiments = exp
        
        ff = tk.Frame(master,bg=selectorColor,width=266,height=930,borderwidth= 3,relief='solid')
        ff.grid(column=0,row=0)
        ff.grid_propagate(0)

        # STORE A DICTIONARY WITH ALL THE FRAMES ON IT TO BE CALLED ON ANY FUNCTION AND AVOID PASSING FRAMES.
####        expElements = {}
##        experiments[selectedtab] = expElements
        experiments[selectedtab]['results'] = {}
        experiments[selectedtab]['firstFrame'] = ff

        self.HELP = tk.Button(text = "How's it going ?", command = lambda: functions.howsitgoing(experiments,selectedtab))
        self.HELP.place(x=0,y=700)
        

class SecondFrame():
    def __init__(self,master,whichtab,exp):
        global experiments
        global selectedtab
        
        
        selectedtab = whichtab
        experiments = exp

##        general_first_time = class_KORALI.general_first_time
      
        self.sf = tk.Frame(master,bg=bgColor,width=500,height=930,borderwidth=2,relief='raised')
        self.sf.grid(column=1,row=0)
        self.sf.grid_propagate(0)
        
        experiments[selectedtab]['secondFrame'] = self.sf

        self.tf = tk.Frame(master,bg='white',width=1083,height=930,borderwidth=2,relief='raised')
        self.tf.grid(column=2,row=0)
        self.tf.grid_propagate(0)

        experiments[selectedtab]['thirdFrame'] = self.tf

        ## General Settings Button:
        ff = experiments[selectedtab]['firstFrame']
        space1 = tk.Label(ff,text='',font='Arial 20', background = selectorColor)
        space2 = tk.Label(ff,text='', font='Arial 20', background =selectorColor )
        space1.grid(row=0,column=0)
        space2.grid(row=1,column=0)
        general_settings = tk.Button(ff,text = '+  General Settings',fg =extraColor,highlightcolor=selectorColor,
                               relief = 'flat', background = selectorColor, anchor = 'w',activeforeground='teal',
                               activebackground= selectorColor,font= 'Arial 16', command = lambda:class_GeneralSettings.GeneralSettings(self.sf,selectedtab,experiments))
        general_settings.config(highlightbackground=selectorColor)
        general_settings.place(x = 0, y = 70)

    
        ## Here because frames need to be finished before starting.
        cont= 0
        cascade(mainPath,configTreeDB,cont,selectedtab,experiments)
        cont+=1
        cascade(mainPath2,solverDB,cont,selectedtab,experiments)

        variables = tk.Button(ff,text = '+  Variables',fg =extraColor,highlightcolor=selectorColor,
                               relief = 'flat', background = selectorColor, anchor = 'w',activeforeground='teal',
                               activebackground= selectorColor,font= 'Arial 16', command = lambda:class_Variables.Variables.Show_frame(experiments,selectedtab))
        variables.config(highlightbackground=selectorColor)
        variables.place(x = 0, y= 415)

        distributions = tk.Button(ff,text = '+  Distributions',fg =extraColor,highlightcolor=selectorColor,
                               relief = 'flat', background = selectorColor, anchor = 'w',activeforeground='teal',
                               activebackground= selectorColor,font= 'Arial 16', command = lambda: class_Distributions.Distributions(self.sf,selectedtab,experiments))
        distributions.config(highlightbackground=selectorColor)
        distributions.place(x = 0, y= 525)

class ThirdFrame():
    def __init__(self,master,selectedtab,experiments):
        
        tf = experiments[selectedtab]['thirdFrame']
##        self.navigation_bar = class_BrowserFrame.NavigationBar(tf)
##        self.navigation_bar.grid(row=0, column=0,
##                                 sticky=(tk.N + tk.S + tk.E + tk.W))
        
        self.browser_frame = class_BrowserFrame.BrowserFrame(tf)
        self.browser_frame.grid(row=1, column=0,
                                sticky=(tk.N + tk.S + tk.E + tk.W))
##        tk.Grid.rowconfigure(self, 1, weight=1)
##        tk.Grid.columnconfigure(self, 0, weight=1)
        
    def get_browser(self):
        if self.browser_frame:
            return self.browser_frame.browser
        return None


configTreeDB = {}
solverDB = {}
variables = {}

readDirs(mainPath, configTreeDB,default)
readDirs(mainPath2,solverDB, default)
