#include <XMC4500.h>
#include <xmc_gpio.h>
#include <xmc_ccu4.h>
#include <xmc_scu.h>
#include <stdlib.h>
#include <stdio.h>

/*
 ----------------MORSE CODE ----------------

  a => .-      n => -.    0 => -----
  b => -...    o => ---   1 => .----
  c => -.-.    p => .--.  2 => ..---
  d => -..     q => --.-  3 => ...--
  e => .       r => .-.   4 => ....-
  f => ..-.    s => ...   5 => .....
  g => --.     t => -     6 => -....
  h => ....    u => ..-   7 => --...
  i => ..      v => ...-  8 => ---..
  j => .---    w => .--   9 => ----.
  k => -.-     x => -..-
  l => .-..    y => -.--
  m => --      z => --..

*/

/*
-------------Rules-------------

1. The length of a dot is 100ms +- 10ms.
2. A dash is 3 times the length of a dot.
3. The space between symbols (dots and dashes) of the same letter is 1 dot.
4. The space between letters is 3 dots.
5. The space between words is 7 dots.

The string to send out is: I CAN MORSE

*/

#define LED_PIN 1
#define BUTTON_1_PIN 14
#define BUTTON_2_PIN 15

#define DOT_LENGTH 100
#define DASH_LENGTH (DOT_LENGTH * 3)
#define SYMBOL_SPACE_LENGTH DOT_LENGTH
#define LETTER_SPACE_LENGTH (DOT_LENGTH * 3)
#define WORD_SPACE_LENGTH (DOT_LENGTH * 7)

volatile uint32_t system_ticks = 0;

volatile uint32_t button_1_pressed_time = 0;
volatile uint32_t prev_button_1_pressed_time = 0;
char time_difference[20];

void(SysTick_Handler)(void);

void delay_ms(uint32_t delay_val);

void dot_LED(void);

void dash_LED(void);

void space_symbol(void);

void space_letter(void);

void space_word(void);

void output_morse_code(char *string);

// Initialize the GPIO pin for the LED
const XMC_GPIO_CONFIG_t led_config = {
    .mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
    .output_level = XMC_GPIO_OUTPUT_LEVEL_LOW};

// Initialize the GPIO pin for the button_1
const XMC_GPIO_CONFIG_t BUTTON_1_config =
    {.mode = XMC_GPIO_MODE_INPUT_PULL_DOWN};

// Initialize the GPIO pin for the button_2
const XMC_GPIO_CONFIG_t BUTTON_2_config =
    {.mode = XMC_GPIO_MODE_INPUT_PULL_DOWN};

int main(void)
{
  // Initialize the XMC Peripheral Library and other necessary peripherals

  XMC_GPIO_Init(XMC_GPIO_PORT1, LED_PIN, &led_config);

  XMC_GPIO_Init(XMC_GPIO_PORT1, BUTTON_1_PIN, &BUTTON_1_config);

  XMC_GPIO_Init(XMC_GPIO_PORT1, BUTTON_2_PIN, &BUTTON_2_config);

  SysTick_Config(SystemCoreClock / 1000); // Generate interrupt every 1 ms

  while (1)
  {

    if (!(XMC_GPIO_GetInput(XMC_GPIO_PORT1, BUTTON_1_PIN)))
    {

      if (button_1_pressed_time == 0)
      {
        button_1_pressed_time = system_ticks;
      }
      else
      {
        prev_button_1_pressed_time = button_1_pressed_time;
        button_1_pressed_time = system_ticks;
      }

      output_morse_code("I CAN MORSE");
    }

    if (!(XMC_GPIO_GetInput(XMC_GPIO_PORT1, BUTTON_2_PIN)))
    {
      // If Button1 has not been pressed yet, send a single 0 digit.
      if (button_1_pressed_time == 0)
      {
        output_morse_code("0");
      }
      // If Button1 has only been pressed once, send the time between boot and the first
      // press of Button1.
      if (button_1_pressed_time != 0 && prev_button_1_pressed_time == 0)
      {
        // Convert uint32_t to char

        sprintf(time_difference, "%lu", button_1_pressed_time);
        output_morse_code(time_difference);
      }
      // Once Button2 is pressed down (not when it is released), the time between the last and
      // second last press of Button1 in milliseconds is sent once
      // The time has to be sent in decimal notation without leading zeroes or decimal point.
      // Round off to full milliseconds.
      if (button_1_pressed_time != 0 && prev_button_1_pressed_time != 0)
      {
        // Convert uint32_t to char

        sprintf(time_difference, "%lu", button_1_pressed_time - prev_button_1_pressed_time);

        output_morse_code(time_difference);
      }
    }
  }

  return 0;
}

void SysTick_Handler(void)
{
  system_ticks++;
}

void delay_ms(uint32_t delay_val)
{

  // Get the current system tick count
  uint32_t start_ticks = system_ticks;

  // Calculate the number of ticks required for the delay
  uint32_t delay_ticks = delay_val;

  // Wait until the required delay has elapsed
  while ((system_ticks - start_ticks) < delay_ticks)
  {
  }
}

void dot_LED(void)
{
  XMC_GPIO_SetOutputHigh(XMC_GPIO_PORT1, LED_PIN); // Turn on the LED
  delay_ms(DOT_LENGTH);                            // Delay for 100 milliseconds
  XMC_GPIO_SetOutputLow(XMC_GPIO_PORT1, LED_PIN);  // Turn off the LED
}

void dash_LED(void)
{
  XMC_GPIO_SetOutputHigh(XMC_GPIO_PORT1, LED_PIN); // Turn on the LED
  delay_ms(DASH_LENGTH);                           // Delay for 300 milliseconds
  XMC_GPIO_SetOutputLow(XMC_GPIO_PORT1, LED_PIN);  // Turn off the LED
}

void space_symbol(void)
{
  delay_ms(SYMBOL_SPACE_LENGTH); // Delay for 100 milliseconds
}

void space_letter(void)
{
  delay_ms(LETTER_SPACE_LENGTH); // Delay for 300 milliseconds
}

void space_word(void)
{
  delay_ms(WORD_SPACE_LENGTH); // Delay for 700 milliseconds
}

void output_morse_code(char *string)
{
  int i = 0;
  while (string[i] != '\0')
  {
    switch (string[i] | 0x20 /* Convert to lower case */)
    {
    case 'a':
      dot_LED();
      space_symbol();
      dash_LED();
      break;
    case 'b':
      dash_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dot_LED();
      break;
    case 'c':
      dash_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dash_LED();
      space_symbol();
      dot_LED();
      break;
    case 'd':
      dash_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dot_LED();
      break;
    case 'e':
      dot_LED();
      break;
    case 'f':
      dot_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dash_LED();
      space_symbol();
      dot_LED();
      break;
    case 'g':
      dash_LED();
      space_symbol();
      dash_LED();
      space_symbol();
      dot_LED();
      break;
    case 'h':
      dot_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dot_LED();
      break;
    case 'i':
      dot_LED();
      space_symbol();
      dot_LED();
      break;
    case 'j':
      dot_LED();
      space_symbol();
      dash_LED();
      space_symbol();
      dash_LED();
      space_symbol();
      dash_LED();
      break;
    case 'k':
      dash_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dash_LED();
      break;
    case 'l':
      dot_LED();
      space_symbol();
      dash_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dot_LED();
      break;
    case 'm':
      dash_LED();
      space_symbol();
      dash_LED();
      break;
    case 'n':
      dash_LED();
      space_symbol();
      dot_LED();
      break;
    case 'o':
      dash_LED();
      space_symbol();
      dash_LED();
      space_symbol();
      dash_LED();
      break;
    case 'p':
      dot_LED();
      space_symbol();
      dash_LED();
      space_symbol();
      dash_LED();
      space_symbol();
      dot_LED();
      break;
    case 'q':
      dash_LED();
      space_symbol();
      dash_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dash_LED();
      break;
    case 'r':
      dot_LED();
      space_symbol();
      dash_LED();
      space_symbol();
      dot_LED();
      break;
    case 's':
      dot_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dot_LED();
      break;
    case 't':
      dash_LED();
      break;
    case 'u':
      dot_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dash_LED();
      break;
    case 'v':
      dot_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dash_LED();
      break;
    case 'w':
      dot_LED();
      space_symbol();
      dash_LED();
      space_symbol();
      dash_LED();
      break;
    case 'x':
      dash_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dash_LED();
      break;
    case 'y':
      dash_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dash_LED();
      space_symbol();
      dash_LED();
      break;
    case 'z':
      dash_LED();
      space_symbol();
      dash_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dot_LED();
      break;
    case '0':
      dash_LED();
      space_symbol();
      dash_LED();
      space_symbol();
      dash_LED();
      space_symbol();
      dash_LED();
      space_symbol();
      dash_LED();
      break;
    case '1':
      dot_LED();
      space_symbol();
      dash_LED();
      space_symbol();
      dash_LED();
      space_symbol();
      dash_LED();
      space_symbol();
      dash_LED();
      break;
    case '2':
      dot_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dash_LED();
      space_symbol();
      dash_LED();
      space_symbol();
      dash_LED();
      break;
    case '3':
      dot_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dash_LED();
      space_symbol();
      dash_LED();
      break;
    case '4':
      dot_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dash_LED();
      break;
    case '5':
      dot_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dot_LED();
      break;
    case '6':
      dash_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dot_LED();
      break;
    case '7':
      dash_LED();
      space_symbol();
      dash_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dot_LED();
      break;
    case '8':
      dash_LED();
      space_symbol();
      dash_LED();
      space_symbol();
      dash_LED();
      space_symbol();
      dot_LED();
      space_symbol();
      dot_LED();
      break;
    case '9':
      dash_LED();
      space_symbol();
      dash_LED();
      space_symbol();
      dash_LED();
      space_symbol();
      dash_LED();
      space_symbol();
      dot_LED();
      break;
    case ' ':
      space_word();
      break;
    default:
      break;
    }
    if (string[i] != ' ' && (string[i + 1] != ' ' && string[i + 1] != '\0'))
    {
      space_letter();
    }

    i++;
  }
}