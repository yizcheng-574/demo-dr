#include <math.h>

class Transformer {
    private:
        double PN; // 变压器额定功率
        double deltaThetaOr, deltaThetaHr, x, y, tauO, tauW, k11, k21, k22, R; // 热参数
        double tempTopOil0, deltaTempHotspot10, deltaTempHotspot20, tempAmbient0; // 初始条件：顶油温度， 绕组温升， 环境温度
        double tempTopOil, tempAmbient; // 当前周期: 顶油温度（可测时），环境温度
        double eps = 1e-3; // 方程求解允许误差
        bool canMeasureTopoilTemp; // 是否能够测的变压器顶油温度
        double maxHotspotTemp = 120; // 顶油温度上限
        double deltaT; // 控制周期市场(h)

        double calculateEquivalentLoadRatio(double historyLoadRatio[], int len) {
            double sq = 0;
            for (int i = 0; i < len; i++) {
                sq += pow(historyLoadRatio[i], 2);
            }
            return sqrt(sq / len);
        }

        // 读取顶油温度，并存在tempTopOil里
        double getTempTopoil() {

        }

        // 原方程
        double f(double K) {
            double deltaTempHotspot = (k21 * pow(K, y) * deltaThetaHr - deltaTempHotspot10) * (1 - exp(- deltaT / k22 / tauW))
                - ((k21 -1) * pow(K, y) * deltaThetaHr - deltaTempHotspot20) * (1 - exp(- deltaT / tauO * k22));
            if (!canMeasureTopoilTemp) {
                return tempTopOil0 + deltaTempHotspot10 - deltaTempHotspot20 
                + (pow((1 + K * K * R) / (1 + R), x) * deltaThetaOr - tempTopOil0 + tempAmbient) * (1 - exp(- deltaT / k11 / tauO)) + deltaTempHotspot - maxHotspotTemp;
            } else {
                return tempTopOil + deltaTempHotspot - maxHotspotTemp;
            }
        }

        // 导函数
        double df(double K) {
            double dDeltaTempHotspot = y * pow(K, y - 1) * deltaThetaHr * (k21 * (1 - exp(- deltaT / k22 / tauW)) - (k21 - 1) * (1 - exp(- deltaT / tauO * k22)));
            if (!canMeasureTopoilTemp) {
                return dDeltaTempHotspot + 2 * K * R / (1 + R) * x * pow((1 + K * K * R) / (1 + R), x - 1) * deltaThetaOr * (1 - exp(- deltaT / k11 / tauO)); 
            } else {
                return dDeltaTempHotspot;
            }
        }

         // 牛顿法求解方程
        double newton(double x) {
            do
            {
                x = x - f(x) / df(x);
            } while (fabs(f(x)) > eps);
            return x;
        }

        // 计算初始温度条件
        void calculateInitialTemperature(double K) {
            if (!canMeasureTopoilTemp) {
                tempTopOil0 = pow((1 + K * K * R) / (1 + K), x) * deltaThetaOr + tempAmbient0;
            } else {
                getTempTopoil();
            }
            deltaTempHotspot10 = k21 * pow(K, y) * deltaThetaHr;
            deltaTempHotspot20 = (k21 - 1) * pow(K, y) * deltaThetaHr;
        }

    public:
        
        double calculateMaxPower() {
            double a[6] = {1, 1, 0.8, 0.8, 0.9, 0.7}; // demo
            double K0 = calculateEquivalentLoadRatio(a, 6); // 先根据历史数据计算等效负载率
            calculateInitialTemperature(K0);
            return newton(1) * PN;
        }

       void setInfo(double _PN, double _deltaThetaOr, double _deltaThetaHr, double _x, double _y, double _tauO, double _tauW, double _k11, double _k21, double _k22, double _R) {
            PN = _PN;
            deltaThetaOr = _deltaThetaOr; deltaThetaHr = _deltaThetaHr;
            x = _x; y = _y;
            tauO = _tauO; tauW = _tauW;
            k11 = _k11; k21 = _k21; k22 = _k22;
            R = _R;
       }

       void setHotspotLimit(double _maxHotspotTemp) {
            maxHotspotTemp = _maxHotspotTemp;
       }

       Transformer(double _deltaT, bool _canMeasureTopoilTemp) {
           deltaT = _deltaT;
           canMeasureTopoilTemp = _canMeasureTopoilTemp;
       }


};