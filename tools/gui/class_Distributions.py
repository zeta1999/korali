import tkinter as tk
from tkinter import *
import json

#FILES import
import classes_FRAMES
import class_KORALI
import functions

# Frame Variables:
selectorColor = 'snow'
forbidden = ['Variables','Problem','Solver']




class Distributions():
    def __init__(self,master,selectedtab,experiments):
        # master is the frame from the previous class where we want to insert data.
        if class_KORALI.dist_first_time == False: # If its not the first time we initialize the class Distribution...
            if functions.import_dist_done == False: # If this function is called inside a loop of the functions file...
                self.dist = experiments[selectedtab]['distributions']
                self.display_Distributions(self.dist,0,4,selectedtab,experiments)
                return
            else:
                self.Show_frame(experiments,selectedtab)
                return

        self.dist = tk.Frame(master,bg=selectorColor,width=717,height=925)
        self.dist.grid(column=0,row=0)
        self.dist.grid_propagate(0)

        if class_KORALI.dist_times != 1:
            for widget in self.dist.winfo_children():
                widget.destroy()
            class_KORALI.rows = 0
            class_KORALI.dist_times = 1   

        # Distribution Labels:
        experiments[selectedtab]['distributions'] = self.dist
            
        self.efake =tk.Label(self.dist, text='', fg=selectorColor, bg=selectorColor)
        self.efake.grid(row=0, column=0,columnspan=4,pady = 20 ,padx=10, sticky='nw')
        self.e =tk.Button(self.dist, text='Distributions',activebackground='aliceblue', font="Arial 20", fg='black', bg=selectorColor,
                          borderwidth=2,relief='solid', command = lambda : self.display_Distributions(self.dist,c,4,selectedtab,experiments)) #bg = 'darkcyan', fg='white')
        self.e.grid(row=1, column=0,columnspan=2,pady = 4 ,padx=10, sticky='n')
        self.edescr =tk.Label(self.dist, text='Click title to add a Distribution...', font="Arial 10", fg='navy', bg=selectorColor) #bg = 'darkcyan', fg='white')
        self.edescr.grid(row=2, column=0,columnspan=4,rowspan=1,pady = 4 ,padx=10, sticky='n')
        self.efake =tk.Label(self.dist, text='', fg=selectorColor, bg=selectorColor)
        self.efake.grid(row=3, column=0,columnspan=4,pady = 10 ,padx=10, sticky='nw')
        
        class_KORALI.rows = 4

        # STORE THIS FRAME IN THE experiments dictionary.
        results = experiments[selectedtab]['results']
        cont = 4
        
        results[cont] = {}
        res_dist = {}
        experiments[selectedtab]['dist_label'] = res_dist
        c = 0
        

        self.display_Distributions(self.dist,c,cont,selectedtab,experiments)
        class_KORALI.dist_times += 1
        class_KORALI.dist_first_time = False
        

        
    def Show_frame(dist,experiments,selectedtab):
        dist = experiments[selectedtab]['distributions']
        dist.tkraise()

    def delDistr(self,which,selectedtab,experiments):
        global rows_counter_var
        print('ENTRO : ',class_KORALI.rows)

        results = experiments[selectedtab]['results']
        for key in results[4].keys():
            if which > class_KORALI.dist_times:
                functions.popupmsgwarning('First, please delete the last Distribution')
                return
        results = experiments[selectedtab]['results']
        res_dist = experiments[selectedtab]['dist_label']
        # Key value '4' is for Distributions
        res = results[4][which-1]
        res2 = res_dist[which-1]
        
        for entry in res.values():
            entry.destroy()
        for entry in res2.values():
            entry.destroy()

        del results[4][which-1]
        del res_dist[which-1]
        class_KORALI.dist_times -= 1
        class_KORALI.rows -= 7
        print('SALGO : ',class_KORALI.rows)
        
    
    def display_Distributions(self,dist,c,cont,selectedtab,experiments):
        
        if class_KORALI.dist_times > 3:
            functions.popupmsgwarning('No more Distributions available')
            return
        if class_KORALI.rows>4:
            results = experiments[selectedtab]['results']
            results[4][class_KORALI.dist_times]={}
            print('SECOND TIME OF DISTRIBUTIONS = ',class_KORALI.dist_times)
            
            res_dist = experiments[selectedtab]['dist_label']
            res_dist[class_KORALI.dist_times]={}
##            photo = PhotoImage(file = "trash1.ico")   image = photo,  -> añadir a button.
            res = results[4][class_KORALI.dist_times]
            res2 = res_dist[class_KORALI.dist_times]
            class_KORALI.rows += 1
            res2['l'] =tk.Label(self.dist, text='', fg=selectorColor, bg=selectorColor)
            res2['l'].grid(row=class_KORALI.rows, column=0,columnspan=4,pady = 20 ,padx=10, sticky='nw')
            class_KORALI.rows+=1
            res2['b'] =tk.Button(self.dist, text='Distribution '+str(class_KORALI.dist_times),activebackground='aliceblue', font="Arial 16", fg='black', bg=selectorColor,
                          borderwidth=1,relief='solid',command = lambda: self.delDistr(class_KORALI.dist_times,selectedtab,experiments)) #bg = 'darkcyan', fg='white')
            res2['b'].grid(row=class_KORALI.rows, column=0,columnspan=2,pady = 4 ,padx=10, sticky='n')
            class_KORALI.dist_times += 1
        else:
            results = experiments[selectedtab]['results']
            results[4][class_KORALI.dist_times]={}
            print('FIRST TIME OF DISTRIBUTIONS == ',class_KORALI.dist_times)
            res = results[4][class_KORALI.dist_times]
            
            res_dist = experiments[selectedtab]['dist_label']
            res_dist[class_KORALI.dist_times]={}
            res2 = res_dist[class_KORALI.dist_times]

        stringVar = {}

    ##    texto = 'Distribution '+str(times_distr)
    ##    l1 =tk.Label(expFrame, text=texto,justify='left', anchor="w", font="Arial 17", fg='black', bg='white') #bg = 'darkcyan', fg='white')
    ##    l1.grid(row=r, column=c,columnspan= 1,pady = 24 ,padx=4, sticky='w')

        class_KORALI.rows += 1
        texto1 = 'Name'
        res2['l2'] = tk.Label(dist, text = texto1,  width = 25,justify='left', bg='snow', anchor="w", font="Arial 15")
        res2['l2'].grid(row=class_KORALI.rows, column=c,columnspan= 1,pady = 2 ,padx=4, sticky='w')
        stringVar[texto1] = tk.StringVar()
        res[texto1] = tk.Entry(dist,width=18,font='Arial 11',textvariable = stringVar[texto1])
        res[texto1].grid(row=class_KORALI.rows, column=c+1, pady= 2,padx=4, sticky = 'w')

        class_KORALI.rows += 1
        texto2 = 'Type'
        res2['l3'] = tk.Label(dist, text = texto2, width = 25,justify='left', bg='snow', anchor="w", font="Arial 15")
        res2['l3'].grid(row=class_KORALI.rows, column=c,columnspan= 1,pady = 2 ,padx=4, sticky='w')
        stringVar[texto2] = tk.StringVar()
        res[texto2] = tk.Entry(dist,width=18,font='Arial 11',textvariable = stringVar[texto2])
        res[texto2].grid(row=class_KORALI.rows, column=c+1, pady= 2,padx=4, sticky = 'w')

        class_KORALI.rows += 1
        texto3 = 'Maximum'
        res2['l4']=tk.Label(dist, text=texto3, width = 25,justify='left', bg='snow', anchor="w", font="Arial 15")
        res2['l4'].grid(row=class_KORALI.rows, column=c,pady = 2,padx=4, sticky='w')
        num3 = tk.IntVar()
        res[texto3] = Spinbox(dist, width=18, from_=0,font='Arial 11', to=9999, wrap=True, textvariable=num3, state='normal')
        res[texto3].grid(row=class_KORALI.rows, column=c+1, pady= 2,padx=6)
        corrector = dist.register(functions.validardigit)
        res[texto3].config(validate = 'key',validatecommand = (corrector,'%P')) # %P represents the parameter we want to pass to validate.

        class_KORALI.rows += 1
        texto4 = 'Minimum'
        res2['l5']=tk.Label(dist, text=texto4, width = 25,justify='left', bg='snow', anchor="w", font="Arial 15")
        res2['l5'].grid(row=class_KORALI.rows, column=c,pady = 2,padx=4, sticky='w')
        num4 = tk.IntVar()
        res[texto4] = Spinbox(dist, width=18, from_=0,font='Arial 11', to=9999, wrap=True, textvariable=num4, state='normal')
        res[texto4].grid(row=class_KORALI.rows, column=c+1, pady= 2,padx=6)
        corrector = dist.register(functions.validardigit)
        res[texto4].config(validate = 'key',validatecommand = (corrector,'%P')) # %P represents the parameter we want to pass to validate.

        class_KORALI.rows += 1
        texto5 = 'Random Seed'
        res2['l6']=tk.Label(dist, text=texto5, width = 25,justify='left', bg='snow', anchor="w", font="Arial 15")
        res2['l6'].grid(row=class_KORALI.rows, column=c,pady = 2,padx=4, sticky='w')
        num5 = tk.IntVar()
        res[texto5] = Spinbox(dist, width=18, from_=0,font='Arial 11', to=9999, wrap=True, textvariable=num5, state='normal')
        res[texto5].grid(row=class_KORALI.rows, column=c+1, pady= 2,padx=6)
        corrector = dist.register(functions.validardigit)
        res[texto5].config(validate = 'key',validatecommand = (corrector,'%P')) # %P represents the parameter we want to pass to validate.

