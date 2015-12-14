from pylab import *
import time
import matplotlib


def streamData ():

    ion()
    clf()

    f = open('data2015-12-13_14-51-57.txt','rt')
    data = [[float(token) for token in line.split()] for line in f.readlines()[::]]

    timewindow = 30
    x = [0] * 50 # x-array
    z = [0] * 50


    ax1 = plt.axes()
    ax1.set_xlabel('Time (s)')
    ax2 = ax1.twinx()

    ax1.set_ylabel('Inst. Speed (m/s)', color='b')
    ax2.set_ylabel('Total Dist (m)', color='r')

    lineS,  = ax1.plot(x,z, 'b')
    lineD, = ax2.plot(x,z, 'r')
    i = 0
    t= []
    y= []
    d= []


    while i<len(data):

        # update the data
        lineS.set_xdata(t)
        lineS.set_ydata(y)  
        lineD.set_xdata(t)
        lineD.set_ydata(d)
        
        #update data array
        y.append(data[i][4])
        t.append(data[i][0])
        d.append(data[i][3])
        
        draw()                         # redraw the canvas
        
        #scale axes to fit
        ax1.set_ylim([min(y)-1, max(y)+1])
        ax2.set_ylim([min(d)-1, max(d)+1])
        
        # move time window to keep real-time
        if i > timewindow:
            plt.xlim([0, t[i-1]])
            
        i+= 1
            

def regraphData(spe,dist,t,fig,flag):
    if flag == 0:
        ion()
        fig = figure()

        data = [[float(token) for token in line.split()] for line in f.readlines()[::]]

        timewindow = 30
        x = [0] * 50 # x-array
        z = [0] * 50


        ax1 = plt.axes()
        ax1.set_xlabel('Time (s)')
        ax2 = ax1.twinx()

        ax1.set_ylabel('Inst. Speed (m/s)', color='b')
        ax2.set_ylabel('Total Dist (m)', color='r')

        lineS,  = ax1.plot(x,z, 'b')
        lineD, = ax2.plot(x,z, 'r')
        i = 0
        t= []
        spe= []
        dist= []
        flag = 1

    lineS.set_xdata(t)
    lineS.set_ydata(spe)  
    lineD.set_xdata(t)
    lineD.set_ydata(dist)
    
    #update data array
    spe.append(data[4])
    t.append(data[0])
    dist.append(data[3])
    
    draw()                         # redraw the canvas
    
    #scale axes to fit
    ax1.set_ylim([min(spe)-1, max(spe)+1])
    ax2.set_ylim([min(dist)-1, max(dist)+1])

    return spe, dist, t, fig, flag



