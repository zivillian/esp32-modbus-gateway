#include "config.h"

Config::Config()
    :_prefs(NULL)
    ,_tcpPort(502)
    ,_modbusBaudRate(9600)
    ,_modbusSerialConfig(SERIAL_8N1)
{}

void Config::begin(Preferences *prefs)
{
    _prefs = prefs;
    _tcpPort = _prefs->getUShort("tcpPort", _tcpPort);
    _modbusBaudRate = _prefs->getULong("modbusBaudRate", _modbusBaudRate);
    _modbusSerialConfig = _prefs->getULong("modbusSerialConfig", _modbusSerialConfig);
}

uint16_t Config::getTcpPort(){
    return _tcpPort;
}

void Config::setTcpPort(uint16_t value){
    if (_tcpPort == value) return;
    _tcpPort = value;
    _prefs->putUShort("tcpPort", _tcpPort);
}

uint32_t Config::getModbusSerialConfig(){
    return _modbusSerialConfig;
}

unsigned long Config::getModbusBaudRate(){
    return _modbusBaudRate;
}

void Config::setModbusBaudRate(unsigned long value){
    if (_modbusBaudRate == value) return;
    _modbusBaudRate = value;
    _prefs->putULong("modbusBaudRate", _modbusBaudRate);
}

uint8_t Config::getModbusDataBits(){
    return ((_modbusSerialConfig & 0xc) >> 2) + 5;
}

void Config::setModbusDataBits(uint8_t value){
    auto dataBits = getModbusDataBits();
    value -= 5;
    value = (value << 2) & 0xc;
    if (value == dataBits) return;
    _modbusSerialConfig = (_modbusSerialConfig & 0xfffffff3) | value;
    _prefs->putULong("modbusSerialConfig", _modbusSerialConfig);
}

uint8_t Config::getModbusParity(){
    return _modbusSerialConfig & 0x3;
}

void Config::setModbusParity(uint8_t value){
    auto parity = getModbusParity();
    value = value & 0x3;
    if (parity == value) return;
    _modbusSerialConfig = (_modbusSerialConfig & 0xfffffffc) | value;
    _prefs->putULong("modbusSerialConfig", _modbusSerialConfig);
}

uint8_t Config::getModbusStopBits(){
    return (_modbusSerialConfig & 0x30) >> 4;
}

void Config::setModbusStopBits(uint8_t value){
    auto stopbits = getModbusStopBits();
    value = (value << 4) & 0x30;
    if (stopbits == value) return;
    _modbusSerialConfig = (_modbusSerialConfig & 0xffffffcf) | value;
    _prefs->putULong("modbusSerialConfig", _modbusSerialConfig);
}