/*
* Оригинальная идея (c) Sebra
* Алгоритм регулирования (c) Chatterbox
* 
* Вольный перевод в библиотеку Tomat7
* Version 0.7
* 
* A0 - подключение "измерителя" напряжения (трансформатор, диодный мост, делитель напряжения)
* A1 - подключение датчика тока ACS712
* D5 - управление триаком
* D3 - детектор нуля
*
*/
#ifndef ACpower_h
#define ACpower_h

#include "Arduino.h"

#define LIBVERSION "ACpower_v20180422 zeroI: "
#define ZERO_OFFSET 100			// минимальный угол открытия. *** возможно нужно больше!! ***
#define MAX_OFFSET 18000     	// Максимальный угол открытия триака. (определяет минимально возможную мощность)
#define PMIN 50				// минимально допустимая устанавливаемая мощность (наверное можно и меньше)
#define ACS_RATIO5 0.024414063	// Коэффициент датчика ACS712 |5А - 0.024414063 | 20А - 0.048828125 | 30A - 0.073242188 |
#define ACS_RATIO20 0.048828125	// Коэффициент датчика ACS712 |5А - 0.024414063 | 20А - 0.048828125 | 30A - 0.073242188 |
#define ACS_RATIO30 0.073242188	// Коэффициент датчика ACS712 |5А - 0.024414063 | 20А - 0.048828125 | 30A - 0.073242188 |

//#define CALIBRATE_ZERO  // выполнять процедуру калибровки ноля датчика тока
#ifndef CALIBRATE_ZERO
#define _zeroI 512
#endif

#define EXTEND_U_RANGE	// увеличение "динамического диапазона" измерений напряжения в 4 раза
// требуется изменение схемы и перекалибровка измерителя напряжения
// при Uratio=1 подсчет напряжения идет как и раньше

//#define U_RATIO 0.2857	// множитель напряжения - теперь он в public и задается при создании объекта
//#define LAG_FACTOR 10		// Коэффициент интеграции в расчете угла DEBUG! **выпилил за ненадобностью**
//#define AVG_FACTOR 2		// Фактор усреднения измеренного тока DEBUG! **выпилил за ненадобностью**

class ACpower
{
public:
	ACpower(uint16_t Pm);
	ACpower(uint16_t Pm, byte pinZeroCross, byte pinTriac, byte pinVoltage, byte pinACS712);
	//ACpower(uint16_t Pm, float Ur);
	float Inow = 0;   		// переменная расчета RMS тока
	float Unow = 0;   		// переменная расчета RMS напряжения

	int Angle = MAX_OFFSET;
	uint16_t Pnow;
	uint16_t Pavg;
	uint16_t Pset = 0;
	uint16_t Pmax;


	//uint8_t LagFactor = 1;	// DEBUG!! убрать
	uint16_t ADCperiod;		// DEBUG!! убрать

	void init();
	void init(byte ACS712type);
	void init(byte ACS712type, float Ur);
	//void init(uint16_t Pm);
	
	void control();
	void setpower(uint16_t setP);
	//=== Прерывания
	static void ZeroCross_int() __attribute__((always_inline));
	static void GetADC_int() __attribute__((always_inline));
	static void OpenTriac_int() __attribute__((always_inline));
	static void CloseTriac_int() __attribute__((always_inline));
	// === test
	#ifdef CALIBRATE_ZERO
	int calibrate();
	#endif
	
protected:
	float _Uratio;
	float _Iratio;
	volatile static bool getI;
	volatile static unsigned int _cntr;
	volatile static unsigned long _Summ;
	volatile static unsigned int _angle;
	unsigned long _ADCmillis;
	volatile static byte _pinTriac;
	byte _pinZCross;
	byte _pinI;
	byte _pinU;
	byte _admuxI;
	byte _admuxU;
	#ifdef CALIBRATE_ZERO
	volatile static int _zeroI;
	#endif
};
//extern ACpower TEH;

#endif
