//WiFi
char ssid[] = "FRITZ!Box 6591 Cable CW";
char password[] = "09806196161046264509";

//Volumio
char host[] = "volumio";
int port = 80;

//Touch-pins
#define PIN_TOUCH0 4
#define PIN_TOUCH1 0
#define PIN_TOUCH2 2
#define PIN_TOUCH3 15
#define PIN_TOUCH4 13
#define PIN_TOUCH5 12
#define PIN_TOUCH6 14
#define PIN_TOUCH7 27 //used by left encoder
#define PIN_TOUCH8 33 //used by right encoder
#define PIN_TOUCH9 32 //used by richt encoder

//Display-pins
#define PIN_SPI_MOSI 23 //VSPID    DIN - 3
#define PIN_SPI_CLK 18  //VSPICLK  CLK - 4
#define PIN_SPI_CS 5    //VSPICS0  CS  - 5
#define PIN_SPI_DC 21   //         DC  - 6

//Left encoder
#define PIN_LeftEncoder_SW 27 //blocks one touch pin
#define PIN_LeftEncoder_DT 25
#define PIN_LeftEncoder_CLK 26

//Right encoder
#define PIN_RightEncoder_SW 33 //blocks one touch pin
#define PIN_RightEncoder_DT 32 //blocks one touch pin
#define PIN_RightEncoder_CLK 35 //not good, has no pullup!

//Min and Max possible volume on my System, standard would be 0-100
int VolumeMinimum = 0;
int VolumeMaximum = 40;
int VolumeSetDelay = 300; //ms
int VolumeDuration = 2000; //ms

//How long toest messages are displayed
int ToastDuration = 5000; //ms

//scroll one pixel each x ms
int ScrollInterval = 10; //ms

//Gap between end and start of scrolling text
u8g2_uint_t scrollGap = 32;//px

//After x ms without operation go back to status display
long delayBackStatus = 10* 1000;//ms

//After x ms without operation switch display off, if not play
long delayDisplayOffWhenNotPlay = 60 * 1000; //ms

//After x ms without operation switch display off
long delayDisplayOff = 0; //ms


//###########################################################
// Display
//###########################################################

#define DisplayWidth 128
#define DisplayHeight 128

/*
https://github.com/olikraus/u8g2/blob/master/doc/faq.txt#L159
Q: How to activate 16 Bit mode?
A: Search for the line
//#define U8G2_16BIT
in "u8g2.h". Uncomment this line:
#define U8G2_16BIT
The file "u8g2.h" is located in "/libraries/U8g2_Arduino/src/clib" inside your default
sketch folder.
*/

U8G2_SSD1327_WS_128X128_1_4W_HW_SPI display(U8G2_R0,PIN_SPI_CS,PIN_SPI_DC,U8X8_PIN_NONE);



//###########################################################
// Menu
//###########################################################

#define MenuItemHeight 16
#define MenuTextHeight 10
#define MenuVisibleItems 8
#define MenuPixelHeight 128
#define MenuPixelWidth 128


  //https://github.com/olikraus/u8g2/wiki/fntlistall#u8g2-font-names
#define MenuTextFont u8g2_font_t0_12_tf
#define StatusTextFont u8g2_font_t0_12_tf
#define ToastTextFont u8g2_font_t0_12_tf

#define MenuIconFont u8g2_font_open_iconic_all_1x_t

