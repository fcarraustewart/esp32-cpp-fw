#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/i2c.h>
#include <stdio.h>
#include <stdint.h>
#include <Services/IMU.hpp>
#include <Services/BLE.hpp>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(IMU, LOG_LEVEL_INF);

#define IMU_ACCEL_ADDRESS (0x4a)
#define IMU_MAG_ADDRESS (0x4a  )

#define SCALE_Q(n) (1.0f / (1 << n))
static constexpr double pi = 3.14159265358979323846;
static constexpr double scaleRadToDeg = (double)180.0f / pi;
static constexpr double conversion = ((double)180.0f / 3.14159265358)*((double)SCALE_Q(14));

// The IMU's interrupt output pin is connected to P0.25
int IMU_readRegister(uint8_t RegNum, uint8_t *Value);
int IMU_writeRegister(uint8_t RegNum, uint8_t Value);
int IMU_writeMagRegister(uint8_t RegNum, uint8_t Value);
void readCalibrationData();
//static const struct spi_config * cfg;
static const struct device *i2c;
int Service::IMU::Begin()
{
	int nack = 0;
	uint8_t header[4];
	uint8_t dummy_value[64];
	uint8_t buf[32];

	i2c = DEVICE_DT_GET(DT_NODELABEL(i2c0));
	if (i2c==NULL)
	{
		LOG_DBG("Error acquiring i2c0 interface\n");
		return -1;
	}
	
    zpp::this_thread::sleep_for(std::chrono::milliseconds(600)); // allow interface MCU complete booting before dummy read

					// Read SHTP config reg:
	
	bool 		previous_data_buffer_continues = 0;
	uint16_t 	length = 0;
	uint16_t 	seq_num = 0;
	enum class state_t{
		WriteCmd_Process,
		Header_Process,
		Buffer_Process,
		End,
	};

	state_t state = state_t::WriteCmd_Process;
	do
	{
		switch (state) {

			case state_t::WriteCmd_Process : 
			{	
				//dummy_value[0]=0xf8;
				//nack = i2c_write(i2c,	dummy_value,	1,	IMU_ACCEL_ADDRESS); 
				if(nack) return nack;

				previous_data_buffer_continues = 0;
				length = 0;
				seq_num = 0;
				state = state_t::Header_Process;
			}	break;

			case state_t::Header_Process : 
			{	
				nack = i2c_read(i2c,	header,			4,	IMU_ACCEL_ADDRESS);  
				if(nack) return nack;
								// Process SHTP header:
				previous_data_buffer_continues = header[1]&0x80;
				length = (((uint16_t)(header[1])<<8)&0x7f00 )|( (header[0])&0x00ff);
				if(length == 0) {
					state = state_t::End; // abort
					LOG_INF("\t\t\t\t Header_Process: Ending. Length = 0 for [cmd=0xf8] with length=0x%x\n", length);
				}

				seq_num = 0;
				LOG_INF("\t\t\t\t Header_Process: new header[cmd=0xf8] with length=0x%x\n", length);

				state = state_t::Buffer_Process;
			} 	break;

			case state_t::Buffer_Process : 
			{	
				//uint16_t read_size = (length > 256) ? 256 : length;
				nack = i2c_read(	i2c,	buf,	32,		IMU_ACCEL_ADDRESS);
				if(nack) return nack;

							// Process SHTP cargo header again:
				memcpy(header, buf, 4);
				previous_data_buffer_continues = header[1]&0x80;
				length = (((uint16_t)(header[1])<<8)&0x7f00 )|( (header[0])&0x00ff);
				if(length == 0) {
					state = state_t::End; // abort
					LOG_INF("\t\t\t\t Buffer_Process: Ending. Length = 0 for [cmd=0xf8] with length=0x%x\n", length);
					return -1;
				}

				LOG_HEXDUMP_INF(buf, 32, "\t\t\t\t Buffer_Process: Read buf[length]"); 
				seq_num += 32;
				LOG_INF("\t\t\t\t Buffer_Process: Length [cmd=0xf8]=0x%x \t seq_num = 0x%x\n", length, seq_num);

				if(previous_data_buffer_continues == true)
					state = state_t::Header_Process;
				if(seq_num > length)
					state = state_t::End;
				if(previous_data_buffer_continues == false)
					state = state_t::End;

			} 	break;


			default:
				break;
		}
		zpp::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	while(state != state_t::End);

	/*
			SHTP BNO085 PROTOCOL
		bno.enable_feature(BNO_REPORT_ACCELEROMETER)
		bno.enable_feature(BNO_REPORT_GYROSCOPE)
		bno.enable_feature(BNO_REPORT_MAGNETOMETER)
		bno.enable_feature(BNO_REPORT_ROTATION_VECTOR)

		See:
			https://github.com/adafruit/Adafruit_CircuitPython_BNO08x/blob/main/adafruit_bno08x/__init__.py#L52
			https://www.ceva-ip.com/wp-content/uploads/2019/10/BNO080_085-Datasheet.pdf
			https://docs.sparkfun.com/SparkFun_VR_IMU_Breakout_BNO086_QWIIC/assets/component_documentation/Sensor-Hub-Transport-Protocol.pdf
	*/
	length = 1;
	while(length > 0){
		nack = i2c_read(	i2c,	buf,	32,		IMU_ACCEL_ADDRESS);

		memcpy(header, buf, 4);
		length = ((((uint16_t)(header[1])<<8)&0x7f00) )|( ((header[0])&0x00ff));
		
		LOG_DBG("\t\t\t\t Empty reads i2c %x\n", length);
	}
	memset(dummy_value, 0, 64);
	dummy_value[0]=0x15; //Set Feature 
	dummy_value[1]=0x00;
	dummy_value[2]=0x02;
	dummy_value[3]=0x00;
	dummy_value[4]=0xFD;//CHANNEL CONTROL CMD
	dummy_value[5]=0x01;//SET_FEATURE ACCEL
	dummy_value[6]=0x00;
	dummy_value[7]=0x00;
	dummy_value[8]=0x00;
	dummy_value[9]=0x60; //PERIOD 50ms 
	dummy_value[10]=0xEA;
						 //SENSOR_SPECIFICS 0
	nack = i2c_write(i2c,	dummy_value,	17,	IMU_ACCEL_ADDRESS); 
	length = 1;
	while(length > 0){
		nack = i2c_read(	i2c,	buf,	32,		IMU_ACCEL_ADDRESS);

		memcpy(header, buf, 4);
		length = (((uint16_t)(header[1])<<8)&0x7f00) | ((header[0])&0x00ff);
		
		LOG_DBG("\t\t\t\t Empty reads i2c %x\n", length);
	}
	memset(dummy_value, 0, 64);
	dummy_value[0]=0x15; //Set Feature 
	dummy_value[1]=0x00;
	dummy_value[2]=0x02;
	dummy_value[3]=0x00;
	dummy_value[4]=0xFD;//CHANNEL CONTROL CMD
	dummy_value[5]=0x02;//SET_FEATURE GYRO
	dummy_value[6]=0x00;
	dummy_value[7]=0x00;
	dummy_value[8]=0x00;
	dummy_value[9]=0x60; //PERIOD 50ms 
	dummy_value[10]=0xEA;

	nack = i2c_write(i2c,	dummy_value,	20,	IMU_ACCEL_ADDRESS); 
	length = 1;
	while(length > 0){
		nack = i2c_read(	i2c,	buf,	32,		IMU_ACCEL_ADDRESS);

		memcpy(header, buf, 4);
		length = (((uint16_t)(header[1])<<8)&0x7f00) | ((header[0])&0x00ff);
		
		LOG_DBG("\t\t\t\t Empty reads i2c %x\n", length);
	}
	memset(dummy_value, 0, 64);
	dummy_value[0]=0x15; //Set Feature 
	dummy_value[1]=0x00;
	dummy_value[2]=0x02;
	dummy_value[3]=0x00;
	dummy_value[4]=0xFD;//CHANNEL CONTROL CMD
	dummy_value[5]=0x03;//SET_FEATURE MAG
	dummy_value[6]=0x00;
	dummy_value[7]=0x00;
	dummy_value[8]=0x00;
	dummy_value[9]=0x60; //PERIOD 50ms 
	dummy_value[10]=0xEA;
	nack = i2c_write(i2c,	dummy_value,	20,	IMU_ACCEL_ADDRESS); 
// Enable Quaternion Rotation vector Channel 0x5 with period 50ms = 50000

	length = 1;
	while(length > 0){
		nack = i2c_read(	i2c,	buf,	32,		IMU_ACCEL_ADDRESS);

		memcpy(header, buf, 4);
		length = (((uint16_t)(header[1])<<8)&0x7f00) | ((header[0])&0x00ff);
		
		LOG_DBG("\t\t\t\t Empty reads i2c %x\n", length);
	}
	memset(dummy_value, 0, 64);
	dummy_value[0]=0x15; //Set Feature HEADER CMD LENGTH
	dummy_value[1]=0x00; //Set Feature HEADER CMD LENGTH
	dummy_value[2]=0x02; //Set Feature HEADER CMD 
	dummy_value[3]=0x00; //Set Feature HEADER CMD LENGTH
	dummy_value[4]=0xFD;//CHANNEL CONTROL CMD
	dummy_value[5]=0x05;//SET_FEATURE ROT VECT
	dummy_value[6]=0x00;
	dummy_value[7]=0x00;
	dummy_value[8]=0x00;
	dummy_value[9]=0x60; //PERIOD 50ms 
	dummy_value[10]=0xEA;
	nack = i2c_write(i2c,	dummy_value,	0x15,	IMU_ACCEL_ADDRESS); 
	LOG_INF("\t\t\t\t End Begin IMU, read what the BNO085 had to say.\n");

	length = 1;
	while(length > 0){
		nack = i2c_read(	i2c,	buf,	32,		IMU_ACCEL_ADDRESS);

		memcpy(header, buf, 4);
		length = (((uint16_t)(header[1])<<8)&0x7f00) | ((header[0])&0x00ff);
		
		LOG_DBG("\t\t\t\t Empty reads i2c %x\n", length);
	}
	
	return nack;
}
int Service::IMU::CountSteps(volatile uint32_t * pCount)
{
    return 0;
}
struct IMU_SHTP_PACKET
{
	uint32_t header;
	uint8_t data[64];
};

int Service::IMU::ReadRotXYZ() // returns Temperature * 100
{
	int 		nack;
	uint8_t 	header[4];
	uint8_t 	dummy_value[32];
	bool 		previous_data_buffer_continues = 0;
	uint16_t 	length = 0;
	uint8_t		send[17];

	memset(dummy_value, 0, 32);
	dummy_value[0]=0x15; //Set Feature HEADER CMD LENGTH
	dummy_value[1]=0x00; //Set Feature HEADER CMD LENGTH
	dummy_value[2]=0x03; //Get Feature HEADER CMD 
	dummy_value[3]=0x00; //Set Feature HEADER CMD LENGTH
	dummy_value[4]=0xFE;// GET_FEATURE
	dummy_value[5]=0x05;// ROT VECT
	dummy_value[6]=0x00;
	dummy_value[7]=0x00;
	dummy_value[8]=0x00;
	// Write COMMAND SHTP Sensor Host Transport Protocol CEVA
    nack = i2c_write(	i2c,	dummy_value,	19,	IMU_ACCEL_ADDRESS); 
	do {
		// Process SHTP header:
		nack = i2c_read(	i2c,	dummy_value,	32,		IMU_ACCEL_ADDRESS);

		memcpy(header, dummy_value, 4);
		previous_data_buffer_continues = header[1]&0x80;
		length = (((uint16_t)(header[1])<<8)&0x7f00) | ((header[0])&0x00ff); 
		if(header[2]==3){
			LOG_HEXDUMP_INF(dummy_value, length, "ROT"); 

            //sign_roll = -1 if(message[0]&0x80) else 1
            //sign_pitch = -1 if(message[2]&0x80) else 1
            //sign_yaw = -1 if(message[4]&0x80) else 1

			double roll 	= (	(dummy_value[14]&0x80)==0?1:-1 )* 	conversion*(double)(((uint16_t)dummy_value[13]&0x00ff) 	| (((uint16_t)dummy_value[14])&0x7f )<<8); 
			double pitch 	= ( (dummy_value[16]&0x80)==0?1:-1 )*	conversion*(double)(((uint16_t)dummy_value[15]&0x00ff) 	| (((uint16_t)dummy_value[16])&0x7f )<<8); 
			double yaw 		= (	(dummy_value[18]&0x80)==0?1:-1 )*	conversion*(double)(((uint16_t)dummy_value[17]&0x00ff) 	| (((uint16_t)dummy_value[18])&0x7f )<<8); 

			send[0]=dummy_value[14];
			send[1]=dummy_value[15];
			send[2]=dummy_value[16];
			send[3]=dummy_value[17];
			send[4]=dummy_value[18];
			send[5]=dummy_value[19];
			Service::BLE::Send(send);
		
			LOG_DBG("Rot : \t\t(\t%d, \t%d, \t%d).\n",(int)roll,(int)pitch,(int)yaw);
		}

		
	}
	while(length != 0);
    return length;    
}
int Service::IMU::ReadGyroXYZ() // returns Temperature * 100
{
	int 		nack;
	uint8_t 	header[4];
	uint8_t 	dummy_value[32];
	bool 		previous_data_buffer_continues = 0;
	uint16_t 	length = 0;

	memset(dummy_value, 0, 32);
	dummy_value[0]=0x15; //Set Feature HEADER CMD LENGTH
	dummy_value[1]=0x00; //Set Feature HEADER CMD LENGTH
	dummy_value[2]=0x03; //Get Feature HEADER CMD 
	dummy_value[3]=0x00; //Set Feature HEADER CMD LENGTH
	dummy_value[4]=0xFE;// GET_FEATURE
	dummy_value[5]=0x02;// GYRO VECT
	dummy_value[6]=0x00;
	dummy_value[7]=0x00;
	dummy_value[8]=0x00;
	// Write COMMAND SHTP Sensor Host Transport Protocol CEVA
    nack = i2c_write(	i2c,	dummy_value,	19,	IMU_ACCEL_ADDRESS); 
	do {
		// Process SHTP header:
		nack = i2c_read(	i2c,	dummy_value,	32,		IMU_ACCEL_ADDRESS);

		memcpy(header, dummy_value, 4);
		previous_data_buffer_continues = header[1]&0x80;
		length = (((uint16_t)(header[1])<<8)&0x7f00 )| ((header[0])&0x00ff); 
		if(header[2]==3){
			LOG_HEXDUMP_DBG(dummy_value, length, "Gyro"); 
			int roll = (((int)(dummy_value[14])<<8)&0x7f00) | ((int)(dummy_value[13])&0x00ff); 
			int pitch = (((int)(dummy_value[16])<<8)&0x7f00) |((int)(dummy_value[15])&0x00ff); 
			int yaw = (((int)(dummy_value[18])<<8)&0x7f00) | ((int)(dummy_value[17])&0x00ff); 

			LOG_DBG("Gyro : (%d, %d, %d).\n",roll,pitch,yaw);
		}

		
	}
	while(length != 0);
    return length;       
}
int Service::IMU::ReadMagXYZ() // returns Temperature * 100
{
	int 		nack;
	uint8_t 	header[4];
	uint8_t 	dummy_value[32];
	bool 		previous_data_buffer_continues = 0;
	uint16_t 	length = 0;

	memset(dummy_value, 0, 32);
	dummy_value[0]=0x15; //Set Feature HEADER CMD LENGTH
	dummy_value[1]=0x00; //Set Feature HEADER CMD LENGTH
	dummy_value[2]=0x03; //Get Feature HEADER CMD 
	dummy_value[3]=0x00; //Set Feature HEADER CMD LENGTH
	dummy_value[4]=0xFE;// GET_FEATURE
	dummy_value[5]=0x03;// MAG VECT
	dummy_value[6]=0x00;
	dummy_value[7]=0x00;
	dummy_value[8]=0x00;
	// Write COMMAND SHTP Sensor Host Transport Protocol CEVA
    nack = i2c_write(	i2c,	dummy_value,	19,	IMU_ACCEL_ADDRESS); 
	do {
		// Process SHTP header:
		nack = i2c_read(	i2c,	dummy_value,	32,		IMU_ACCEL_ADDRESS);

		memcpy(header, dummy_value, 4);
		previous_data_buffer_continues = header[1]&0x80;
		length = (((uint16_t)(header[1])<<8)&0x7f00) | ((header[0])&0x00ff); 
		if(header[2]==3){
			LOG_HEXDUMP_DBG(dummy_value, length, "Mag"); 

			int roll = (((int)(dummy_value[14])<<8)&0x7f00) | ((int)(dummy_value[13])&0x00ff); 
			int pitch = (((int)(dummy_value[16])<<8)&0x7f00) |((int)(dummy_value[15])&0x00ff); 
			int yaw = (((int)(dummy_value[18])<<8)&0x7f00 )|( (int)(dummy_value[17])&0x00ff); 

			LOG_DBG("Mag : (%d, %d, %d).\n",roll,pitch,yaw);
		}

		
	}
	while(length != 0);
    return length;         
}

int Service::IMU::ReadAccelXYZ() // returns Temperature * 100
{
	int 		nack;
	uint8_t 	header[4];
	uint8_t 	dummy_value[32];
	bool 		previous_data_buffer_continues = 0;
	uint16_t 	length = 0;

	memset(dummy_value, 0, 32);
	dummy_value[0]=0x15; //Set Feature HEADER CMD LENGTH
	dummy_value[1]=0x00; //Set Feature HEADER CMD LENGTH
	dummy_value[2]=0x03; //Get Feature HEADER CMD 
	dummy_value[3]=0x00; //Set Feature HEADER CMD LENGTH
	dummy_value[4]=0xFE;// GET_FEATURE
	dummy_value[5]=0x01;// Accel VECT
	dummy_value[6]=0x00;
	dummy_value[7]=0x00;
	dummy_value[8]=0x00;
	// Write COMMAND SHTP Sensor Host Transport Protocol CEVA
    nack = i2c_write(	i2c,	dummy_value,	19,	IMU_ACCEL_ADDRESS); 
	do {
		// Process SHTP header:
		nack = i2c_read(	i2c,	dummy_value,	32,		IMU_ACCEL_ADDRESS);

		memcpy(header, dummy_value, 4);
		previous_data_buffer_continues = header[1]&0x80;
		length = (((uint16_t)(header[1])<<8)&0x7f00 )|( (header[0])&0x00ff); 
		if(header[2]==3){
			LOG_HEXDUMP_DBG(dummy_value, length, "Accel"); 

			int roll = (((int)(dummy_value[14])<<8)&0x7f00 )|( (int)(dummy_value[13])&0x00ff); 
			int pitch = (((int)(dummy_value[16])<<8)&0x7f00 )|((int)(dummy_value[15])&0x00ff); 
			int yaw = (((int)(dummy_value[18])<<8)&0x7f00 )| ((int)(dummy_value[17])&0x00ff); 

			LOG_DBG("Accel : (%d, %d, %d).\n",roll,pitch,yaw);
		}

		
	}
	while(length != 0);
    return length;         
}





// int Service::IMU::readMagZ() // returns Temperature * 100
// {
// 	int16_t mag;
// 	uint8_t buf[2];
// 	i2c_burst_read(i2c,IMU_MAG_ADDRESS,0x05, buf,2);
// 	mag = buf[1];
// 	mag = mag << 8;
// 	mag = mag + buf[0];
// 	int mag_32 = mag; // type convert and preserve sign
//     return mag_32;    
// }


// int Service::IMU::ReadRegister(uint8_t RegNum, uint8_t *Value)
// {
// 	    //reads a byte from a specific register
//     int nack;   
// 	nack=i2c_reg_read_byte(i2c,IMU_ACCEL_ADDRESS,RegNum,Value);
// 	return nack;
// }
// int Service::IMU::WriteRegister(uint8_t RegNum, uint8_t Value)
// {
// 	//writes a byte to a specific register
//     uint8_t Buffer[2];    
//     Buffer[0]= Value;    
//     int nack;    
// 	nack=i2c_reg_write_byte(i2c,IMU_ACCEL_ADDRESS,RegNum,Value);
//     return nack;
// }
// int Service::IMU::WriteMagRegister(uint8_t RegNum, uint8_t Value)
// {
// 	//writes a byte to a specific register
//     uint8_t Buffer[2];    
//     Buffer[0]= Value;    
//     int nack;    
// 	nack=i2c_reg_write_byte(i2c,IMU_MAG_ADDRESS,RegNum,Value);
//     return nack;
// }
void Service::IMU::InitializeDriver() {
	int err;
	err = Service::IMU::Begin();
	if (err < 0)
	{
		LOG_ERR("Error initializing IMU.  Error code = %d\n",err);  
		while(1)
		{
			zpp::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
}

#include <Drivers/BNO085.h>
#include <edge-impulse-sdk/dsp/numpy.hpp>

static BNO085 Motion;

struct euler_t {
  float yaw;
  float pitch;
  float roll;
} ypr;

#ifdef FAST_MODE
	// Top frequency is reported to be 1000Hz (but freq is somewhat variable)
	sh2_SensorId_t reportType = SH2_GYRO_INTEGRATED_RV;
	long reportIntervalUs = 2000;
#else
	// Top frequency is about 250Hz but this report is more accurate
	sh2_SensorId_t reportType = SH2_ARVR_STABILIZED_RV;
	long reportIntervalUs = 5000;
#endif

void setReports(sh2_SensorId_t reportType, long report_interval) {
  LOG_INF("Setting desired reports");
}

void Service::IMU::Initialize() {
    // #define EVENTS_INTERESTED RTOS::MsgBroker::Event::BLE_Connected , ...
    // System::mMsgBroker::Subscribe<EVENTS_INTERESTED>();  
	InitializeDriver();
	/*
	ADAFRUIT
	*/
	uint8_t imu_timer=0;
	RTOS::Hal::Delay(600);
	while(imu_timer>128){
		RTOS::Hal::Delay(60);
		ReadRotXYZ();
		imu_timer++;
	}

	imu_timer = 0;
	RTOS::Hal::Delay(1000);
	Wire.begin();
	Wire.beginTransmission(0x4a);
	// Wire.setClock(100000);
	while (Motion.begin( &Wire ) == false) {
		RTOS::Hal::Delay(100);
	}
  	Wire.endTransmission();

	if(Motion.enableRotationVector(60) == true) {
		LOG_DBG("enableGeomagneticRotationVector enabled");
	}
	if(Motion.enableGeomagneticRotationVector(60) == true) {
		LOG_DBG("enableGeomagneticRotationVector enabled");
	}
	if(Motion.enableGameRotationVector(60) == true) {
		LOG_DBG("enableGeomagneticRotationVector enabled");
	}

	LOG_INF("%s: IMU Module Initialized correctly.", __FUNCTION__);
}

void quaternionToEuler(float qr, float qi, float qj, float qk, euler_t* ypr, bool degrees = false) {

    float sqr = ei::numpy::sqrt(qr);
    float sqi = ei::numpy::sqrt(qi);
    float sqj = ei::numpy::sqrt(qj);
    float sqk = ei::numpy::sqrt(qk);

    ypr->yaw = atan2(2.0 * (qi * qj + qk * qr), (sqi - sqj - sqk + sqr));
    ypr->pitch = asin(-2.0 * (qi * qk - qj * qr) / (sqi + sqj + sqk + sqr));
    ypr->roll = atan2(2.0 * (qj * qk + qi * qr), (-sqi - sqj + sqk + sqr));

    if (degrees) {
      ypr->yaw *= scaleRadToDeg;
      ypr->pitch *= scaleRadToDeg;
      ypr->roll *= scaleRadToDeg;
    }
}

void quaternionToEulerRV(sh2_RotationVectorWAcc_t* rotational_vector, euler_t* ypr, bool degrees = false) {
    quaternionToEuler(rotational_vector->real, rotational_vector->i, rotational_vector->j, rotational_vector->k, ypr, degrees);
}

void quaternionToEulerGI(sh2_GyroIntegratedRV_t* rotational_vector, euler_t* ypr, bool degrees = false) {
    quaternionToEuler(rotational_vector->real, rotational_vector->i, rotational_vector->j, rotational_vector->k, ypr, degrees);
}

void Service::IMU::Handle(const uint8_t arg[]) {
	uint8_t imu_read_timer = 0U;
	float roll=0, pitch=0, yaw=0;
	float Rr = 0;
	float Ri = 0;
	float Rj = 0;
	float Rk = 0;

    /**
     * Handle arg packet.
     */
    switch(arg[0])
    {
		case 0xA5:
		{
			LOG_DBG("sensor was reset. Reset Reason: %d", Motion.getResetReason());
			
			if(Motion.enableRotationVector(1000) == true) {
				LOG_DBG("enableGeomagneticRotationVector enabled");
			}
		} break;
        default:
        {	
			if (Motion.getSensorEvent() == false) {
				if (imu_read_timer > 300U) {
					LOG_DBG("IMU sensor not responding. Resetting sensor.");
					uint8_t msg[] = {0xA5, 0, 0, 0, 0};
					Send( msg );
					break;
				} else {
					imu_read_timer++;
				}
				RTOS::Hal::Delay(1);
			} else {
				imu_read_timer = 0;
			}

			uint8_t result = Motion.getSensorEventID();
			if (result == SENSOR_REPORTID_ROTATION_VECTOR) {
				Rr = Motion.getRot_R();
				Ri = Motion.getRot_I();
				Rj = Motion.getRot_J();
				Rk = Motion.getRot_K();
				
				roll = atan2(2.0 * (Rr * Ri + Rj * Rk), 1.0 - 2.0 * (Ri * Ri + Rj * Rj)) * scaleRadToDeg;  /* ROLL */
				pitch = atan2(2.0 * (Rr * Rj - Rk * Ri), 1.0 - 2.0 * (Rj * Rj + Ri * Ri)) * scaleRadToDeg; /* PITCH */
				yaw = atan2(2.0 * (Rr * Rk + Ri * Rj), 1.0 - 2.0 * (Rj * Rj + Rk * Rk)) * scaleRadToDeg;   /* YAW */
			
				ypr = { .yaw=yaw, .pitch=pitch, .roll=roll };	
				LOG_DBG("IMU: ROLL: %d, PITCH: %d, YAW: %d\n", (int)roll, (int)pitch, (int)yaw);				
			}
			else if (result == SENSOR_REPORTID_GAME_ROTATION_VECTOR) {
				struct { 
					float i = Motion.getGameI(); 
					float j = Motion.getGameJ(); 
					float k = Motion.getGameK(); 
					float real = Motion.getGameReal(); 
				} game_rotation_vector;
				quaternionToEuler(game_rotation_vector.real, game_rotation_vector.i, game_rotation_vector.j, game_rotation_vector.k, &ypr, true);

				LOG_INF("Game Rotation Vector: (%d, %d, %d, %d)\n", game_rotation_vector.i, game_rotation_vector.j, game_rotation_vector.k, game_rotation_vector.real);
			}
			else if (result == SENSOR_REPORTID_GYRO_INTEGRATED_ROTATION_VECTOR) {
				LOG_INF("SENSOR_REPORTID_GYRO_INTEGRATED_ROTATION_VECTOR: (NYI)");
			}
			else if (result == SENSOR_REPORTID_AR_VR_STABILIZED_ROTATION_VECTOR) {
				LOG_INF("SENSOR_REPORTID_AR_VR_STABILIZED_ROTATION_VECTOR: (NYI)");
			}
			else if (result == SENSOR_REPORTID_GEOMAGNETIC_ROTATION_VECTOR) {
				LOG_INF("SENSOR_REPORTID_GEOMAGNETIC_ROTATION_VECTOR: (NYI)");
			}
			else
			{
				LOG_DBG("Result is not a rotation vector. Result: %d", result);
			}


            break;
        }
    };
}

/**
 * Build the static members on the RTOS::ActiveObject
 */
namespace Service
{
    using                       _IMU = RTOS::ActiveObject<Service::IMU>;

    template <>
    const char               	_IMU::mName[] =  "IMU";
    template <>
    uint8_t                     _IMU::mCountLoops = 0;
    template <>
    const uint8_t               _IMU::mInputQueueItemLength = 16;
    template <>
    const uint8_t               _IMU::mInputQueueItemSize = 16*sizeof(uint16_t);
    template <>
    const size_t                _IMU::mInputQueueSizeBytes = 
                                        RTOS::ActiveObject<Service::IMU>::mInputQueueItemLength 
                                        * RTOS::ActiveObject<Service::IMU>::mInputQueueItemSize;
    template <>
    char                        _IMU::mInputQueueAllocation[
                                        RTOS::ActiveObject<Service::IMU>::mInputQueueSizeBytes
                                    ] = { 0 };
    template <>
    RTOS::QueueHandle_t         _IMU::mInputQueue = RTOS::Hal::QueueCreate(
                                        RTOS::ActiveObject<Service::IMU>::mInputQueueItemLength,
                                        RTOS::ActiveObject<Service::IMU>::mInputQueueItemSize,
                                        RTOS::ActiveObject<Service::IMU>::mInputQueueAllocation
                                    );
    template <>
    uint8_t                     _IMU::mReceivedMsg[
                                        RTOS::ActiveObject<Service::IMU>::mInputQueueItemLength
                                    ] = { 0 };

	namespace {
    ZPP_KERNEL_STACK_DEFINE(cIMUThreadStack, 2048);
    template <>
    zpp::thread_data            _IMU::mTaskControlBlock = zpp::thread_data();
    template <>
    zpp::thread                 _IMU::mHandle = zpp::thread(
                                        mTaskControlBlock, 
                                        Service::cIMUThreadStack(), 
                                        RTOS::cThreadAttributes, 
                                        Service::_IMU::Run
                                    );
    } //https://www.reddit.com/r/cpp/comments/4ukhh5/what_is_the_purpose_of_anonymous_namespaces/#:~:text=The%20purpose%20of%20an%20anonymous,will%20not%20have%20internal%20linkage.



                                    
}
