Atmega640 OLED (1.3")
=====================

A library for using an oled 1.3 inch 128x64 with the Atmega640 , written in C.

The code was developed wnd adapted for the Atmega from these 2 projects 

https://github.com/nopnop2002/esp-idf-ssd1306 \
https://github.com/antoineleclair/avr-nrf24l01

## Build

Clean and build

```bash

$ make clean
$ make build
```

## Flash to board

```bash
# fuse setting
$ sudo avrdude -c usbtiny -p m640 -U lfuse:w:0x62:m -U hfuse:w:0x99:m -U efuse:w:0xff:m 
# flash
$ sudo avrdude -c usbtiny -p m640 -v -Uflash:w:ssd1306.hex:i 

```

## Usage (example)

```bash

int main(void) {
  // sei();
  SSD1306_t *dev;
  dev = SSD1306_init();

  _delay_ms(200);
  ssd1306_clear_screen(dev,false);

  while(true) {
    ssd1306_display_text(dev, 0, "Hello world!", 12, false);
    _delay_ms(1500);
    ssd1306_clear_screen(dev,false);
  }
  
  return 0
}
	
```
