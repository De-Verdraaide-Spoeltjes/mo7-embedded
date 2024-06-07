#include "audioController.h"

XIicPs Iic;

XStatus iicConfig() {
    XIicPs_Config *Config;
    int Status;

    // Look up the configuration in the config table
    Config = XIicPs_LookupConfig(XPAR_XIICPS_0_DEVICE_ID);
    if(NULL == Config) {
        return XST_FAILURE;
    }

    // Initialise the IIC driver configuration
    Status = XIicPs_CfgInitialize(&Iic, Config, Config->BaseAddress);
    if(Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    //Set the IIC serial clock rate.
    XIicPs_SetSClk(&Iic, IIC_CLK);

    return XST_SUCCESS;
}

void iicWriteReg(unsigned char u8RegAddr, unsigned char u8Data) {
    unsigned char u8TxData[3];

	u8TxData[0] = 0x40;
	u8TxData[1] = u8RegAddr;
	u8TxData[2] = u8Data;

	XIicPs_MasterSendPolled(&Iic, u8TxData, 3, (IIC_SLAVE_ADDR >> 1));
	while(XIicPs_BusIsBusy(&Iic));
}

void audioPllConfig() {
    unsigned char u8TxData[8], u8RxData[6];

	// Disable Core Clock
	iicWriteReg(R0_CLOCK_CONTROL, 0x0E);

	/* 	MCLK = 10 MHz
		R = 0100 = 4, N = 0x02 0x3C = 572, M = 0x02 0x71 = 625

		PLL required output = 1024x48 KHz
		(PLLout)			= 49.152 MHz

		PLLout/MCLK			= 49.152 MHz/10 MHz
							= 4.9152 MHz
							= R + (N/M)
							= 4 + (572/625) */

	// Write 6 bytes to R1 @ register address 0x4002
	u8TxData[0] = 0x40; // Register write address [15:8]
	u8TxData[1] = 0x02; // Register write address [7:0]
	u8TxData[2] = 0x02; // byte 6 - M[15:8]
	u8TxData[3] = 0x71; // byte 5 - M[7:0]
	u8TxData[4] = 0x02; // byte 4 - N[15:8]
	u8TxData[5] = 0x3C; // byte 3 - N[7:0]
	u8TxData[6] = 0x21; // byte 2 - 7 = reserved, bits 6:3 = R[3:0], 2:1 = X[1:0], 0 = PLL operation mode
	u8TxData[7] = 0x01; // byte 1 - 7:2 = reserved, 1 = PLL Lock, 0 = Core clock enable

	// Write bytes to PLL Control register R1 @ 0x4002
	XIicPs_MasterSendPolled(&Iic, u8TxData, 8, (IIC_SLAVE_ADDR >> 1));
	while(XIicPs_BusIsBusy(&Iic));

	// Register address set: 0x4002
	u8TxData[0] = 0x40;
	u8TxData[1] = 0x02;

	// Poll PLL Lock bit
	do {
		XIicPs_MasterSendPolled(&Iic, u8TxData, 2, (IIC_SLAVE_ADDR >> 1));
		while(XIicPs_BusIsBusy(&Iic));
		XIicPs_MasterRecvPolled(&Iic, u8RxData, 6, (IIC_SLAVE_ADDR >> 1));
		while(XIicPs_BusIsBusy(&Iic));
	}
	while((u8RxData[5] & 0x02) == 0); // while not locked

	iicWriteReg(R0_CLOCK_CONTROL, 0x0F);	// 1111
												// bit 3:		CLKSRC = PLL Clock input
												// bits 2:1:	INFREQ = 1024 x fs
												// bit 0:		COREN = Core Clock enabled
}

void lineinLineoutConfig() {

	iicWriteReg(R17_CONVERTER_CONTROL_0, 0x05);//48 KHz
	iicWriteReg(R64_SERIAL_PORT_SAMPLING_RATE, 0x05);//48 KHz
	iicWriteReg(R19_ADC_CONTROL, 0x13);
	iicWriteReg(R36_DAC_CONTROL_0, 0x03);
	iicWriteReg(R35_PLAYBACK_POWER_MANAGEMENT, 0x03);
	iicWriteReg(R58_SERIAL_INPUT_ROUTE_CONTROL, 0x01);
	iicWriteReg(R59_SERIAL_OUTPUT_ROUTE_CONTROL, 0x01);
	iicWriteReg(R65_CLOCK_ENABLE_0, 0x7F);
	iicWriteReg(R66_CLOCK_ENABLE_1, 0x03);

	iicWriteReg(R4_RECORD_MIXER_LEFT_CONTROL_0, 0x01);
	iicWriteReg(R5_RECORD_MIXER_LEFT_CONTROL_1, 0x05);//0 dB gain
	iicWriteReg(R6_RECORD_MIXER_RIGHT_CONTROL_0, 0x01);
	iicWriteReg(R7_RECORD_MIXER_RIGHT_CONTROL_1, 0x05);//0 dB gain

	iicWriteReg(R22_PLAYBACK_MIXER_LEFT_CONTROL_0, 0x21);
	iicWriteReg(R24_PLAYBACK_MIXER_RIGHT_CONTROL_0, 0x41);
	iicWriteReg(R26_PLAYBACK_LR_MIXER_LEFT_LINE_OUTPUT_CONTROL, 0x03);//0 dB
	iicWriteReg(R27_PLAYBACK_LR_MIXER_RIGHT_LINE_OUTPUT_CONTROL, 0x09);//0 dB
	iicWriteReg(R29_PLAYBACK_HEADPHONE_LEFT_VOLUME_CONTROL, 0xE7);//0 dB
	iicWriteReg(R30_PLAYBACK_HEADPHONE_RIGHT_VOLUME_CONTROL, 0xE7);//0 dB
	iicWriteReg(R31_PLAYBACK_LINE_OUTPUT_LEFT_VOLUME_CONTROL, 0xE6);//0 dB
	iicWriteReg(R32_PLAYBACK_LINE_OUTPUT_RIGHT_VOLUME_CONTROL, 0xE6);//0 dB
}

XStatus initAudioController() {
    int status;

    status = iicConfig();
    if(status != XST_SUCCESS) {
        xil_printf("Error initializing IIC\n\r");
        return XST_FAILURE;
    }

	audioPllConfig();
	lineinLineoutConfig();

	xil_printf("Audio configured\n\r");

    return XST_SUCCESS;
}

void waitAudioReady() {
	int ready = 0;
	u32 reg;
	while (!ready) {
		reg = Xil_In32(I2S_STATUS_REG);
		ready = (reg >> 21) & 0x01;
	}

	reg = reg & (u32)(~(1<<21));
	Xil_Out32(I2S_STATUS_REG, reg);
}

void readAudio(audioData *data) {
	u32 inLeft = Xil_In32(I2S_DATA_RX_L_REG);
	u32 inRight = Xil_In32(I2S_DATA_RX_R_REG);

	inLeft = inLeft << 8;
	inRight = inRight << 8;

	data->left = (s32)inLeft >> 8;
	data->right = (s32)inRight >> 8;
}

void writeAudio(audioData *data) {
	Xil_Out32(I2S_DATA_TX_L_REG, data->left);
	Xil_Out32(I2S_DATA_TX_R_REG, data->right);
}