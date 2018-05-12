#include "fsl_device_registers.h"
#include "fsl_os_abstraction.h"
#include "fsl_dspi_master_driver.h"
#include "board.h"
#define RED 0x52
#define GREEN 0x47
#define BLUE 0x42
bool pushflag = false;
uint8_t spiSourceBuffer = RED;
uint8_t spiSinkBuffer;
uint8_t RGBdataOut[3] = {RED, GREEN, BLUE};
uint8_t RGBcount = 0;

int main(void)
{
    /* Write your code here */
    hardware_init();
    dbg_uart_init();
    OSA_Init();

    GPIO_DRV_Init(switchPins, ledPins);

    printf("SPI master example, press SW2 to send 0x52(RED), 0x47 (GREEN) or 0x42(BLUE)
    \n\r");

    configure_spi_pins(HW_SPI0);
    dspi_master_state_t dspiMasterState; // simply allocate memory for this
    // configure the members of the user config //
    dspi_master_user_config_t userConfig;
    userConfig.isChipSelectContinuous = false;
    userConfig.isSckContinuous = false;
    userConfig.pcsPolarity = kDspiPcs_ActiveLow;
    userConfig.whichCtar = kDspiCtar0;
    userConfig.whichPcs = kDspiPcs0; //Selects the Chip select
    // init the DSPI module //
    DSPI_DRV_MasterInit(HW_SPI0, &dspiMasterState, &userConfig);
    
    // Define bus configuration.
    uint32_t calculatedBaudRate;
    dspi_device_t spiDevice;
    spiDevice.dataBusConfig.bitsPerFrame = 8;
    spiDevice.dataBusConfig.clkPhase = kDspiClockPhase_SecondEdge;
    spiDevice.dataBusConfig.clkPolarity = kDspiClockPolarity_ActiveHigh;
    spiDevice.dataBusConfig.direction = kDspiMsbFirst;
    spiDevice.bitsPerSec = 50000;
    // configure the SPI bus //
    DSPI_DRV_MasterConfigureBus(HW_SPI0, &spiDevice, &calculatedBaudRate);

    for (;;) {

    if(GPIO_DRV_ReadPinInput(kGpioSW1) == 0) //is switch pressed?
    {
        pushflag = true;
    }
    OSA_TimeDelay(250);
    if(pushflag) {
        pushflag = false;
        GPIO_DRV_TogglePinOutput(BOARD_GPIO_LED_BLUE);
        printf ("Sending 0x%x \n\r",spiSourceBuffer & 0xFF);
        //SEND DATA
        dspi_status_t Error = DSPI_DRV_MasterTransferBlocking(HW_SPI0,
                                                                NULL,
                                                                &spiSourceBuffer,
                                                                &spiSinkBuffer,
                                                                1,
                                                                1000);
        if (Error == kStatus_DSPI_Success) {
            printf ("Transmission succeed \n\r");
            printf ("Received data 0x%x \n\n\r",spiSinkBuffer & 0xFF);
            RGBcount++;
            if(RGBcount > 2 ){RGBcount = 0;}
            spiSourceBuffer = RGBdataOut[RGBcount];
            }
        }
    }
    return 0;
}