#ifndef PROJECTWEATHER_WEATHER_H
#define PROJECTWEATHER_WEATHER_H

#include <iostream>
#include <sstream>
#include <fstream>
#include "httplib.h"
#include <vector>
#include <iomanip>
#include <algorithm>
#include <regex>
#include <string>
#include <locale>
#include <codecvt>

using namespace std;

struct ApiKeys {
    std::string weatherApiKey;
    std::string ipstackApiKey;
};

struct WeatherInfo {
    string city;
    double temperature;
    string description;
    double minTemperature;
    double maxTemperature;
    int humidity;
    double windSpeed;
};

///convert degrees/units
int kelvinToFahrenheit(const double kelvin);
int kelvinToCelsius(const double kelvin);
int mpsToKph(const double speedInMps);
int mpsToMilesPH(const double speedInMps);

/// extract a value from a JSON object
string extractFromJson(const string& json, const string& key);

///using API to get weather data
WeatherInfo getWeather(const string& city,const ApiKeys& keys);

///using API to get current city from computer IP
string getCityFromIP(const ApiKeys& keys);

///ask the user if wants to use current location and operate accordingly
string getCityFromUser(const ApiKeys& keys);

///ask the user in what units of measurement display the temperature
int getDegrees();

///ask the user in what units of measurement display the temperature
int getWindUnits();

///prints all the data
void printData(const WeatherInfo weatherData, const string city, const int degreesChoice, const int windChoice);

///start the run of the program
int startRun(WeatherInfo& weatherData);

///read API keys from configuration file
ApiKeys readConfig(const std::string& configFile);


#endif //PROJECTWEATHER_WEATHER_H
