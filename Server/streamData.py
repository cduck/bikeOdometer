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

flag = 0
lines = [None]*4
axs = []
allGraphs = False

def regraphData(data):
    if len(data) <= 0:
        return
    global flag, lines, axs

    timewindow = 30
    pps = 4
    alt = [0] * timewindow*pps # x-array
    inc = [0] * timewindow*pps
    dis = [0] * timewindow*pps
    spe = [0] * timewindow*pps
    t = [0] * timewindow*pps
    minI = len(data) - len(t)
    minJ = 0
    if minI < 0:
      minJ = -minI
    for i in xrange(minJ, len(t)):
      t[i] = data[minI + i][0]
      alt[i] = data[minI + i][1]
      inc[i] = data[minI + i][2]
      dis[i] = data[minI + i][3]
      spe[i] = data[minI + i][4]

    if flag == 0:
        flag = 1
        ion()
        fig = figure()

        axs = [plt.axes()]
        axs[0].set_xlabel('Time (s)')
        for i in xrange(3 if allGraphs else 1):
          axs.append(axs[0].twinx())

        axs[0].set_ylabel('Inst. Speed (m/s)', color='b')
        axs[1].set_ylabel('Total Dist (m)', color='r')
        if allGraphs:
          axs[2].set_ylabel('Altitude (m)', color='k')
          axs[3].set_ylabel('Incline (deg)', color='g')

        lines[0], = axs[0].plot(spe, t, 'b')
        lines[1], = axs[1].plot(dis, t, 'r')
        if allGraphs:
          lines[2], = axs[2].plot(alt, t, 'k')
          lines[3], = axs[3].plot(inc, t, 'g')

    lines[0].set_xdata(t)
    lines[0].set_ydata(spe)  
    lines[1].set_xdata(t)
    lines[1].set_ydata(dis)  
    if allGraphs:
      lines[2].set_xdata(t)
      lines[2].set_ydata(alt)  
      lines[3].set_xdata(t)
      lines[3].set_ydata(inc)  
    
    #scale axes to fit
    axs[0].set_ylim([min(spe)-1, max(spe)+1])
    axs[1].set_ylim([min(dis)-1, max(dis)+1])
    if allGraphs:
      axs[2].set_ylim([min(alt)-1, max(alt)+1])
      axs[3].set_ylim([min(inc)-1, max(inc)+1])
    xlim = [max(t)-timewindow, max(t)]
    axs[0].set_xlim(xlim)
    axs[1].set_xlim(xlim)
    if allGraphs:
      axs[2].set_xlim(xlim)
      axs[3].set_xlim(xlim)

    print dis
    
    show()
    pause(0.0001)                         # redraw the canvas

