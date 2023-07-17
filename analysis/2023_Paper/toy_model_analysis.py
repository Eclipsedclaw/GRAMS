import pandas as pd
import numpy as np
import time
import math
import matplotlib.pyplot as plt





# this function calculate distance in 3D space
def calculate_distance(x1, y1, z1, x2, y2, z2):
    distance = math.hypot(math.hypot(x2 - x1, y2 - y1), z2 - z1)
    return distance

# this function calculate angle based on momentum
def calculate_angle(px1, py1, pz1, px2, py2, pz2):
    vector1 = np.array([px1, py1, pz1])
    vector2 = np.array([px2, py2, pz2])

    dot_product = np.dot(vector1, vector2)
    norm_product = np.linalg.norm(vector1) * np.linalg.norm(vector2)
    
    if norm_product == 0:
        return np.nan
    
    cosine_angle = dot_product / norm_product
    angle_rad = np.arccos(cosine_angle)
    angle_deg = np.degrees(angle_rad)
    return angle_deg

# this function calculate angle based on location
def calculate_angle_location(x1, y1, z1, x2, y2, z2, x, y, z, px1, py1, pz1):
    vector1 = np.array([x2 - x1, y2 - y1, z2 - z1])
    vector2 = np.array([x, y, z])

    dot_product = np.dot(vector1, vector2)
    norm_product = np.linalg.norm(vector1) * np.linalg.norm(vector2)

    if norm_product == 0:
        angle_deg = calculate_angle(px1, py1, pz1, x, y, x)
        return angle_deg

    cosine_angle = dot_product / norm_product
    angle_rad = np.arccos(cosine_angle)
    angle_deg = np.degrees(angle_rad)

    return angle_deg


# this function checks if a new element is already exist in the original list or not
def check_value(value, value_list):
    for item in value_list:
        if item == value:
            return True
    return False

# this function makes a process bar for the analysis progress
def display_progress_bar(i, N, bar_length=50):
    progress = i / N
    num_bar_filled = int(progress * bar_length)
    num_bar_empty = bar_length - num_bar_filled

    bar = '[' + '#' * num_bar_filled + ' ' * num_bar_empty + ']'
    percentage = progress * 100

    # Color the percentage in red
    percentage_text = f'\033[91m{percentage:.2f}%\033[0m'

    print(f'\r{bar} {percentage_text} {i}/{N}', end='')


# This function reads toy model data and reconstruct it in a more user friendly format. 
# After this, each event will have their own data structire
# If you want to all the event ID from the 12th event, you could do:
# 
# data = read_data_from_toymodel(file_path)
# list(data.keys())[12]
# 
# If you want to call all the data from the 12th event, you could do:
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
        display_progress_bar(i+1, len(Event_list))
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
        display_progress_bar(i+1, len(Event_list))
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
        display_progress_bar(i+1, len(Event_list))
        display_progress_bar(i+1, len(Event_list))
    Result_Vector = np.array([TOF_out_E, TOF_in_E])
    print("\n")
    print("number of events is:"+str(Event_list[len(Event_list)-1]))
    return Result_Vector, Event_list[len(Event_list)-1]

class daughters:
    def __init__(self, number_of_events, columns):
        self.rows = number_of_events
        self.columns = columns
        self.data = [[0] * columns for _ in range(number_of_events)]

    def get_value(self, row, column):
        return self.data[row][column]

    def set_value(self, row, column, value):
        self.data[row][column] = value

    def append_element(self, value):
        self.data.append([value] * self.columns)
        self.rows += 1

        

# This function doing primary daughter particles analysis including energy and directions
def Analyze_daughter(data_3d, particle_ID, stop_event=False, in_flight_event=False):
    
    # This is the event number in the raw data
    Event_list = list(data_3d.keys())
    
    # Define result vector based on the property of the daughter particles
    Result_vector = daughters(len(Event_list), 2)
        
    # Construct an empty vector for initial primary particle energy store
    Init_Energy = []
    
    # Construct an empty vector for angle of the primary particles
    Primary_Angle = []
    
    # Construct an empty vector for Energy deposit in the outer TOF
    E_TOF_OUT = []
    
    # Construct an empty vector for Energy deposit in the inner TOF
    E_TOF_IN = []
    
    # Construct an empty vector for time difference in the outer and inner TOF
    Delta_T = []
    
    for i in range(len(Event_list)):
        
        # Construct an empty vector for daughter particle ID
        Daughter_ID = []
    
        # Construct an empty vector for initial daughter particle energy store
        Daughter_Energy = []
        
        # Construct an empty vector for angle of the daughter particles
        Daughter_Angle = []
    
        # Construct an empty vector for angle change of the primary daughter particles
        Angle_Change = []
    
        # Construct an empty vector for distance traveled inside LAr
        Distance_Traveled = []
        
        Energy_out = 0
        Energy_in = 0
        Time_out = 0
        Time_in = 0

        data_track = []
        track_ID = []
        # Name each data group with track ID
        data_track = pd.DataFrame(data_3d[Event_list[i]])
        grouped = data_track.groupby(data_track.iloc[:, 1])

        # Create an empty dictionary to store the data structure just for this particular event
        data_temp = {}

        # Iterate over each group and populate the dictionary
        for group_name, group_data in grouped:
            data_temp[group_name] = group_data.values
        track_ID = list(data_temp.keys())

        # general condition that all the events has to match, here shows the last point is inside Liquid Argon
        # currently not working properly, please fix this in the future
        # condition = data_3d[Event_list[i]][data_3d[Event_list[i]][:, 8] == np.min(data_3d[Event_list[i]][:, 8])][0, 6] == 'LAr' 
        condition = True
        
        # conditions for stop events, here shows last point has 0 energy left and 0 energy deposit
        condition_stop = True
        # condition_stop = np.min(data_3d[Event_list[i]][:, 8]) == 0 and np.min(data_3d[Event_list[i]][:, 9]) == 0 and stop_event
        # conditions for in flight annihilation events, here shows last point has energy larger than 0
        condition_in_flight = True
        # condition_in_flight = np.min(data_3d[Event_list[i]][:, 8]) != 0 and in_flight_event
        
        # these are conditions for GRASP, need to update
        # general condition that all the events has to match, here shows the last point is inside Liquid Argon
        #condition = data_3d[Event_list[i]][data_3d[Event_list[i]][:, 8] == np.min(data_3d[Event_list[i]][:, 8])][0, 6] == 'LAr' and Energy_in != 0 and Energy_out != 0
        # conditions for stop events, here shows last point has 0 energy left and 0 energy deposit
        #condition_stop = np.min(data_3d[Event_list[i]][:, 8]) == 0 and np.min(data_3d[Event_list[i]][:, 9]) == 0 and stop_event
        # conditions for in flight annihilation events, here shows last point has energy larger than 0
        #condition_in_flight = np.min(data_3d[Event_list[i]][:, 8]) != 0 and in_flight_event
        
        # Need to optimize this in the future
        for j in range(len(track_ID)):
            if(list(data_temp.keys())[j] == 1):
                for k in range(len(data_temp[track_ID[j]])):
                    if(str(data_temp[track_ID[j]][k, 5]) == '-10000' or str(data_temp[track_ID[j]][k, 5]) == '-10001' or str(data_temp[track_ID[j]][k, 5]) == '-10002' or str(data_temp[track_ID[j]][k, 5]) == '-10003' or str(data_temp[track_ID[j]][k, 5]) == '-10004' or str(data_temp[track_ID[j]][k, 5]) == '-10005' and str(data_3d[Event_list[i]][3]) == str(particle_ID)):
                        Energy_out = Energy_out + float(data_temp[track_ID[j]][k, 9])
                        if(Time_out == 0):
                            Time_out = data_temp[track_ID[j]][k, 7]
                    if(str(data_temp[track_ID[j]][k, 5]) == '-11000' or str(data_temp[track_ID[j]][k, 5]) == '-11001' or str(data_temp[track_ID[j]][k, 5]) == '-11002' or str(data_temp[track_ID[j]][k, 5]) == '-11003' or str(data_temp[track_ID[j]][k, 5]) == '-11004' or str(data_temp[track_ID[j]][k, 5]) == '-11005' and str(data_3d[Event_list[i]][3]) == str(particle_ID)):
                        Energy_in = Energy_in + float(data_temp[track_ID[j]][k, 9])
                        if(Time_in == 0):
                            Time_in = data_temp[track_ID[j]][k, 7]
            else:
                Daughter_ID.append(data_temp[track_ID[j]][0, 3])
                Daughter_Energy.append(max(data_temp[track_ID[j]][:, 8]))
                
                # this is for checking angle bug
                #if(np.linalg.norm(np.array([data_temp[track_ID[j]][0, 12], data_temp[track_ID[j]][0, 13], data_temp[track_ID[j]][0, 14]]))==0):
                    #print("Event " + str(Event_list[i]) + ", Track " + str(track_ID[j]))
                
                Daughter_Angle.append(calculate_angle_location(data_temp[track_ID[0]][len(data_temp[track_ID[0]])-1, 16], data_temp[track_ID[0]][len(data_temp[track_ID[0]])-1, 17], data_temp[track_ID[0]][len(data_temp[track_ID[0]])-1, 18], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 16], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 17], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 18], 0, 0, -1, data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 12], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 13], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 14]))
                #Daughter_Angle.append(i)
                #Angle_Change.append(calculate_angle(data_3d[Event_list[i]][0, 12], data_3d[Event_list[i]][0, 13], data_3d[Event_list[i]][0, 14], data_temp[track_ID[j]][0, 12], data_temp[track_ID[j]][0, 13], data_temp[track_ID[j]][0, 14]))
                Angle_Change.append(i)
                Distance_Traveled.append(calculate_distance(data_temp[track_ID[0]][len(data_temp[track_ID[0]])-1, 16], data_temp[track_ID[0]][len(data_temp[track_ID[0]])-1, 17], data_temp[track_ID[0]][len(data_temp[track_ID[0]])-1, 18], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 16], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 17], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 18]))
        
        if(condition_stop or condition_in_flight and condition):
            E_TOF_OUT.append(Energy_out)
            E_TOF_IN.append(Energy_in)
            Init_Energy.append(data_3d[Event_list[i]][0, 8])
            Delta_T.append(Time_in - Time_out)
            Primary_Angle.append(i)
            #Primary_Angle.append(calculate_angle(data_3d[Event_list[i]][0, 12], data_3d[Event_list[i]][0, 13], data_3d[Event_list[i]][0, 14], 0, 0, -1))  
                #Daughter_ID.append(d_ID)
                #Daughter_Energy.append(d_energy)
                #Daughter_Angle.append(i)
                #Angle_Change.append(angle_change)
                #Distance_Traveled.append(distance_traveled)

        display_progress_bar(i+1, len(Event_list))
        
        Result_vector.set_value(i, 0, [Event_list[i], Init_Energy[i], Primary_Angle[i], E_TOF_OUT[i], E_TOF_IN[i], Delta_T[i]])
        Result_vector.set_value(i, 1, [track_ID, Daughter_ID, Daughter_Energy, Daughter_Angle, Angle_Change, Distance_Traveled])
        
    print("\n We got " + str(len(E_TOF_IN)) + " events selected")
    print("Last event is "+ str(Event_list[len(Event_list)-1]))
    
    
    # return two things, first one is the inital energy array and second one is the number of the event generated.
    return Result_vector





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
    ax.set_xlabel("energy [MeV/n]", fontsize='large')
    ax.set_ylabel("GRASP [m^2 sr]", fontsize='large')
    ax.legend(fontsize='large', loc='upper right')
    
    # This is for customized legend location
    #ax.legend(fontsize='large', loc='upper left', bbox_to_anchor=(0.3, -0.08))
    ax.grid(alpha=0.2)
    ax.set_title('GRASP (Geometric Acceptance)', fontsize='large')
    return ax






#This function is for identify particles in geant4 including particle ID and Z numbers etc
def number_of_nucleons(particle_ID):
    data = [[2212, 1000010020, 1000020030, -2212, -1000010020, -1000020030],
            [1, 2, 3, 1, 2, 3]]
    df = pd.DataFrame(data, columns=['proton', 'deuteron', 'helium3', 'antiproton', 'antideuteron', 'antihelium3'])
    matching_columns = df.columns[df.eq(particle_ID).any()]
    particle_info = df[matching_columns[0]]
    return particle_info
