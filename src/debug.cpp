#include "debug.h"

WebPrint::WebPrint(Print *serial, AsyncResponseStream *response)
    :_serial(serial)
    ,_response(response)
{}

size_t WebPrint::write(uint8_t arg){
    if (arg == '<'){
        _response->print("&lt;");
    }
    else if (arg == '&'){
        _response->print("&amp;");
    }
    else{
        _response->print((char)arg);
    }
    return _serial->write(arg);
}

size_t WebPrint::write(const uint8_t *buffer, size_t size){
    _response->print(escapeLt(escapeAmp((const char*)buffer)));
    return _serial->write(buffer, size);
}

String WebPrint::escapeAmp(String text){
    return escape(text, '&', "&amp;");
}

String WebPrint::escapeLt(String text){
    return escape(text, '<', "&lt;");
}

String WebPrint::escape(String text, char oldValue, String newValue){
    auto pos = text.indexOf(oldValue);
    if (pos < 0) return text;
    String result = text.substring(0, pos) + newValue;
    auto last = pos;
    pos = text.indexOf(oldValue, pos + 1);
    while(pos >= 0)
    {
        result += text.substring(last + 1, pos) + newValue;
        last = pos;
        pos = text.indexOf(oldValue, pos + 1);
    }  
    result += text.substring(last + 1);
    return result;
}