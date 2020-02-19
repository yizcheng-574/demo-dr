#include "bid.h"
#include <time.h>  

class AirConditioner {
    private:
        long did; // 设备id
        double power; // 空调额定功率(kW)
        double isOn = false; // 上一周期状态
        double setpoint; // 用户设定的温度（摄氏度）
        double tempDeadband; // 用户设定的死区
        double controlDeadband; // 调温控制的死区 
        double currentTemp; // 当前室温
        bool isInCoolingMode; // 当前模式 true=制冷, false=制热
        double biddingPrice; // 本周期投标价格

        double calculateDoS() {
            getCurrentTemp();
            double DoS = (currentTemp - setpoint) / tempDeadband;
            if (!isInCoolingMode) {
                DoS = - DoS;
            }
            return DoS;
        }

        // 调节空调设备的设定温度
        void setTempSet(double tempSet) {
        }

        // 获取当前室温存入currentTemp
        void getCurrentTemp() {
        }
    
    public:
        BID bid() {
            BID result;
            result.did = did;
            result.power = power;
            result.isOn = isOn;
            result.timestamp = time(0);
            double DoS = calculateDoS();
            // 根据当前状态计算投标价格
            if (isOn) {
                result.price = (DoS + 1) / 2;
            } else {
                result.price = (DoS - 1) / 2;
            }
            biddingPrice = result.price;
            return result;
        }

        void response(double clearingPrice) {
            if (clearingPrice > biddingPrice) {
                isOn = true;
            } else if (clearingPrice < biddingPrice) {
                isOn = false;
            }
            // 使用调温控制方式控制空调
            if (isOn == isInCoolingMode) {
                setTempSet(currentTemp - controlDeadband);
            } else {
                setTempSet(currentTemp + controlDeadband);
            }
        }

        // 设定用户设置最适温度
        void setSetpoint(double _setpoint) {
            setpoint = _setpoint;
        }

        // 设定用户设置的死区
        void setTempDeadband(double _tempDeadband) {
            tempDeadband = _tempDeadband;
        }

        // 设定空调运行模式
        void setMode(bool _isInCoolingMode) {
            isInCoolingMode = _isInCoolingMode;
        }
};
