#ifndef CONFIG_H
	#define CONFIG_H

	//MasterNode--------------------------------------------------------------------------------------------------
	#ifdef MASTER_NODE_CAPE

		//adress and type
		#define MY_ADRESS 0x01
		#define MY_TYPE 0x00

		//pin setup
		#define INIT_SPI_PINS	P1DIR |= BIT0; P2DIR |= BIT0 + BIT3 + BIT4 + BIT5;
		#define LED(val)	(val)?(P1OUT |= BIT0):(P1OUT &= ~BIT0)	//LED2 	Pin 1.0
		#define CE(val)		(val)?(P2OUT |= BIT3):(P2OUT &= ~BIT3)	//CE  	Pin 2.3
		#define CSN(val)	(val)?(P2OUT |= BIT4):(P2OUT &= ~BIT4)	//CSN	Pin 2.4
		#define SCK(val)	(val)?(P2OUT |= BIT5):(P2OUT &= ~BIT5)	//SCK	Pin 2.5
		//#define IRQ(val)	(val)?(P2OUT |= BIT2):(P2OUT &= ~BIT2)	//IRQ	Pin 2.2
		#define MOSI(val)	(val)?(P2OUT |= BIT0):(P2OUT &= ~BIT0)	//MOSI	Pin 2.0
		#define MISO		(P2IN & BIT1)?1:0						//MISO	Pin 2.1
		#define PUSH_BUTTON	(P1IN & BIT3)?1:0						//PSH_BUTTON	Pin 1.3
		#define LED_STATUS	(P1OUT & BIT0)?1:0						//LED_STATUS	Pin 1.0

		#define USE_INTERRUPT 1
		#define ENABLE_INTERRUPT() 	P2IE |= BIT2;  /*2.2 interrupt enabled*/	 P2IES |= BIT2;  /* P2.2 Hi/lo edge */
		#define CLEAR_INTERRUPT()	P2IFG &= ~BIT2;   // P2.2 IFG cleared
		#define DISABLE_INTERRUPT()	P2IE ^= BIT2;
		#define INTERRUPT_VECTOR	PORT2_VECTOR
	#endif


	//SensorNode--------------------------------------------------------------------------------------------------
	#ifdef SENSOR_NODE_CAPE_A

		//address and type
		#define MY_ADRESS 0x03
		#define MY_TYPE 0x03

		//pin setup
		#define INIT_SPI_PINS	P1DIR |= BIT0; P2DIR |= BIT0 + BIT3 + BIT4 + BIT5;
		#define LED(val)	(val)?(P1OUT |= BIT0):(P1OUT &= ~BIT0)	//LED2 	Pin 1.0
		#define CE(val)		(val)?(P2OUT |= BIT3):(P2OUT &= ~BIT3)	//CE  	Pin 2.3
		#define CSN(val)	(val)?(P2OUT |= BIT4):(P2OUT &= ~BIT4)	//CSN	Pin 2.4
		#define SCK(val)	(val)?(P2OUT |= BIT5):(P2OUT &= ~BIT5)	//SCK	Pin 2.5
		//#define IRQ(val)	(val)?(P2OUT |= BIT2):(P2OUT &= ~BIT2)	//IRQ	Pin 2.2
		#define MOSI(val)	(val)?(P2OUT |= BIT0):(P2OUT &= ~BIT0)	//MOSI	Pin 2.0
		#define MISO		(P2IN & BIT1)?1:0						//MISO	Pin 2.1
		#define PUSH_BUTTON	(P1IN & BIT3)?1:0						//PSH_BUTTON	Pin 1.3
		#define LED_STATUS	(P1OUT & BIT0)?1:0						//LED_STATUS	Pin 1.0

		#define USE_INTERRUPT 1
		#define ENABLE_INTERRUPT() 	P2IE |= BIT2;  /*2.2 interrupt enabled*/	 P2IES |= BIT2;  /* P2.2 Hi/lo edge */
		#define CLEAR_INTERRUPT()	P2IFG &= ~BIT2;   // P2.2 IFG cleared
		#define DISABLE_INTERRUPT()	P2IE ^= BIT2;
		#define INTERRUPT_VECTOR	PORT2_VECTOR
	#endif

	#ifdef SENSOR_NODE_B

		//address and type
		#define MY_ADRESS 0x03
		#define MY_TYPE 0x04

		//pin setup
		#define INIT_SPI_PINS	P1DIR |= BIT0 + BIT4 + BIT7; P2DIR |= BIT0 + BIT1 + BIT3;
		#define LED(val)	(val)?(P1OUT |= BIT0):(P1OUT &= ~BIT0)	//LED2 	Pin 1.0
		#define CE(val)		(val)?(P2OUT |= BIT1):(P2OUT &= ~BIT1)	//CE  	Pin 2.1
		#define CSN(val)	(val)?(P2OUT |= BIT0):(P2OUT &= ~BIT0)	//CSN	Pin 2.0
		#define SCK(val)	(val)?(P1OUT |= BIT4):(P1OUT &= ~BIT4)	//SCK	Pin 1.4
		#define IRQ(val)	(val)?(P2OUT |= BIT3):(P2OUT &= ~BIT3)	//IRQ	Pin 2.3
		#define MOSI(val)	(val)?(P1OUT |= BIT7):(P1OUT &= ~BIT7)	//MOSI	Pin 1.2 2.5	1.7
		#define MISO		(P2IN & BIT4)?1:0						//MISO	Pin 1.1 2.4
		#define PUSH_BUTTON	(P1IN & BIT3)?1:0						//PSH_BUTTON	Pin 1.3
		#define LED_STATUS	(P1OUT & BIT0)?1:0						//LED_STATUS	Pin 1.0

		#define USE_INTERRUPT 1
		#define ENABLE_INTERRUPT() 	P2IE |= BIT5;  /*2.5 interrupt enabled*/	 P2IES |= BIT5;  /* P2.5 Hi/lo edge */
		#define CLEAR_INTERRUPT()	P2IFG &= ~BIT5;   // P2.5 IFG cleared
		#define DISABLE_INTERRUPT()	P2IE ^= BIT5;
		#define INTERRUPT_VECTOR	PORT2_VECTOR
	#endif

	#ifdef SENSOR_NODE_C

		//address and type
		#define MY_ADRESS 0x04
		#define MY_TYPE 0x05

		//pin setup
		#define INIT_SPI_PINS	P1DIR |= BIT0 + BIT2 + BIT4; P2DIR |= BIT0 + BIT1 + BIT3;
		#define LED(val)	(val)?(P1OUT |= BIT0):(P1OUT &= ~BIT0)	//LED2 	Pin 1.0
		#define CE(val)		(val)?(P2OUT |= BIT1):(P2OUT &= ~BIT1)	//CE  	Pin 2.1
		#define CSN(val)	(val)?(P2OUT |= BIT0):(P2OUT &= ~BIT0)	//CSN	Pin 2.0

		#define USE_INTERRUPT 0
		#define SCK(val)	(val)?(P1OUT |= BIT4):(P1OUT &= ~BIT4)	//SCK	Pin 1.4
		#define MOSI(val)	(val)?(P1OUT |= BIT2):(P1OUT &= ~BIT2)	//MOSI	Pin 1.2 
		#define MISO		(P1IN & BIT1)?1:0						//MISO	Pin 1.1 
		#define LED_STATUS	(P1OUT & BIT0)?1:0						//LED_STATUS	Pin 1.0

		#define USE_INTERRUPT 1
		#define ENABLE_INTERRUPT() 	P2IE |= BIT5;  /*2.5 interrupt enabled*/	 P2IES |= BIT5;  /* P2.5 Hi/lo edge */
		#define CLEAR_INTERRUPT()	P2IFG &= ~BIT5;   // P2.5 IFG cleared
		#define DISABLE_INTERRUPT()	P2IE ^= BIT5;
		#define INTERRUPT_VECTOR	PORT2_VECTOR
	#endif

#endif
