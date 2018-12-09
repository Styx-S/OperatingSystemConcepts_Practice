import pandas as pd
import matplotlib.pyplot as plt

if __name__ == "__main__":
    indexes = list()
    workloadNum = 12
    workload = "workload"
    frames = [100, 500, 1000, 2000, 5000]
    frameNum = 5
    for i in range(workloadNum):
        indexes.append(workload + str(i+1))
    df = pd.read_csv("result.csv", names=indexes)
    print(df)
    fifo = df[0:5].reset_index(drop=True)
    lru = df[5:10].reset_index(drop=True)
    clock = df[10:15].reset_index(drop=True)
    # 算法固定，不同页框大小下的缺页率
    for i in range(workloadNum):
        name = workload + str(i + 1)
        plt.title(name)
        plt.xlabel('页框大小', fontproperties='SimHei')
        plt.ylabel('缺页率', fontproperties='SimHei')
        plt.plot(frames, list(fifo[name]), color='red', label='FIFO')
        plt.plot(frames, list(lru[name]), color='green', label="LRU")
        plt.plot(frames, list(clock[name]), color='blue', label="CLOCK")
        plt.legend()
        plt.show()

    # 页框固定，不同算法在不同workload下的的缺页率
    for i in range(frameNum):
        workloads = range(1, 1 + workloadNum)
        plt.title("页框大小：" + str(frames[i]), fontproperties='SimHei')
        plt.xlabel('workload序号', fontproperties='SimHei')
        plt.ylabel('缺页率', fontproperties='SimHei')
        plt.plot(workloads, fifo.ix[i], color='red', label='FIFO')
        plt.plot(workloads, lru.ix[i], color='green', label='LRU')
        plt.plot(workloads, clock.ix[i], color='blue', label='CLOCK')
        plt.legend()
        plt.show()




