#include "bid.h"
#include <time.h>
#include <math.h>

class ElectricVehicle {
    private:
        long did; // 设备id
        double PN; // 充电桩额定充电功率(kW)
        double isOn = false; // 上一周期状态
        long arrivalTime; // 接入充电桩时间(unix timestamp)
        long departureTime; // 用户设置的驶离时间(unix timestamp)
        double pluggedEnergy; //接入充电桩时电量(kWh)
        double targetEnergy; // 用户设置的目标电量(kWh)
        double energyDeadband; // 允许电量偏移（kWh)
        double currentEnergy; // 当前电量
        bool willCalculateByReferenceTrajectory; // 满意度计算方法 true=基于参考轨迹，false=基于最大灵活性
        double biddingPrice; // 本周期投标价格
        double calculateDoS(long timestamp) {
            getCurrentEnergy();
            double DoS;
            if (willCalculateByReferenceTrajectory) { //基于参考轨迹
                double timeElapsedRatio = (timestamp - arrivalTime) / (departureTime - arrivalTime);
                double refEnergy = pluggedEnergy + (targetEnergy - pluggedEnergy) * timeElapsedRatio;
                DoS = - (currentEnergy - refEnergy) / energyDeadband;
            } else { // 基于最大灵活性
                double averagePower = (targetEnergy - currentEnergy) / ((departureTime - timestamp) / 1000 / 3600);
                DoS = 2 * averagePower / PN - 1;
            }
            return DoS;
        }

        // 读取当前电量存入currentEnergy中
        void getCurrentEnergy() {
        }

    public:
        BID bid() {
            BID result;
            result.did = did;
            result.power = PN;
            result.isOn = isOn;
            result.timestamp = time(0);        
            result.price = calculateDoS(result.timestamp);
            biddingPrice = result.price;
            return result;
        }

        void response(double clearingPrice) {
            if (clearingPrice > biddingPrice) {
                isOn = true;
            } else if (clearingPrice < biddingPrice) {
                isOn = false;
            }
        }
        
        //设置充电信息，EV接入时调用
        void setChargeInfo(long _arrivalTime, long _departureTime, double _targetEnergy) {
            arrivalTime = _arrivalTime;
            departureTime = _departureTime;
            targetEnergy = _targetEnergy;
        } 

        void setCalculatedMode(bool _willCalculateByReferenceTrajectory) {
            willCalculateByReferenceTrajectory = _willCalculateByReferenceTrajectory;
        }

        void setEnergyDeadband(double _energyDeadband) {
            energyDeadband = _energyDeadband;
        }
};