/*
* Оригинальная идея (c) Sebra
* Алгоритм регулирования (c) Chatterbox
* 
* Вольный перевод в библиотеку Tomat7
* Version 0.7
* 
* A0 - подключение "измерителя" напряжения (трансформатор, диодный мост, делитель напряжения)
* A1 - подключение "выхода" датчика тока ACS712
* D5 - управление триаком
* D3 - детектор нуля
* это условный "стандарт", потому как все эти  входы-выходы можно менять
* детектор нуля может быть на D2 или D3
* управление триаком почти на любом цифровом выходе порта D, то есть D2-D7
* эти входы-выходы могут (или должны) задаваться при инициализации объекта ACpower
*
*	ACpower(uint16_t Pm, byte pinZeroCross, byte pinTriac, byte pinVoltage, byte pinACS712);
	Pm - максимальная мощность. регулятор не позволит установить мощность больше чем MAXPOWER
	pinZeroCross - номер пина к которому подключен детектор нуля (2 или 3)
	pinTriac - номер пина который управляет триаком (2-7)
	pinVoltage - "имя" вывода к которому подключен "датчик напряжения" - трансформатор с обвязкой (A0-A7)
	pinACS712 - "имя" вывода к которому подключен "датчик тока" - ACS712 (A0-A7)
*	
* 	ACpower(uint16_t Pm) = ACpower(MAXPOWER, 3, 5, A0, A1) - так тоже можно
*/
#ifndef ACpower_esp32_h
#define ACpower_esp32_h

#if defined(ESP32)

#define LIBVERSION "ACpower_v20190610 "

#define ZC_CRAZY		// если ZeroCross прерывание выполняется слишком часто :-(
#define ZC_EDGE RISING	// FALLING, RISING

#define ADC_RATE 200    // количество отсчетов АЦП на ПОЛУволну - 200 (для прерываний)
#define ADC_WAVES 10    // количество обсчитываемых ПОЛУволн - 4
#define ADC_NOISE 1000  // попробуем "понизить" шум АЦП

#define U_ZERO 1931     //2113
#define I_ZERO 1942     //1907
#define U_RATIO 0.2
#define I_RATIO 0.0129
//#define U_CORRECTION {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.5,0.6,0.7,2.8,8.9,12,14.1,15.2,17.3,18.4}

#define PIN_U 39
#define PIN_I 36
#define PIN_ZCROSS 25
#define PIN_TRIAC 26

#define ANGLE_MIN 1000		// минимальный угол открытия - определяет MIN возможную мощность
#define ANGLE_MAX 10100		// максимальный угол открытия триака - определяет MAX возможную мощность
#define ANGLE_DELTA 100		// запас по времени для открытия триака
#define POWER_MAX 3500		// больше этой мощности установить не получится
#define POWER_MIN 50		// минимально допустимая устанавливаемая мощность (наверное можно и меньше)

#define TIMER_TRIAC 0
#define TIMER_ADC 1

#define DEBUG1
#define DEBUG2

class ACpower
{
public:
	ACpower(uint16_t Pm, uint8_t pinZeroCross, uint8_t pinTriac, uint8_t pinVoltage, uint8_t pinCurrent);
	
	float Inow = 0;   		// переменная расчета RMS тока
	float Unow = 0;   		// переменная расчета RMS напряжения

	int16_t Angle = 0;
	uint16_t Pnow;
	uint16_t Pset = 0;
	uint16_t Pmax;
	
	volatile static uint32_t CounterZC;
	volatile static uint32_t CounterTR;
	uint32_t CounterRMS = 0;

	volatile static int16_t Xnow;
	volatile static uint32_t X2;
	
	void init(float Iratio, float Uratio);
	
	void control();
	void check();
	void setpower(uint16_t setP);
	void printConfig();
	void calibrate();
	//=== Прерывания
	static void ZeroCross_int(); // __attribute__((always_inline));
	static void GetADC_int(); // __attribute__((always_inline));
	static void OpenTriac_int(); // __attribute__((always_inline));
	//static void CloseTriac_int(); //__attribute__((always_inline));
	// === test
#ifdef DEBUG2
	volatile static uint32_t ZCcore;
	volatile static uint16_t ZCprio;
	volatile static uint32_t ADCcore;
	volatile static uint16_t ADCprio;
	volatile static uint32_t TRIACcore;
	volatile static uint16_t TRIACprio;
	volatile static uint32_t CounterTRopen;
	volatile static uint32_t CounterTRclose;
	volatile static uint64_t TRIACtimerOpen, TRIACtimerClose;
	uint32_t RMScore;
	uint16_t RMSprio;
#endif


protected:
	void setup_ZeroCross();
	void setup_Triac();
	void setup_ADC();
	
	float _Uratio;
	float _Iratio;
	
	hw_timer_t* timerADC = NULL;
	static hw_timer_t* timerTriac;
	volatile static SemaphoreHandle_t smphRMS;
	static portMUX_TYPE muxADC;
	//volatile static SemaphoreHandle_t smphTriac;
	//portMUX_TYPE muxTriac = portMUX_INITIALIZER_UNLOCKED;
	
	volatile static bool getI;
	volatile static bool takeADC;

	volatile static uint8_t _zero;
	volatile static uint8_t _pin;
	static uint8_t _pinI;
	static uint8_t _pinU;
	static uint8_t _pinTriac;
	uint8_t _pinZCross;
	
	volatile static uint64_t _summ;
	volatile static uint64_t _I2summ;
	volatile static uint64_t _U2summ;

	volatile static uint32_t _cntr;
	volatile static uint32_t _Icntr;
	volatile static uint32_t _Ucntr;

	volatile static uint16_t _zerolevel;
	static uint16_t _Izerolevel;
	static uint16_t _Uzerolevel;

	volatile static uint32_t _msZCmillis;
    //volatile static bool trOpened;

	//volatile static uint32_t _cntrZC;
	//volatile static uint32_t _tmrTriacNow;

	volatile static uint16_t _angle; 
	
#ifdef CALIBRATE_ZERO
	volatile static int _zeroI;
#endif

#ifdef U_CORRECTION
	//static const 
	float Ucorr[25] = U_CORRECTION;
#endif
};

#endif // ESP32

#endif //ACpower_esp32_h
