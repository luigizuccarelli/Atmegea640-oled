#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "ssd1306.h"


void spi_init(SSD1306_t *dev) {
  set_as_output(dev->sck);
  set_as_output(dev->mosi);
  set_as_output(dev->res);
  set_as_output(dev->dc);
  set_as_output(dev->cs);
  set_high(dev->cs);
  set_high(dev->res);
  set_low(dev->dc);
}

void spi_display_text(SSD1306_t * dev, int page, int seg, uint8_t data[8]) {
  int _seg = seg+2;
  uint8_t columLow = _seg & 0x0F;
  uint8_t columHigh = (_seg >> 4) & 0x0F;
  set_low(dev->cs);
  // enable oled display
  // Set Lower Column Start Address for Page Addressing Mode
  spi_master_write_command(dev, (0x00 + columLow));
  // Set Higher Column Start Address for Page Addressing Mode
  spi_master_write_command(dev, (0x10 + columHigh));
  // Set Page Start Address for Page Addressing Mode
  spi_master_write_command(dev, 0xB0 | page);

  int x;
  for (x=0; x<8; x++) {
    spi_master_write_data(dev, data[x]);
  }
  // set cs high - disable oled
  set_high(dev->cs);
}


uint8_t spi_master_write_command(SSD1306_t * dev, uint8_t cmd) {
  set_low(dev->dc);
  spi_transfer(dev,cmd);
  return 0;
}

uint8_t spi_master_write_data(SSD1306_t * dev, uint8_t data) {
  set_high(dev->dc);
  spi_transfer(dev,data);
  return 0;
}

void spi_transfer(SSD1306_t * dev, uint8_t data) {
  unsigned char mask = 0x80;
  while (mask != 0) {
    set_low(dev->sck);
    if ((mask & data) == mask) {
      set_high(dev->mosi);
    } else {
      set_low(dev->mosi);
    }
    set_high(dev->sck);
    set_low(dev->sck);
    mask = mask >> 1;
  }
}


void set_as_output(gpio_pin pin) {
  volatile uint8_t *ddr = pin.port - 1;
  *ddr |= _BV(pin.pin);
}

void set_as_input(gpio_pin pin) {
  volatile uint8_t *ddr = pin.port - 1;
  *ddr &= ~_BV(pin.pin);
}

void set_high(gpio_pin pin) {
  *pin.port |= _BV(pin.pin);
}

void set_low(gpio_pin pin) {
  *pin.port &= ~_BV(pin.pin);
}


void device_init(SSD1306_t * dev) {
  dev->_pages = 8;
  if (dev->_height == 32) dev->_pages = 4;

  set_low(dev->cs);
  spi_master_write_command(dev, OLED_CMD_DISPLAY_OFF);
  spi_master_write_command(dev, OLED_CMD_SET_MUX_RATIO);
  spi_master_write_command(dev, 0x3F);
  spi_master_write_command(dev, OLED_CMD_SET_DISPLAY_OFFSET);
  spi_master_write_command(dev, 0x00);
  spi_master_write_command(dev, OLED_CONTROL_BYTE_DATA_STREAM);
  if (dev->_flip) {
    spi_master_write_command(dev, OLED_CMD_SET_SEGMENT_REMAP_0);
  } else {
    spi_master_write_command(dev, OLED_CMD_SET_SEGMENT_REMAP_1);
  }
  spi_master_write_command(dev, OLED_CMD_SET_COM_SCAN_MODE);
  spi_master_write_command(dev, OLED_CMD_SET_DISPLAY_CLK_DIV);
  spi_master_write_command(dev, 0x80);
  spi_master_write_command(dev, OLED_CMD_SET_COM_PIN_MAP);
  spi_master_write_command(dev, 0x12);
  spi_master_write_command(dev, OLED_CMD_SET_CONTRAST);
  spi_master_write_command(dev, 0xFF);
  spi_master_write_command(dev, OLED_CMD_DISPLAY_RAM);
  spi_master_write_command(dev, OLED_CMD_SET_VCOMH_DESELCT);
  spi_master_write_command(dev, 0x40);
  //spi_master_write_command(dev, OLED_CMD_SET_MEMORY_ADDR_MODE);
  spi_master_write_command(dev, OLED_CMD_SET_HORI_ADDR_MODE);
  spi_master_write_command(dev, OLED_CMD_SET_PAGE_ADDR_MODE);
  // Set Lower Column Start Address for Page Addressing Mode
  spi_master_write_command(dev, 0x00);
  // Set Higher Column Start Address for Page Addressing Mode
  spi_master_write_command(dev, 0x10);
  spi_master_write_command(dev, OLED_CMD_SET_CHARGE_PUMP);
  spi_master_write_command(dev, 0x14);
  spi_master_write_command(dev, OLED_CMD_DEACTIVE_SCROLL);
  spi_master_write_command(dev, OLED_CMD_DISPLAY_NORMAL);
  spi_master_write_command(dev, OLED_CMD_DISPLAY_ON);
  set_high(dev->cs);
}


void spi_contrast(SSD1306_t * dev, int contrast) {
  int _contrast = contrast;
  if (contrast < 0x0) _contrast = 0;
  if (contrast > 0xFF) _contrast = 0xFF;

  spi_master_write_command(dev, OLED_CMD_SET_CONTRAST);
  spi_master_write_command(dev, _contrast);
}

void spi_hardware_scroll(SSD1306_t * dev, ssd1306_scroll_type_t scroll)
{

  if (scroll == SCROLL_RIGHT) {
    spi_master_write_command(dev, OLED_CMD_HORIZONTAL_RIGHT);
    spi_master_write_command(dev, 0x00); // Dummy byte
    spi_master_write_command(dev, 0x00); // Define start page address
    spi_master_write_command(dev, 0x07); // Frame frequency
    spi_master_write_command(dev, 0x07); // Define end page address
    spi_master_write_command(dev, 0x00); //
    spi_master_write_command(dev, 0xFF); //
    spi_master_write_command(dev, OLED_CMD_ACTIVE_SCROLL);
  }

  if (scroll == SCROLL_LEFT) {
    spi_master_write_command(dev, OLED_CMD_HORIZONTAL_LEFT);
    spi_master_write_command(dev, 0x00); // Dummy byte
    spi_master_write_command(dev, 0x00); // Define start page address
    spi_master_write_command(dev, 0x07); // Frame frequency
    spi_master_write_command(dev, 0x07); // Define end page address
    spi_master_write_command(dev, 0x00); //
    spi_master_write_command(dev, 0xFF); //
    spi_master_write_command(dev, OLED_CMD_ACTIVE_SCROLL);
  }

  if (scroll == SCROLL_DOWN) {
    spi_master_write_command(dev, OLED_CMD_CONTINUOUS_SCROLL);
    spi_master_write_command(dev, 0x00); // Dummy byte
    spi_master_write_command(dev, 0x00); // Define start page address
    spi_master_write_command(dev, 0x07); // Frame frequency
    //spi_master_write_command(dev, 0x01); // Define end page address
    spi_master_write_command(dev, 0x00); // Define end page address
    spi_master_write_command(dev, 0x3F); // Vertical scrolling offset

    spi_master_write_command(dev, OLED_CMD_VERTICAL);
    spi_master_write_command(dev, 0x00);
    if (dev->_height == 64)
      spi_master_write_command(dev, 0x40);
    if (dev->_height == 32)
      spi_master_write_command(dev, 0x20);
    spi_master_write_command(dev, OLED_CMD_ACTIVE_SCROLL);
  }

  if (scroll == SCROLL_UP) {
    spi_master_write_command(dev, OLED_CMD_CONTINUOUS_SCROLL);
    spi_master_write_command(dev, 0x00); // Dummy byte
    spi_master_write_command(dev, 0x00); // Define start page address
    spi_master_write_command(dev, 0x07); // Frame frequency
    //spi_master_write_command(dev, 0x01); // Define end page address
    spi_master_write_command(dev, 0x00); // Define end page address
    spi_master_write_command(dev, 0x01); // Vertical scrolling offset

    spi_master_write_command(dev, OLED_CMD_VERTICAL);
    spi_master_write_command(dev, 0x00);
    if (dev->_height == 64)
      spi_master_write_command(dev, 0x40);
    if (dev->_height == 32)
      spi_master_write_command(dev, 0x20);
    spi_master_write_command(dev, OLED_CMD_ACTIVE_SCROLL);
  }

  if (scroll == SCROLL_STOP) {
    spi_master_write_command(dev, OLED_CMD_DEACTIVE_SCROLL);
  }
}
