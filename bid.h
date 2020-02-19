typedef struct s_bid {
	long did; // 设备id		
	double power;
	double price;
	bool isOn; // on=true, off=false
    long timestamp;
} BID;