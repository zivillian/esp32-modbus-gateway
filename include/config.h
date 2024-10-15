#ifndef CONFIG_H
    #define CONFIG_H
    #include <Arduino.h>
    #include <Preferences.h>
    #if !defined(DEBUG_SERIAL)
        #define DEBUG_SERIAL Serial
    #endif
    #if !defined(MODBUS_SERIAL)
        #define MODBUS_SERIAL Serial2
    #endif
    #if !defined(MODBUS_MAX_ADDRESS)
        #define MODBUS_MAX_ADDRESS 247
    #endif

    class Config{
        private:
            Preferences *_prefs;
            int16_t _tcpPort;
            uint32_t _tcpTimeout;
            unsigned long _modbusBaudRate;
            uint32_t _modbusConfig;
            int8_t _modbusRtsPin;
            uint32_t _rtuTimeout;
            unsigned long _serialBaudRate;
            uint32_t _serialConfig;
        public:
            Config();
            void begin(Preferences *prefs);
            uint16_t getTcpPort();
            void setTcpPort(uint16_t value);
            uint32_t getTcpTimeout();
            void setTcpTimeout(uint32_t value);
            uint32_t getModbusConfig();
            unsigned long getModbusBaudRate();
            void setModbusBaudRate(unsigned long value);
            uint8_t getModbusDataBits();
            void setModbusDataBits(uint8_t value);
            uint8_t getModbusParity();
            void setModbusParity(uint8_t value);
            uint8_t getModbusStopBits();
            void setModbusStopBits(uint8_t value);
            int8_t getModbusRtsPin();
            void setModbusRtsPin(int8_t value);
            uint32_t getRtuTimeout();
            void setRtuTimeout(uint32_t value);
            uint32_t getSerialConfig();
            unsigned long getSerialBaudRate();
            void setSerialBaudRate(unsigned long value);
            uint8_t getSerialDataBits();
            void setSerialDataBits(uint8_t value);
            uint8_t getSerialParity();
            void setSerialParity(uint8_t value);
            uint8_t getSerialStopBits();
            void setSerialStopBits(uint8_t value);
    };
#endif /* CONFIG_H */