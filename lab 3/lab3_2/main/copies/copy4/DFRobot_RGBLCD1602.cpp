#include "DFRobot_RGBLCD1602.h"

// Constructor
DFRobot_RGBLCD1602::DFRobot_RGBLCD1602(i2c_port_t i2c_port, uint8_t lcd_addr, uint8_t rgb_addr) {
    _i2c_port = i2c_port;
    _lcdAddr = lcd_addr;
    _rgbAddr = rgb_addr;
}

void DFRobot_RGBLCD1602::init() {
    _showFunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
    begin(2); // Initialize with 2 rows
}

void DFRobot_RGBLCD1602::begin(uint8_t rows, uint8_t charsize) {
    if (rows > 1) {
        _showFunction |= LCD_2LINE;
    }
    _numLines = rows;
    _currLine = 0;
    
    // Wait for power-up
    vTaskDelay(pdMS_TO_TICKS(50));
    
    // Initialize display
    command(LCD_FUNCTIONSET | _showFunction);
    vTaskDelay(pdMS_TO_TICKS(5));
    command(LCD_FUNCTIONSET | _showFunction);
    vTaskDelay(pdMS_TO_TICKS(5));
    command(LCD_FUNCTIONSET | _showFunction);
    
    // Turn on display, no cursor or blink
    _showControl = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    display();
    
    // Clear display
    clear();
    
    // Initialize entry mode
    _showMode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    command(LCD_ENTRYMODESET | _showMode);
    
    // Initialize backlight
    setRGB(255, 255, 255);
}

void DFRobot_RGBLCD1602::clear() {
    command(LCD_CLEARDISPLAY);
    vTaskDelay(pdMS_TO_TICKS(2));
}

void DFRobot_RGBLCD1602::home() {
    command(LCD_RETURNHOME);
    vTaskDelay(pdMS_TO_TICKS(2));
}

void DFRobot_RGBLCD1602::setCursor(uint8_t col, uint8_t row) {
    int row_offsets[] = { 0x00, 0x40 };
    if (row >= _numLines) row = _numLines - 1;
    command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

void DFRobot_RGBLCD1602::noDisplay() {
    _showControl &= ~LCD_DISPLAYON;
    command(LCD_DISPLAYCONTROL | _showControl);
}

void DFRobot_RGBLCD1602::display() {
    _showControl |= LCD_DISPLAYON;
    command(LCD_DISPLAYCONTROL | _showControl);
}

void DFRobot_RGBLCD1602::setRGB(uint8_t r, uint8_t g, uint8_t b) {
    setReg(0x04, r);
    setReg(0x03, g);
    setReg(0x02, b);
}

void DFRobot_RGBLCD1602::printstr(const char* str) {
    while(*str) {
        write(*str++);
    }
}

void DFRobot_RGBLCD1602::command(uint8_t value) {
    send(value, 0);
}

void DFRobot_RGBLCD1602::write(uint8_t value) {
    send(value, 1);
}

void DFRobot_RGBLCD1602::send(uint8_t value, uint8_t mode) {
    uint8_t data[2];
    data[0] = mode ? 0x40 : 0x80;
    data[1] = value;
    i2c_write_bytes(_lcdAddr, data, 2);
    vTaskDelay(pdMS_TO_TICKS(1));
}

void DFRobot_RGBLCD1602::setReg(uint8_t addr, uint8_t data) {
    uint8_t buf[2] = {addr, data};
    i2c_write_bytes(_rgbAddr, buf, 2);
}

esp_err_t DFRobot_RGBLCD1602::i2c_write_bytes(uint8_t addr, uint8_t* data, size_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(cmd, data, len, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(_i2c_port, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
    return ret;
}
