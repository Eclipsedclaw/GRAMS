import pandas as pd
import numpy as np
import time
import math
import matplotlib.pyplot as plt





# this function calculate distance in 3D space
def calculate_distance(x1, y1, z1, x2, y2, z2):
    distance = math.hypot(math.hypot(x2 - x1, y2 - y1), z2 - z1)
    return distance





# This function reads toy model data and reconstruct it in a more user friendly format. 
# After this, each event will have their own data structire
# If you want to all the event ID from the 12th event, you could do:
# 
# data = read_data_from_toymodel(file_path)
# list(data.keys())[12]
# 
# If you want to call all the dataa from the 12th event, you could do:
# 
# data[list(data.keys())[12]]
# 
def read_data_from_toymodel(file_path):
    print("Reading data...\n")
    try:
        df = pd.read_csv(file_path, delimiter='\t')  # Assumes tab-separated values in the text file

        # Name each data group with event ID
        grouped = df.groupby(df.iloc[:, 0])

        # Create an empty dictionary to store the 3D data structure
        data_3d = {}

        # Iterate over each group and populate the dictionary
        for group_name, group_data in grouped:
            data_3d[group_name] = group_data.values
        
        print("Toy Model data imported\nFound " + str(len(list(data_3d.keys()))) + " Events!")

        # Clear the data buffer
        df = None
        
        return data_3d
    except FileNotFoundError:
        print(f"Error: File '{file_path}' not found.")
        return None
    except pd.errors.ParserError:
        print(f"Error: Unable to parse data from '{file_path}'. Please ensure the file has the correct format.")
        return None


    
    
    
    
# This function doing GRASP analysis
def Analyze_GRASP(data_3d, particle_ID, stop_event=False, in_flight_event=False):
    
    # This is the event number in the raw data
    Event_list = list(data_3d.keys())
    
    # Construct a empty vector for initial energy store
    Init_Energy = []
    
    # Counting the number of stop events
    N_stop_event = 0
    
    
    for i in range(len(Event_list)):
        # general condition that all the events has to match, here shows the last point is inside Liquid Argon
        # currently not working properly, please fix this in the future
        # condition = data_3d[Event_list[i]][data_3d[Event_list[i]][:, 8] == np.min(data_3d[Event_list[i]][:, 8])][0, 6] == 'LAr' 
        condition = True
        
        # conditions for stop events, here shows last point has 0 energy left and 0 energy deposit
        condition_stop = np.min(data_3d[Event_list[i]][:, 8]) == 0 and np.min(data_3d[Event_list[i]][:, 9]) == 0 and stop_event
        # conditions for in flight annihilation events, here shows last point has energy larger than 0
        condition_in_flight = np.min(data_3d[Event_list[i]][:, 8]) != 0 and in_flight_event
        
        if(condition_stop or condition_in_flight and condition):
            N_stop_event = N_stop_event + 1
            Init_Energy.append(data_3d[Event_list[i]][0, 8])
        print("Now processing (" + str(i) + "/" + str(len(Event_list))+")",  end='\r')
    print("\n We got " + str(N_stop_event) + " events selected")
    print("Last event is "+ str(Event_list[len(Event_list)-1]))
    
    # return two things, first one is the inital energy array and second one is the number of the event generated.
    return np.array(Init_Energy), Event_list[len(Event_list)-1]




# This function analyze distance traveled inside LAr
def Analyze_trace(data_3d, particle_ID, stop_event=False, in_flight_event=False):
    
    # This is the event number in the raw data
    Event_list = list(data_3d.keys())
    
    # Construct a empty vector for initial energy store
    Init_Energy = []
    
    # record the depth that primary particle traveled inside LAr
    Depth = []
    
    # record the distance that primary particle traveled inside LAr
    Distance = []
    
    for i in range(len(Event_list)):
        # general condition that all the events has to match, here shows the last point is inside Liquid Argon
        condition = data_3d[Event_list[i]][data_3d[Event_list[i]][:, 8] == np.min(data_3d[Event_list[i]][:, 8])][0, 6] == 'LAr' 
        # conditions for stop events, here shows last point has 0 energy left and 0 energy deposit
        condition_stop = np.min(data_3d[Event_list[i]][:, 8]) == 0 and np.min(data_3d[Event_list[i]][:, 9]) == 0 and stop_event
        # conditions for in flight annihilation events, here shows last point has energy larger than 0
        condition_in_flight = np.min(data_3d[Event_list[i]][:, 8]) != 0 and in_flight_event
        
        if(condition_stop or condition_in_flight and condition):
            x1 = data_3d[Event_list[i]][data_3d[Event_list[i]][:, 8] == np.min(data_3d[Event_list[i]][:, 8])][0, 16]
            y1 = data_3d[Event_list[i]][data_3d[Event_list[i]][:, 8] == np.min(data_3d[Event_list[i]][:, 8])][0, 17]
            z1 = data_3d[Event_list[i]][data_3d[Event_list[i]][:, 8] == np.min(data_3d[Event_list[i]][:, 8])][0, 18]
            minimum_time = np.min(data_3d[Event_list[i]][data_3d[Event_list[i]][:, 6] == 'LAr'][:, 7])
            x2 = data_3d[Event_list[i]][data_3d[Event_list[i]][:, 7] == minimum_time][0, 16]
            y2 = data_3d[Event_list[i]][data_3d[Event_list[i]][:, 7] == minimum_time][0, 17]
            z2 = data_3d[Event_list[i]][data_3d[Event_list[i]][:, 7] == minimum_time][0, 18]
            #print(minimum_time)
            Distance.append(calculate_distance(x1, y1, z1, x2, y2, z2))
            Depth.append(z2-z1)
            Init_Energy.append(data_3d[Event_list[i]][0, 8])
        print("Now processing (" + str(i) + "/" + str(len(Event_list))+")",  end='\r')
    print("\n We got " + str(len(Depth)) + " events selected")
    print("Last event is "+ str(Event_list[len(Event_list)-1]))
    
    # return two things, first one is the inital energy array and second one is the depth traveled inside LAr.
    return np.array(Init_Energy), np.array(Depth), np.array(Distance)






# This function is for analyzing TOF and angle of the trace
# Need update!
def Analyze_TOF(data_3d, particle_ID, stop_event=False):
    Event_list = list(data_3d.keys())
    TOF_out_E = []
    TOF_in_E = []
    for i in range(len(Event_list)):
        Energy_out = 0
        Energy_in = 0
        for j in range(len(data_3d[Event_list[i]])):
            if(str(data_3d[Event_list[i]][j, 5]) == '-10000' or str(data_3d[Event_list[i]][j, 5]) == '-10001' or str(data_3d[Event_list[i]][j, 5]) == '-10002' or str(data_3d[Event_list[i]][j, 5]) == '-10003' or str(data_3d[Event_list[i]][j, 5]) == '-10004' or str(data_3d[Event_list[i]][j, 5]) == '-10005' and str(data_3d[Event_list[i]][3]) == str(particle_ID)):
                Energy_out = Energy_out + float(data_3d[Event_list[i]][j, 9])
                #print(Energy_out)
                #if str(data_3d[Event_list[i]][j+1, 5]) != str(data_3d[Event_list[i]][j, 5]):
                    #break
            if(str(data_3d[Event_list[i]][j, 5]) == '-11000' or str(data_3d[Event_list[i]][j, 5]) == '-11001' or str(data_3d[Event_list[i]][j, 5]) == '-11002' or str(data_3d[Event_list[i]][j, 5]) == '-11003' or str(data_3d[Event_list[i]][j, 5]) == '-11004' or str(data_3d[Event_list[i]][j, 5]) == '-11005' and str(data_3d[Event_list[i]][3]) == str(particle_ID)):
                Energy_in = Energy_in + float(data_3d[Event_list[i]][j, 9])
        if(stop_event==True):
            condition = np.min(data_3d[Event_list[i]][:, 8]) == 0 and np.min(data_3d[Event_list[i]][:, 9]) == 0 and stop_event
        else:
            condition = True

        if(Energy_in != 0 and Energy_out != 0 and condition):
            TOF_out_E.append(Energy_out)
            TOF_in_E.append(Energy_in)
        print("Now processing (" + str(i) + "/" + str(len(Event_list))+")",  end='\r')
    Result_Vector = np.array([TOF_out_E, TOF_in_E])
    print("\n")
    print("number of events is:"+str(Event_list[len(Event_list)-1]))
    return Result_Vector, Event_list[len(Event_list)-1]




# This function doing primary daughter particles analysis including energy and directions
def Analyze_daughter(data_3d, particle_ID, stop_event=False, in_flight_event=False):
    
    # This is the event number in the raw data
    Event_list = list(data_3d.keys())
    
    # Construct a empty vector for initial e energy store
    Init_Energy = []
    
    # Construct a empty vector for angle of the primary daughter particles
    Primary_Angle = []
    
    # Construct a empty vector for angle change of the primary daughter particles
    Scatter_Angle = []
    
    # Construct a empty vector for 
    
    
    for i in range(len(Event_list)):
        # general condition that all the events has to match, here shows the last point is inside Liquid Argon
        condition = data_3d[Event_list[i]][data_3d[Event_list[i]][:, 8] == np.min(data_3d[Event_list[i]][:, 8])][0, 6] == 'LAr' 
        # conditions for stop events, here shows last point has 0 energy left and 0 energy deposit
        condition_stop = np.min(data_3d[Event_list[i]][:, 8]) == 0 and np.min(data_3d[Event_list[i]][:, 9]) == 0 and stop_event
        # conditions for in flight annihilation events, here shows last point has energy larger than 0
        condition_in_flight = np.min(data_3d[Event_list[i]][:, 8]) != 0 and in_flight_event
        
        if(condition_stop or condition_in_flight and condition):
            N_stop_event = N_stop_event + 1
            Init_Energy.append(data_3d[Event_list[i]][0, 8])
        print("Now processing (" + str(i) + "/" + str(len(Event_list))+")",  end='\r')
    print("\n We got " + str(N_stop_event) + " events selected")
    print("Last event is "+ str(Event_list[len(Event_list)-1]))
    
    # return two things, first one is the inital energy array and second one is the number of the event generated.
    return np.array(Init_Energy), Event_list[len(Event_list)-1]





# This function is for plotting the histgram of GRASP
def GRASP_hist(GRASP_vector, particle_ID, label, total_event, energy_min, energy_max, num_bins, ax=None):
    if ax is None:
        fig, ax = plt.subplots()
    
    GRASP_bin = []
    
    Xaxis = np.linspace(energy_min, energy_max, num_bins + 1) / number_of_nucleons(particle_ID)[1]
    centersXaxis = Xaxis[:-1] + np.diff(Xaxis) / 2

    for i in range(num_bins):
        bin_start = i * ((energy_max - energy_min) / num_bins)
        bin_end = (i + 1) * ((energy_max - energy_min) / num_bins)
        GRASP_bin.append(400 * math.pi * len(GRASP_vector[(GRASP_vector > bin_start) & (GRASP_vector < bin_end)]) / (total_event / num_bins))
        
        # For debugging each bin
        # print("GRASP_bin[" + str(i) + "] is " + str(GRASP_bin[i]))
        
    ax.step(centersXaxis, GRASP_bin, where='mid', label=str(label)+"("+str(len(GRASP_vector))+" events)", alpha=0.8)
    ax.set_xlabel("energy [MeV/n]")
    ax.set_ylabel("GRASP [m^2 sr]")
    ax.legend()
    ax.grid(alpha=0.2)
    ax.set_title('GRASP (Geometric Acceptance)')
    return ax






#This function is for identify particles in geant4 including particle ID and Z numbers etc
def number_of_nucleons(particle_ID):
    data = [[2212, 1000010020, 1000020030, -2212, -1000010020, -1000020030],
            [1, 2, 3, 1, 2, 3]]
    df = pd.DataFrame(data, columns=['proton', 'deuteron', 'helium3', 'antiproton', 'antideuteron', 'antihelium3'])
    matching_columns = df.columns[df.eq(particle_ID).any()]
    particle_info = df[matching_columns[0]]
    return particle_info
