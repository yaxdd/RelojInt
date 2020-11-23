;************************************************************************
; Tmanaño de la lcd es de 2 por 16 caracteres
; usar bandera de ocupado por lo que en la señal R/W la voy a conetar a un pin
;negativo = PIN1    ;;   vss = 0v  ;; 
;positivo   =   pin 2    ;; vdd=5v
;varible = pin 3  varia entre 0 a 5v el cual es el contraste
;Pb5 = pin 4  ;;  rs = portA pin 0
;PB6 = pin 5 R/W portA pin 1
;PB7 = PIN 6 E PORTA PIN 0

;PB0 = PIN 7 DB0 PORT D PIN 0
;PB1 = PIN 8 DB0 PORT D PIN 1
;PB2 = PIN 9 DB0 PORT D PIN 2
;PB3 = PIN 10 DB0 PORT D PIN 3
;PB4 = PIN 11 DB0 PORT D PIN 4
;PB5 = PIN 12 DB0 PORT D PIN 5
;PB6 = PIN 13 DB0 PORT D PIN 6
;PB7 = PIN 14 DB0 PORT D PIN 7

;;;; 32 CARACTERES CONFIGURADOS EN DOS FILAS DE 16 
;DIREECECION FILA1 00 01 02 03 04 ... 0F
;DIREECECION FILA2 40 41 42 43 44 ... 4F
;;************************************************************

GPIO_PORT_D EQU 0X4005B000     ;DIRECCION BASE DEL PUERTO D
GPIO_PORT_A EQU 0x40061000     ;DIRECCION BASE DEL PUERTO A
;*******************************************************************	

MASKESC_A EQU 0X1C 				; MASCARA DE ESCRITURA 3 LSB BITS PUERTO A
MASKESC_D EQU 0X3FC 			; MASCARA DE ESCRIBIR PARA PUERTO D  10 BITS
;----------------------------------------------------------------------------
DIR 	EQU 0X400 				;  DEZPLAZAMIENTO PARA FIJAR EL FLUJO DE DATOS
AFSEL 	EQU 0X420 				; DESPLAZZAMIENTO PARA FIJAR ""ALTERNATIVE FUNCTION SELECT
DEN 	EQU 0X51C 				; DESPLAZAMIENTO PARA EL DIGITAL ENABLE
DR2R 	EQU 0X500 				; DESPLAZAMIENTO PARA FIJAR CANTIDAD DE CORIENTE 4ma
DR8R 	EQU 0X508 				;DESPLAZAMIENTO PARA FIJAR CANTIDAD DE CORIENTE 8ma
LOCK 	EQU 0X520 				;  DESBLOQUEAR PUERTO D PIN7
CR 		EQU 0X524   			; DESBLOQUEAR PUERTO D PIN7
;-------------------------------------------------------------------
; DIRECCION PARA ACTIVAR LOS PUERTOS Y VERIFICAR SU FUNCIONAMIENTO
;---------------------direcciones de sistema para activar los puertos--------------------

SYSCTL_RCGCGPIO EQU 0X400FE608 ; DIRECCION PARA ACTIVAR PUERTOS
SYSCTL_PRGPIO 	EQU 0X400FEA08 ; DIRECCION PARA VERIFICAR SI EL PUERTO ESTA LISTO
	
VAL_ACTI_PORTA EQU 0X01 ; VALOR PARA ACTIVAR EL PUERTO A
VAL_ACTI_PORTD EQU 0X0C ; VALOR PARA ACTIVAR EL PUERTO D


		
;--- REGISTROS PARA ACTIVAR SYSTICK-------------------

SYSCTL_STCURRENT 	EQU 0XE000E018	;valor concurrente
SYSCTL_STRELOAD 	EQU 0XE000E014	;valor de recarga
SYSCTL_STCTRL 		EQU 0XE000E010	;control
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;--------------------------------------------------

		AREA |.text|, CODE ,READONLY, ALIGN=2
			THUMB
				
			EXPORT 	UP_LCD		
			EXPORT	LCD_DELAY
			EXPORT	LCD_E
			EXPORT	LCD_BUSY
			EXPORT	LCD_DATO
			EXPORT	LCD_INI 
			EXPORT	LCD_REG
			EXPORT	FUNCION_SET
			EXPORT	BORRA_Y_HOME
			EXPORT	DISPLAY_ON_CUR_OFF
			EXPORT	RETURN_HOME
			EXPORT	SET_CURSOR
;;;;;;;;;;;;;;;;;;;;;;--------------------- INICIALIZAR PUERTO D---------------;;;;;;;;;;;;;;;
;UP_LCD-> CONFIGURA EL PUERTO D COMO SALIDA. BUS DE DATOS DEL LCD 8BITS
;;;;;;;;;;;;;;;;;;;;;----------------------CONFIGURA PUERTO D COMO SALIDA -----------------------------------;;;;;;;;;;;;;;;;;;;;;;;;


UP_LCD
		;PRENDO EL PUERTO D---------------------------
		PUSH {R0,R1,R2,LR}				;SALVO EN LA PILA LO REGISTROS QUE VOY A MODIFICAR
		
		LDR R1,=SYSCTL_RCGCGPIO 		;TOMO LA DIRECCION SYSCTL_RCGCGPIO 
		LDR R0,[R1]						;LEO EL REGISTRO					
		ORR R0,R0,#VAL_ACTI_PORTD		;PENDO EL PUERTO D
		STR R0,[R1]						;LO ALMACENO EN EL REGISTRO
		
		;VERIFICA SI EL PUERTO D ESTA LISTO PARA PROGRAMAR-------
		LDR R1, =SYSCTL_PRGPIO			;TOMO LA DIRECCIÓN DE SYSCT_PRGPIO
CHECA
		LDR R0, [R1]					;LEO EL REGISTRO
		ANDS R0,R0, #VAL_ACTI_PORTD		;HAGO LA OPERACIÓN AND PARA DERETMINAR SI ESTA EN UNO
		BEQ CHECA
		
;---------------- DESBLOQUEAR PIN7 PUERTO D ----------------------------------
		LDR R2,=GPIO_PORT_D		;GUARDO EN R2 LA DIRECCIÓN BASE DEL PUERTO D
		LDR R1,[R2,#LOCK] 		;A LA DIR BASE SUMALE DESPLASAMIENTO DE LOCK Y LO LEO
		MOV32 R0, #0X4C4F434B	;MUEVO VALOR RECOMENDADO
		STR R0 ,[R1]			;LO ALMACENO
;----------------------------------------------------------------------------------
VERF
		;;; VERIFICA SI SE DESBLOQUEO
		LDR R2,=GPIO_PORT_D	
		LDR R0,[R2,#LOCK]
		ANDS R0,#0X001
		BNE VERF
;-------------------------------------------------------------------------------		
		;DESBLOQUEO EL PIN 7
		LDR R2,=GPIO_PORT_D	
		LDR R0,[R2,#CR]
		ORR R0,#0X080
		STR R0, [R2]
;--------------PUERTO DE PINES DEL 0 AL 7 COMO SALIDA-----------------------------
		;PONGO EL PUERTO D COMO SALIDA
		LDR R2,=GPIO_PORT_D	
		LDR R0,[R2,#DIR]
		ORR R0, #0XFF
		STR R0, [R2,#DIR]
;---------------FUNCIÓN ALTERNATIVA COM PUERTO DIGITAL NORMAL--------------------
		LDR R2,=GPIO_PORT_D	
		LDR R0,[R2,#AFSEL]
		BIC R0, #0XFF
		STR R0, [R2,#AFSEL]
;------------CONFIGURAR PUERTO D COMO DIGITAL-----------------------------------------------------
		LDR R2,=GPIO_PORT_D	
		LDR R0,[R2,#DEN]
		ORR R0, #0XFF
		STR R0, [R2,#DEN]
;--------------DESACTIVO CORRIENTE DE 2mA----------------------------------------------
		LDR R2,=GPIO_PORT_D	
		LDR R0,[R2,#DR2R]
		BIC R0, #0XFF
		STR R0, [R2,#DR2R]
;---------------ACTIVA CORRIENTE DE 8 mA------------------------
		LDR R2,=GPIO_PORT_D	
		LDR R0,[R2,#DR8R]
		ORR R0, #0XFF
		STR R0, [R2,#DR8R]
;----------------------------------------------------------------
; BUS DE CONTROL EL PUERTO A  A0=RS,  A1 =W/R, A2=E
;----------------------------------------------------------------
		;PRENDO EL PUETO A
		LDR R1,=SYSCTL_RCGCGPIO 		;TOMO LA DIRECCION SYSCTL_RCGCGPIO 
		LDR R0,[R1]						;LEO EL REGISTRO					
		ORR R0,R0,#VAL_ACTI_PORTA		;PENDO EL PUERTO D
		STR R0,[R1]						;LO ALMACENO EN EL REGISTRO
;-----------------------------------------------------------------------------------		
		;VERIFICA SI EL PUERTO A ESTA LISTO PARA PROGRAMAR-------
		LDR R1, =SYSCTL_PRGPIO			;TOMO LA DIRECCIÓN DE SYSCT_PRGPIO
CHECA1
		LDR R0, [R1]					;LEO EL REGISTRO
		ANDS R0,R0, #VAL_ACTI_PORTA		;HAGO LA OPERACIÓN AND PARA DERETMINAR SI ESTA EN UNO
		BEQ CHECA1
;---------PONGO AL PUERTO A COMO SALIDA----------------------------------------------------
		LDR R2,=GPIO_PORT_A	
		LDR R0,[R2,#DIR]
		ORR R0, #0XFF
		STR R0, [R2,#DIR]
;--------------FUNCION ALTERNATIVA COMO DIGITAL NORMAL--------------------------------------
		LDR R2,=GPIO_PORT_A	
		LDR R0,[R2,#AFSEL]
		BIC R0, #0XFF
		STR R0, [R2,#AFSEL]
;------------PONGO PUERTO COMO DIGITAL----------------------------------------------
		LDR R2,=GPIO_PORT_A
		LDR R0,[R2,#DEN]
		ORR R0, #0XFF
		STR R0, [R2,#DEN]
;--------------DESACTIVO CORRIENTE DE 2mA----------------------------------------------
		LDR R2,=GPIO_PORT_A
		LDR R0,[R2,#DR2R]
		BIC R0, #0XFF
		STR R0, [R2,#DR2R]
;---------------ACTIVA CORRIENTE DE 8 mA------------------------
		LDR R2,=GPIO_PORT_A	
		LDR R0,[R2,#DR8R]
		ORR R0, #0XFF
		STR R0, [R2,#DR8R]
;----------------------------------------------------------------
		LDR R2,=GPIO_PORT_A	
		MOV R0, #0X01
		STR R0, [R2, #MASKESC_A]
		BL LCD_INI
		POP  {R0,R1,R2,LR}
		BX LR
;*************************************************************************************
;RUTINA LCD_DELAY SE TRATA DE UNA RUTINA QUE IMPLEMENTA UN RETARDO 
;DE 5 ms 
;----------------------------------------------------------------------------
LCD_DELAY		
		
		LDR R0, =SYSCTL_STCTRL 
		LDR R1, [R0]
		AND R1, #0X000
		STR R1, [R0]
;------------------------VALOR QUE VOY A CARGAR ----------------------------
		LDR R0, =SYSCTL_STRELOAD  
		MOV R1, #0X0EA5F
		STR R1, [R0]
;------------------PONGO A CEROS EL VALOR CONCURRENTE-------------------------		
		LDR R0, =SYSCTL_STCURRENT  
		MOV R1, #0X000
		STR R1, [R0]
;------------------PENDO EL SYSTICK------------------------------------------
		LDR R0, =SYSCTL_STCTRL 
		LDR R1, [R0]
		ORR R1, #0X0001
		STR R1, [R0]
;--------------------------EPERO SE DESBORDE----------------------------------
		LDR R0, =SYSCTL_STCTRL 
L1
		LDR R1, [R0]
		ANDS R1, #0X0001
		CBNZ R1, salta
		B	L1
salta
		BX LR
;*****************PORTA0=>RS, SI RS=1 DATO, SI RS=0 COMANDO,  PORTA1=>RD/RW, PORTA2=>E*****	
;FUNCIÓN QUE HABILITA EL LCD
;---------------------------------------------------------------------------------------
LCD_E
		LDR R0, =GPIO_PORT_A
		LDR R1, [R0, #MASKESC_A]
		ORR R1, #0X04
		STR R1, [R0, #MASKESC_A]
		NOP
		NOP
		NOP
		BIC	R1, #0X04
		STR R1, [R0, #MASKESC_A]
		POP {R0, R1}
		BX LR
;***********************************************************************
;LCD_BUSY LECTURA DE BANDERA DE OCUPADO Y LA DIRECCIÓN
;RS=0 DEBE ESTAR EN MODO COMADO
;-----------------------------------------------------------------------
LCD_BUSY

;------PONER LCD EN MODO LECTURA RD=1******POTRA0=>RS,POTRA1=>RD/RW, POTRA2=>E
		;COLOCO EN UMO RD=1
		LDR R0, =GPIO_PORT_A
		LDR R1, [R0, #MASKESC_A]
		ORR R1, #0X02
		STR R1, [R0, #MASKESC_A]
;----------------CONFIGURO EL PUERTO D COMO ENTRADA--------------------------------
		LDR R2,=GPIO_PORT_D	
		LDR R0,[R2,#DIR]
		BIC R0, #0XFF
		STR R0, [R2,#DIR]
;---ACTIVAR EL ENABLE  E=1--------------------------------------------------------
		LDR R0, =GPIO_PORT_A
		LDR R1, [R0, #MASKESC_A]
		ORR R1, #0X04
		STR R1, [R0, #MASKESC_A]
;-------------------LEER EL PUERTO D---------------------------------------------
L2
		LDR R0, =GPIO_PORT_D
		LDR R1, [R0, #MASKESC_D]
		ANDS R1, #0X80
		CBZ R1, CONT1
		B L2
;------------YA SE DESOCUPO ---------------------
 ;---DESACTIVAR EL ENABLE  E=0--------------
CONT1
		LDR R0, =GPIO_PORT_A
		LDR R1, [R0, #MASKESC_A]
		BIC R1, #0X04
		STR R1, [R0, #MASKESC_A]
;----------------CONFIGURO EL PUERTO D COMO SALIDA--------------------------------
		LDR R2,=GPIO_PORT_D	
		LDR R0,[R2,#DIR]
		ORR R0, #0XFF
		STR R0, [R2,#DIR]
;------PONER LCD EN MODO LECTURA RD=0******POTRA0=>RS,POTRA1=>RD/RW, POTRA2=>E
		;COLOCO EN UMO RD=1
		LDR R0, =GPIO_PORT_A
		LDR R1, [R0, #MASKESC_A]
		BIC R1, #0X02
		STR R1, [R0, #MASKESC_A]
		BX LR
;---------------------------FUNCIO PARA EMVIAR DATOS--------------------
;	RS=0 RD/RW=0 E=0
;-----------------------------------------------------------------------
LCD_DATO
		LDR R0, =GPIO_PORT_A
		LDR R1, [R0, #MASKESC_A]
		BIC R1, #0X01
		STR R1, [R0, #MASKESC_A]
;--------PARA QUE ESTO FUNCIONE EL DATO A IMPRIMIR EN LA PANTALLA---
;TIENE QUE VENIR EN EL REGISTRO R6
;-------------------------------------------------------------------
		PUSH {LR}
		BL LCD_BUSY
		POP {LR}
		LDR R2, =GPIO_PORT_D
		STR R6, [R2, #MASKESC_D]
		
		LDR R0, =GPIO_PORT_A
		LDR R1, [R0, #MASKESC_A]
		ORR R1, #0X01
		STR R1, [R0, #MASKESC_A]
		B LCD_E
;------------FUNCIÓN  PARA ENVIAR UN COMANDO----------------
LCD_REG
		LDR R0, =GPIO_PORT_A
		LDR R1, [R0, #MASKESC_A]
		BIC R1, #0X01
		STR R1, [R0, #MASKESC_A]		
		PUSH {LR}
		BL LCD_BUSY
		POP {LR}		
		LDR R2, =GPIO_PORT_D
		STR R6, [R2, #MASKESC_D]
		B LCD_E
;**********************************************************************
;Funcion que inicializa el lcd
;----------------------------------------------------------------------
;LCD_INI  INICIALIZA EL LCD

LCD_INI 
		PUSH {LR}
		LDR R0, =GPIO_PORT_A
		LDR R1, [R0, #MASKESC_A]
		BIC R1, #0X01
		STR R1, [R0, #MASKESC_A]	

		LDR R2, =GPIO_PORT_D
		MOV R0, #0X30
		STR R0, [R2, #MASKESC_D]
		BL LCD_E
		BL LCD_DELAY
;---------------------------------------------------------------------------------
		LDR R0, =GPIO_PORT_A
		LDR R1, [R0, #MASKESC_A]
		BIC R1, #0X01
		STR R1, [R0, #MASKESC_A]	

		LDR R2, =GPIO_PORT_D
		MOV R0, #0X30
		STR R0, [R2, #MASKESC_D]
		BL LCD_E
		BL LCD_DELAY
;---------------------------------------------------------------
		LDR R0, =GPIO_PORT_A
		LDR R1, [R0, #MASKESC_A]
		BIC R1, #0X01
		STR R1, [R0, #MASKESC_A]	

		LDR R2, =GPIO_PORT_D
		MOV R0, #0X30
		STR R0, [R2, #MASKESC_D]
		BL LCD_E
		BL LCD_DELAY
		POP {LR}
		BX LR
;*********************************************************************
; FUNCION SET:  Interfas de 8 bits DL=1, número de líneas dos N=1, matriz 5x8
;		0 0 1 DL N F--   =  0011 1000 =0x38
;--------------------------------------------------------------------------
FUNCION_SET
		PUSH {LR}
		MOV R6, #0X38
		BL LCD_REG
		POP {LR}
		BX LR
;---------------------------------------------------------------------------
;BORRA_Y_HOME:		 Borra el display y lo pone en en posición 0
;
BORRA_Y_HOME
		PUSH {LR}
		MOV R6, #0X01
		BL LCD_REG
		POP {LR}
		BX LR
;---------------------------------------------------------
;DISPLAY ON CURSOR OF

DISPLAY_ON_CUR_OFF

		PUSH {LR}
		MOV R6, #0X0E
		BL LCD_REG
		POP {LR}
		BX LR
;-----------------------------------------------------------
RETURN_HOME
		PUSH {LR}
		MOV R6, #0X02
		BL LCD_REG
		POP {LR}
		BX LR
;---------------------------------------------------------
SET_CURSOR
		PUSH {LR}
		MOV R6, #0X10
		BL LCD_REG
		POP {LR}
		BX LR
;---------------------------------------------------------
	ALIGN
	END
