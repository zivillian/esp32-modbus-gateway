#include "config.h"

Config::Config()
    :_prefs(NULL)
    ,_tcpPort(502)
    ,_tcpTimeout(10000)
    ,_modbusBaudRate(9600)
    ,_modbusConfig(SERIAL_8N1)
    ,_modbusRtsPin(-1)
    ,_serialBaudRate(115200)
    ,_serialConfig(SERIAL_8N1)
    ,_webPassword("")
{}

void Config::begin(Preferences *prefs)
{
    _prefs = prefs;
    _tcpPort = _prefs->getUShort("tcpPort", _tcpPort);
    _tcpTimeout = _prefs->getULong("tcpTimeout", _tcpTimeout);
    _modbusBaudRate = _prefs->getULong("modbusBaudRate", _modbusBaudRate);
    _modbusConfig = _prefs->getULong("modbusConfig", _modbusConfig);
    _modbusRtsPin = _prefs->getChar("modbusRtsPin", _modbusRtsPin);
    _serialBaudRate = _prefs->getULong("serialBaudRate", _serialBaudRate);
    _serialConfig = _prefs->getULong("serialConfig", _serialConfig);
    _webPassword = _prefs->getString("webPassword", _webPassword);
}

uint16_t Config::getTcpPort(){
    return _tcpPort;
}

void Config::setTcpPort(uint16_t value){
    if (_tcpPort == value) return;
    _tcpPort = value;
    _prefs->putUShort("tcpPort", _tcpPort);
}

uint32_t Config::getTcpTimeout(){
    return _tcpTimeout;
}

void Config::setTcpTimeout(uint32_t value){
    if (_tcpTimeout == value) return;
    _tcpTimeout = value;
    _prefs->putULong("tcpTimeout", _tcpTimeout);
}

uint32_t Config::getModbusConfig(){
    return _modbusConfig;
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
    return ((_modbusConfig & 0xc) >> 2) + 5;
}

void Config::setModbusDataBits(uint8_t value){
    auto dataBits = getModbusDataBits();
    value -= 5;
    value = (value << 2) & 0xc;
    if (value == dataBits) return;
    _modbusConfig = (_modbusConfig & 0xfffffff3) | value;
    _prefs->putULong("modbusConfig", _modbusConfig);
}

uint8_t Config::getModbusParity(){
    return _modbusConfig & 0x3;
}

void Config::setModbusParity(uint8_t value){
    auto parity = getModbusParity();
    value = value & 0x3;
    if (parity == value) return;
    _modbusConfig = (_modbusConfig & 0xfffffffc) | value;
    _prefs->putULong("modbusConfig", _modbusConfig);
}

uint8_t Config::getModbusStopBits(){
    return (_modbusConfig & 0x30) >> 4;
}

void Config::setModbusStopBits(uint8_t value){
    auto stopbits = getModbusStopBits();
    value = (value << 4) & 0x30;
    if (stopbits == value) return;
    _modbusConfig = (_modbusConfig & 0xffffffcf) | value;
    _prefs->putULong("modbusConfig", _modbusConfig);
}

int8_t Config::getModbusRtsPin(){
    return _modbusRtsPin;
}

void Config::setModbusRtsPin(int8_t value){
    if (_modbusRtsPin == value) return;
    _modbusRtsPin = value;
    _prefs->putChar("modbusRtsPin", _modbusRtsPin);
}

uint32_t Config::getSerialConfig(){
    return _serialConfig;
}

unsigned long Config::getSerialBaudRate(){
    return _serialBaudRate;
}

void Config::setSerialBaudRate(unsigned long value){
    if (_serialBaudRate == value) return;
    _serialBaudRate = value;
    _prefs->putULong("serialBaudRate", _serialBaudRate);
}

uint8_t Config::getSerialDataBits(){
    return ((_serialConfig & 0xc) >> 2) + 5;
}

void Config::setSerialDataBits(uint8_t value){
    auto dataBits = getSerialDataBits();
    value -= 5;
    value = (value << 2) & 0xc;
    if (value == dataBits) return;
    _serialConfig = (_serialConfig & 0xfffffff3) | value;
    _prefs->putULong("serialConfig", _serialConfig);
}

uint8_t Config::getSerialParity(){
    return _serialConfig & 0x3;
}

void Config::setSerialParity(uint8_t value){
    auto parity = getSerialParity();
    value = value & 0x3;
    if (parity == value) return;
    _serialConfig = (_serialConfig & 0xfffffffc) | value;
    _prefs->putULong("serialConfig", _serialConfig);
}

uint8_t Config::getSerialStopBits(){
    return (_serialConfig & 0x30) >> 4;
}

void Config::setSerialStopBits(uint8_t value){
    auto stopbits = getSerialStopBits();
    value = (value << 4) & 0x30;
    if (stopbits == value) return;
    _serialConfig = (_serialConfig & 0xffffffcf) | value;
    _prefs->putULong("serialConfig", _serialConfig);
}


String Config::getWebPassword(){
    return _webPassword;
}

void Config::setWebPassword(String value){
    auto webpass = getWebPassword();
    if (webpass == value) return;
    _webPassword = value;
    _prefs->putString("webPassword", _webPassword);
}
