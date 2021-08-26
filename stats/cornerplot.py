import corner
import numpy as np
import matplotlib.pyplot as plt
import statistics
from numpy import array
import pandas as pd
sample = []

data1 = pd.read_table("/Users/jiancheng/GRAMS/Toy Model/Machine Learning/210822_anti_deuteron_80MeV_vector.txt")
data2 = pd.read_table("/Users/jiancheng/GRAMS/Toy Model/Machine Learning/210822_anti_proton_40MeV_vector.txt")
# Set up the parameters of the problem.
#ndim, nsamples = 6, len(data.iloc[:,0])

# Plot it.
figure1 = corner.corner(data1, labels=[r"$PeD$", r"$EnD$", r"$pion+$", r"$pion nrutral$", r"pion-", r"proton"], color='b',
                       show_titles=False, title_kwargs={"fontsize": 12})
#corner.corner(data2, labels=[r"$PeD$", r"$EnD$", r"$pion+$", r"$pion nrutral$", r"pion-", r"proton"], color='r',
#                       show_titles=True, title_kwargs={"fontsize": 12})
figure2 = corner.corner(data2, color='r', labels=[r"$PeD$", r"$EnD$", r"$pion+$", r"$pion nrutral$", r"pion-", r"proton"],
                       show_titles=True, title_kwargs={"fontsize": 12}, fig=figure1)
plt.show()
