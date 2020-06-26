'''
*Usage*                                                                                                                                   #####
*Reads the Folders and Files needed and stores its information into 2 Dictionaries [ ConfigTreeDB (for Problem), SolverDB (for Solver)    #####
Creates the 3 MAIN FRAMES of the GUI..                                                                                                    #####
                                                                                                                                          #####
*Functions and Classes*                                                                                                                   #####
*3 classes: FirstFrame, SecondFrame and ThirdFrame.                                                                                       #####
*Main Function: ReadDirs - Is in charge of managing the creation and the amount of Distributions in the SecondFrame                       #####
                                                                                                                                          #####
*CONT = 0 for PROBLEM.                                                                                                                    #####
*CONT = 1 for SOLVER.                                                                                                                     #####
*CONT = 2 for VARIABLES.                                                                                                                  #####
*CONT = 4 for DISTRIBUTIONS.                                                                                                              #####
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
import copy
from PIL import Image, ImageTk


# FILES import
import class_GeneralSettings
import functions
import class_Variables
import class_Distributions
import class_BrowserFrame
import class_KORALI


## Frame Variables:
clickables = ['General Settings', 'Problem', 'Solver', 'Variables', 'Distributions']
bgColor = 'azure'
darkColor ='lightseagreen'
lightColor = '#00A3E0'
extraColor = '#183A54'
selectorColor = 'aliceblue'

menus = [] # Store which directories have already been read.
listofproblems=[]
listofsolvers=[]

## Global Variables:
configTreeDB = {}
solverDB = {}
variables = {}
mainPath = '../../source/modules/problem'
mainPath2 = '../../source/modules/solver'

default = False # readirs function default variable.


### ------- Creating DICTIONARIES before starting:
def recursiveUpdate(dest, defaults):            # Stores the Heritage of each .config File.
 if (isinstance(defaults, dict)):               # If default is a dictionary...
  for k, x in defaults.items():                 # For every key and value...
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
                    finally:
                        archivo.close()
                elif fileName.endswith('.rst'): # Ask for the name inside the README.rst
                    try:
                        file = open(filePath+'/'+fileName,"r")
                        lines = file.readlines()
                        for i in range(0,len(lines)):
                            line = lines[i]
                            if '****\n' in line or '===\n' in line:
                                name = lines[i+1]
                                dirInfoDic['names'] = name.replace('\n','') # Remove '\n'
                                break
                            else:
                                pass
                    finally:
                        file.close() ## Close File README.rst
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
######### End - CREATING DICTIONARIES before starting ------------
    
### Create VARIABLES dictionary by combining the variables from both previous dictionaries
def crearVariables(DB):
    for key in DB.keys():
        if 'Variables Configuration' in DB[key]['herencia'].keys():
            if DB[key]['herencia']['Variables Configuration']:
                variables[DB[key]['config']] = DB[key]['herencia']['Variables Configuration']
            else:
                variables[DB[key]['config']] = {}
        else:
            variables[DB[key]['config']] = {}
### End - VARIABLES dictionary.

### START - Creating Cascade Menus for Problem and Solver.
def createMenu(padre,directorio,DB,cont,x_pos,y_pos,selectedtab,experiments):            
    global menus
    global listofproblems
    global listofsolvers
    global variables

    tf = experiments[selectedtab]['thirdFrame']
    sf = experiments[selectedtab]['secondFrame']
    ff = experiments[selectedtab]['firstFrame']
    nombre = DB[directorio]['names']
    if directorio not in menus: # Check if the directory is in the Menu list.
        subMenu = Menu(padre,activeforeground = 'teal')
        dirInfo=DB[directorio]
        if len(dirInfo['children']) == 0:
            padre.add_command(label = nombre, command = lambda :functions.checkFormulario(sf,experiments,selectedtab, directorio,nombre, DB, cont,x_pos,y_pos,tf))
        else:
            padre.add_cascade(label=nombre, menu = subMenu)
            configPath=dirInfo['config']
            if configPath != "NULL" and len(dirInfo['children']) == 0: # If its a leaf...
                dirs=splitPath(configPath)
                subMenu.add_command(label = dirs[len(dirs)-1], command = lambda : functions.checkFormulario(sf,experiments,selectedtab, directorio,nombre, DB, cont,x_pos,y_pos,tf))
            children=dirInfo['children']
            for child in children:
                createMenu(subMenu,child,DB,cont,x_pos,y_pos,selectedtab,experiments)
    menus.append(directorio)
    if cont == 0:
        if directorio not in listofproblems:
            listofproblems.append(directorio)
    elif cont == 1:
        if directorio not in listofsolvers:
            listofsolvers.append(directorio)

def cascade(mainPath,DB,cont,selectedtab,experiments):  
    dirs = splitPath(mainPath)
    for directorio in DB.keys():
        if directorio in dirs:
            titulo = directorio
            break
    ff = experiments[selectedtab]['firstFrame']
    button_name = DB[titulo]['names'][:-1]
    menuButton = tk.Menubutton(ff, text = ' +   '+button_name,fg =extraColor,highlightcolor=selectorColor,
                               borderwidth = 0, background = selectorColor, anchor = 'w',activeforeground='teal',
                               activebackground= selectorColor,font= 'Arial 17 bold')
    menuButton.config(cursor = 'fleur')
    menuPadre = tk.Menu(menuButton, tearoff=False,activeforeground = 'teal')
    menuButton.configure(menu=menuPadre)
    y_pos = (215+(int(cont)*120))
    x_pos = 0
    menuButton.place(x=x_pos,y=y_pos)
        
    menus.append(titulo) # Adding the Button's title to avoid repetition in cascades.    
      
    # Starting the menus recursivly calling to CreateMenu function.
    for directorio in DB.keys():
        createMenu(menuPadre,directorio,DB,cont,x_pos,y_pos,selectedtab,experiments)
    menus.clear() # Allows creating different experiments by emptying the menus list.
    
####### END - Creating Cascade Menus

class FirstFrame():
    def __init__(self,master,whichtab,exp):

        selectedtab = whichtab
        experiments = exp
       
        ff = tk.Frame(master,bg=selectorColor,width=266,height=930,borderwidth= 3,relief='solid')
        ff.grid(column=0,row=0)
        ff.grid_propagate(0)

        # STORE A DICTIONARY WITH ALL THE FRAMES ON IT TO BE CALLED ON ANY FUNCTION AND AVOID PASSING FRAMES.
        experiments[selectedtab]['results'] = {}
        experiments[selectedtab]['firstFrame'] = ff      

        canvasff = tk.Canvas(ff, width=255, height=220,borderwidth = 3, relief = 'solid')

        img = ImageTk.PhotoImage(Image.open('images_KORALI/CSELAB_ETH_logo.png').resize((255, 220), Image.ANTIALIAS)) 
        canvasff.background = img  # Keep a reference in case this code is put in a function.
        bg = canvasff.create_image(0, 0, anchor=tk.NW, image=img)
        canvasff.place(x=0,y=700)

class SecondFrame():
    def __init__(self,master,whichtab,exp):
        
        selectedtab = whichtab
        experiments = exp
        
        self.sf = tk.Frame(master,bg=bgColor,width=500,height=930,borderwidth=2,relief='ridge')
        self.sf.grid(column=1,row=0)
        self.sf.grid_propagate(0)
        
        experiments[selectedtab]['secondFrame'] = self.sf

        self.tf = tk.Frame(master,width=1083,height=930,borderwidth=2,relief='raised')

        experiments[selectedtab]['thirdFrame'] = self.tf

        # Create KORALI background for the third frame:
        canvas = tk.Canvas(self.tf, width=1083, height=930)

        img = ImageTk.PhotoImage(Image.open('images_KORALI/korali_screen.png').resize((1083, 930), Image.ANTIALIAS))
        canvas.background = img 
        bg = canvas.create_image(0, 0, anchor=tk.NW, image=img)
        self.tf.grid(column=2,row=0)
        self.tf.grid_propagate(0)
        canvas.pack()

        ## General Settings Button:
        ff = experiments[selectedtab]['firstFrame']
        space1 = tk.Label(ff,text='',font='Arial 20', background = selectorColor)
        space2 = tk.Label(ff,text='', font='Arial 20', background =selectorColor )
        space1.grid(row=0,column=0)
        space2.grid(row=1,column=0)
        general_settings = tk.Button(ff,text = '+  General Settings',fg =extraColor,highlightcolor=selectorColor,
                               relief = 'flat', background = selectorColor, anchor = 'w',activeforeground='teal',
                               activebackground= selectorColor,font= 'Arial 17 bold',
                                     command = lambda:class_GeneralSettings.GeneralSettings(self.sf,selectedtab,experiments))
        general_settings.config(highlightbackground=selectorColor)
        general_settings.place(x = 0, y = 100)

        cont = 0
        cascade(mainPath,configTreeDB,cont,selectedtab,experiments)
        cont+=1
        cascade(mainPath2,solverDB,cont,selectedtab,experiments)

        variables = tk.Button(ff,text = '+  Variables',fg =extraColor,highlightcolor=selectorColor,
                               relief = 'flat', background = selectorColor, anchor = 'w',activeforeground='teal',
                               activebackground= selectorColor,font= 'Arial 17 bold',
                              command = lambda:class_Variables.Variables.Show_frame(experiments,selectedtab))
        variables.config(highlightbackground=selectorColor)
        variables.place(x = 0, y= 445)

        distributions = tk.Button(ff,text = '+  Distributions',fg =extraColor,highlightcolor=selectorColor,
                               relief = 'flat', background = selectorColor, anchor = 'w',activeforeground='teal',
                               activebackground= selectorColor,font= 'Arial 17 bold',
                                  command = lambda: class_Distributions.Distributions(self.sf,selectedtab,experiments))
        distributions.config(highlightbackground=selectorColor)
        distributions.place(x = 0, y= 555)

class ThirdFrame():
    def __init__(self,master,selectedtab,experiments):

        tf = experiments[selectedtab]['thirdFrame']
        
        self.browser_frame = class_BrowserFrame.BrowserFrame(tf)
        #self.browser_frame.grid(row=1, column=0,sticky=(tk.N + tk.S + tk.E + tk.W)) # Make .grid later so it doesn't appear when nothing is displayed.
        
    def get_browser(self):
        self.browser_frame.grid(row=1, column=0,
                                sticky=(tk.N + tk.S + tk.E + tk.W))                  # Adding the .grid now.
        if self.browser_frame:
            return self.browser_frame.browser
        return None




configTreeDB = {}
solverDB = {}
variables = {}

readDirs(mainPath, configTreeDB,default)
readDirs(mainPath2,solverDB, default)
