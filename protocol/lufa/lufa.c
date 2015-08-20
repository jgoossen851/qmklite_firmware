/* 
 * Copyright 2012 Jun Wako <wakojun@gmail.com>
 * This file is based on:
 *     LUFA-120219/Demos/Device/Lowlevel/KeyboardMouse
 *     LUFA-120219/Demos/Device/Lowlevel/GenericHID
 */

/*
             LUFA Library
     Copyright (C) Dean Camera, 2012.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2012  Dean Camera (dean [at] fourwalledcubicle [dot] com)
  Copyright 2010  Denver Gingerich (denver [at] ossguy [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

#include "report.h"
#include "host.h"
#include "host_driver.h"
#include "keyboard.h"
#include "action.h"
#include "led.h"
#include "sendchar.h"
#include "debug.h"
#ifdef SLEEP_LED_ENABLE
#include "sleep_led.h"
#endif
#include "suspend.h"

#include "descriptor.h"
#include "lufa.h"


uint8_t keyboard_idle = 0;
uint8_t keyboard_protocol = 1;
static uint8_t keyboard_led_stats = 0;

static report_keyboard_t keyboard_report_sent;


/* Host driver */
static uint8_t keyboard_leds(void);
static void send_keyboard(report_keyboard_t *report);
static void send_mouse(report_mouse_t *report);
static void send_system(uint16_t data);
static void send_consumer(uint16_t data);
void usb_send_func(MidiDevice * device, uint16_t cnt, uint8_t byte0, uint8_t byte1, uint8_t byte2);
void usb_get_midi(MidiDevice * device);
void midi_usb_init(MidiDevice * device);
host_driver_t lufa_driver = {
    keyboard_leds,
    send_keyboard,
    send_mouse,
    send_system,
    send_consumer,
    usb_send_func,
    usb_get_midi,
    midi_usb_init
};

void SetupHardware(void);

USB_ClassInfo_MIDI_Device_t USB_MIDI_Interface =
{
  .Config =
  {
    .StreamingInterfaceNumber = 1,
    .DataINEndpoint           =
    {
      .Address          = (ENDPOINT_DIR_IN | MIDI_STREAM_IN_EPNUM),
      .Size             = MIDI_STREAM_EPSIZE,
      .Banks            = 1,
    },
    .DataOUTEndpoint          =
    {
      .Address          = (ENDPOINT_DIR_OUT | MIDI_STREAM_OUT_EPNUM),
      .Size             = MIDI_STREAM_EPSIZE,
      .Banks            = 1,
    },
  },
};

#define SYSEX_START_OR_CONT 0x40
#define SYSEX_ENDS_IN_1 0x50
#define SYSEX_ENDS_IN_2 0x60
#define SYSEX_ENDS_IN_3 0x70

#define SYS_COMMON_1 0x50
#define SYS_COMMON_2 0x20
#define SYS_COMMON_3 0x30


/*******************************************************************************
 * Console
 ******************************************************************************/
#ifdef CONSOLE_ENABLE
static void Console_Task(void)
{
    /* Device must be connected and configured for the task to run */
    if (USB_DeviceState != DEVICE_STATE_Configured)
        return;

    uint8_t ep = Endpoint_GetCurrentEndpoint();

#if 0
    // TODO: impl receivechar()/recvchar()
    Endpoint_SelectEndpoint(CONSOLE_OUT_EPNUM);

    /* Check to see if a packet has been sent from the host */
    if (Endpoint_IsOUTReceived())
    {
        /* Check to see if the packet contains data */
        if (Endpoint_IsReadWriteAllowed())
        {
            /* Create a temporary buffer to hold the read in report from the host */
            uint8_t ConsoleData[CONSOLE_EPSIZE];
 
            /* Read Console Report Data */
            Endpoint_Read_Stream_LE(&ConsoleData, sizeof(ConsoleData), NULL);
 
            /* Process Console Report Data */
            //ProcessConsoleHIDReport(ConsoleData);
        }

        /* Finalize the stream transfer to send the last packet */
        Endpoint_ClearOUT();
    }
#endif

    /* IN packet */
    Endpoint_SelectEndpoint(CONSOLE_IN_EPNUM);
    if (!Endpoint_IsEnabled() || !Endpoint_IsConfigured()) {
        Endpoint_SelectEndpoint(ep);
        return;
    }

    // fill empty bank
    while (Endpoint_IsReadWriteAllowed())
        Endpoint_Write_8(0);

    // flash senchar packet
    if (Endpoint_IsINReady()) {
        Endpoint_ClearIN();
    }

    Endpoint_SelectEndpoint(ep);
}
#else
static void Console_Task(void)
{
}
#endif


/*******************************************************************************
 * USB Events
 ******************************************************************************/
/*
 * Event Order of Plug in:
 * 0) EVENT_USB_Device_Connect
 * 1) EVENT_USB_Device_Suspend
 * 2) EVENT_USB_Device_Reset
 * 3) EVENT_USB_Device_Wake
*/
void EVENT_USB_Device_Connect(void)
{
    print("[C]");
    /* For battery powered device */
    if (!USB_IsInitialized) {
        USB_Disable();
        USB_Init();
        USB_Device_EnableSOFEvents();
    }
}

void EVENT_USB_Device_Disconnect(void)
{
    print("[D]");
    /* For battery powered device */
    USB_IsInitialized = false;
/* TODO: This doesn't work. After several plug in/outs can not be enumerated. 
    if (USB_IsInitialized) {
        USB_Disable();  // Disable all interrupts
	USB_Controller_Enable();
        USB_INT_Enable(USB_INT_VBUSTI);
    }
*/
}

void EVENT_USB_Device_Reset(void)
{
    print("[R]");
}

void EVENT_USB_Device_Suspend()
{
    print("[S]");
    matrix_power_down();
#ifdef SLEEP_LED_ENABLE
    sleep_led_enable();
#endif
}

void EVENT_USB_Device_WakeUp()
{
    print("[W]");
    suspend_wakeup_init();

#ifdef SLEEP_LED_ENABLE
    sleep_led_disable();
    // NOTE: converters may not accept this
    led_set(host_keyboard_leds());
#endif
}

void EVENT_USB_Device_StartOfFrame(void)
{
    Console_Task();
}

/** Event handler for the USB_ConfigurationChanged event.
 * This is fired when the host sets the current configuration of the USB device after enumeration.
 */
void EVENT_USB_Device_ConfigurationChanged(void)
{
    bool ConfigSuccess = true;

    /* Setup Keyboard HID Report Endpoints */
    ConfigSuccess &= ENDPOINT_CONFIG(KEYBOARD_IN_EPNUM, EP_TYPE_INTERRUPT, ENDPOINT_DIR_IN,
                                     KEYBOARD_EPSIZE, ENDPOINT_BANK_SINGLE);

#ifdef MOUSE_ENABLE
    /* Setup Mouse HID Report Endpoint */
    ConfigSuccess &= ENDPOINT_CONFIG(MOUSE_IN_EPNUM, EP_TYPE_INTERRUPT, ENDPOINT_DIR_IN,
                                     MOUSE_EPSIZE, ENDPOINT_BANK_SINGLE);
#endif

#ifdef EXTRAKEY_ENABLE
    /* Setup Extra HID Report Endpoint */
    ConfigSuccess &= ENDPOINT_CONFIG(EXTRAKEY_IN_EPNUM, EP_TYPE_INTERRUPT, ENDPOINT_DIR_IN,
                                     EXTRAKEY_EPSIZE, ENDPOINT_BANK_SINGLE);
#endif

#ifdef CONSOLE_ENABLE
    /* Setup Console HID Report Endpoints */
    ConfigSuccess &= ENDPOINT_CONFIG(CONSOLE_IN_EPNUM, EP_TYPE_INTERRUPT, ENDPOINT_DIR_IN,
                                     CONSOLE_EPSIZE, ENDPOINT_BANK_DOUBLE);
#if 0
    ConfigSuccess &= ENDPOINT_CONFIG(CONSOLE_OUT_EPNUM, EP_TYPE_INTERRUPT, ENDPOINT_DIR_OUT,
                                     CONSOLE_EPSIZE, ENDPOINT_BANK_SINGLE);
#endif
#endif

#ifdef NKRO_ENABLE
    /* Setup NKRO HID Report Endpoints */
    ConfigSuccess &= ENDPOINT_CONFIG(NKRO_IN_EPNUM, EP_TYPE_INTERRUPT, ENDPOINT_DIR_IN,
                                     NKRO_EPSIZE, ENDPOINT_BANK_SINGLE);
#endif


    ConfigSuccess &= MIDI_Device_ConfigureEndpoints(&USB_MIDI_Interface);
}



/*
Appendix G: HID Request Support Requirements

The following table enumerates the requests that need to be supported by various types of HID class devices.

Device type     GetReport   SetReport   GetIdle     SetIdle     GetProtocol SetProtocol
------------------------------------------------------------------------------------------
Boot Mouse      Required    Optional    Optional    Optional    Required    Required
Non-Boot Mouse  Required    Optional    Optional    Optional    Optional    Optional
Boot Keyboard   Required    Optional    Required    Required    Required    Required
Non-Boot Keybrd Required    Optional    Required    Required    Optional    Optional
Other Device    Required    Optional    Optional    Optional    Optional    Optional
*/
/** Event handler for the USB_ControlRequest event.
 *  This is fired before passing along unhandled control requests to the library for processing internally.
 */
void EVENT_USB_Device_ControlRequest(void)
{
    uint8_t* ReportData = NULL;
    uint8_t  ReportSize = 0;

    MIDI_Device_ProcessControlRequest(&USB_MIDI_Interface);

    /* Handle HID Class specific requests */
    switch (USB_ControlRequest.bRequest)
    {
        case HID_REQ_GetReport:
            if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
            {
                Endpoint_ClearSETUP();

                // Interface
                switch (USB_ControlRequest.wIndex) {
                case KEYBOARD_INTERFACE:
                    // TODO: test/check
                    ReportData = (uint8_t*)&keyboard_report_sent;
                    ReportSize = sizeof(keyboard_report_sent);
                    break;
                }

                /* Write the report data to the control endpoint */
                Endpoint_Write_Control_Stream_LE(ReportData, ReportSize);
                Endpoint_ClearOUT();
            }

            break;
        case HID_REQ_SetReport:
            if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
            {

                // Interface
                switch (USB_ControlRequest.wIndex) {
                case KEYBOARD_INTERFACE:
#ifdef NKRO_ENABLE
                case NKRO_INTERFACE:
#endif
                    Endpoint_ClearSETUP();

                    while (!(Endpoint_IsOUTReceived())) {
                        if (USB_DeviceState == DEVICE_STATE_Unattached)
                          return;
                    }
                    keyboard_led_stats = Endpoint_Read_8();

                    Endpoint_ClearOUT();
                    Endpoint_ClearStatusStage();
                    break;
                }

            }

            break;

        case HID_REQ_GetProtocol:
            if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
            {
                if (USB_ControlRequest.wIndex == KEYBOARD_INTERFACE) {
                    Endpoint_ClearSETUP();
                    while (!(Endpoint_IsINReady()));
                    Endpoint_Write_8(keyboard_protocol);
                    Endpoint_ClearIN();
                    Endpoint_ClearStatusStage();
                }
            }

            break;
        case HID_REQ_SetProtocol:
            if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
            {
                if (USB_ControlRequest.wIndex == KEYBOARD_INTERFACE) {
                    Endpoint_ClearSETUP();
                    Endpoint_ClearStatusStage();

                    keyboard_protocol = ((USB_ControlRequest.wValue & 0xFF) != 0x00);
#ifdef NKRO_ENABLE
                    keyboard_nkro = !!keyboard_protocol;
#endif
                    clear_keyboard();
                }
            }

            break;
        case HID_REQ_SetIdle:
            if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
            {
                Endpoint_ClearSETUP();
                Endpoint_ClearStatusStage();

                keyboard_idle = ((USB_ControlRequest.wValue & 0xFF00) >> 8);
            }

            break;
        case HID_REQ_GetIdle:
            if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
            {
                Endpoint_ClearSETUP();
                while (!(Endpoint_IsINReady()));
                Endpoint_Write_8(keyboard_idle);
                Endpoint_ClearIN();
                Endpoint_ClearStatusStage();
            }

            break;
    }
}

/*******************************************************************************
 * Host driver 
 ******************************************************************************/
static uint8_t keyboard_leds(void)
{
    return keyboard_led_stats;
}

static void send_keyboard(report_keyboard_t *report)
{
    uint8_t timeout = 255;

    if (USB_DeviceState != DEVICE_STATE_Configured)
        return;

    /* Select the Keyboard Report Endpoint */
#ifdef NKRO_ENABLE
    if (keyboard_nkro) {
        /* Report protocol - NKRO */
        Endpoint_SelectEndpoint(NKRO_IN_EPNUM);

        /* Check if write ready for a polling interval around 1ms */
        while (timeout-- && !Endpoint_IsReadWriteAllowed()) _delay_us(4);
        if (!Endpoint_IsReadWriteAllowed()) return;

        /* Write Keyboard Report Data */
        Endpoint_Write_Stream_LE(report, NKRO_EPSIZE, NULL);
    }
    else
#endif
    {
        /* Boot protocol */
        Endpoint_SelectEndpoint(KEYBOARD_IN_EPNUM);

        /* Check if write ready for a polling interval around 10ms */
        while (timeout-- && !Endpoint_IsReadWriteAllowed()) _delay_us(40);
        if (!Endpoint_IsReadWriteAllowed()) return;

        /* Write Keyboard Report Data */
        Endpoint_Write_Stream_LE(report, KEYBOARD_EPSIZE, NULL);
    }

    /* Finalize the stream transfer to send the last packet */
    Endpoint_ClearIN();

    keyboard_report_sent = *report;
}

static void send_mouse(report_mouse_t *report)
{
#ifdef MOUSE_ENABLE
    uint8_t timeout = 255;

    if (USB_DeviceState != DEVICE_STATE_Configured)
        return;

    /* Select the Mouse Report Endpoint */
    Endpoint_SelectEndpoint(MOUSE_IN_EPNUM);

    /* Check if write ready for a polling interval around 10ms */
    while (timeout-- && !Endpoint_IsReadWriteAllowed()) _delay_us(40);
    if (!Endpoint_IsReadWriteAllowed()) return;

    /* Write Mouse Report Data */
    Endpoint_Write_Stream_LE(report, sizeof(report_mouse_t), NULL);

    /* Finalize the stream transfer to send the last packet */
    Endpoint_ClearIN();
#endif
}

static void send_system(uint16_t data)
{
    uint8_t timeout = 255;

    if (USB_DeviceState != DEVICE_STATE_Configured)
        return;

    report_extra_t r = {
        .report_id = REPORT_ID_SYSTEM,
        .usage = data
    };
    Endpoint_SelectEndpoint(EXTRAKEY_IN_EPNUM);

    /* Check if write ready for a polling interval around 10ms */
    while (timeout-- && !Endpoint_IsReadWriteAllowed()) _delay_us(40);
    if (!Endpoint_IsReadWriteAllowed()) return;

    Endpoint_Write_Stream_LE(&r, sizeof(report_extra_t), NULL);
    Endpoint_ClearIN();
}

static void send_consumer(uint16_t data)
{
    uint8_t timeout = 255;

    if (USB_DeviceState != DEVICE_STATE_Configured)
        return;

    report_extra_t r = {
        .report_id = REPORT_ID_CONSUMER,
        .usage = data
    };
    Endpoint_SelectEndpoint(EXTRAKEY_IN_EPNUM);

    /* Check if write ready for a polling interval around 10ms */
    while (timeout-- && !Endpoint_IsReadWriteAllowed()) _delay_us(40);
    if (!Endpoint_IsReadWriteAllowed()) return;

    Endpoint_Write_Stream_LE(&r, sizeof(report_extra_t), NULL);
    Endpoint_ClearIN();
}


/*******************************************************************************
 * sendchar
 ******************************************************************************/
#ifdef CONSOLE_ENABLE
#define SEND_TIMEOUT 5
int8_t sendchar(uint8_t c)
{
    // Not wait once timeouted.
    // Because sendchar() is called so many times, waiting each call causes big lag.
    static bool timeouted = false;

    if (USB_DeviceState != DEVICE_STATE_Configured)
        return -1;

    uint8_t ep = Endpoint_GetCurrentEndpoint();
    Endpoint_SelectEndpoint(CONSOLE_IN_EPNUM);
    if (!Endpoint_IsEnabled() || !Endpoint_IsConfigured()) {
        goto ERROR_EXIT;
    }

    if (timeouted && !Endpoint_IsReadWriteAllowed()) {
        goto ERROR_EXIT;
    }

    timeouted = false;

    uint8_t timeout = SEND_TIMEOUT;
    while (!Endpoint_IsReadWriteAllowed()) {
        if (USB_DeviceState != DEVICE_STATE_Configured) {
            goto ERROR_EXIT;
        }
        if (Endpoint_IsStalled()) {
            goto ERROR_EXIT;
        }
        if (!(timeout--)) {
            timeouted = true;
            goto ERROR_EXIT;
        }
        _delay_ms(1);
    }

    Endpoint_Write_8(c);

    // send when bank is full
    if (!Endpoint_IsReadWriteAllowed())
        Endpoint_ClearIN();

    Endpoint_SelectEndpoint(ep);
    return 0;
ERROR_EXIT:
    Endpoint_SelectEndpoint(ep);
    return -1;
}
#else
int8_t sendchar(uint8_t c)
{
    return 0;
}
#endif





void usb_send_func(MidiDevice * device, uint16_t cnt, uint8_t byte0, uint8_t byte1, uint8_t byte2) {
  MIDI_EventPacket_t event;
  event.Data1 = byte0;
  event.Data2 = byte1;
  event.Data3 = byte2;

  //if the length is undefined we assume it is a SYSEX message
  if (midi_packet_length(byte0) == UNDEFINED) {
    switch(cnt) {
      case 3:
        if (byte2 == SYSEX_END)
          event.Event = MIDI_EVENT(0, SYSEX_ENDS_IN_3);
        else
          event.Event = MIDI_EVENT(0, SYSEX_START_OR_CONT);
        break;
      case 2:
        if (byte1 == SYSEX_END)
          event.Event = MIDI_EVENT(0, SYSEX_ENDS_IN_2);
        else
          event.Event = MIDI_EVENT(0, SYSEX_START_OR_CONT);
        break;
      case 1:
        if (byte0 == SYSEX_END)
          event.Event = MIDI_EVENT(0, SYSEX_ENDS_IN_1);
        else
          event.Event = MIDI_EVENT(0, SYSEX_START_OR_CONT);
        break;
      default:
        return; //invalid cnt
    }
  } else {
    //deal with 'system common' messages
    //TODO are there any more?
    switch(byte0 & 0xF0){
      case MIDI_SONGPOSITION:
        event.Event = MIDI_EVENT(0, SYS_COMMON_3);
        break;
      case MIDI_SONGSELECT:
      case MIDI_TC_QUARTERFRAME:
        event.Event = MIDI_EVENT(0, SYS_COMMON_2);
        break;
      default:
        event.Event = MIDI_EVENT(0, byte0);
        break;
    }
  }

  MIDI_Device_SendEventPacket(&USB_MIDI_Interface, &event);
  MIDI_Device_Flush(&USB_MIDI_Interface);
  MIDI_Device_USBTask(&USB_MIDI_Interface);
  USB_USBTask();
}

void usb_get_midi(MidiDevice * device) {
  MIDI_EventPacket_t event;
  while (MIDI_Device_ReceiveEventPacket(&USB_MIDI_Interface, &event)) {

    midi_packet_length_t length = midi_packet_length(event.Data1);
    uint8_t input[3];
    input[0] = event.Data1;
    input[1] = event.Data2;
    input[2] = event.Data3;
    if (length == UNDEFINED) {
      //sysex
      if (event.Event == MIDI_EVENT(0, SYSEX_START_OR_CONT) || event.Event == MIDI_EVENT(0, SYSEX_ENDS_IN_3)) {
        length = 3;
      } else if (event.Event == MIDI_EVENT(0, SYSEX_ENDS_IN_2)) {
        length = 2;
      } else if(event.Event ==  MIDI_EVENT(0, SYSEX_ENDS_IN_1)) {
        length = 1;
      } else {
        //XXX what to do?
      }
    }

    //pass the data to the device input function
    if (length != UNDEFINED)
      midi_device_input(device, length, input);
  }
  MIDI_Device_USBTask(&USB_MIDI_Interface);
  USB_USBTask();
}

void midi_usb_init(MidiDevice * device){
  midi_device_init(device);
  midi_device_set_send_func(device, usb_send_func);
  midi_device_set_pre_input_process_func(device, usb_get_midi);

  SetupHardware();
  sei();
}





/*******************************************************************************
 * main
 ******************************************************************************/
void SetupHardware(void)
{
    /* Disable watchdog if enabled by bootloader/fuses */
    MCUSR &= ~(1 << WDRF);
    wdt_disable();

    /* Disable clock division */
    clock_prescale_set(clock_div_1);

    // Leonardo needs. Without this USB device is not recognized.
    USB_Disable();

    USB_Init();

    // for Console_Task
    USB_Device_EnableSOFEvents();
    print_set_sendchar(sendchar);
}

void fallthrough_callback(MidiDevice * device,
    uint16_t cnt, uint8_t byte0, uint8_t byte1, uint8_t byte2);
void cc_callback(MidiDevice * device,
    uint8_t chan, uint8_t num, uint8_t val);
void sysex_callback(MidiDevice * device,
    uint16_t start, uint8_t length, uint8_t * data);

int main(void)  __attribute__ ((weak));
int main(void)
{
    //setup the device

    midi_device_init(&midi_device);
    midi_device_set_send_func(&midi_device, usb_send_func);
    midi_device_set_pre_input_process_func(&midi_device, usb_get_midi);

    SetupHardware();
    sei();

    midi_register_fallthrough_callback(&midi_device, fallthrough_callback);
    midi_register_cc_callback(&midi_device, cc_callback);
    midi_register_sysex_callback(&midi_device, sysex_callback);

    midi_send_cc(&midi_device, 0, 1, 2);
    midi_send_cc(&midi_device, 15, 1, 0);
    midi_send_noteon(&midi_device, 0, 64, 127);
    midi_send_noteoff(&midi_device, 0, 64, 127);

    /* wait for USB startup & debug output */
    while (USB_DeviceState != DEVICE_STATE_Configured) {
#if defined(INTERRUPT_CONTROL_ENDPOINT)
        ;
#else
        USB_USBTask();
#endif
    }
    print("USB configured.\n");

    /* init modules */
    keyboard_init();
    host_set_driver(&lufa_driver);
#ifdef SLEEP_LED_ENABLE
    sleep_led_init();
#endif

    print("Keyboard start.\n");
    while (1) {
        while (USB_DeviceState == DEVICE_STATE_Suspended) {
            print("[s]");
            suspend_power_down();
            if (USB_Device_RemoteWakeupEnabled && suspend_wakeup_condition()) {
                    USB_Device_SendRemoteWakeup();
            }
        }

        keyboard_task();

        midi_device_process(&midi_device);

#if !defined(INTERRUPT_CONTROL_ENDPOINT)
        USB_USBTask();
#endif
    }
}

//echo data back
void fallthrough_callback(MidiDevice * device,
    uint16_t cnt, uint8_t byte0, uint8_t byte1, uint8_t byte2){
  //pass the data back to the device, using the general purpose send data
  //function, any bytes after cnt are ignored
}

void cc_callback(MidiDevice * device,
    uint8_t chan, uint8_t num, uint8_t val) {
  //sending it back on the next channel
  midi_send_cc(device, (chan + 1) % 16, num, val);
}

void sysex_callback(MidiDevice * device,
    uint16_t start, uint8_t length, uint8_t * data) {
  for (int i = 0; i < length; i++)
    midi_send_cc(device, 15, 0x7F & data[i], 0x7F & (start + i));
}
