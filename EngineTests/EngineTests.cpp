#include "pch.h"
#include <iostream>
#include <string.h>

const int VALUES_NUMBER = 6; // Количество значений зависимости
const float TIME_STEP = 0.1; // Шаг времени

using namespace std;
using VA = float[VALUES_NUMBER];

// Класс двигателя 
class Engine	
{
public:
	Engine();
	Engine(float inertia, VA torque, VA rotationSpeed, float overheatTemp, float torqueHeatFactor,
		float rotationSpeedHeatFactor, float coolingFactor);
	~Engine();

	float engineOverheatTest(float tempOut);

	// Вывод в консоль параметров двигателя
	void show()
	{
		cout << "lends:" << endl;
		cout << inertia << endl << overheatTemp << endl << torqueHeatFactor << endl << rotationSpeedHeatFactor << endl << coolingFactor << endl;
		cout << "Arrays:" << endl;
		for (float elm : torque)
			cout << elm << ' ';
		cout << endl;
		for (float elm : rotationSpeed)
			cout << elm << ' ';
		cout << endl;
	}

private:
	float inertia;						// Момент инерции
	VA torque;							// Крутящий момент
	VA rotationSpeed;					// Скороть вращения вала
	float overheatTemp;					// Температура перегрева
	float torqueHeatFactor;				// Коэффициент нагрева крутящего момента
	float rotationSpeedHeatFactor;		// Коэффициент нагрева скорости вращения
	float coolingFactor;				// Коэффициент охлаждения

	// Рассчет значения  крутящего момента по скорости вращения коленвала
	float convert(float value)
	{
		float From1, From2, To2, To1;
		int i;

		for (i = 0; i < VALUES_NUMBER - 1; i++)
		{
			if (value >= rotationSpeed[i] && value <= rotationSpeed[i + 1])
				break;
		}

		From1 = rotationSpeed[i];
		From2 = rotationSpeed[i + 1];
		To2 = torque[i + 1];
		To1 = torque[i];

		return (value - From1) / (From2 - From1)*(To2 - To1) + To1;
	}
};

// Конструктор по умолчанию
Engine::Engine() : torque{ 20, 75, 100, 105, 75, 0 }, rotationSpeed{ 0, 75, 150, 200, 250, 300 }
{
	inertia = 10;
	overheatTemp = 110;
	torqueHeatFactor = 0.01;
	rotationSpeedHeatFactor = 0.0001;
	coolingFactor = 0.1;
}

// Конструктор для пользовательского двигателя
Engine::Engine(float inertia, VA torque, VA rotationSpeed, float overheatTemp,	float torqueHeatFactor,
	float rotationSpeedHeatFactor, float coolingFactor)
{
	this->inertia = inertia;
	memcpy(this->torque, torque, sizeof(this->torque));
	memcpy(this->rotationSpeed, rotationSpeed, sizeof(this->rotationSpeed));
	this->overheatTemp = overheatTemp;
	this->torqueHeatFactor = torqueHeatFactor;
	this->rotationSpeedHeatFactor = rotationSpeedHeatFactor;
	this->coolingFactor = coolingFactor;
}

Engine::~Engine()
{
}

// Метод реализующий тест двигателя
float Engine::engineOverheatTest(float tempOut)
{
	float time = 0;
	float engineTemp = tempOut;
	float previousEngineTemp;
	float crankshaftSpeed = 0;
	float crankshaftAcceleration;
	float heatingSpeed;
	float coolingSpead;
	
	while (engineTemp < overheatTemp)
	{
		crankshaftAcceleration = convert(crankshaftSpeed) / inertia;  // расчет ускорения коленвала
		heatingSpeed = convert(crankshaftSpeed) * torqueHeatFactor + (crankshaftSpeed * crankshaftSpeed)   // Расчет скорости нагрева
			* rotationSpeedHeatFactor;
		coolingSpead = coolingFactor * (tempOut - engineTemp);			// Расчет скорости охлаждения
		previousEngineTemp = engineTemp;								
		engineTemp = engineTemp + (heatingSpeed * TIME_STEP) + (coolingSpead * TIME_STEP); // Расчет температуры двигателя
		crankshaftSpeed = crankshaftAcceleration * TIME_STEP + crankshaftSpeed;			// Расчет скорости коленвала
		time = time + TIME_STEP;
		if (engineTemp == previousEngineTemp) // Условие окончания теста, когда двигатель не перегревается
			return -1;
	}

	return time;
}

int main()
{
	float tempOut;					// Температура окружающей среды
	float overHeatTime;				// Время перегрева двигателя
	bool endF = false;
	Engine e1;
	while (!endF)
	{
		cout << "Please enter outside temperature(999 is exite):" << endl;
		cin >> tempOut;
		if (tempOut == 999)
		{
			endF = true;
			break;
		}
		while (cin.fail())
		{
			cout << "Please enter correct temperature." << endl;
			cin.clear();
			cin.ignore(32767, '\n');
			cin >> tempOut;
			if (tempOut == 999)
			{
				endF = true;
				break;
			}
		}
		overHeatTime = e1.engineOverheatTest(tempOut);
		if (overHeatTime == -1)
			cout << "Engine not overheating." << endl;
		else
			cout << "Engine overheating after " << overHeatTime << " seconds." << endl;
	}
}
