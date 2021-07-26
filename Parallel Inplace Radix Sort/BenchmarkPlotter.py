import csv
import matplotlib.pyplot as plt
import os
import numpy
from scipy.ndimage.filters import gaussian_filter1d
from path import Path


def PlotBenchmarkResult(results):
    plt.xlabel('Number of Elements')
    plt.ylabel('Seconds')
    plt.title('Sorting Algorithms Benchmark')
    plt.ticklabel_format(useOffset=False, style='plain')

    maxX = 0
    maxY = 0
    for result in results:
        csvFile = result[0]
        try:
            reader = csv.reader(open(csvFile, "r"))
            x_axis = []
            y_axis = []
            for row in reader:
                thisX = int(row[0])
                thisY = float(row[1])
                x_axis.append(thisX)
                y_axis.append(thisY)
            #ysmoothed = gaussian_filter1d(y_axis, sigma=1)
            plt.plot(x_axis, y_axis, label=os.path.splitext(
                csvFile)[0], color=result[1])
            maxX = max(maxX, max(x_axis))
            maxY = max(maxY, 50)
        except IOError as e:
            print(csvFile + " not included in this benchmark as it does not exist...")

    # plt.xlim(0, maxX)
    # plt.ylim(0, maxY)
    plt.legend()
    plt.show()


results = [["PIRSort.csv", 'b'], ["RadixSort.csv", 'g'], ["SelectionSort.csv", 'r'], ["BubbleSort.csv", 'c'], [
    "InsertionSort.csv", 'indigo'], ["MergeSort.csv", 'm'], ["QuickSort.csv", 'y'], ["IntroSort.csv", 'k'], ["ParallelQuicksort.csv", 'saddlebrown']]
#results = [["PIRSort.csv", 'b'], ["QuickSort.csv", 'y'], ["IntroSort.csv", 'k'], ["RadixSort.csv", 'g'],  ["ParallelQuicksort.csv", 'w']]
PlotBenchmarkResult(results)
