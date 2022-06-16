#include "config.h"

Config::Config()
    :_prefs(NULL)
    ,_tcpPort(502)
    ,_baud(9600)
    ,_serialConfig(SERIAL_8N1)
{}

void Config::begin(Preferences *prefs)
{
    _prefs = prefs;
    _tcpPort = _prefs->getUShort("tcpPort", _tcpPort);
    _baud = _prefs->getULong("baud", _baud);
    _serialConfig = _prefs->getULong("serialConfig", _serialConfig);
}

uint16_t Config::getTcpPort(){
    return _tcpPort;
}

unsigned long Config::getBaud(){
    return _baud;
}

uint32_t Config::getSerialConfig(){
    return _serialConfig;
}

void Config::setTcpPort(uint16_t value){
    if (_tcpPort == value) return;
    _tcpPort = value;
    _prefs->putUShort("tcpPort", _tcpPort);
}

void Config::setBaud(unsigned long value){
    if (_baud == value) return;
    _baud = value;
    _prefs->putULong("baud", _baud);
}

uint8_t Config::getDataBits(){
    return ((_serialConfig & 0xc) >> 2) + 5;
}

void Config::setDataBits(uint8_t value){
    auto dataBits = getDataBits();
    value -= 5;
    value = (value << 2) & 0xc;
    if (value == dataBits) return;
    _serialConfig = (_serialConfig & 0xfffffff3) | value;
    _prefs->putULong("serialConfig", _serialConfig);
}

uint8_t Config::getParity(){
    return _serialConfig & 0x3;
}

void Config::setParity(uint8_t value){
    auto parity = getParity();
    value = value & 0x3;
    if (parity == value) return;
    _serialConfig = (_serialConfig & 0xfffffffc) | value;
    _prefs->putULong("serialConfig", _serialConfig);
}

uint8_t Config::getStopBits(){
    return (_serialConfig & 0x30) >> 4;
}

void Config::setStopBits(uint8_t value){
    auto stopbits = getStopBits();
    value = (value << 4) & 0x30;
    if (stopbits == value) return;
    _serialConfig = (_serialConfig & 0xffffffcf) | value;
    _prefs->putULong("serialConfig", _serialConfig);
}