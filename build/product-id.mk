

ifndef SPARK_PRODUCT_ID
SPARK_PRODUCT_ID=0
#$(info SPARK_PRODUCT_ID not set, assuming 0)
endif

# Determine which is the target device

ARCH=arm

ifeq ("$(SPARK_PRODUCT_ID)","0")
STM32_DEVICE=STM32F10X_MD
PLATFORM_NAME=core-v1
PLATFORM_MCU=STM32F1xx
PLATFORM_NET=CC3000
PRODUCT_DESC=Spark core
endif

ifeq ("$(SPARK_PRODUCT_ID)","1")
STM32_DEVICE=STM32F10X_MD
PLATFORM_NAME=core-v1
PLATFORM_MCU=STM32F1xx
PLATFORM_NET=CC3000
endif

ifeq ("$(SPARK_PRODUCT_ID)","2")
STM32_DEVICE=STM32F10X_HD
PLATFORM_NAME=core-v1
PLATFORM_MCU=STM32F1xx
PLATFORM_NET=CC3000
PRODUCT_DESC=Spark core (STM32F103 HD - 256k flash, 48k ram)
endif

ifeq ("$(SPARK_PRODUCT_ID)","3")
PLATFORM_NAME=gcc
PLATFORM_MCU=gcc
PLATFORM_NET=gcc
ARCH=gcc
PRODUCT_DESC=GCC xcompile
endif


ifeq ("$(PLATFORM_MCU)","")
$(error PLATFORM_MCU not defined. Check product id $(SPARK_PRODUCT_ID))
endif

ifeq ("$(PLATFORM_NET)","")
$(error PLATFORM_NET not defined. Check product id $(SPARK_PRODUCT_ID))
endif

# lower case version of the STM32_DEVICE string for use in filenames
STM32_DEVICE_LC  = $(shell echo $(STM32_DEVICE) | tr A-Z a-z)

ifdef STM32_DEVICE
CFLAGS += -D$(STM32_DEVICE)
endif

CFLAGS += -DSPARK_PRODUCT_ID=$(SPARK_PRODUCT_ID)

MAKE_ARGS += SPARK_PRODUCT_ID=$(SPARK_PRODUCT_ID)

