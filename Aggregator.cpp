#include "bid.h"
#include <string.h>

class Aggregator {
    private:
        BID *biddingData = NULL; // 收集所有设备投标数据
        long *keylist = NULL; // 存储价格降序列指针
        double targetPower; // 控制目标
        double lastPower; // 上一周期实测变压器功率
        double totalOn = 0; // 上一周期运行状态为on的负荷功率
        double totalOff = 0; // 上一周期运行状态为off的负荷功率
        int totalBidders = 0; // 灵活资源数目
        int len ;
        void sort(BID *list, long *key, const int len) {
        // 二分排序
        if (len>1)
        {
            int split = len / 2;
            long *a = key, *b = key + split;
            if (split > 1) sort(list, a, split);
            if (len - split > 1) sort(list, b, len - split);
            long *res = new long[len];
            long *p = res;
            do {
                if (list[*a].price >= list[*b].price)
                    *p++ = *a++;
                else
                    *p++ = *b++;
            } while (a < key + split && b < key + len);
            while (a < key + split)
                *p++ = *a++;
            while (b < key+len)
                *p++ = *b++;
            memcpy(key, res, sizeof(long)*len);
            delete[] res;
        }
    }

    public:
        void aggregate() {
            sort(biddingData, keylist, totalBidders);
        }

        double clear() {
            int i = 0;
            double Pac = 0;
            double fixedP = calculateFixedP();
            double clearingPrice = 1;
            if (fixedP + Pac >= targetPower) return clearingPrice;
            while (i < totalBidders)
            {   
                int index = keylist[i];
                if (biddingData[index].price < 0) {
                    clearingPrice = 0;
                    break;
                }
                Pac += biddingData[index].power;
                if (fixedP + Pac < targetPower) {
                    i++;
                } else {
                    clearingPrice =biddingData[index].price;
                    break;
                }
            }
            return clearingPrice;
        }

        double calculateFixedP() {
            return lastPower - totalOn;
        }

        double calculateFlexP() {
            return totalOn + totalOff;
        }

        void collectBiddingData(BID *bid) {
            if (len == 0) { // create the bid list
                len = 8;
                biddingData = new BID[len];
                keylist = new long[len];
            }
            else if (totalBidders==len) // grow the bid list
            {
                BID *newBiddingData = new BID[len * 2];
                long *newKeylist = new long[len * 2];
                memcpy(newBiddingData,biddingData,len * sizeof(BID));
                memcpy(newKeylist,keylist,len * sizeof(long));
                delete[] biddingData;
                delete[] keylist;
                biddingData = newBiddingData;
                keylist = newKeylist;
                len *= 2;
            }
            keylist[totalBidders] = totalBidders;
            BID *next = biddingData + totalBidders; // address
            *next = *bid; 
            switch (bid->isOn) {
                case false:
                    totalOff += bid->power;
                    break;
                case true:
                    totalOn += bid->power;
            }
            totalBidders++;
        }

        Aggregator(void) {
            biddingData = NULL;
            keylist = NULL;
            totalOn = 0;
            totalOff = 0;
            totalBidders = 0;
            len = 0;
        }
        ~Aggregator(void) {
            delete[] biddingData;
            delete[] keylist;
        }
};