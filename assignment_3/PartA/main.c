#include "KeyboardHID.h"
#include "german_keyboardCodes.h"

#define LED1 P1_1
#define LED2 P1_0
#define TICKS_PER_SECOND 10000
static volatile uint32_t system_ticks;
static uint32_t total_char[84];
int first_time = 1;
static uint8_t name_counter = 0;
static uint8_t sent = 0, sent_idx = 0;
static bool caps_closed = false;
static bool is_enter = false;
static char password_chars[20];
static bool is_next_char = false;
static bool success = false;
static uint8_t rnd_idx = 0;
static uint8_t write_next_char = 0;
static uint8_t num_success_char = 0;

static uint8_t name_arr[39] = {
	GERMAN_KEYBOARD_SC_E,											  // 0
	GERMAN_KEYBOARD_SC_C,											  // 1
	GERMAN_KEYBOARD_SC_H,											  // 2
	GERMAN_KEYBOARD_SC_O,											  // 3
	GERMAN_KEYBOARD_SC_SPACE,										  // 4
	GERMAN_KEYBOARD_SC_2_AND_QUOTES,								  // 5
	GERMAN_KEYBOARD_SC_M,											  // 6
	GERMAN_KEYBOARD_SC_E,											  // 7
	GERMAN_KEYBOARD_SC_V,											  // 8
	GERMAN_KEYBOARD_SC_L,											  // 9
	GERMAN_KEYBOARD_SC_U,											  // 10
	GERMAN_KEYBOARD_SC_T,											  // 11
	GERMAN_KEYBOARD_SC_SPACE,										  // 12
	GERMAN_KEYBOARD_SC_Y,											  // 13
	GERMAN_KEYBOARD_SC_I,											  // 14
	GERMAN_KEYBOARD_SC_L,											  // 15
	GERMAN_KEYBOARD_SC_D,											  // 16
	GERMAN_KEYBOARD_SC_I,											  // 17
	GERMAN_KEYBOARD_SC_R,											  // 18
	GERMAN_KEYBOARD_SC_I,											  // 19
	GERMAN_KEYBOARD_SC_M,											  // 20
	GERMAN_KEYBOARD_SC_2_AND_QUOTES,								  // 21
	GERMAN_KEYBOARD_SC_SPACE,										  // 22
	GERMAN_KEYBOARD_SC_LESS_THAN_AND_GREATER_THAN_AND_PIPE,			  // 23
	GERMAN_KEYBOARD_SC_SPACE,										  // 24
	GERMAN_KEYBOARD_SC_4_AND_DOLLAR,								  // 25
	GERMAN_KEYBOARD_SC_H,											  // 26
	GERMAN_KEYBOARD_SC_O,											  // 27
	GERMAN_KEYBOARD_SC_M,											  // 28
	GERMAN_KEYBOARD_SC_E,											  // 29
	GERMAN_KEYBOARD_SC_7_AND_SLASH_AND_OPENING_BRACE,				  // 30
	GERMAN_KEYBOARD_SC_0_AND_EQUAL_AND_CLOSING_BRACE,				  // 31
	GERMAN_KEYBOARD_SC_3_AND_PARAGRAPH,								  // 32
	GERMAN_KEYBOARD_SC_7_AND_SLASH_AND_OPENING_BRACE,				  // 33
	GERMAN_KEYBOARD_SC_8_AND_OPENING_PARENTHESIS_AND_OPENING_BRACKET, // 34
	GERMAN_KEYBOARD_SC_3_AND_PARAGRAPH,								  // 35
	GERMAN_KEYBOARD_SC_7_AND_SLASH_AND_OPENING_BRACE,				  // 36
	GERMAN_KEYBOARD_SC_0_AND_EQUAL_AND_CLOSING_BRACE,				  // 37
	GERMAN_KEYBOARD_SC_1_AND_EXCLAMATION,							  // 38
};

XMC_SCU_CLOCK_CONFIG_t clock_config = {
	.syspll_config.p_div = 2,
	.syspll_config.n_div = 80,
	.syspll_config.k_div = 4,
	.syspll_config.mode = XMC_SCU_CLOCK_SYSPLL_MODE_NORMAL,
	.syspll_config.clksrc = XMC_SCU_CLOCK_SYSPLLCLKSRC_OSCHP,
	.enable_oschp = true,
	.calibration_mode = XMC_SCU_CLOCK_FOFI_CALIBRATION_MODE_FACTORY,
	.fsys_clksrc = XMC_SCU_CLOCK_SYSCLKSRC_PLL,
	.fsys_clkdiv = 1,
	.fcpu_clkdiv = 1,
	.fccu_clkdiv = 1,
	.fperipheral_clkdiv = 1};

void SystemCoreClockSetup(void);

void SysTick_Handler(void)
{
	system_ticks++;
}

uint8_t findTheChar();

bool CALLBACK_HID_Device_CreateHIDReport(
	USB_ClassInfo_HID_Device_t *const HIDInterfaceInfo,
	uint8_t *const ReportID,
	const uint8_t ReportType,
	void *ReportData,
	uint16_t *const ReportSize);

void modification(int sent_idx, USB_KeyboardReport_Data_t *report);
void codification(int sent_idx, USB_KeyboardReport_Data_t *report, int i);

void CALLBACK_HID_Device_ProcessHIDReport(
	USB_ClassInfo_HID_Device_t *const HIDInterfaceInfo,
	const uint8_t ReportID,
	const uint8_t ReportType,
	const void *ReportData,
	const uint16_t ReportSize);

int main(void)
{
	XMC_GPIO_SetMode(LED1, XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
	XMC_GPIO_SetMode(LED2, XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
	USB_Init();

	for (int i = 0; i < 800000; i++)
		;
	SysTick_Config(SystemCoreClock / TICKS_PER_SECOND);
	XMC_GPIO_SetOutputHigh(LED2);
	while (1)
	{
		HID_Device_USBTask(&Keyboard_HID_Interface);
	}
}

void modification(int sent_idx, USB_KeyboardReport_Data_t *report)
{
	uint8_t modifierArray[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 2, 0, 2, HID_KEYBOARD_MODIFIER_RIGHTALT, 2, 2, 0, 0, 0, 2, HID_KEYBOARD_MODIFIER_RIGHTALT, HID_KEYBOARD_MODIFIER_RIGHTALT, HID_KEYBOARD_MODIFIER_RIGHTALT, 2, 2, 2, 2, 0, 2};
	report->Modifier = modifierArray[sent_idx];
}

void codification(int sent_idx, USB_KeyboardReport_Data_t *report, int i)
{
	uint8_t codifierArray[] = {0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1D, 0x1C, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1D, 0x1C, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x1E, 0x25, 0x26, 0x38, 0x38, 0x30, 0x27, 0x30, 0x36, 0x37, 0x36, 0x37, 0x64, 0x64, 0x25, 0x26, 0x24, 0x27, 0x24, 0x2D, 0x23, 0x21, 0x2C, 0x1F};
	report->KeyCode[0] = codifierArray[sent_idx];
}

bool CALLBACK_HID_Device_CreateHIDReport(
	USB_ClassInfo_HID_Device_t *const HIDInterfaceInfo,
	uint8_t *const ReportID,
	const uint8_t ReportType,
	void *ReportData,
	uint16_t *const ReportSize)
{
	USB_KeyboardReport_Data_t *report = (USB_KeyboardReport_Data_t *)ReportData;
	*ReportSize = sizeof(USB_KeyboardReport_Data_t);
	static bool isReleased = true;
	static bool capsLockPressed = false;
	if (!success)
	{
		if (sent_idx < 84 && is_next_char)
		{
			if (sent)
			{

				report->Modifier = 0;
				report->Reserved = 0;
				report->KeyCode[0] = 0;
				sent = 0;
				if (!is_enter)
				{
					is_next_char = false;
					++sent_idx;
				}
			}
			else if (is_enter)
			{
				report->Modifier = 0;
				report->Reserved = 0;
				report->KeyCode[0] = 0x28;
				is_enter = false;
				total_char[sent_idx] = system_ticks;
				system_ticks = 0;
				sent = 1;
			}
			else
			{
				if (isReleased)
				{
					report->Modifier = 0;
					if (num_success_char == write_next_char)
					{
						modification(sent_idx, report);
						codification(sent_idx, report, 0);
						write_next_char = 0;
						is_enter = true;
						isReleased = false;
					}
					else
					{
						modification(password_chars[write_next_char], report);
						codification(password_chars[write_next_char], report, 0);
						write_next_char++;
						isReleased = false;
					}
				}
				else
				{
					report->Modifier = 0;
					report->Reserved = 0;
					report->KeyCode[0] = 0;
					isReleased = true;
				}
			}
		}
		else
		{
			report->Modifier = 0;
			report->Reserved = 0;
			report->KeyCode[0] = 0;
			sent = 0;
		}
	}
	else if (!caps_closed)
	{
		if (capsLockPressed)
		{
			report->Modifier = 0;
			report->Reserved = 0;
			report->KeyCode[0] = 0;
			sent = 0;
			caps_closed = true;
		}
		else if (!caps_closed)
		{
			report->Modifier = 0;
			report->Reserved = 0;
			report->KeyCode[0] = HID_KEYBOARD_SC_CAPS_LOCK;
			capsLockPressed = true;
			for (int i = 0; i < 10e5; ++i)
				;
		}
	}
	else if (caps_closed)
	{
		if (name_counter < 39 && name_counter != 37)
		{
			report->Modifier = (name_counter == 5 || name_counter == 21 || name_counter == 23 ||
								name_counter == 25 || name_counter == 26 || name_counter == 27 ||
								name_counter == 28 || name_counter == 29 || name_counter == 30)
								   ? 2
								   : 0;

			report->KeyCode[0] = name_arr[name_counter];
			name_counter++;
		}
		else if (name_counter == 37 && first_time == 1)
		{
			report->Modifier = 0;
			report->Reserved = 0;
			report->KeyCode[0] = 0;
			first_time = 0;
		}
		else if (name_counter == 37 && first_time == 0)
		{
			report->KeyCode[0] = name_arr[name_counter];
			name_counter++;
		}
		else if (name_counter == 39)
		{
			report->Modifier = 0;
			report->Reserved = 0;
			report->KeyCode[0] = 0x28;
			name_counter++;
		}
	}

	return true;
}

uint8_t findTheChar()
{
	uint8_t loc = 0;
	uint32_t max = 0;
	for (int i = 0; i < 84; i++)
	{
		if (max < total_char[i])
		{
			max = total_char[i];
			loc = i;
		}
	}
	return loc - 1;
}

void CALLBACK_HID_Device_ProcessHIDReport(
	USB_ClassInfo_HID_Device_t *const HIDInterfaceInfo,
	const uint8_t ReportID,
	const uint8_t ReportType,
	const void *ReportData,
	const uint16_t ReportSize)
{
	uint8_t *report = (uint8_t *)ReportData;

	if (*report & HID_KEYBOARD_LED_NUMLOCK)
	{
		XMC_GPIO_SetOutputHigh(LED1);
		if (sent_idx == 84)
		{
			rnd_idx = findTheChar();
			password_chars[num_success_char] = rnd_idx;
			num_success_char++;
			sent_idx = 0;
		}
		is_next_char = true;
	}
	else
	{
		XMC_GPIO_SetOutputLow(LED1);
		is_next_char = false;
	}

	if (*report & HID_KEYBOARD_LED_CAPSLOCK)
	{
		XMC_GPIO_SetOutputHigh(LED2);
		success = true;
	}
	else
	{
		XMC_GPIO_SetOutputLow(LED2);
	}
}

// This function is given by the instructor
void SystemCoreClockSetup(void)
{
	/* Setup settings for USB clock */
	XMC_SCU_CLOCK_Init(&clock_config);

	XMC_SCU_CLOCK_EnableUsbPll();
	XMC_SCU_CLOCK_StartUsbPll(2, 64);
	XMC_SCU_CLOCK_SetUsbClockDivider(4);
	XMC_SCU_CLOCK_SetUsbClockSource(XMC_SCU_CLOCK_USBCLKSRC_USBPLL);
	XMC_SCU_CLOCK_EnableClock(XMC_SCU_CLOCK_USB);

	SystemCoreClockUpdate();
}