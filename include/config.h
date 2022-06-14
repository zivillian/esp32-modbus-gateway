#ifndef CONFIG_H
    #define CONFIG_H
    #include <Arduino.h>
    #define debugSerial Serial
    #define modbusSerial Serial2
    #define DEBUG

    typedef struct {
        unsigned int tcpPort;
        bool enableRtuOverTcp;
        unsigned long baud;
        unsigned long serialConfig;
    } config_type;

    const config_type default_config = {
        502,
        false,
        19200,
        SERIAL_8E1
    };
    #ifdef DEBUG
    #define dbg(x...) debugSerial.print(x);
    #define dbgln(x...) debugSerial.println(x);
    #else /* DEBUG */
    #define dbg(x...) ;
    #define dbgln(x...) ;
    #endif /* DEBUG */
#endif /* CONFIG_H */