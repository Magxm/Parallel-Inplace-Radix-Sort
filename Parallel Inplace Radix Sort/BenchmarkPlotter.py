import csv
import matplotlib.pyplot as plt
import os
import numpy
from scipy.ndimage.filters import gaussian_filter1d
from path import Path


def GenerateColorFromString(string):
    r = 0
    g = 0
    b = 0

    for character in string:
        characterValue = ord(character)
        r += characterValue
        g += characterValue * 9
        b += characterValue * 853

    r = float(r % 255) / 255.0
    g = float(g % 255) / 255.0
    b = float(b % 255) / 255.0

    return (r, g, b, 1.0)


def PlotBenchmarkResult(results):
    plt.xlabel('Number of Elements')
    plt.ylabel('Seconds')
    plt.title('Sorting Algorithms Benchmark')
    plt.ticklabel_format(useOffset=False, style='plain')

    maxX = 0
    maxY = 0
    for csvFile in results:
        try:
            reader = csv.reader(open(csvFile, "r"))
            x_axis = []
            y_axis = []
            for row in reader:
                thisX = int(row[0])
                thisY = float(row[1])
                x_axis.append(thisX)
                y_axis.append(thisY)
            ysmoothed = gaussian_filter1d(y_axis, sigma=4)
            plt.plot(x_axis, ysmoothed, label=Path(csvFile).stem) #, color=GenerateColorFromString(csvFile))
            maxX = max(maxX, max(x_axis))
            maxY = max(maxY, 50)
        except IOError as e:
            print(csvFile + " not included in this benchmark as it does not exist...")

    # plt.xlim(0, maxX)
    # plt.ylim(0, maxY)
    plt.legend()
    plt.show()

def GetListOfCSVFiles(path):
        return sorted([path + "\\" + f for f in os.listdir(path) if f.endswith(".csv")])

files = GetListOfCSVFiles(os.getcwd())
PlotBenchmarkResult(files)

#results = [["PIRSort.csv", 'b'], ["RadixSort.csv", 'g'], ["SelectionSort.csv", 'r'], ["BubbleSort.csv", 'c'], ["InsertionSort.csv", 'indigo'], ["MergeSort.csv", 'm'], ["QuickSort.csv", 'y'], ["IntroSort.csv", 'k'], ["ParallelQuicksort.csv", 'saddlebrown']]
#results = [["PIRSort.csv", 'b'], ["QuickSort.csv", 'y'], ["IntroSort.csv", 'k'], ["RadixSort.csv", 'g'],  ["ParallelQuicksort.csv", 'w']]
#PlotBenchmarkResult(results)
