/**
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __USB_DEVICE_CDC_H__
#define __USB_DEVICE_CDC_H__

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
typedef struct _usb_device_cdc_pipe
{
    uint8_t ep;
    uint8_t isBusy;
    uint8_t *pipeDataBuffer;
    uint32_t pipeDataLen;
    uint8_t pipeStall;
} usb_device_cdc_pipe_t;

typedef struct _usb_device_cdc_struct
{
    usb_device_handle handle;
    usb_device_class_config_struct_t *config;
    usb_device_interface_struct_t *commInterfaceHandle;
    usb_device_interface_struct_t *dataInterfaceHandle;
    usb_device_cdc_pipe_t bulkIn;
    usb_device_cdc_pipe_t bulkOut;
    usb_device_cdc_pipe_t interruptIn;
    uint8_t configurationValue;
    uint8_t interfaceNumber;
    uint8_t alternate;
} usb_device_cdc_struct_t;

typedef enum _usb_device_cdc_event
{
    kUSB_DeviceCdcEventSendResponse,
    kUSB_DeviceCdcEventRecvResponse,
    kUSB_DeviceCdcEventNotifyResponse,

    kUSB_DeviceCdcEventSendEncapsulatedCommand,
    kUSB_DeviceCdcEventGetEncapsulatedResponse,

    kUSB_DeviceCdcEventSetCommFeature,
    kUSB_DeviceCdcEventGetCommFeature,
    kUSB_DeviceCdcEventClearCommFeature,

    kUSB_DeviceCdcEventGetLineCoding,
    kUSB_DeviceCdcEventSetLineCoding,
    kUSB_DeviceCdcEventSetControlLineState,
    kUSB_DeviceCdcEventSendBreak,

    kUSB_DeviceCdcEventSetEthernetMulticastFilters,
    kUSB_DeviceCdcEventSetEthernetPowerManagementPatternFilter,
    kUSB_DeviceCdcEventGetEthernetPowerManagementPatternFilter,
    kUSB_DeviceCdcEventSetEthernetPacketFilter,
    kUSB_DeviceCdcEventGetEthernetStatistic,
} usb_device_cdc_event_t;

usb_status_t USB_DeviceCdcInit(uint8_t controllerId, usb_device_class_config_struct_t *config, class_handle_t *handle);

usb_status_t USB_DeviceCdcDeinit(class_handle_t handle);

usb_status_t USB_DeviceCdcSend(class_handle_t handle, uint8_t ep, uint8_t *buffer, uint32_t length);

usb_status_t USB_DeviceCdcRecv(class_handle_t handle, uint8_t ep, uint8_t *buffer, uint32_t length);

usb_status_t USB_DeviceCdcEvent(void *handle, uint32_t event, void *param);
#endif
