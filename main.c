#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <string.h>
#include <util/delay.h>
#include "ssd1306.h"


// Atemega640 pin layout
// MISO     PB3
// MOSI     PB2
// SPICLK   PB1
// OLED_CS  PA0
// OLED_DC  PA1
// OLED_RES PA2

// fuse settings: -U lfuse:w:0x62:m -U hfuse:w:0x99:m -U efuse:w:0xff:m

// SSD1306_init - function to setup ports and pins
// and initialise the oled 1.3" display
SSD1306_t *SSD1306_init(void) {
  SSD1306_t *dev;
  memset(&dev,0,sizeof(dev));
  dev->res.port = &PORTA;
  dev->res.pin = PA2;
  dev->dc.port = &PORTA;
  dev->dc.pin = PA1;
  dev->cs.port = &PORTA;
  dev->cs.pin = PA0;
  dev->sck.port = &PORTA;
  dev->sck.pin = PA3;
  dev->mosi.port = &PORTA;
  dev->mosi.pin = PA4;
  dev->miso.port = &PORTA;
  dev->miso.pin = PA5;
  dev->_flip = false;
  dev->_address = SPIAddress;
  dev->_height = 64;
  dev->_width = 128;
  dev->_pages = 8;

  // spi init - port and pin assignement
  spi_init(dev);

  _delay_ms(100);

  // physically reset and setup oled
  set_high(dev->res);
  _delay_ms(50);
  set_low(dev->res);
  _delay_ms(50);
  set_high(dev->res);
  _delay_ms(50);
  set_low(dev->dc);
  set_low(dev->cs);
  device_init(dev);
  set_high(dev->cs);
  return dev;
}


int main(void) {
  // sei();
  SSD1306_t *dev;
  dev = SSD1306_init();

  _delay_ms(200);
  ssd1306_clear_screen(dev,false);

  while(true) {
    ssd1306_display_text(dev, 0, "Luigi Zuccarelli", 16, false);
	  ssd1306_display_text(dev, 1, "RedHat Inc.", 11, false);
	  ssd1306_display_text(dev, 2, "This is so cool",15, false);
	  ssd1306_display_text(dev, 3, "Ciao Bello!", 11, false);
	  ssd1306_display_text(dev, 4, "30-10-2021 17:32", 16, false);
    _delay_ms(2500);
    ssd1306_clear_screen(dev,false);
  }

  return 0;
}
