"""
Plot data: plot data that just been read
@aurthor Jiancheng Zeng
@April 30 2021

"""

import numpy as np
import matplotlib.pyplot as plt
import statistics
from numpy import array

"""
Select by name of the column
---------------------
"""
n = 19
title = ["eventID", "trackID", "parentID", "particleID", "copyNb1 (mother volume)", "copyNb", "material", "time (ns)", "energy(MeV)", "energy deposit(MeV)", "parent process name", "process name", "px (MeV/c)", "py (MeV/c)", "pz (MeV/c)", "step length (cm)", "x (cm)", "y (cm)", "z (cm)"]
STDEV = []  #standard deviation
#C = ['black', 'springgreen', 'forestgreen', 'darkgreen', 'lightcoral', 'indianred', 'brown', 'royalblue', 'blue', 'navy']  #customize plot color
#W = [1, 0.5, 0.5, 0.5, 1, 1, 1, 1.5, 1.5, 1.5] #customize linewidth

"""
Read data from total data file
--------------------
"""
data = np.genfromtxt("/Users/jiancheng/CloudDocuments/Documents/GitHub/Geant4-simulation/Original_2021/output/test.dat", skip_footer=0, skip_header=0, names=None, delimiter = " , ")    #get data file



#End = len(data[:,0])    #identify data length
Start = 0  #choose start


"""
making plot and calculate standard deviation for equilibrium state
----------------------
"""
for i in n:
    plt.plot(data[:, i-1],  label = title[i], linewidth=0.8)
    #STDEV.append(statistics.stdev(data[10600:14600, i-1]))
    #print("stadard deviation for RTD",RTDn[n],RTDt[n]," is",STDEV[n],'degree C')


"""
Make legends and axis
-------------------
"""
plt.xlabel('Temp')
plt.ylabel('Temp')
#plt.ylim([-60, -20])
plt.title('Test selection')
plt.grid(color='k', linestyle='--', linewidth=.1)
plt.legend()
#plt.annotate("start reservoir heater", [0,0])
plt.show()
