/**
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __USB_DEVICE_CDC_ECM_H__
#define __USB_DEVICE_CDC_ECM_H__

/*******************************************************************************
 * Includes
 ******************************************************************************/

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define USB_DEVICE_CDC_COMM_CLASS (0x02U)
#define USB_DEVICE_CDC_COMM_SUBCLASS_ACM (0x02U)
#define USB_DEVICE_CDC_COMM_SUBCLASS_ECM (0x06U)
#define USB_DEVICE_CDC_COMM_PROTOCOL_UNUSED (0x00U)

#define USB_DEVICE_CDC_DATA_CLASS (0x0AU)
#define USB_DEVICE_CDC_DATA_SUBCLASS_UNUSED (0x00U)
#define USB_DEVICE_CDC_DATA_PROTOCOL_UNUSED (0x00U)

#define USB_DEVICE_CDC_FUNC_TYPE_CS_INTERFACE (0x24U)
#define USB_DEVICE_CDC_FUNC_TYPE_CS_ENDPOINT (0x25U)
#define USB_DEVICE_CDC_FUNC_SUBTYPE_HEADER (0x00U)
#define USB_DEVICE_CDC_FUNC_SUBTYPE_UNION (0x06U)
#define USB_DEVICE_CDC_FUNC_SUBTYPE_ABSTRACT_CONTROL_MANAGEMENT (0x02U)
#define USB_DEVICE_CDC_FUNC_SUBTYPE_ETHERNET_NETWORKING (0x0FU)

#define USB_DEVICE_CDC_FUNC_HEADER_LENGTH (0x05U)

#define USB_DEVICE_CDC_REQUEST_SEND_ENCAPSULATED_COMMAND (0x00U)
#define USB_DEVICE_CDC_REQUEST_GET_ENCAPSULATED_RESPONSE (0x01U)

#define USB_DEVICE_CDC_REQUEST_SET_COMM_FEATURE (0x02U)
#define USB_DEVICE_CDC_REQUEST_GET_COMM_FEATURE (0x03U)
#define USB_DEVICE_CDC_REQUEST_CLEAR_COMM_FEATURE (0x04U)

#define USB_DEVICE_CDC_REQUEST_SET_LINE_CODING (0x20U)
#define USB_DEVICE_CDC_REQUEST_GET_LINE_CODING (0x21U)
#define USB_DEVICE_CDC_REQUEST_SET_CONTROL_LINE_STATE (0x22U)
#define USB_DEVICE_CDC_REQUEST_SEND_BREAK (0x23U)

#define USB_DEVICE_CDC_REQUEST_SET_ETHERNET_MULTICAST_FILTER (0x40U)
#define USB_DEVICE_CDC_REQUEST_SET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER (0x41U)
#define USB_DEVICE_CDC_REQUEST_GET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER (0x42U)
#define USB_DEVICE_CDC_REQUEST_SET_ETHERNET_PACKET_FILTER (0x43U)
#define USB_DEVICE_CDC_REQUEST_GET_ETHERNET_STATISTIC (0x44U)

#define USB_DEVICE_CDC_NOTIFICATION_NETWORK_CONNECTION (0x00U)
#define USB_DEVICE_CDC_NOTIFICATION_RESPONSE_AVAILABLE (0x01U)
#define USB_DEVICE_CDC_NOTIFICATION_SERIAL_STATE (0x20U)
#define USB_DEVICE_CDC_NOTIFICATION_CONNECTION_SPEED_CHANGE (0x2AU)

#define USB_DEVICE_CDC_COMM_FEATURE_RESERVED (0x00U)
#define USB_DEVICE_CDC_COMM_FEATURE_ABSTRACT_STATE (0X01U)
#define USB_DEVICE_CDC_COMM_FEATURE_COUNTRY_SETTING (0X02U)

#define USB_DEVICE_CDC_CONTROL_SIGNAL_BITMAP_CARRIER_CONTROL_MASK (1U << 1)
#define USB_DEVICE_CDC_CONTROL_SIGNAL_BITMAP_DCE_MASK (1U << 0)

#define USB_DEVICE_CDC_SERIAL_STATE_BITMAP_OVER_RUN_MASK (1U << 6)
#define USB_DEVICE_CDC_SERIAL_STATE_BITMAP_PARITY_MASK (1U << 5)
#define USB_DEVICE_CDC_SERIAL_STATE_BITMAP_FRAMING_MASK (1U << 4)
#define USB_DEVICE_CDC_SERIAL_STATE_BITMAP_RING_SIGNAL_MASK (1U << 3)
#define USB_DEVICE_CDC_SERIAL_STATE_BITMAP_BREAK_MASK (1U << 2)
#define USB_DEVICE_CDC_SERIAL_STATE_BITMAP_TX_CARRIER_MASK (1U << 1)
#define USB_DEVICE_CDC_SERIAL_STATE_BITMAP_RX_CARRIER_MASK (1U << 0)

#define USB_DEVICE_CDC_ETHERNET_PACKET_FILTER_BITMAP_PACKET_TYPE_MULTICAST_MASK (1U << 4)
#define USB_DEVICE_CDC_ETHERNET_PACKET_FILTER_BITMAP_PACKET_TYPE_BROADCAST_MASK (1U << 3)
#define USB_DEVICE_CDC_ETHERNET_PACKET_FILTER_BITMAP_PACKET_TYPE_DIRECTED_MASK (1U << 2)
#define USB_DEVICE_CDC_ETHERNET_PACKET_FILTER_BITMAP_PACKET_TYPE_ALL_MULTICAST_MASK (1U << 1)
#define USB_DEVICE_CDC_ETHERNET_PACKET_FILTER_BITMAP_PACKET_TYPE_PROMISCUOUS_MASK (1U << 0)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
typedef struct _usb_device_cdc_ecm_pipe_status
{
    uint8_t bulkInIsBusy;
    uint8_t bulkOutIsBusy;
    uint8_t interruptInIsBusy;
} usb_device_cdc_ecm_pipe_status_t;

usb_status_t USB_DeviceCdcEcmBulkIn(usb_device_handle handle, usb_device_endpoint_callback_message_struct_t *message, void *callbackParam);

usb_status_t USB_DeviceCdcEcmBulkOut(usb_device_handle handle, usb_device_endpoint_callback_message_struct_t *message, void *callbackParam);

usb_status_t USB_DeviceCdcEcmInterruptIn(usb_device_handle handle, usb_device_endpoint_callback_message_struct_t *message, void *callbackParam);

usb_status_t USB_DeviceProcessClassRequest(usb_device_handle handle, usb_setup_struct_t *setup, uint32_t *length, uint8_t **buffer);

usb_status_t USB_DeviceGetSetupBuffer(usb_device_handle handle, usb_setup_struct_t **setupBuffer);

usb_status_t USB_DeviceGetClassReceiveBuffer(usb_device_handle handle, usb_setup_struct_t *setup, uint32_t *length, uint8_t **buffer);

usb_status_t USB_DeviceCdcEcmSend(usb_device_handle handle, uint8_t ep, uint8_t *buffer, uint32_t length);

usb_status_t USB_DeviceCdcEcmRecv(usb_device_handle handle, uint8_t ep, uint8_t *buffer, uint32_t length);
#endif
