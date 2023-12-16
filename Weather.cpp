#include "Weather.h"

int kelvinToFahrenheit(const double kelvin) {
    return round(((kelvin - 273.15) * 9.0 / 5.0 + 32.0));
}

int kelvinToCelsius(const double kelvin) {
    return round(kelvin - 273.15);
}

int mpsToKph(const double speedInMps){
    return round(speedInMps * 3.6);
}

int mpsToMilesPH(const double speedInMps){
    return round(speedInMps * 2.23694);
}


string extractFromJson(const string& json, const string& key) {
    string stringToSearch = "\"" + key + "\":";
    size_t startPosition = json.find(stringToSearch);
    if (startPosition != string::npos) {
        startPosition += stringToSearch.length();
        size_t endPosition = json.find_first_of(",}", startPosition);
        if (endPosition != string::npos) {
            return json.substr(startPosition, endPosition - startPosition);
        }
    }
    return ""; // Return an empty string if the key is not found
}

WeatherInfo getWeather(const string& city, const ApiKeys& keys){
    httplib::Client client("http://api.openweathermap.org");
    auto result = client.Get("/data/2.5/weather?q=" + city + "&appid=" + keys.weatherApiKey);
    WeatherInfo myWeather;

    if (result) {
        if (result->status == 200) {
            string json_data = result->body;
            myWeather.city = extractFromJson(json_data, "name");
            myWeather.temperature = stod(extractFromJson(json_data, "temp"));
            myWeather.minTemperature = stod(extractFromJson(json_data, "temp_min"));
            myWeather.maxTemperature = stod(extractFromJson(json_data, "temp_max"));
            myWeather.description = extractFromJson(json_data, "description");
            myWeather.humidity = stod(extractFromJson(json_data, "humidity"));
            myWeather.windSpeed = stod(extractFromJson(json_data, "speed"));
        } else {
            cerr << "Error: " << result->status << " - " << result->reason << endl;
        }
    } else {
        cerr << "Failed to fetch weather data. Check your internet connection." << endl;
    }

    return myWeather;
}

string getCityFromIP(const ApiKeys& keys) {
    httplib::Client client("http://api.ipstack.com");
    auto result = client.Get("/api/check?access_key=" + string(keys.ipstackApiKey));
    if (result) {
        if (result->status == 200) {
            string json_data = result->body;
            string city = extractFromJson(json_data, "city");

            // Remove leading and trailing spaces and quotes
            city.erase(remove(city.begin(), city.end(), '\"'), city.end());
            city = std::regex_replace(city, std::regex("^ +| +$|( ) +"), "$1");
            if (!city.empty()) {
                cout << "Detected location: " << city << endl;
                return city;
            } else {
                cerr << "Error: Unable to extract city from JSON response.\n";
            }
        } else {
            cerr << "Error: " << result->status << " - " << result->reason << endl;
        }
    } else {
        cerr << "Failed to fetch location data. Check your internet connection.\n";
    }

    return ""; // Return an empty string if location detection fails
}

string getCityFromUser(const ApiKeys& keys){
    char choice;
    string city;
    cout << "Do you want to use the current location? (y/n):" << endl;
    cin >> choice;
    if(tolower(choice) != 'y' && tolower(choice) != 'n'){
        throw std::runtime_error("Invalid Choice");
    }

    if (tolower(choice) == 'y') {
        cout << "Detecting your location...\n";
        city = getCityFromIP(keys);
        if (city.empty()) {
            cout << "Enter city name:" << endl;
            getline(cin, city);
        }
    }else{
        cout << "Enter city name:" << endl;
        cin >> city;
    }
    return city;
}

int getDegrees(){
    int degreesChoice;
    cout << "Temperature in Celsius(0), Fahrenheit(1), Kelvin(2):" << endl;
    cin >> degreesChoice;
    if (degreesChoice != 0 && degreesChoice != 1 && degreesChoice != 2) {
        throw std::runtime_error("Invalid Choice Of Degrees");
    }
    return degreesChoice;
}

int getWindUnits(){
    int userChoice;
    cout << "Wind speed units - Kilometers per hour(0) or Miles per hour(1)?:" << endl;
    cin >> userChoice;
    if(userChoice != 0 && userChoice != 1){
        throw std::runtime_error("Invalid Choice Of Units of measurements");
    }
    return userChoice;
}


void printData(const WeatherInfo weatherData, const string city, const int degreesChoice, const int windChoice){
    cout << "Weather in " << weatherData.city << ":" << endl;
    if (degreesChoice == 0) {
        cout << "Average temperature: " << kelvinToCelsius(weatherData.temperature) << " Celsius" << endl;
        cout << "The minimum temperature is: " << kelvinToCelsius(weatherData.minTemperature) << " Celsius" << endl;
        cout << "The maximum temperature is: " << kelvinToCelsius(weatherData.maxTemperature) << " Celsius" << endl;
    } else if (degreesChoice == 1) {
        cout << "Average temperature: " << kelvinToFahrenheit(weatherData.temperature) << " Fahrenheit" << endl;
        cout << "The minimum temperature is: " << kelvinToFahrenheit(weatherData.minTemperature) << " Fahrenheit" << endl;
        cout << "The maximum temperature is: " << kelvinToFahrenheit(weatherData.maxTemperature) << " Fahrenheit" << endl;
    } else {
        cout << "Average temperature: " << weatherData.temperature << " Kelvin" << endl;
        cout << "The minimum temperature is: " << weatherData.minTemperature << " Kelvin" << endl;
        cout << "The maximum temperature is: " << weatherData.maxTemperature << " Kelvin" << endl;
    }
    cout << "Description: " << weatherData.description << endl;
    cout << "Humidity: " << weatherData.humidity << "%" << endl;

    if(!windChoice){
        cout << "Wind speed: " << mpsToKph(weatherData.windSpeed) << " Km/h" << endl;
    }else {
        cout << "Wind speed: " << mpsToMilesPH(weatherData.windSpeed) << " MPH" << endl;
    }
}

int startRun(WeatherInfo& weatherData){
    int degreesChoice, windSpeedUnits;
    try{
        ApiKeys keys = readConfig("config.txt");
        weatherData = getWeather(getCityFromUser(keys), keys);
        degreesChoice = getDegrees();
        windSpeedUnits = getWindUnits();
    }catch(const std::exception& e){
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return 0;
    }
    printData(weatherData,weatherData.city,degreesChoice,windSpeedUnits);
    return 0;
}

ApiKeys readConfig(const string& configFile) {
    ApiKeys keys;
    ifstream file(configFile);
    if (!file.is_open()) {
        cerr << "Error opening file: " << configFile << endl;
        return keys;
    }

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string key, value;

        if (getline(iss, key, '=') && getline(iss, value)) {
            if (key == "WEATHER_API_KEY") {
                keys.weatherApiKey = value;
            } else if (key == "IPSTACK_API_KEY") {
                keys.ipstackApiKey = value;
            }
        }
    }
    file.close();
    return keys;
}
