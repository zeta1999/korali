import tkinter as tk
from tkinter import *
import os, sys
import json
import copy      

## Frame Variables:
darkColor ='lightseagreen'
lightColor = '#00A3E0'
extraColor = '#183A54'
selectorColor = 'whitesmoke'
bgColor = 'azure'

clickables = ['General Settings', 'Problem', 'Solver', 'Variables', 'Distributions']


experiments = {} # Global dictionary for the frames and experiments.
menus = [] # Store which directories have already been read.
listofproblems=[]
listofsolvers=[]

## Global Variables:
contador = 1    # Which is the num of the experiment where we are?
selectedtab = '' # Which is the name of the experiment where we are?
configTreeDB = {}
solverDB = {}
variables = {}
mainPath = '../../source/modules/problem'
mainPath2 = '../../source/modules/solver'

default = False # readirs function.

experiments = {} # Global dictionary for the frames and experiments.




def display_general_settings():
    print('Not done yet')

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
##    crearVariables(DB)

def printConfig(a,b,c):
    print('printConfig')

def printVariables(a,b,c,d):
    print('Variables')
    
def crearMenu(padre,directorio,DB,cont):
    global menus
    global experiments
    global listofproblems
    global listofsolvers
    
    sf = experiments[selectedtab]['secondFrame']
    nombre = DB[directorio]['names']
    if directorio not in menus: # Checkeamos si el directorio está en la lista de menus.
        subMenu = Menu(padre,activeforeground = 'teal')
        dirInfo=DB[directorio]
        if len(dirInfo['children']) == 0:
            padre.add_command(label = nombre, command = lambda : [printConfig(directorio,DB, cont),printVariables(sf.tab2,directorio,DB,cont)])
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

def cascade(mainPath,DB,cont):
    global experiments
    
    dirs = splitPath(mainPath)
    
    for directorio in DB.keys():
        if directorio in dirs:
            titulo = directorio
            break

    ff = experiments[selectedtab]['firstFrame']

    menuButton = tk.Menubutton(ff, text = ' +   '+DB[titulo]['names'],anchor = 'w',font = 'Arial 16', indicatoron=True, borderwidth = 1, relief='raised', width=15, border=3)
    menuPadre = tk.Menu(menuButton, tearoff=False,activeforeground = 'teal')
    menuButton.configure(menu=menuPadre)
    y_pos = (200+(int(cont)*120))
    menuButton.place(x=70,y=y_pos)
        
    menus.append(titulo) # Añadimos el título del boton, para no repetirlo en los desplegables.
    
      
    # Empezar los menus recursivamente llamando a la funcion crearMenu:
    for directorio in DB.keys():
        crearMenu(menuPadre,directorio,DB,cont)
    menus.clear() # Allows creating different experiments by emptying the menus list.

    


class FirstFrame():
    def __init__(self,master):
        
        ff = tk.Frame(master,bg=selectorColor,width=416,height=930,borderwidth= 3,relief='solid')
        ff.grid(column=0,row=0)
        ff.grid_propagate(0)

        general_settings = tk.Button(ff,text = '+  General Settings',borderwidth =1, anchor = 'w',font= 'Arial 16',  command = lambda:display_general_settings())
        general_settings.place(x=180, y=100, anchor="center")

        # STORE A DICTIONARY WITH ALL THE FRAMES ON IT TO BE CALLED ON ANY FUNCTION AND AVOID PASSING FRAMES.
        expElements = {}
        experiments[selectedtab] = expElements
        experiments[selectedtab]['results'] = {}
        expElements['firstFrame'] = ff
        
        # STORE A DICTIONARY WITH ALL THE FRAMES ON IT TO BE CALLED ON ANY FUNCTION AND AVOID PASSING FRAMES.


class SecondFrame():
    def __init__(self,master):
        global experiments
        global selectedtab
      
        sf = tk.Frame(master,bg=bgColor,width=717,height=930,borderwidth=2,relief='raised')
        sf.grid(column=1,row=0)
        sf.grid_propagate(0)

        experiments[selectedtab]['secondFrame'] = sf

        ## Here because frames need to be finished before starting.
        cont= 0
        cascade(mainPath,configTreeDB,cont)
        cont+=1
        cascade(mainPath2,solverDB,cont)


class ThirdFrame():
    def __init__(self,master):
        
        tf = tk.Frame(master,bg=bgColor,width=716,height=930,borderwidth=2,relief='raised')
        tf.grid(column=2,row=0)
        tf.grid_propagate(0)


configTreeDB = {}
solverDB = {}
variables = {}

readDirs(mainPath, configTreeDB,default)
readDirs(mainPath2,solverDB, default)
