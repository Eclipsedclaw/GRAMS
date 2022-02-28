import numpy as np
import matplotlib.pyplot as plt
import statistics
from numpy import array
import pandas as pd
import math
import time
from os import listdir
from os.path import isfile, join
pd.set_option('display.float_format', lambda x: '%0.4f' % x)

"""
Read data from total data file

"""

Drop = 100    #don't use the last 100 data

print("Reading data ...")
#data = pd.read_table("/Users/jiancheng/GRAMS/Toy Model/Machine Learning/210824_anti_proton_250MeV_Lin.dat")    #get data file
Files = [f for f in listdir("/home/jiancheng/HDD/jiancheng/GRAMS/ToyModelData/processed_data/ORI_Product_deuteron_1e8_Ene_0_600/") if isfile(join("/home/jiancheng/HDD/jiancheng/GRAMS/ToyModelData/processed_data/ORI_Product_deuteron_1e8_Ene_0_600/", f))]

for z in range(len(Files)):
    data = np.array(pd.read_table("/home/jiancheng/HDD/jiancheng/GRAMS/ToyModelData/processed_data/ORI_Product_deuteron_1e8_Ene_0_600/"+Files[z]))
    #data = np.array(pd.read_table("/home/jiancheng/HDD/jiancheng/GRAMS/ToyModelData/test.dat"))
    print("finish reading data file "+str(z+1)+"! total "+str(len(Files))+" files!")

    """
    select analysis particle type
    """
    Particle_Type = 1000010020    #[antiproton]-2212    [antideuteron]-1000010020    [antiHe3]-1000020030

    """
    Analyzed variables
    """

    Nevent = -1
    Downsample = []    #inital counting vector

    print("Modifing data ...")
    for i in range(len(data[:,0])-Drop):
    #for i in range(27):
        Nevent = Nevent + 1
        if(data[i][0] != data[i+1][0]):
            temp = [str(data[i+1][0])]
            angle = 0
            distance = 0
            for k in range(Nevent+1):
                if(str(data[i-Nevent+k][6]) == 'LAr' and float(data[i-Nevent+k][3]) == float(Particle_Type)):    #including stop events and annihilation in flight events
                    for m in range(Nevent+1-k):
                        if(str(data[i-Nevent+k+m][3]) != str(data[i-Nevent+k+m-1][3])):
                            distance = np.sqrt((float(data[i-Nevent+k][15]) - float(data[i-Nevent+k+m-1][15])) ** 2 + (float(data[i-Nevent+k][16]) - float(data[i-Nevent+k+m-1][16])) ** 2 + (float(data[i-Nevent+k][17]) - float(data[i-Nevent+k+m-1][17])) ** 2)    #distance of track in LAr
                           # if(distance>10):
                            temp.append(str(data[i-Nevent+k+m-1][3]))
                           # angle = np.arctan(np.sqrt((float(data[i-Nevent+k+m-1][16])-float(data[i-Nevent+k][16])) ** 2 + (float(data[i-Nevent+k+m-1][15])-float(data[i-Nevent+k][15])) ** 2)/(float(data[i-Nevent+k+m-1][17])-float(data[i-Nevent+k][17])))    #angle of the track
                           # temp.append(angle)
                           # temp.append(distance)
            if(len(temp) != 1):
                Downsample.append(temp)
            Nevent = -1
        #print("TOF data process:"+str(int(100*i/len(data[:,0])))+"%", end='\\r')
                    
    print("\n identification:finished!")

    #Save analysis vector
    with open("/home/jiancheng/HDD/jiancheng/GRAMS/ToyModelData/vector/test.txt", "a") as f:    #input file
        np.savetxt(f, Downsample, fmt='%s')
    print("\n finished processing file "+ str(z+1)+"\n")
print("finished!")
