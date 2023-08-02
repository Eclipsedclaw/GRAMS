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



# This function doing muon rate analysis
def Analyze_muon(data_3d, particle_ID):
    
    # This is the event number in the raw data
    Event_list = list(data_3d.keys())
    
    # Construct a empty vector for initial energy store
    Init_Energy = []
    
    # Counting the number of Events that pass through sci1
    N_event_1 = 0
    
    # Counting the number of Events that pass through sci2
    N_event_2 = 0
    
    # Counting the number of Events that pass both
    N_both = 0
    
    # Counting how many events selected
    N_event = 0
    
    for i in range(len(Event_list)):
        condition1 = np.any(np.array(data_3d[Event_list[i]][data_3d[Event_list[i]][:, 6] == 'Plastic', 4]) == 1)
        condition2 = np.any(np.array(data_3d[Event_list[i]][data_3d[Event_list[i]][:, 6] == 'Plastic', 4]) == 0)
        
        if(condition1):
            N_event_1 = N_event_1 + 1
        if(condition2):
            N_event_2 = N_event_2 + 1
        if(condition1 and condition2):
            N_both = N_both + 1
        if(condition1 or condition2):
            N_event = N_event + 1
        display_progress_bar(i+1, len(Event_list))
    print("\n We got " + str(N_event) + " events selected")
    print("Last event is "+ str(Event_list[len(Event_list)-1]))
    
    # return two things, first one is the inital energy array and second one is the number of the event generated.
    return np.array(N_event_1), np.array(N_event_2), np.array(N_both)



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
        self.data = [[0] * columns for _ in range(number_of_events)]

    def get_value(self, row, column):
        return self.data[row][column]

    def set_value(self, row, column, value):
        self.data[row][column] = value

    def remove_unused_rows(self, used_rows):
        if used_rows < len(self.data):
            self.data = self.data[:used_rows]
    
    def get_number_of_events(self):
        return len(self.data)



# This function doing primary daughter particles analysis including energy and directions
def Analyze_daughter(data_3d, particle_ID, stop_event=True, in_flight_event=False):
    
    # This is the event number in the raw data
    Event_list = list(data_3d.keys())
    
    # Define result vector based on the property of the daughter particles
    Result_vector = daughters(len(Event_list), 2)
        
    # Construct an empty vector for initial primary particle energy store
    Init_Energy = []
    
    # Construct an empty vector for angle of the primary particles
    Primary_Angle = []
    
    # Construct an empty vector for distance traveled of primary particles
    Primary_Distance = []
    
    # Construct an empty vector for Energy deposit in the outer TOF
    E_TOF_OUT = []
    
    # Construct an empty vector for Energy deposit in the inner TOF
    E_TOF_IN = []
    
    # Construct an empty vector for time difference in the outer and inner TOF
    Delta_T = []
    
    # Creat an 0 variable that record how many evnets selected
    n = 0
    
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
        Event_out = 0
        Event_in = 0
        
        mother_ID_second_vertex_anti_deuteron = [0, 0, 0, 0, 0]
        mother_ID_second_vertex_anti_proton = [0, 0, 0, 0, 0]
        
        #only for antiHe3 analysis
        if(particle_ID == -1000010020):
            # There might be daughter antiparticle scattering, be carefule with this. Also need to update antideuteron daughters
            if np.any(data_3d[Event_list[i]][:, 3] == -1000010020):
                mother_ID_second_vertex_anti_deuteron = np.unique(np.array(data_3d[Event_list[i]][data_3d[Event_list[i]][:, 3] == -1000010020, 1]))
                mother_ID_second_vertex_anti_deuteron = np.pad(mother_ID_second_vertex_anti_deuteron, (0, 5 - len(mother_ID_second_vertex_anti_deuteron)), mode='constant')
                #mother_ID_third_vertex_anti_deuteron
        
            # Assume the scattering always happen at the end of the track data stream, so we only take the first 3 data points. Need to be fixed in the future
            if np.any(data_3d[Event_list[i]][:, 3] == -2212):
                mother_ID_second_vertex_anti_proton = np.unique(np.array(data_3d[Event_list[i]][data_3d[Event_list[i]][:, 3] == -2212, 1]))
                #print('\s'+str(mother_ID_second_vertex_anti_proton)+' and this is event '+ str(Event_list[i]))
                mother_ID_second_vertex_anti_proton = np.pad(mother_ID_second_vertex_anti_proton, (0, 5 - len(mother_ID_second_vertex_anti_proton)), mode='constant')

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
        condition_stop = np.min(data_temp[track_ID[0]][:, 8]) == 0 and np.min(data_temp[track_ID[0]][:, 9]) == 0 and stop_event
        
        # conditions for in flight annihilation events, here shows last point has energy larger than 0
        condition_in_flight = np.min(data_temp[track_ID[0]][:, 8]) != 0 and in_flight_event
        
        # these are conditions for GRASP, need to update
        # general condition that all the events has to match, here shows the last point is inside Liquid Argon
        #condition = data_3d[Event_list[i]][data_3d[Event_list[i]][:, 8] == np.min(data_3d[Event_list[i]][:, 8])][0, 6] == 'LAr' and Energy_in != 0 and Energy_out != 0
        # conditions for stop events, here shows last point has 0 energy left and 0 energy deposit
        #condition_stop = np.min(data_3d[Event_list[i]][:, 8]) == 0 and np.min(data_3d[Event_list[i]][:, 9]) == 0 and stop_event
        # conditions for in flight annihilation events, here shows last point has energy larger than 0
        #condition_in_flight = np.min(data_3d[Event_list[i]][:, 8]) != 0 and in_flight_event
        
        # Need to optimize this in the future
        for j in range(len(track_ID)):
            if(j == 0):
                for k in range(len(data_temp[track_ID[0]])):
                    
                    # Outer TOF info
                    if(str(data_temp[track_ID[0]][k, 5]) == '-10000' or str(data_temp[track_ID[0]][k, 5]) == '-10001' or str(data_temp[track_ID[0]][k, 5]) == '-10002' or str(data_temp[track_ID[0]][k, 5]) == '-10003' or str(data_temp[track_ID[0]][k, 5]) == '-10004' or str(data_temp[track_ID[0]][k, 5]) == '-10005' and str(data_temp[track_ID[0]][k, 3]) == str(particle_ID)):
                        #print('\nEvent'+str(Event_list[i])+' energy out is '+str(data_temp[track_ID[0]][k, 9])+'\n')
                        Energy_out = Energy_out + float(data_temp[track_ID[0]][k, 9])
                        if(Time_out == 0):
                            Time_out = data_temp[track_ID[0]][k, 7]
                            
                    # Inner TOF info
                    if(str(data_temp[track_ID[0]][k, 5]) == '-11000' or str(data_temp[track_ID[0]][k, 5]) == '-11001' or str(data_temp[track_ID[0]][k, 5]) == '-11002' or str(data_temp[track_ID[0]][k, 5]) == '-11003' or str(data_temp[track_ID[0]][k, 5]) == '-11004' or str(data_temp[track_ID[0]][k, 5]) == '-11005', str(data_temp[track_ID[0]][k, 3]) == str(particle_ID)):
                        #print('\nEvent'+str(Event_list[i])+' energy in is '+str(data_temp[track_ID[0]][k, 9])+'\n')
                        Energy_in = Energy_in + float(data_temp[track_ID[0]][k, 9])
                        Event_in = k
                        if(Time_in == 0):
                            Time_in = data_temp[track_ID[0]][k, 7]
            
            # Only for the primary vertex                
            elif(str(data_temp[track_ID[j]][0, 2]) == '1'):
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

                
                
            # check the antideuteron vertex #2
            elif(mother_ID_second_vertex_anti_deuteron[0] != 0 and str(data_temp[track_ID[j]][0, 2]) == str(mother_ID_second_vertex_anti_deuteron[0])):
                Daughter_ID.append(data_temp[track_ID[j]][0, 3])
                Daughter_Energy.append(max(data_temp[track_ID[j]][:, 8]))
                
                # this is for checking angle bug
                #if(np.linalg.norm(np.array([data_temp[track_ID[j]][0, 12], data_temp[track_ID[j]][0, 13], data_temp[track_ID[j]][0, 14]]))==0):
                    #print("Event " + str(Event_list[i]) + ", Track " + str(track_ID[j]))
                Daughter_Angle.append(calculate_angle_location(data_temp[mother_ID_second_vertex_anti_deuteron[0]][len(data_temp[mother_ID_second_vertex_anti_deuteron[0]])-1, 16], data_temp[mother_ID_second_vertex_anti_deuteron[0]][len(data_temp[mother_ID_second_vertex_anti_deuteron[0]])-1, 17], data_temp[mother_ID_second_vertex_anti_deuteron[0]][len(data_temp[mother_ID_second_vertex_anti_deuteron[0]])-1, 18], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 16], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 17], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 18], 0, 0, -1, data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 12], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 13], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 14]))
                #Daughter_Angle.append(i)
                #Angle_Change.append(calculate_angle(data_3d[Event_list[i]][0, 12], data_3d[Event_list[i]][0, 13], data_3d[Event_list[i]][0, 14], data_temp[track_ID[j]][0, 12], data_temp[track_ID[j]][0, 13], data_temp[track_ID[j]][0, 14]))
                Angle_Change.append(i)
                Distance_Traveled.append(calculate_distance(data_temp[mother_ID_second_vertex_anti_deuteron[0]][len(data_temp[mother_ID_second_vertex_anti_deuteron[0]])-1, 16], data_temp[mother_ID_second_vertex_anti_deuteron[0]][len(data_temp[mother_ID_second_vertex_anti_deuteron[0]])-1, 17], data_temp[mother_ID_second_vertex_anti_deuteron[0]][len(data_temp[mother_ID_second_vertex_anti_deuteron[0]])-1, 18], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 16], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 17], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 18]))
                

                
                
                
            # check the antiproton vertex #2 first
            elif(mother_ID_second_vertex_anti_proton[0] != 0 and str(data_temp[track_ID[j]][0, 2]) == str(mother_ID_second_vertex_anti_proton[0])):
                #print("found one vertex #2 has antiproton! And this event is # "+ str(i))
                Daughter_ID.append(data_temp[track_ID[j]][0, 3])
                Daughter_Energy.append(max(data_temp[track_ID[j]][:, 8]))
                
                # this is for checking angle bug
                #if(np.linalg.norm(np.array([data_temp[track_ID[j]][0, 12], data_temp[track_ID[j]][0, 13], data_temp[track_ID[j]][0, 14]]))==0):
                    #print("Event " + str(Event_list[i]) + ", Track " + str(track_ID[j]))
                
                Daughter_Angle.append(calculate_angle_location(data_temp[mother_ID_second_vertex_anti_proton[0]][len(data_temp[mother_ID_second_vertex_anti_proton[0]])-1, 16], data_temp[mother_ID_second_vertex_anti_proton[0]][len(data_temp[mother_ID_second_vertex_anti_proton[0]])-1, 17], data_temp[mother_ID_second_vertex_anti_proton[0]][len(data_temp[mother_ID_second_vertex_anti_proton[0]])-1, 18], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 16], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 17], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 18], 0, 0, -1, data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 12], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 13], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 14]))
                #Daughter_Angle.append(i)
                #Angle_Change.append(calculate_angle(data_3d[Event_list[i]][0, 12], data_3d[Event_list[i]][0, 13], data_3d[Event_list[i]][0, 14], data_temp[track_ID[j]][0, 12], data_temp[track_ID[j]][0, 13], data_temp[track_ID[j]][0, 14]))
                Angle_Change.append(i)
                Distance_Traveled.append(calculate_distance(data_temp[mother_ID_second_vertex_anti_proton[0]][len(data_temp[mother_ID_second_vertex_anti_proton[0]])-1, 16], data_temp[mother_ID_second_vertex_anti_proton[0]][len(data_temp[mother_ID_second_vertex_anti_proton[0]])-1, 17], data_temp[mother_ID_second_vertex_anti_proton[0]][len(data_temp[mother_ID_second_vertex_anti_proton[0]])-1, 18], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 16], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 17], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 18]))
                

                
                
                
            # check the proton vertex #2 second
            elif(mother_ID_second_vertex_anti_proton[1] != 0 and str(data_temp[track_ID[j]][0, 2]) == str(mother_ID_second_vertex_anti_proton[1])):
                Daughter_ID.append(data_temp[track_ID[j]][0, 3])
                Daughter_Energy.append(max(data_temp[track_ID[j]][:, 8]))
                
                # this is for checking angle bug
                #if(np.linalg.norm(np.array([data_temp[track_ID[j]][0, 12], data_temp[track_ID[j]][0, 13], data_temp[track_ID[j]][0, 14]]))==0):
                    #print("Event " + str(Event_list[i]) + ", Track " + str(track_ID[j]))
                
                Daughter_Angle.append(calculate_angle_location(data_temp[mother_ID_second_vertex_anti_proton[1]][len(data_temp[mother_ID_second_vertex_anti_proton[1]])-1, 16], data_temp[mother_ID_second_vertex_anti_proton[1]][len(data_temp[mother_ID_second_vertex_anti_proton[1]])-1, 17], data_temp[mother_ID_second_vertex_anti_proton[1]][len(data_temp[mother_ID_second_vertex_anti_proton[1]])-1, 18], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 16], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 17], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 18], 0, 0, -1, data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 12], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 13], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 14]))
                #Daughter_Angle.append(i)
                #Angle_Change.append(calculate_angle(data_3d[Event_list[i]][0, 12], data_3d[Event_list[i]][0, 13], data_3d[Event_list[i]][0, 14], data_temp[track_ID[j]][0, 12], data_temp[track_ID[j]][0, 13], data_temp[track_ID[j]][0, 14]))
                Angle_Change.append(i)
                Distance_Traveled.append(calculate_distance(data_temp[mother_ID_second_vertex_anti_proton[1]][len(data_temp[mother_ID_second_vertex_anti_proton[1]])-1, 16], data_temp[mother_ID_second_vertex_anti_proton[1]][len(data_temp[mother_ID_second_vertex_anti_proton[1]])-1, 17], data_temp[mother_ID_second_vertex_anti_proton[1]][len(data_temp[mother_ID_second_vertex_anti_proton[1]])-1, 18], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 16], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 17], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 18]))
                

                
                
                
            # check the antideuteron vertex #2 third
            elif(mother_ID_second_vertex_anti_proton[2] != 0 and str(data_temp[track_ID[j]][0, 2]) == str(mother_ID_second_vertex_anti_proton[2])):
                Daughter_ID.append(data_temp[track_ID[j]][0, 3])
                Daughter_Energy.append(max(data_temp[track_ID[j]][:, 8]))
                
                # this is for checking angle bug
                #if(np.linalg.norm(np.array([data_temp[track_ID[j]][0, 12], data_temp[track_ID[j]][0, 13], data_temp[track_ID[j]][0, 14]]))==0):
                    #print("Event " + str(Event_list[i]) + ", Track " + str(track_ID[j]))
                
                Daughter_Angle.append(calculate_angle_location(data_temp[mother_ID_second_vertex_anti_proton[2]][len(data_temp[mother_ID_second_vertex_anti_proton[2]])-1, 16], data_temp[mother_ID_second_vertex_anti_proton[2]][len(data_temp[mother_ID_second_vertex_anti_proton[2]])-1, 17], data_temp[mother_ID_second_vertex_anti_proton[2]][len(data_temp[mother_ID_second_vertex_anti_proton[2]])-1, 18], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 16], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 17], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 18], 0, 0, -1, data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 12], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 13], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 14]))
                #Daughter_Angle.append(i)
                #Angle_Change.append(calculate_angle(data_3d[Event_list[i]][0, 12], data_3d[Event_list[i]][0, 13], data_3d[Event_list[i]][0, 14], data_temp[track_ID[j]][0, 12], data_temp[track_ID[j]][0, 13], data_temp[track_ID[j]][0, 14]))
                Angle_Change.append(i)
                Distance_Traveled.append(calculate_distance(data_temp[mother_ID_second_vertex_anti_proton[2]][len(data_temp[mother_ID_second_vertex_anti_proton[2]])-1, 16], data_temp[mother_ID_second_vertex_anti_proton[2]][len(data_temp[mother_ID_second_vertex_anti_proton[2]])-1, 17], data_temp[mother_ID_second_vertex_anti_proton[2]][len(data_temp[mother_ID_second_vertex_anti_proton[2]])-1, 18], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 16], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 17], data_temp[track_ID[j]][len(data_temp[track_ID[j]][:, 1])-1, 18]))
                
        
        if(condition_stop or condition_in_flight and condition):
            E_TOF_OUT.append(Energy_out)
            E_TOF_IN.append(Energy_in)
            Init_Energy.append(data_3d[Event_list[i]][0, 8])
            Delta_T.append(Time_in - Time_out)
            Primary_Angle.append(calculate_angle_location(data_temp[track_ID[0]][len(data_temp[track_ID[0]])-1, 16], data_temp[track_ID[0]][len(data_temp[track_ID[0]])-1, 17], data_temp[track_ID[0]][len(data_temp[track_ID[0]])-1, 18], data_temp[track_ID[0]][len(data_temp[track_ID[0]])-2, 16], data_temp[track_ID[0]][len(data_temp[track_ID[0]])-2, 17], data_temp[track_ID[0]][len(data_temp[track_ID[0]])-2, 18], 0, 0, -1, data_temp[track_ID[0]][len(data_temp[track_ID[0]])-1, 12], data_temp[track_ID[0]][len(data_temp[track_ID[0]])-1, 13], data_temp[track_ID[0]][len(data_temp[track_ID[0]])-1, 14]))
            Primary_Distance.append(calculate_distance(data_temp[track_ID[0]][len(data_temp[track_ID[0]])-1, 16], data_temp[track_ID[0]][len(data_temp[track_ID[0]])-1, 17], data_temp[track_ID[0]][len(data_temp[track_ID[0]])-1, 18], data_temp[track_ID[0]][Event_in, 16], data_temp[track_ID[0]][Event_in, 17], data_temp[track_ID[0]][Event_in, 18]))
            #Primary_Angle.append(calculate_angle(data_3d[Event_list[i]][0, 12], data_3d[Event_list[i]][0, 13], data_3d[Event_list[i]][0, 14], 0, 0, -1))  
            #Daughter_ID.append(d_ID)
            #Daughter_Energy.append(d_energy)
            #Daughter_Angle.append(i)
            #Angle_Change.append(angle_change)
            #Distance_Traveled.append(distance_traveled)
            Result_vector.set_value(n, 0, [Event_list[n], Init_Energy[n], Primary_Angle[n], Primary_Distance[n], E_TOF_OUT[n], E_TOF_IN[n], Delta_T[n]])
            Result_vector.set_value(n, 1, [track_ID, Daughter_ID, Daughter_Energy, Daughter_Angle, Angle_Change, Distance_Traveled])
            n = n + 1

        display_progress_bar(i+1, len(Event_list))
                
    print("\n We got " + str(len(E_TOF_IN)) + " events selected")
    print("Last event is "+ str(Event_list[len(Event_list)-1]))
    
    Result_vector.remove_unused_rows(len(Init_Energy))
    
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






# This function is for identify particles in geant4 including particle ID and Z numbers etc
def number_of_nucleons(particle_ID):
    data = [[2212, 1000010020, 1000020030, -2212, -1000010020, -1000020030, -2112, -211, 11, 22, 111, 211, 2112, 2212, 1000010030, 1000020040, 1000020060, 1000030060, 1000030070, 1000030080, 1000030090, 1000040070, 1000040090, 1000040100, 1000040110, 1000050100, 1000050101, 1000050110, 1000050120, 1000050130, 1000060110, 1000060120, 1000060130, 1000060140, 1000060150, 1000060151, 1000060160, 1000070130, 1000070140, 1000070150, 1000070160, 1000070161, 1000070170, 1000070180, 1000080150, 1000080160, 1000080170, 1000080180, 1000080191, 1000080200, 1000080210, 1000080220, 1000090180, 1000090181, 1000090190, 1000090191, 1000090200, 1000090210, 1000090211, 1000090220, 1000100191, 1000100200, 1000100210, 1000100220, 1000100230, 1000100240, 1000100250, 1000110210, 1000110220, 1000110221, 1000110230, 1000110240, 1000110241, 1000110250, 1000110251, 1000110260, 1000110261, 1000110270, 1000120230, 1000120232, 1000120240, 1000120250, 1000120251, 1000120260, 1000120270, 1000120280, 1000120290, 1000120291, 1000120300, 1000130250, 1000130251, 1000130260, 1000130262, 1000130270, 1000130280, 1000130281, 1000130290, 1000130291, 1000130300, 1000130301, 1000130310, 1000130330, 1000140270, 1000140280, 1000140290, 1000140300, 1000140310, 1000140320, 1000140330, 1000140331, 1000140340, 1000140350, 1000140351, 1000140360, 1000150290, 1000150300, 1000150310, 1000150320, 1000150330, 1000150340, 1000150341, 1000150350, 1000150360, 1000150370, 1000160310, 1000160320, 1000160330, 1000160340, 1000160350, 1000160351, 1000160360, 1000160361, 1000160370, 1000160380, 1000170330, 1000170340, 1000170341, 1000170350, 1000170360, 1000170361, 1000170370, 1000170380, 1000170381, 1000170390, 1000180350, 1000180360, 1000180370, 1000180371, 1000180380, 1000180390, 1000180391, 1000180400, 1000190380, 1000190390, 1000260540, 1000260560, 1000260570],
            [1, 2, 3, 1, 2, 3, 1, np.nan, np.nan, np.nan, np.nan, np.nan, 1, 1, 3, 4, 6, 6, 7, 8, 9, 7, 9, 10, 11, 10, np.nan, 11, 12, 13, 11, 12, 13, 14, 15, np.nan, 16, 13, 14, 15, 16, np.nan, 17, 18, 15, 16, 17, 18, np.nan, 20, 21, 22, 18, np.nan, 19, np.nan, 20, 21, np.nan, 22, np.nan, 20, 21, 22, 23, 24, 25, 21, 22, np.nan, 23, 24, np.nan, 25, np.nan, 26, np.nan, 27, 23, np.nan, 24, 25, np.nan, 26, 27, 28, 29, np.nan, 30,25, np.nan, 26, np.nan, 27, 28, np.nan, 29, np.nan, 30, np.nan, 31, 33, 27, 28, 29, 30, 31, 32, 33, np.nan, 34, 35, np.nan, 36, 29, 30,31, 32, 33, 34, np.nan, 35, 36, 37, 31, 32, 33, 34, 35, np.nan, 36, np.nan, 37, 38, 33, 34, np.nan, 35, 36, np.nan, 37, 38,np.nan, 39, 35, 36, 37, np.nan, 38, 39, np.nan, 40, 38, 39, 54, 56, 57]]
    df = pd.DataFrame(data, columns=['proton', 'deuteron', 'helium3', 'antiproton', 'antideuteron', 'antihelium3', 'antineutron', 'pi-', 'electron', 'photon(gamma)', 'pi0', 'pi+', 'neutron', 'proton', 'triton', 'alpha', 'He6', 'Li6', 'Li7', 'Li8', 'Li9', 'Be7', 'Be9', 'Be10', 'Be11', 'B10', '1000050101', 'B11', 'B12', 'B13', 'C11', 'C12', 'C13', 'C14', 'C15', '1000060151', 'C16', 'N13', 'N14', 'N15', 'N16', 'N16[120.420]', 'N17', 'N18', 'O15', 'O16', 'O17', 'O18', '1000080191', 'O20', 'O21', 'O22', 'F18', '1000090181', 'F19', '1000090191', 'F20', 'F21', '1000090211', 'F22', '1000100191', 'Ne20', 'Ne21', 'Ne22', 'Ne23', 'Ne24', 'Ne25', 'Na21', 'Na22', '1000110221', 'Na23', 'Na24', 'Na24[472.207]', 'Na25', '1000110251', 'Na26', 'Na26[82.500]', 'Na27', 'Mg23', '1000120232', 'Mg24', 'Mg25', '1000120251', 'Mg26', 'Mg27', 'Mg28', 'Mg29', '1000120291', 'Mg30', 'Al25', '1000130251', 'Al26', '1000130262', 'Al27', 'Al28', '1000130281', 'Al29', '1000130291', 'Al30', '1000130301', 'Al31', 'Al33', 'Si27', 'Si28', 'Si29', 'Si30', 'Si31', 'Si32', 'Si33', '1000140331', 'Si34', 'Si35', '1000140351', 'Si36', 'P29', 'P30', 'P31', 'P32', 'P33', 'P34', '1000150341', 'P35', 'P36', 'P37', 'S31', 'S32', 'S33', 'S34', 'S35', '1000160351', 'S36', '1000160361', 'S37', 'S38', 'Cl33', 'Cl34', 'Cl34[146.360]', 'Cl35', 'Cl36', '1000170361', 'Cl37', 'Cl38', 'Cl38[671.365]', 'Cl39', 'Ar35', 'Ar36', 'Ar37', '1000180371', 'Ar38', 'Ar39', '1000180391', 'Ar40', 'K38', 'K39', 'Fe54', 'Fe56', 'Fe57'])
    matching_columns = df.columns[df.eq(particle_ID).any()]
    particle_info = df[matching_columns[0]]
    return particle_info

# This function is for finding particle name based on particle ID
def name_of_particle(particle_ID):
    data = [[2212, 1000010020, 1000020030, -2212, -1000010020, -1000020030, -2112, -211, 11, 22, 111, 211, 2112, 2212, 1000010030, 1000020040, 1000020060, 1000030060, 1000030070, 1000030080, 1000030090, 1000040070, 1000040090, 1000040100, 1000040110, 1000050100, 1000050101, 1000050110, 1000050120, 1000050130, 1000060110, 1000060120, 1000060130, 1000060140, 1000060150, 1000060151, 1000060160, 1000070130, 1000070140, 1000070150, 1000070160, 1000070161, 1000070170, 1000070180, 1000080150, 1000080160, 1000080170, 1000080180, 1000080191, 1000080200, 1000080210, 1000080220, 1000090180, 1000090181, 1000090190, 1000090191, 1000090200, 1000090210, 1000090211, 1000090220, 1000100191, 1000100200, 1000100210, 1000100220, 1000100230, 1000100240, 1000100250, 1000110210, 1000110220, 1000110221, 1000110230, 1000110240, 1000110241, 1000110250, 1000110251, 1000110260, 1000110261, 1000110270, 1000120230, 1000120232, 1000120240, 1000120250, 1000120251, 1000120260, 1000120270, 1000120280, 1000120290, 1000120291, 1000120300, 1000130250, 1000130251, 1000130260, 1000130262, 1000130270, 1000130280, 1000130281, 1000130290, 1000130291, 1000130300, 1000130301, 1000130310, 1000130330, 1000140270, 1000140280, 1000140290, 1000140300, 1000140310, 1000140320, 1000140330, 1000140331, 1000140340, 1000140350, 1000140351, 1000140360, 1000150290, 1000150300, 1000150310, 1000150320, 1000150330, 1000150340, 1000150341, 1000150350, 1000150360, 1000150370, 1000160310, 1000160320, 1000160330, 1000160340, 1000160350, 1000160351, 1000160360, 1000160361, 1000160370, 1000160380, 1000170330, 1000170340, 1000170341, 1000170350, 1000170360, 1000170361, 1000170370, 1000170380, 1000170381, 1000170390, 1000180350, 1000180360, 1000180370, 1000180371, 1000180380, 1000180390, 1000180391, 1000180400, 1000190380, 1000190390, 1000260540, 1000260560, 1000260570],
            [1, 2, 3, 1, 2, 3, 1, np.nan, np.nan, np.nan, np.nan, np.nan, 1, 1, 3, 4, 6, 6, 7, 8, 9, 7, 9, 10, 11, 10, np.nan, 11, 12, 13, 11, 12, 13, 14, 15, np.nan, 16, 13, 14, 15, 16, np.nan, 17, 18, 15, 16, 17, 18, np.nan, 20, 21, 22, 18, np.nan, 19, np.nan, 20, 21, np.nan, 22, np.nan, 20, 21, 22, 23, 24, 25, 21, 22, np.nan, 23, 24, np.nan, 25, np.nan, 26, np.nan, 27, 23, np.nan, 24, 25, np.nan, 26, 27, 28, 29, np.nan, 30,25, np.nan, 26, np.nan, 27, 28, np.nan, 29, np.nan, 30, np.nan, 31, 33, 27, 28, 29, 30, 31, 32, 33, np.nan, 34, 35, np.nan, 36, 29, 30,31, 32, 33, 34, np.nan, 35, 36, 37, 31, 32, 33, 34, 35, np.nan, 36, np.nan, 37, 38, 33, 34, np.nan, 35, 36, np.nan, 37, 38,np.nan, 39, 35, 36, 37, np.nan, 38, 39, np.nan, 40, 38, 39, 54, 56, 57]]
    df = pd.DataFrame(data, columns=['proton', 'deuteron', 'helium3', 'antiproton', 'antideuteron', 'antihelium3', 'antineutron', 'pi-', 'electron', 'photon(gamma)', 'pi0', 'pi+', 'neutron', 'proton', 'triton', 'alpha', 'He6', 'Li6', 'Li7', 'Li8', 'Li9', 'Be7', 'Be9', 'Be10', 'Be11', 'B10', '1000050101', 'B11', 'B12', 'B13', 'C11', 'C12', 'C13', 'C14', 'C15', '1000060151', 'C16', 'N13', 'N14', 'N15', 'N16', 'N16[120.420]', 'N17', 'N18', 'O15', 'O16', 'O17', 'O18', '1000080191', 'O20', 'O21', 'O22', 'F18', '1000090181', 'F19', '1000090191', 'F20', 'F21', '1000090211', 'F22', '1000100191', 'Ne20', 'Ne21', 'Ne22', 'Ne23', 'Ne24', 'Ne25', 'Na21', 'Na22', '1000110221', 'Na23', 'Na24', 'Na24[472.207]', 'Na25', '1000110251', 'Na26', 'Na26[82.500]', 'Na27', 'Mg23', '1000120232', 'Mg24', 'Mg25', '1000120251', 'Mg26', 'Mg27', 'Mg28', 'Mg29', '1000120291', 'Mg30', 'Al25', '1000130251', 'Al26', '1000130262', 'Al27', 'Al28', '1000130281', 'Al29', '1000130291', 'Al30', '1000130301', 'Al31', 'Al33', 'Si27', 'Si28', 'Si29', 'Si30', 'Si31', 'Si32', 'Si33', '1000140331', 'Si34', 'Si35', '1000140351', 'Si36', 'P29', 'P30', 'P31', 'P32', 'P33', 'P34', '1000150341', 'P35', 'P36', 'P37', 'S31', 'S32', 'S33', 'S34', 'S35', '1000160351', 'S36', '1000160361', 'S37', 'S38', 'Cl33', 'Cl34', 'Cl34[146.360]', 'Cl35', 'Cl36', '1000170361', 'Cl37', 'Cl38', 'Cl38[671.365]', 'Cl39', 'Ar35', 'Ar36', 'Ar37', '1000180371', 'Ar38', 'Ar39', '1000180391', 'Ar40', 'K38', 'K39', 'Fe54', 'Fe56', 'Fe57'])
    
    # Find the index of the particle_ID in data[0]
    index = np.where(np.array(data[0]) == particle_ID)[0]
    
    if len(index) > 0:
        # Get the corresponding column name from the DataFrame
        column_name = df.columns[index[0]]
        return column_name
    else:
        return None