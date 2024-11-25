/**
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"
#include "usb_device_class.h"
#include "usb_device_cdc.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define USB_DEVICE_CONFIG_CDC (USB_DEVICE_CONFIG_CDC_ACM + USB_DEVICE_CONFIG_CDC_ECM)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
USB_GLOBAL USB_RAM_ADDRESS_ALIGNMENT(USB_DATA_ALIGN_SIZE) static usb_device_cdc_struct_t s_cdcHandle[USB_DEVICE_CONFIG_CDC];

/*******************************************************************************
 * Code
 ******************************************************************************/
static usb_status_t USB_DeviceCdcAllocateHandle(usb_device_cdc_struct_t **handle)
{
    for (uint32_t cnt = 0U; cnt < (uint32_t)USB_DEVICE_CONFIG_CDC; cnt++)
    {
        if (NULL == s_cdcHandle[cnt].handle)
        {
            *handle = &s_cdcHandle[cnt];
            return kStatus_USB_Success;
        }
    }

    return kStatus_USB_AllocFail;
}

static void USB_DeviceCdcFreeHandle(usb_device_cdc_struct_t *handle)
{
    handle->handle = NULL;
    handle->config = NULL;
    handle->alternate = 0U;
    handle->configurationValue = 0U;
}

static usb_status_t USB_DeviceCdcBulkIn(usb_device_handle handle, usb_device_endpoint_callback_message_struct_t *message, void *callbackParam)
{
    usb_device_cdc_struct_t *cdcHandle = (usb_device_cdc_struct_t *)callbackParam;
    usb_status_t status = kStatus_USB_Error;

    if (NULL == cdcHandle)
    {
        return kStatus_USB_InvalidHandle;
    }

    if ((NULL != cdcHandle->config) && (NULL != cdcHandle->config->classCallback))
    {
        /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap, it is from the second parameter of classInit */
        status = cdcHandle->config->classCallback((class_handle_t)cdcHandle, kUSB_DeviceCdcEventSendResponse, message);
    }

    cdcHandle->bulkIn.isBusy = 0U;

    return status;
}

static usb_status_t USB_DeviceCdcBulkOut(usb_device_handle handle, usb_device_endpoint_callback_message_struct_t *message, void *callbackParam)
{
    usb_device_cdc_struct_t *cdcHandle = (usb_device_cdc_struct_t *)callbackParam;
    usb_status_t status = kStatus_USB_Error;

    if (NULL == cdcHandle)
    {
        return kStatus_USB_InvalidHandle;
    }

    if ((NULL != cdcHandle->config) && (NULL != cdcHandle->config->classCallback))
    {
        /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap, it is from the second parameter of classInit */
        status = cdcHandle->config->classCallback((class_handle_t)cdcHandle, kUSB_DeviceCdcEventRecvResponse, message);
    }

    cdcHandle->bulkOut.isBusy = 0U;

    return status;
}

static usb_status_t USB_DeviceCdcInterruptIn(usb_device_handle handle, usb_device_endpoint_callback_message_struct_t *message, void *callbackParam)
{
    usb_device_cdc_struct_t *cdcHandle = (usb_device_cdc_struct_t *)callbackParam;
    usb_status_t error = kStatus_USB_Error;

    if (NULL == cdcHandle)
    {
        return kStatus_USB_InvalidHandle;
    }

    if ((NULL != cdcHandle->config) && (NULL != cdcHandle->config->classCallback))
    {
        /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap, it is from the second parameter of classInit */
        error = cdcHandle->config->classCallback((class_handle_t)cdcHandle, kUSB_DeviceCdcEventNotifyResponse, message);
    }

    cdcHandle->interruptIn.isBusy = 0U;

    return error;
}

static usb_status_t USB_DeviceCdcEndpointsInit(usb_device_cdc_struct_t *cdcHandle)
{
    usb_status_t status = kStatus_USB_Error;
    usb_device_interface_list_t *interfaceList;
    usb_device_interface_struct_t *interface = NULL;
    usb_device_endpoint_callback_struct_t epCallback;

    if (NULL == cdcHandle)
    {
        return kStatus_USB_InvalidHandle;
    }

    epCallback.callbackFn = NULL;

    /* return error when configuration is invalid (0 or more than the configuration number) */
    if ((cdcHandle->configurationValue == 0U) || (cdcHandle->configurationValue > cdcHandle->config->classInfomation->configurations))
    {
        return status;
    }

    interfaceList = &cdcHandle->config->classInfomation->interfaceList[cdcHandle->configurationValue - 1U];

    for (uint32_t cnt = 0U; cnt < interfaceList->count; cnt++)
    {
        if (interfaceList->interfaces[cnt].classCode == USB_DEVICE_CDC_COMM_CLASS)
        {
            for (uint32_t idx = 0U; idx < interfaceList->interfaces[cnt].count; idx++)
            {
                if (interfaceList->interfaces[cnt].interface[idx].alternateSetting == cdcHandle->alternate)
                {
                    interface = &interfaceList->interfaces[cnt].interface[idx];
                    break;
                }
            }
            cdcHandle->interfaceNumber = interfaceList->interfaces[cnt].interfaceNumber;
            break;
        }
    }

    if (NULL == interface)
    {
        return status;
    }

    cdcHandle->commInterfaceHandle = interface;

    for (uint32_t cnt = 0U; cnt < interface->endpointList.count; cnt++)
    {
        usb_device_endpoint_init_struct_t epInitStruct;
        epInitStruct.zlt = 0U;
        epInitStruct.interval = interface->endpointList.endpoint[cnt].interval;
        epInitStruct.endpointAddress = interface->endpointList.endpoint[cnt].endpointAddress;
        epInitStruct.maxPacketSize = interface->endpointList.endpoint[cnt].maxPacketSize;
        epInitStruct.transferType = interface->endpointList.endpoint[cnt].transferType;

        if (((epInitStruct.endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) == USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) && (USB_ENDPOINT_INTERRUPT == epInitStruct.transferType))
        {
            cdcHandle->interruptIn.ep = (epInitStruct.endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK);
            cdcHandle->interruptIn.isBusy = 0U;
            cdcHandle->interruptIn.pipeDataBuffer = (uint8_t *)USB_INVALID_TRANSFER_BUFFER;
            cdcHandle->interruptIn.pipeStall = 0U;
            cdcHandle->interruptIn.pipeDataLen = 0U;
            epCallback.callbackFn = USB_DeviceCdcInterruptIn;
            epCallback.callbackParam = cdcHandle;

            status = USB_DeviceInitEndpoint(cdcHandle->handle, &epInitStruct, &epCallback);
            if (kStatus_USB_Success != status)
            {
                return status;
            }
        }
    }

    interface = NULL;

    for (uint32_t cnt = 0U; cnt < interfaceList->count; cnt++)
    {
        if (interfaceList->interfaces[cnt].classCode == USB_DEVICE_CDC_DATA_CLASS)
        {
            for (uint32_t idx = 0U; idx < interfaceList->interfaces[cnt].count; idx++)
            {
                if (interfaceList->interfaces[cnt].interface[idx].alternateSetting == cdcHandle->alternate)
                {
                    interface = &interfaceList->interfaces[cnt].interface[idx];
                    break;
                }
            }
            break;
        }
    }

    if (NULL == interface)
    {
        return status;
    }

    cdcHandle->dataInterfaceHandle = interface;

    for (uint32_t cnt = 0U; cnt < interface->endpointList.count; cnt++)
    {
        usb_device_endpoint_init_struct_t epInitStruct;
        epInitStruct.zlt = 0U;
        epInitStruct.interval = interface->endpointList.endpoint[cnt].interval;
        epInitStruct.endpointAddress = interface->endpointList.endpoint[cnt].endpointAddress;
        epInitStruct.maxPacketSize = interface->endpointList.endpoint[cnt].maxPacketSize;
        epInitStruct.transferType = interface->endpointList.endpoint[cnt].transferType;

        if (((epInitStruct.endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) == USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) && (USB_ENDPOINT_BULK == epInitStruct.transferType))
        {
            cdcHandle->bulkIn.ep = (epInitStruct.endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK);
            cdcHandle->bulkIn.isBusy = 0U;
            cdcHandle->bulkIn.pipeDataBuffer = (uint8_t *)USB_INVALID_TRANSFER_BUFFER;
            cdcHandle->bulkIn.pipeStall = 0U;
            cdcHandle->bulkIn.pipeDataLen = 0U;
            epCallback.callbackFn = USB_DeviceCdcBulkIn;
            epCallback.callbackParam = cdcHandle;

            status = USB_DeviceInitEndpoint(cdcHandle->handle, &epInitStruct, &epCallback);
            if (kStatus_USB_Success != status)
            {
                return status;
            }
        }
        else if (((epInitStruct.endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) == USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_OUT) && (USB_ENDPOINT_BULK == epInitStruct.transferType))
        {
            cdcHandle->bulkOut.ep = (epInitStruct.endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK);
            cdcHandle->bulkOut.isBusy = 0U;
            cdcHandle->bulkOut.pipeDataBuffer = (uint8_t *)USB_INVALID_TRANSFER_BUFFER;
            cdcHandle->bulkOut.pipeStall = 0U;
            cdcHandle->bulkOut.pipeDataLen = 0U;
            epCallback.callbackFn = USB_DeviceCdcBulkOut;
            epCallback.callbackParam = cdcHandle;

            status = USB_DeviceInitEndpoint(cdcHandle->handle, &epInitStruct, &epCallback);
            if (kStatus_USB_Success != status)
            {
                return status;
            }
        }
    }

    return status;
}

static usb_status_t USB_DeviceCdcEndpointsDeinit(usb_device_cdc_struct_t *cdcHandle)
{
    usb_status_t status = kStatus_USB_Error;

    if ((NULL == cdcHandle->commInterfaceHandle) || (NULL == cdcHandle->dataInterfaceHandle))
    {
        return kStatus_USB_InvalidHandle;
    }

    for (uint32_t cnt = 0U; cnt < cdcHandle->commInterfaceHandle->endpointList.count; cnt++)
    {
        status = USB_DeviceDeinitEndpoint(cdcHandle->handle, cdcHandle->commInterfaceHandle->endpointList.endpoint[cnt].endpointAddress);
        if (kStatus_USB_Success != status)
        {
            return status;
        }
    }

    for (uint32_t cnt = 0U; cnt < cdcHandle->dataInterfaceHandle->endpointList.count; cnt++)
    {
        status = USB_DeviceDeinitEndpoint(cdcHandle->handle, cdcHandle->dataInterfaceHandle->endpointList.endpoint[cnt].endpointAddress);
        if (kStatus_USB_Success != status)
        {
            return status;
        }
    }

    cdcHandle->commInterfaceHandle = NULL;
    cdcHandle->dataInterfaceHandle = NULL;

    return kStatus_USB_Success;
}

usb_status_t USB_DeviceCdcInit(uint8_t controllerId, usb_device_class_config_struct_t *config, class_handle_t *handle)
{
    usb_device_cdc_struct_t *cdcHandle;
    usb_status_t status;

    status = USB_DeviceCdcAllocateHandle(&cdcHandle);
    if (kStatus_USB_Success != status)
    {
        return status;
    }

    status = USB_DeviceClassGetDeviceHandle(controllerId, &cdcHandle->handle);
    if (kStatus_USB_Success != status)
    {
        return status;
    }

    cdcHandle->config = config;
    *handle = (class_handle_t)cdcHandle;

    return status;
}

usb_status_t USB_DeviceCdcDeinit(class_handle_t handle)
{
    usb_device_cdc_struct_t *cdcHandle = (usb_device_cdc_struct_t *)handle;
    usb_status_t status;

    if (NULL == cdcHandle)
    {
        return kStatus_USB_InvalidHandle;
    }

    status = USB_DeviceCdcEndpointsDeinit(cdcHandle);
    USB_DeviceCdcFreeHandle(cdcHandle);

    return status;
}

usb_status_t USB_DeviceCdcSend(class_handle_t handle, uint8_t ep, uint8_t *buffer, uint32_t length)
{
    usb_device_cdc_struct_t *cdcHandle = (usb_device_cdc_struct_t *)handle;
    usb_status_t status = kStatus_USB_InvalidParameter;
    usb_device_cdc_pipe_t *cdcPipe = NULL;

    if (NULL == handle)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (cdcHandle->bulkIn.ep == ep)
    {
        cdcPipe = &(cdcHandle->bulkIn);
    }
    else if (cdcHandle->interruptIn.ep == ep)
    {
        cdcPipe = &(cdcHandle->interruptIn);
    }

    if (cdcPipe)
    {
        if (cdcPipe->isBusy)
        {
            return kStatus_USB_Busy;
        }

        if (cdcPipe->pipeStall)
        {
            cdcPipe->pipeDataBuffer = buffer;
            cdcPipe->pipeDataLen = length;
            return kStatus_USB_Success;
        }

        cdcPipe->isBusy = 1U;

        status = USB_DeviceSendRequest(cdcHandle->handle, ep, buffer, length);
        if (kStatus_USB_Success != status)
        {
            cdcPipe->isBusy = 0U;
        }
    }

    return status;
}

usb_status_t USB_DeviceCdcRecv(class_handle_t handle, uint8_t ep, uint8_t *buffer, uint32_t length)
{
    usb_device_cdc_struct_t *cdcHandle = (usb_device_cdc_struct_t *)handle;
    usb_status_t status = kStatus_USB_InvalidParameter;
    usb_device_cdc_pipe_t *cdcPipe = NULL;

    if (NULL == handle)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (cdcHandle->bulkOut.ep == ep)
    {
        cdcPipe = &(cdcHandle->bulkOut);
    }

    if (cdcPipe)
    {
        if (cdcPipe->isBusy)
        {
            return kStatus_USB_Busy;
        }

        if (cdcPipe->pipeStall)
        {
            cdcPipe->pipeDataBuffer = buffer;
            cdcPipe->pipeDataLen = length;
            return kStatus_USB_Success;
        }

        cdcPipe->isBusy = 1U;

        status = USB_DeviceRecvRequest(cdcHandle->handle, ep, buffer, length);
        if (kStatus_USB_Success != status)
        {
            cdcPipe->isBusy = 0U;
        }
    }

    return status;
}

usb_status_t USB_DeviceCdcEvent(void *handle, uint32_t event, void *param)
{
    usb_device_cdc_struct_t *cdcHandle = (usb_device_cdc_struct_t *)handle;
    usb_device_class_event_t eventCode = (usb_device_class_event_t)event;
    usb_status_t status = kStatus_USB_Error;

    if ((NULL == param) || (NULL == handle))
    {
        return kStatus_USB_InvalidHandle;
    }

    switch (eventCode)
    {
        case kUSB_DeviceClassEventDeviceReset:
            cdcHandle->configurationValue = 0U;
            cdcHandle->interfaceNumber = 0U;
            cdcHandle->alternate = 0U;
            status = USB_DeviceCdcEndpointsDeinit(cdcHandle);
            break;

        case kUSB_DeviceClassEventSetConfiguration:
            if (NULL == cdcHandle->config)
            {
                break;
            }
            if (*((uint8_t *)param) == cdcHandle->configurationValue)
            {
                status = kStatus_USB_Success;
                break;
            }

            cdcHandle->configurationValue = *((uint8_t *)param);
            cdcHandle->alternate = 0U;
            status = USB_DeviceCdcEndpointsInit(cdcHandle);
            if (kStatus_USB_Success != status)
            {
                (void)usb_echo("USB_DeviceCdcEndpointsInit() error in kUSB_DeviceClassEventSetConfiguration of USB_DeviceCdcEvent().\r\n");
            }
            break;

        case kUSB_DeviceClassEventSetInterface:
            if (NULL == cdcHandle->config)
            {
                break;
            }
            if (cdcHandle->interfaceNumber != *((uint8_t *)param + 1))
            {
                break;
            }

            if (*((uint8_t *)param) == cdcHandle->alternate)
            {
                status = kStatus_USB_Success;
                break;
            }
            status = USB_DeviceCdcEndpointsDeinit(cdcHandle);
            cdcHandle->alternate = *((uint8_t *)param);
            status = USB_DeviceCdcEndpointsInit(cdcHandle);
            if (kStatus_USB_Success != status)
            {
                (void)usb_echo("USB_DeviceCdcEndpointsInit() error in kUSB_DeviceClassEventSetInterface of USB_DeviceCdcEvent().\r\n");
            }
            break;

        case kUSB_DeviceClassEventSetEndpointHalt:
            if ((NULL == cdcHandle->config) || (NULL == cdcHandle->commInterfaceHandle) || (NULL == cdcHandle->dataInterfaceHandle))
            {
                break;
            }
            for (uint32_t cnt = 0U; cnt < cdcHandle->commInterfaceHandle->endpointList.count; cnt++)
            {
                if (*((uint8_t *)param) == cdcHandle->commInterfaceHandle->endpointList.endpoint[cnt].endpointAddress)
                {
                    cdcHandle->interruptIn.pipeStall = 1U;
                    status = USB_DeviceStallEndpoint(cdcHandle->handle, *((uint8_t *)param));
                }
            }
            for (uint32_t cnt = 0U; cnt < cdcHandle->dataInterfaceHandle->endpointList.count; cnt++)
            {
                if (*((uint8_t *)param) == cdcHandle->dataInterfaceHandle->endpointList.endpoint[cnt].endpointAddress)
                {
                    if ((*((uint8_t *)param) & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) == USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN)
                    {
                        cdcHandle->bulkIn.pipeStall = 1U;
                    }
                    else
                    {
                        cdcHandle->bulkOut.pipeStall = 1U;
                    }
                    status = USB_DeviceStallEndpoint(cdcHandle->handle, *((uint8_t *)param));
                }
            }
            break;

        case kUSB_DeviceClassEventClearEndpointHalt:
            if ((NULL == cdcHandle->config) || (NULL == cdcHandle->commInterfaceHandle) || (NULL == cdcHandle->dataInterfaceHandle))
            {
                break;
            }
            for (uint32_t cnt = 0U; cnt < cdcHandle->commInterfaceHandle->endpointList.count; cnt++)
            {
                if (*((uint8_t *)param) == cdcHandle->commInterfaceHandle->endpointList.endpoint[cnt].endpointAddress)
                {
                    status = USB_DeviceUnstallEndpoint(cdcHandle->handle, *((uint8_t *)param));
                    if ((*((uint8_t *)param) & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) == USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN)
                    {
                        if (cdcHandle->interruptIn.pipeStall)
                        {
                            cdcHandle->interruptIn.pipeStall = 0U;

                            if ((uint8_t *)USB_INVALID_TRANSFER_BUFFER != cdcHandle->interruptIn.pipeDataBuffer)
                            {
                                status = USB_DeviceSendRequest(cdcHandle->handle, cdcHandle->interruptIn.ep, cdcHandle->interruptIn.pipeDataBuffer, cdcHandle->interruptIn.pipeDataLen);
                                if (kStatus_USB_Success != status)
                                {
                                    usb_device_endpoint_callback_message_struct_t endpointCallbackMessage;
                                    endpointCallbackMessage.buffer = cdcHandle->interruptIn.pipeDataBuffer;
                                    endpointCallbackMessage.length = cdcHandle->interruptIn.pipeDataLen;
                                    endpointCallbackMessage.isSetup = 0U;

                                    if (kStatus_USB_Success != USB_DeviceCdcInterruptIn(cdcHandle->handle, (void *)&endpointCallbackMessage, handle))
                                    {
                                        return kStatus_USB_Error;
                                    }
                                }
                                cdcHandle->interruptIn.pipeDataBuffer = (uint8_t *)USB_INVALID_TRANSFER_BUFFER;
                                cdcHandle->interruptIn.pipeDataLen = 0U;
                            }
                        }
                    }
                }
            }
            for (uint32_t cnt = 0U; cnt < cdcHandle->dataInterfaceHandle->endpointList.count; cnt++)
            {
                if (*((uint8_t *)param) == cdcHandle->dataInterfaceHandle->endpointList.endpoint[cnt].endpointAddress)
                {
                    status = USB_DeviceUnstallEndpoint(cdcHandle->handle, *((uint8_t *)param));
                    if ((*((uint8_t *)param) & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) == USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN)
                    {
                        if (cdcHandle->bulkIn.pipeStall)
                        {
                            cdcHandle->bulkIn.pipeStall = 0U;

                            if ((uint8_t *)USB_INVALID_TRANSFER_BUFFER != cdcHandle->bulkIn.pipeDataBuffer)
                            {
                                status = USB_DeviceSendRequest(cdcHandle->handle, cdcHandle->bulkIn.ep, cdcHandle->bulkIn.pipeDataBuffer, cdcHandle->bulkIn.pipeDataLen);
                                if (kStatus_USB_Success != status)
                                {
                                    usb_device_endpoint_callback_message_struct_t endpointCallbackMessage;
                                    endpointCallbackMessage.buffer = cdcHandle->bulkIn.pipeDataBuffer;
                                    endpointCallbackMessage.length = cdcHandle->bulkIn.pipeDataLen;
                                    endpointCallbackMessage.isSetup = 0U;
                                    if (kStatus_USB_Success != USB_DeviceCdcBulkIn(cdcHandle->handle, (void *)&endpointCallbackMessage, handle))
                                    {
                                        return kStatus_USB_Error;
                                    }
                                }
                                cdcHandle->bulkIn.pipeDataBuffer = (uint8_t *)USB_INVALID_TRANSFER_BUFFER;
                                cdcHandle->bulkIn.pipeDataLen = 0U;
                            }
                        }
                    }
                    else
                    {
                        if (cdcHandle->bulkOut.pipeStall)
                        {
                            cdcHandle->bulkOut.pipeStall = 0U;

                            if ((uint8_t *)USB_INVALID_TRANSFER_BUFFER != cdcHandle->bulkOut.pipeDataBuffer)
                            {
                                status = USB_DeviceRecvRequest(cdcHandle->handle, cdcHandle->bulkOut.ep, cdcHandle->bulkOut.pipeDataBuffer, cdcHandle->bulkOut.pipeDataLen);
                                if (kStatus_USB_Success != status)
                                {
                                    usb_device_endpoint_callback_message_struct_t endpointCallbackMessage;
                                    endpointCallbackMessage.buffer = cdcHandle->bulkOut.pipeDataBuffer;
                                    endpointCallbackMessage.length = cdcHandle->bulkOut.pipeDataLen;
                                    endpointCallbackMessage.isSetup = 0U;
                                    if (kStatus_USB_Success != USB_DeviceCdcBulkOut(cdcHandle->handle, (void *)&endpointCallbackMessage, handle))
                                    {
                                        return kStatus_USB_Error;
                                    }
                                }
                                cdcHandle->bulkOut.pipeDataBuffer = (uint8_t *)USB_INVALID_TRANSFER_BUFFER;
                                cdcHandle->bulkOut.pipeDataLen = 0U;
                            }
                        }
                    }
                }
            }
            break;

        case kUSB_DeviceClassEventClassRequest:
        {
            usb_device_control_request_struct_t *controlRequest = (usb_device_control_request_struct_t *)param;

            if ((controlRequest->setup->bmRequestType & USB_REQUEST_TYPE_RECIPIENT_MASK) != USB_REQUEST_TYPE_RECIPIENT_INTERFACE)
            {
                break;
            }

            if ((USB_SHORT_GET_LOW(controlRequest->setup->wIndex)) != cdcHandle->interfaceNumber)
            {
                break;
            }

            switch (controlRequest->setup->bRequest)
            {
                case USB_DEVICE_CDC_REQUEST_SEND_ENCAPSULATED_COMMAND:
                    if ((controlRequest->setup->bmRequestType & USB_REQUEST_TYPE_DIR_MASK) == USB_REQUEST_TYPE_DIR_OUT)
                    {
                        status = cdcHandle->config->classCallback((class_handle_t)cdcHandle, kUSB_DeviceCdcEventSendEncapsulatedCommand, param);
                    }
                    break;

                case USB_DEVICE_CDC_REQUEST_GET_ENCAPSULATED_RESPONSE:
                    if ((controlRequest->setup->bmRequestType & USB_REQUEST_TYPE_DIR_MASK) == USB_REQUEST_TYPE_DIR_IN)
                    {
                        status = cdcHandle->config->classCallback((class_handle_t)cdcHandle, kUSB_DeviceCdcEventGetEncapsulatedResponse, param);
                    }
                    break;

#if defined(USB_DEVICE_CONFIG_CDC_ACM) && (USB_DEVICE_CONFIG_CDC_ACM > 0U)
                case USB_DEVICE_CDC_REQUEST_SET_COMM_FEATURE:
                    if ((controlRequest->setup->bmRequestType & USB_REQUEST_TYPE_DIR_MASK) == USB_REQUEST_TYPE_DIR_OUT)
                    {
                        status = cdcHandle->config->classCallback((class_handle_t)cdcHandle, kUSB_DeviceCdcEventSetCommFeature, param);
                    }
                    break;

                case USB_DEVICE_CDC_REQUEST_GET_COMM_FEATURE:
                    if ((controlRequest->setup->bmRequestType & USB_REQUEST_TYPE_DIR_MASK) == USB_REQUEST_TYPE_DIR_IN)
                    {
                        status = cdcHandle->config->classCallback((class_handle_t)cdcHandle, kUSB_DeviceCdcEventGetCommFeature, param);
                    }
                    break;

                case USB_DEVICE_CDC_REQUEST_CLEAR_COMM_FEATURE:
                    if ((controlRequest->setup->bmRequestType & USB_REQUEST_TYPE_DIR_MASK) == USB_REQUEST_TYPE_DIR_OUT)
                    {
                        status = cdcHandle->config->classCallback((class_handle_t)cdcHandle, kUSB_DeviceCdcEventClearCommFeature, param);
                    }
                    break;

                case USB_DEVICE_CDC_REQUEST_SET_LINE_CODING:
                    if ((controlRequest->setup->bmRequestType & USB_REQUEST_TYPE_DIR_MASK) == USB_REQUEST_TYPE_DIR_OUT)
                    {
                        status = cdcHandle->config->classCallback((class_handle_t)cdcHandle, kUSB_DeviceCdcEventSetLineCoding, param);
                    }
                    break;

                case USB_DEVICE_CDC_REQUEST_GET_LINE_CODING:
                    if ((controlRequest->setup->bmRequestType & USB_REQUEST_TYPE_DIR_MASK) == USB_REQUEST_TYPE_DIR_IN)
                    {
                        status = cdcHandle->config->classCallback((class_handle_t)cdcHandle, kUSB_DeviceCdcEventGetLineCoding, param);
                    }
                    break;

                case USB_DEVICE_CDC_REQUEST_SET_CONTROL_LINE_STATE:
                    if ((controlRequest->setup->bmRequestType & USB_REQUEST_TYPE_DIR_MASK) == USB_REQUEST_TYPE_DIR_OUT)
                    {
                        status = cdcHandle->config->classCallback((class_handle_t)cdcHandle, kUSB_DeviceCdcEventSetControlLineState, param);
                    }
                    break;

                case USB_DEVICE_CDC_REQUEST_SEND_BREAK:
                    if ((controlRequest->setup->bmRequestType & USB_REQUEST_TYPE_DIR_MASK) == USB_REQUEST_TYPE_DIR_OUT)
                    {
                        status = cdcHandle->config->classCallback((class_handle_t)cdcHandle, kUSB_DeviceCdcEventSendBreak, param);
                    }
                    break;
#endif

#if defined(USB_DEVICE_CONFIG_CDC_ECM) && (USB_DEVICE_CONFIG_CDC_ECM > 0U)
                case USB_DEVICE_CDC_REQUEST_SET_ETHERNET_MULTICAST_FILTER:
                    if ((controlRequest->setup->bmRequestType & USB_REQUEST_TYPE_DIR_MASK) == USB_REQUEST_TYPE_DIR_OUT)
                    {
                        status = cdcHandle->config->classCallback((class_handle_t)cdcHandle, kUSB_DeviceCdcEventSetEthernetMulticastFilters, param);
                    }
                    break;

                case USB_DEVICE_CDC_REQUEST_SET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER:
                    if ((controlRequest->setup->bmRequestType & USB_REQUEST_TYPE_DIR_MASK) == USB_REQUEST_TYPE_DIR_OUT)
                    {
                        status = cdcHandle->config->classCallback((class_handle_t)cdcHandle, kUSB_DeviceCdcEventSetEthernetPowerManagementPatternFilter, param);
                    }
                    break;

                case USB_DEVICE_CDC_REQUEST_GET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER:
                    if ((controlRequest->setup->bmRequestType & USB_REQUEST_TYPE_DIR_MASK) == USB_REQUEST_TYPE_DIR_IN)
                    {
                        status = cdcHandle->config->classCallback((class_handle_t)cdcHandle, kUSB_DeviceCdcEventGetEthernetPowerManagementPatternFilter, param);
                    }
                    break;

                case USB_DEVICE_CDC_REQUEST_SET_ETHERNET_PACKET_FILTER:
                    if ((controlRequest->setup->bmRequestType & USB_REQUEST_TYPE_DIR_MASK) == USB_REQUEST_TYPE_DIR_OUT)
                    {
                        status = cdcHandle->config->classCallback((class_handle_t)cdcHandle, kUSB_DeviceCdcEventSetEthernetPacketFilter, param);
                    }
                    break;

                case USB_DEVICE_CDC_REQUEST_GET_ETHERNET_STATISTIC:
                    if ((controlRequest->setup->bmRequestType & USB_REQUEST_TYPE_DIR_MASK) == USB_REQUEST_TYPE_DIR_IN)
                    {
                        status = cdcHandle->config->classCallback((class_handle_t)cdcHandle, kUSB_DeviceCdcEventGetEthernetStatistic, param);
                    }
                    break;
#endif

                default:
                    break;
            }

            break;
        }

        default:
            break;
    }

    return status;
}
