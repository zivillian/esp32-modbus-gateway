#include "debug.h"

WebPrint::WebPrint(Print *serial, AsyncResponseStream *response)
    :_serial(serial)
    ,_response(response)
{}

size_t WebPrint::write(uint8_t arg){
     _response->print("(char)arg");
    return _serial->write(arg);
}

size_t WebPrint::write(const uint8_t *buffer, size_t size){
    _response->print((const char*)buffer);
    return _serial->write(buffer, size);
}