# USB cache-related MACROs definitions

There are few MACROs in the USB stack to define USB data attributes.

-   USB\_STACK\_USE\_DEDICATED\_RAM

    The following values are used to configure the USB stack to use dedicated RAM or not.

    1. USB\_STACK\_DEDICATED\_RAM\_TYPE\_BDT\_GLOBAL - The USB device global variables \(controller data and device stack data\) are put into the USB-dedicated RAM.

    2. USB\_STACK\_DEDICATED\_RAM\_TYPE\_BDT - The USB device controller global variables \(BDT data\) are put into the USB-dedicated RAM.

    3. 0 - There is no USB-dedicated RAM.

    This macro is set 1 now. It is used to indicate if the USB dedicated RAM is present. This macro is enabled on evkmimxrt595, evkmimxrt685, mimxrt685audevk, frdmk32l2a4s, lpcxpresso54628, lpcxpresso54s018, lpcxpresso54s018m, lpcxpresso55s16, lpcxpresso55s28, lpcxpresso55s69.

-   DATA\_SECTION\_IS\_CACHEABLE

    The following values are used to indicate that the currently used memory is cacheable or not.

    0: non-cacheable

    1: cacheable

    This macro is set 0 by default now. This macro indicates whether the memory is cacheable or not. When set to 1, it indicates the current memory is cacheable. When set to 0, it indicates non-cacheable memory. When memory is cacheable, USB stack uses non-cacheable region to locate USB-transfer buffers and controller data to prevent cache coherency issues. When memory is on-cacheable, no special memory handling is required.
    Please note that this macro is used in combination with USB_DEVICE_CONFIG_BUFFER_PROPERTY_CACHEABLE/USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE. If you still want to locate USB transfer buffers and controller data in cacheable memory, you should use cache maintenance operations and disable the DATA_SECTION_IS_CACHEABLE macro, then enable USB_DEVICE_CONFIG_BUFFER_PROPERTY_CACHEABLE/USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE accordingly. Please refer to the detailed introduction of these two macros below.

-   USB\_DEVICE\_CONFIG\_BUFFER\_PROPERTY\_CACHEABLE

    The following values are used to configure the device stack cache to be enabled or not.

    0: disabled

    1: enable

    This macro is set 1 by default now. It is used to enable cache maintenance for cacheable target (data region is cacheable) on RT4 digits platform and mimxrt700evk. The RT4 digits boards include evkmimxrt1010, evkmimxrt1015, evkmimxrt1020, evkmimxrt1024, evkmimxrt1040, evkbimxrt1050, evkbmimxrt1060, evkcmimxrt1060, evkmimxrt1064, evkmimxrt1160, etc.
    Please note that this macro controls the USB stack's cache behavior and should be configured based on the specific target platform's memory configuration. When memory is cacheable, this macro should be enabled. When memory is non-cacheable, this macro should be disabled. Also note that please keep DATA_SECTION_IS_CACHEABLE disabled when this DUSB_DEVICE_CONFIG_BUFFER_PROPERTY_CACHEABLE is enabled, which means that these two macros have opposite settings - when one is enabled, the other should be disabled.

-   USB\_HOST\_CONFIG\_BUFFER\_PROPERTY\_CACHEABLE

    The following values are used to configure host stack cache to be enabled or not.

    0: disable

    1: enable

    This macro is set 1 by default now. It is used to enable cache maintainance for cacheable target (data region is cacheable) on RT4 digits platform and mimxrt700evk. The RT4 digits boards include evkmimxrt1010, evkmimxrt1015, evkmimxrt1020, evkmimxrt1024, evkmimxrt1040, evkbimxrt1050, evkbmimxrt1060, evkcmimxrt1060, evkmimxrt1064, evkmimxrt1160, etc.
    Please note that this macro controls the USB stack's cache behavior and should be configured based on the specific target platform's memory configuration. When memory is cacheable, this macro should be enabled. When memory is non-cacheable, this macro should be disabled. Also note that please keep DATA_SECTION_IS_CACHEABLE disabled when this DUSB_DEVICE_CONFIG_BUFFER_PROPERTY_CACHEABLE is enabled, which means that these two macros have opposite settings - when one is enabled, the other should be disabled.

Based on the above MACROs, the following cache-related MACROs are defined in the USB stack.

||USB\_DEVICE\_CONFIG\_BUFFER\_PROPERTY\_CACHEABLE \|\|

 USB\_HOST\_CONFIG\_BUFFER\_PROPERTY\_CACHEABLE is true

|USB\_DEVICE\_CONFIG\_BUFFER\_PROPERTY\_CACHEABLE \|\|

 USB\_HOST\_CONFIG\_BUFFER\_PROPERTY\_CACHEABLE is false

|
|USB\_STACK\_USE\_DEDICATED\_RAM’s Value| |DATA\_SECTION\_IS\_CACHEABLE is true|DATA\_SECTION\_IS\_CACHEABLE is false|
|USB\_STACK\_DEDICATED\_RAM\_TYPE\_BDT\_GLOBAL||USB\_GLOBAL|dedicated ram, stack use only|
|USB\_BDT|dedicated ram, stack use only|
|USB\_CONTROLLER\_DATA|NonCachable, stack use only|
|USB\_DMA\_NONINIT\_DATA\_ALIGN\(n\)|cachable ram and alignment|
|USB\_DMA\_INIT\_DATA\_ALIGN\(n\)|cachable ram and alignment|

||USB\_GLOBAL|dedicated ram, stack use only|
|USB\_BDT|dedicated ram, stack use only|
|USB\_CONTROLLER\_DATA|NonCachable, stack use only|
|USB\_DMA\_NONINIT\_DATA\_ALIGN\(n\)|noncachable ram and alignment|
|USB\_DMA\_INIT\_DATA\_ALIGN\(n\)|noncachable ram and alignment|

||USB\_GLOBAL|dedicated ram, stack use only|
|USB\_BDT|dedicated ram, stack use only|
|USB\_CONTROLLER\_DATA|dedicated ram, stack use only|
|USB\_DMA\_NONINIT\_DATA\_ALIGN\(n\)|alignment|
|USB\_DMA\_INIT\_DATA\_ALIGN\(n\)|alignment|

|
|USB\_STACK\_DEDICATED\_RAM\_TYPE\_BDT||USB\_GLOBAL|cachable ram and alignment, stack use only|
|USB\_BDT|dedicated ram, stack use only|
|USB\_CONTROLLER\_DATA|NonCachable, stack use only|
|USB\_DMA\_NONINIT\_DATA\_ALIGN\(n\)|cachable ram and alignment|
|USB\_DMA\_INIT\_DATA\_ALIGN\(n\)|cachable ram and alignment|

||USB\_GLOBAL|NonCachable, stack use only|
|USB\_BDT|dedicated ram, stack use only|
|USB\_CONTROLLER\_DATA|NonCachable, stack use only|
|USB\_DMA\_NONINIT\_DATA\_ALIGN\(n\)|NonCachable and alignment|
|USB\_DMA\_INIT\_DATA\_ALIGN\(n\)|NonCachable and alignment|

||USB\_GLOBAL|NULL, stack use only|
|USB\_BDT|dedicated ram, stack use only|
|USB\_CONTROLLER\_DATA|NULL, stack use only|
|USB\_DMA\_NONINIT\_DATA\_ALIGN\(n\)|alignment|
|USB\_DMA\_INIT\_DATA\_ALIGN\(n\)|alignment|

|
|0||USB\_GLOBAL|cachable ram and alignment, stack use only|
|USB\_BDT|NonCachable, stack use only|
|USB\_CONTROLLER\_DATA|NonCachable, stack use only|
|USB\_DMA\_NONINIT\_DATA\_ALIGN\(n\)|cachable ram and alignment|
|USB\_DMA\_INIT\_DATA\_ALIGN\(n\)|cachable ram and alignment|

||USB\_GLOBAL|NonCachable, stack use only|
|USB\_BDT|NonCachable, stack use only|
|USB\_CONTROLLER\_DATA|NonCachable, stack use only|
|USB\_DMA\_NONINIT\_DATA\_ALIGN\(n\)|NonCachable and alignment|
|USB\_DMA\_INIT\_DATA\_ALIGN\(n\)|NonCachable and alignment|

||USB\_GLOBAL|NULL, stack use only|
|USB\_BDT|NULL, stack use only|
|USB\_CONTROLLER\_DATA|NULL, stack use only|
|USB\_DMA\_NONINIT\_DATA\_ALIGN\(n\)|alignment|
|USB\_DMA\_INIT\_DATA\_ALIGN\(n\)|alignment|

|

**Note:** “NULL” means that the MACRO is empty and has no influence.

There are four assistant MACROs:

|USB\_DATA\_ALIGN\_SIZE|Used in USB stack and application, defines the default align size for USB data.|
|USB\_DATA\_ALIGN\_SIZE\_MULTIPLE\(n\)|Used in USB stack and application, calculates the value that is multiple of the data align size.|
|USB\_DMA\_DATA\_NONCACHEABLE|Used in USB stack and application, puts data in the noncacheable region if the cache is enabled.|
|USB\_GLOBAL\_DEDICATED\_RAM|Used in USB stack and application, puts data in the dedicated RAM if dedicated RAM is enabled.|

**Parent topic:**[USB stack configuration](../topics/usb_stack_configuration.md)

