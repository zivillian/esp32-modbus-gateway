#ifndef CONFIG_H
    #define CONFIG_H
    #include <Arduino.h>
    #include <Preferences.h>
    #define debugSerial Serial
    #define modbusSerial Serial2
    #define DEBUG

    class Config{
        private:
            Preferences *_prefs;
            int16_t _tcpPort;
            unsigned long _baud;
            uint32_t _serialConfig;
        public:
            Config();
            void begin(Preferences *prefs);
            uint16_t getTcpPort();
            unsigned long getBaud();
            uint32_t getSerialConfig();
            void setTcpPort(uint16_t value);
            void setBaud(unsigned long value);
            uint8_t getDataBits();
            void setDataBits(uint8_t value);
            uint8_t getParity();
            void setParity(uint8_t value);
            uint8_t getStopBits();
            void setStopBits(uint8_t value);
    };
    #ifdef DEBUG
    #define dbg(x...) debugSerial.print(x);
    #define dbgln(x...) debugSerial.println(x);
    #else /* DEBUG */
    #define dbg(x...) ;
    #define dbgln(x...) ;
    #endif /* DEBUG */
#endif /* CONFIG_H */