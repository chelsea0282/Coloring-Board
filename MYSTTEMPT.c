//example code in NXP or github
//https://github.com/thijse/Arduino-Code-and-Libraries/blob/master/Libraries/lightuino5/spi.c


void SPI_Init(void) {
    //configuring ports Enable clock network to SPI0
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	SIM->SCGC4 |= SIM_SCGC4_SPI0_MASK;
    //configure output crossbar
    PORTD->PCR[0] = PORT_PCR_MUX(2);  // PCS
	PORTD->PCR[1] = PORT_PCR_MUX(2);  // SCK
	PORTD->PCR[2] = PORT_PCR_MUX(2);  // MOSI
	PORTD->PCR[3] = PORT_PCR_MUX(2);  // MISO

    //configure gipo address select

    // Config registers, turn on SPI0 as master
	// Enable chip select
	SPI0->C1 = 0x52;
	SPI0->C2 = 0x10;
	SPI0->BR = 0x00;

    //clear all registers ?
    SPI0_SR = (SPI_SR_TCF_MASK | SPI_SR_EOQF_MASK | SPI_SR_TFUF_MASK | SPI_SR_TFFF_MASK | SPI_SR_RFOF_MASK | SPI_SR_RFDF_MASK); //clear the status bits (write-1-to-clear)
    SPI0_TCR = 0;
    SPI0_RSER = 0;
    SPI0_PUSHR = 0; //Clear out PUSHR register. Since DSPI is halted, nothing should be transmitted
    SPI0_CTAR0 = 0;
}

void Init_Data(struct spiData* data) {
    /* will have to decide on the kind of data being passed
    whether it be a line (starting point and ending point
    or )
    (*data).slave1.rotation = E1RO;
    (*data).slave1.rawData = 0;
    (*data).slave1.err = 0;
    (*data).slave1.scaled = 0;
    */
}

uint8_t SPI_status(void) {
	return SPI0->S;
}

//read and write
// Write out all characters in supplied buffer to register at address
void SPI_write(uint8_t* p, int size, uint8_t addr) {
	int i;
	// set SPI line to output (BIDROE = 1)
	//SPI0->C2 |= 0x04;
	for (i = 0; i < size; ++i) {
		// poll until empty
		while ((SPI_status() & 0x20) != 0x20);
		SPI0->D = p[i];
	}
}

// Read size number of characters into buffer p from register at address
void SPI_read(uint8_t* p, int size, uint8_t addr) {
	int i;
	// set SPI line to input (BIDROE = 0)
	//SPI0->C2 &= 0xF7;
	for (i = 0; i < size; ++i) {
		// poll until full
		SPI0->D = 0x00;
		while ((SPI_status() & 0x80) != 0x80);
		p[i] = SPI0->D;
	}
}

oid main(void) { //?
    // configure registers

  SPI0_MCR |= SPI_MCR_MSTR_MASK | SPI_MCR_PCSIS_MASK;

  SPI0_MCR &= (~SPI_MCR_DIS_RXF_MASK & ~SPI_MCR_DIS_TXF_MASK); // enable FIFOs

  SPI0_MCR &=  (~SPI_MCR_MDIS_MASK & ~SPI_MCR_HALT_MASK); //enable SPI and start transfer

  SPI0_CTAR0 |=  SPI_CTAR_FMSZ_8BIT | SPI_CTAR_CPOL_MASK | SPI_CTAR_CPHA_MASK | SPI_CTAR_BR(6); // 8 bits, 500khz at 120Mhz
}
 
/*
// Send Steps:

  while(1){

  SPI0_MCR |=  SPI_MCR_HALT_MASK;

  SPI0_MCR |= (SPI_MCR_CLR_RXF_MASK | SPI_MCR_CLR_TXF_MASK); //flush the fifos

  SPI0_SR |= (SPI_SR_TCF_MASK | SPI_SR_EOQF_MASK | SPI_SR_TFUF_MASK | SPI_SR_TFFF_MASK | SPI_SR_RFOF_MASK | SPI_SR_RFDF_MASK); //clear the status bits (write-1-to-clear)

  SPI0_TCR |= SPI_TCR_SPI_TCNT_MASK;

  SPI0_MCR &=  ~SPI_MCR_HALT_MASK;

  SPI0_PUSHR = (SPI_PUSHR_CONT_MASK | SPI_PUSHR_PCS0_ON | cmd);

  while(!(SPI0_SR & SPI_SR_TCF_MASK));

  SPI0_PUSHR = (SPI_PUSHR_CONT_MASK | SPI_PUSHR_PCS0_ON | 0xAA);

  while(!(SPI0_SR & SPI_SR_TCF_MASK));

  SPI0_PUSHR = (SPI_PUSHR_CONT_MASK | SPI_PUSHR_PCS0_ON | 0xBB);

  while(!(SPI0_SR & SPI_SR_TCF_MASK));

  SPI0_PUSHR = (SPI_PUSHR_CONT_MASK | SPI_PUSHR_PCS0_ON | 0xCC);

  while(!(SPI0_SR & SPI_SR_TCF_MASK));

  SPI0_PUSHR = (SPI_PUSHR_CONT_MASK | SPI_PUSHR_PCS0_ON | 0xDD);

  while(!(SPI0_SR & SPI_SR_TCF_MASK));

  SPI0_PUSHR = (SPI_PUSHR_EOQ_MASK | SPI_PUSHR_PCS0_ON | 0xEE); //send last byte

 

  SPI0_SR |= SPI_SR_TFFF_MASK; //clear the status bits (write-1-to-clear)

  cmd++;

  }

//*****************************************************************************************************************************************

//Receive Steps: (tested with memory MX25L3206E)

while(1){

  SPI0_MCR |=  SPI_MCR_HALT_MASK;

  SPI0_MCR |= (SPI_MCR_CLR_RXF_MASK | SPI_MCR_CLR_TXF_MASK); //flush the fifos

  SPI0_SR |= (SPI_SR_TCF_MASK | SPI_SR_EOQF_MASK | SPI_SR_TFUF_MASK | SPI_SR_TFFF_MASK | SPI_SR_RFOF_MASK | SPI_SR_RFDF_MASK); //clear the status bits (write-1-to-clear)

  SPI0_TCR |= SPI_TCR_SPI_TCNT_MASK;

  SPI0_MCR &=  ~SPI_MCR_HALT_MASK;

 

  SPI0_PUSHR = (SPI_PUSHR_CONT_MASK |  SPI_PUSHR_PCS_MASK | 0x9F); //command byte

  while (!(SPI0_SR & SPI_SR_RFDF_MASK)  );

  SPI0_POPR; //dummy read

  SPI0_SR = SPI_SR_RFDF_MASK;   // clear the reception flag (not self-clearing)

 

  SPI0_PUSHR = (SPI_PUSHR_CONT_MASK | SPI_PUSHR_PCS0_ON | 0xFF); //dummy byte to read

  while (!(SPI0_SR & SPI_SR_RFDF_MASK)  );

  receiveBuffer[0] = SPI0_POPR; //read

  SPI0_SR = SPI_SR_RFDF_MASK;   // clear the reception flag (not self-clearing)

 

  SPI0_PUSHR = (SPI_PUSHR_CONT_MASK |  SPI_PUSHR_PCS0_ON | 0xFF); //dummy byte to read

  while (!(SPI0_SR & SPI_SR_RFDF_MASK)  );

  receiveBuffer[1] = SPI0_POPR; //read

  SPI0_SR = SPI_SR_RFDF_MASK;   // clear the reception flag (not self-clearing)

 

  SPI0_PUSHR = (SPI_PUSHR_EOQ_MASK | SPI_PUSHR_PCS0_ON | 0xFF); //send last dummy byte to read

  while (!(SPI0_SR & SPI_SR_RFDF_MASK)  );

  receiveBuffer[2] = SPI0_POPR; //read

  SPI0_SR = SPI_SR_RFDF_MASK;   // clear the reception flag (not self-clearing)

  }

  return 0;
    */
