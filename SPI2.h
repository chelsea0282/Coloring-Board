#include "fsl_dspi_master_driver.h"
#include <stdbool.h>

//may have to adjust
/*! @brief DSPI Clock and Transfer Attributes Register (CTAR) selection.*/
typedef enum _dspi_ctar_selection
{
    kDSPI_Ctar0 = 0U, /*!< CTAR0 selection option for master or slave mode, note that CTAR0 and CTAR0_SLAVE are the
                         same register address. */
    kDSPI_Ctar1 = 1U, /*!< CTAR1 selection option for master mode only. */
    kDSPI_Ctar2 = 2U, /*!< CTAR2 selection option for master mode only , note that some device do not support CTAR2. */
    kDSPI_Ctar3 = 3U, /*!< CTAR3 selection option for master mode only , note that some device do not support CTAR3. */
    kDSPI_Ctar4 = 4U, /*!< CTAR4 selection option for master mode only , note that some device do not support CTAR4. */
    kDSPI_Ctar5 = 5U, /*!< CTAR5 selection option for master mode only , note that some device do not support CTAR5. */
    kDSPI_Ctar6 = 6U, /*!< CTAR6 selection option for master mode only , note that some device do not support CTAR6. */
    kDSPI_Ctar7 = 7U  /*!< CTAR7 selection option for master mode only , note that some device do not support CTAR7. */
} dspi_ctar_selection_t;

/*! @brief DSPI Peripheral Chip Select (Pcs) configuration (which Pcs to configure).*/
typedef enum _dspi_which_pcs_config
{
    kDSPI_Pcs0 = 1U << 0, /*!< Pcs[0] */
    kDSPI_Pcs1 = 1U << 1, /*!< Pcs[1] */
    kDSPI_Pcs2 = 1U << 2, /*!< Pcs[2] */
    kDSPI_Pcs3 = 1U << 3, /*!< Pcs[3] */
    kDSPI_Pcs4 = 1U << 4, /*!< Pcs[4] */
    kDSPI_Pcs5 = 1U << 5  /*!< Pcs[5] */
} dspi_which_pcs_t;

typedef struct {
    bool isChipSelectContinuous;
    bool isSckContinuous;
    pcsPolarity = kDspiPcs_ActiveLow;
    dspi_ctar_selection_t whichCtar;
    dspi_which_pcs_t whichPcs;
} dspi_master_user_config_t;

typedef struct {
    unsigned int dataBusConfig.bitsPerFrame;
    spiDevice.dataBusConfig.clkPhase = kDspiClockPhase_SecondEdge;
    spiDevice.dataBusConfig.clkPolarity = kDspiClockPolarity_ActiveHigh;
    spiDevice.dataBusConfig.direction = kDspiMsbFirst;
    unsigned int bitsPerSec;
} dspi_device_t;

DSPI_DRV_MasterInit(HW_SPI0, &dspiMasterState, &userConfig);

//SPI master transfer
DSPI_DRV_MasterTransferBlocking(HW_SPI0,
                                NULL,
                                &spiSourceBuffer,
                                &spiSinkBuffer,
                                1,
                                1000); 

