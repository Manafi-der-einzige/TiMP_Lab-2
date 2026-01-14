// Lab2_TiMP.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include <windows.h>
#include <iostream>
#include <winreg.h>
#include <fstream>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#define REG_TIME true
#define REG_ITER false
#define DEMO_TIME_LIMIT_SEC 180
#define DEMO_ITER_LIMIT 5

const static char* regname_iter = "DemoIter";
const static char* regname_time = "DemoTime";
const static char* file_name = "list.txt";
const wchar_t* kLocation = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";

using namespace std;

std::atomic<int> g_TotalTimeFromRegistry{ 0 }; // время, прочитанное из реестра при старте
std::atomic<int> g_SessionSeconds{ 0 };        // время текущего сеанса
std::atomic<bool> g_DemoExpired{ false };

void SetRegistry(bool reg_type) {
    HKEY hKey;
    RegOpenKeyExW(HKEY_CURRENT_USER, kLocation, 0, KEY_READ, &hKey);
    DWORD data = 0;
    DWORD bufferSize = sizeof(DWORD);
    if (reg_type == REG_TIME) {
        data = g_SessionSeconds.load() + g_TotalTimeFromRegistry.load();
        RegCloseKey(hKey);
        RegOpenKeyExW(HKEY_CURRENT_USER, kLocation, 0, KEY_SET_VALUE, &hKey);
        RegSetValueExA(hKey, regname_time, 0, REG_DWORD, reinterpret_cast<const BYTE*>(&data), sizeof(data));
        RegCloseKey(hKey);
    }
    else {
        if (RegQueryValueExA(hKey, regname_iter, nullptr, NULL, reinterpret_cast<LPBYTE>(&data), &bufferSize) != ERROR_SUCCESS) {
            data = 1;
        }
        else {
            data += 1;
        }
        RegCloseKey(hKey);
        RegOpenKeyExW(HKEY_CURRENT_USER, kLocation, 0, KEY_SET_VALUE, &hKey);
        RegSetValueExA(hKey, regname_iter, 0, REG_DWORD, reinterpret_cast<const BYTE*>(&data), sizeof(data));
        RegCloseKey(hKey);
    }
}

// Фоновый мониторинг: считает время и проверяет лимит КАЖДУЮ секунду
void MonitoringThread() {
    while (!g_DemoExpired.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        int session = g_SessionSeconds.fetch_add(1) + 1;
        int total = g_TotalTimeFromRegistry.load() + session;

        if (total > DEMO_TIME_LIMIT_SEC) {
            g_DemoExpired = true;
            cout << "\n\nDemo version of product is ended, please, buy licence to continue\n";
            SetRegistry(REG_TIME);
            exit(1); // МГНОВЕННОЕ завершение
        }
    }
}


void checkDemo() {
    HKEY hKey;
    RegOpenKeyExW(HKEY_CURRENT_USER, kLocation, 0, KEY_READ, &hKey);
    DWORD time_data = 0;
    DWORD iter_data = 0;
    DWORD bufferSize = sizeof(DWORD);
    RegQueryValueExA(hKey, regname_time, nullptr, NULL, reinterpret_cast<LPBYTE>(&time_data), &bufferSize);
    RegQueryValueExA(hKey, regname_iter, nullptr, NULL, reinterpret_cast<LPBYTE>(&iter_data), &bufferSize);
    if (time_data > DEMO_TIME_LIMIT_SEC || iter_data > DEMO_ITER_LIMIT) {
        cout << "Demo version of product is ended, please, buy licence to continue" << endl;
        Sleep(INFINITE);
    }
}

int main() {
    thread monitor(MonitoringThread);
    string fullName;
    checkDemo();
    fstream outFile(file_name, ios::in | ios::out | ios::app);
    if (outFile.is_open()) {
        cout << "Write your full name: ";
        cin >> fullName;
        string line;
        while (getline(outFile, line)) {
            if (line == fullName) {
                cout << "You're on the list!" << endl;
                break;
            }
        }
        if (outFile.eof()) {
            cout << "You're not on the list, so we'll write your name down" << endl;
            outFile.clear();
            outFile.seekg(0);
            outFile << fullName << endl;
        }
        SetRegistry(REG_ITER);
    }
    else {
        cerr << "Error while opening file" << endl;
    }
    cout << "This is demo version of app, you're limited by 3 minutes or 5 launches" << endl;

    g_DemoExpired = true;
    if (monitor.joinable()) monitor.join();

    SetRegistry(REG_TIME);
    Sleep(INFINITE);
}