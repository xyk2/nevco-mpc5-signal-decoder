#include <Metro.h>

const uint8_t clock_debug_output_pin = 12;
const uint8_t start_debug_output_pin = 11;

const uint8_t clock_interrupt_pin = 3; // Clock signal from ATTINY85
const uint8_t start_interrupt_pin = 5; // Start signal from ATTINY85
const uint8_t MPC5_input_pin = 6; // Signal from Nevco MPC-5

const uint8_t LED_status = 13;

static volatile uint16_t bit_index = 0; // Increment this on each CLOCK ISR, reset on START ISR
static volatile uint8_t MPC5_bits[260];

Metro serialMetro = Metro(100);

/*
PORTA - 3, 4, 24, 33
PORTB - 1, 16, 17, 18, 19, 25, 32
PORTC - 9, 10, 11, 12, 13, 15, 22, 23, 27, 28, 29, 30
PORTD - 2, 5, 6, 7, 8, 14, 20, 21
PORTE - 26, 31

PORTA is occupied with clock signal IN
*/

/*
	14-17 - 10th of sec
	86-89 - second
	102-105 - 10 sec
*/

void setup() {
	pinMode(clock_debug_output_pin, OUTPUT);
	pinMode(start_debug_output_pin, OUTPUT);
	pinMode(LED_status, OUTPUT);

	pinMode(clock_interrupt_pin, INPUT);
	pinMode(start_interrupt_pin, INPUT);
	pinMode(MPC5_input_pin, INPUT);


	attachInterrupt(clock_interrupt_pin, ISR_SERVICE_ROUTINE_DATA, RISING); // PORTA taken over, porta_isr()
	attachInterrupt(start_interrupt_pin, ISR_SERVICE_ROUTINE_START, RISING);

	Serial.begin(115200);

	//digitalWrite(LED_status, HIGH); // Show init complete
}

void loop() {
	if (serialMetro.check()) { // check if the metro has passed it's interval
		//for(uint16_t x = 0; x < 260; x++)
		//	Serial.print(MPC5_bits[x]);

		cli(); // Disable interupts while writing to serial
		digitalWrite(LED_status, HIGH);

		uint8_t scb_minutes = MPC5_bits[134]*10 + bcd_to_decimal(MPC5_bits[118], MPC5_bits[119], MPC5_bits[120], MPC5_bits[121]);
		uint8_t scb_seconds = bcd_to_decimal(MPC5_bits[102], MPC5_bits[103], MPC5_bits[104], MPC5_bits[105])*10 + bcd_to_decimal(MPC5_bits[86], MPC5_bits[87], MPC5_bits[88], MPC5_bits[89]);
		uint8_t scb_deciseconds = bcd_to_decimal(MPC5_bits[14], MPC5_bits[15], MPC5_bits[16], MPC5_bits[17]);

		uint8_t scb_period = period(MPC5_bits[54], MPC5_bits[55], MPC5_bits[56], MPC5_bits[57]);
		uint8_t scb_shotclock = bcd_to_decimal(MPC5_bits[238], MPC5_bits[239], MPC5_bits[240], 0)*10 + bcd_to_decimal(MPC5_bits[222], MPC5_bits[223], MPC5_bits[224], MPC5_bits[225]);

		uint8_t scb_homescore = MPC5_bits[40]*100 + bcd_to_decimal(MPC5_bits[150], MPC5_bits[151], MPC5_bits[152], MPC5_bits[153])*10 + bcd_to_decimal(MPC5_bits[166], MPC5_bits[167], MPC5_bits[168], MPC5_bits[169]);
		uint8_t scb_guestscore = MPC5_bits[41]*100 + bcd_to_decimal(MPC5_bits[6], MPC5_bits[7], MPC5_bits[8], MPC5_bits[9])*10 + bcd_to_decimal(MPC5_bits[22], MPC5_bits[23], MPC5_bits[24], MPC5_bits[25]);


		Serial.print(scb_minutes);
		Serial.print(":");
		Serial.print(scb_seconds);
		Serial.print(".");
		Serial.print(scb_deciseconds);

		Serial.print(",");
		Serial.print(scb_shotclock);

		Serial.print(","); // Shot clock
		Serial.print(scb_period);

		Serial.print(","); // Home score
		Serial.print(scb_homescore);

		Serial.print(","); // Guest score
		Serial.print(scb_guestscore);

		Serial.println();
		digitalWrite(LED_status, LOW);

		sei(); // Enable interrupts again
	}

}

static uint8_t bcd_to_decimal(uint8_t bit1, uint8_t bit2, uint8_t bit3, uint8_t bit4) {
	uint8_t sum = 0;
	if(bit1) sum += 0b0001;
	if(bit2) sum += 0b0010;
	if(bit3) sum += 0b0100;
	if(bit4) sum += 0b1000;

	if(sum > 9) return 0;
	return sum;
}


static uint8_t period(uint8_t bit1, uint8_t bit2, uint8_t bit3, uint8_t bit4) {
	if(!bit1 && !bit2 && !bit3 && !bit4) return 0;
	if(bit1 && !bit2 && !bit3 && !bit4) return 1;
	if(!bit1 && bit2 && !bit3 && !bit4) return 2;
	if(!bit1 && !bit2 && bit3 && !bit4) return 3;
	if(!bit1 && !bit2 && !bit3 && bit4) return 4;
	return 5;
}

static void ISR_SERVICE_ROUTINE_DATA() {
	//MPC5_bits[bit_index++] = digitalRead(MPC5_input_pin);

	MPC5_bits[bit_index++] = digitalReadFast(6);

	//if(MPC5_bits[bit_index-1]) {
	//	pulsePin(clock_debug_output_pin);
	//}
}

static void ISR_SERVICE_ROUTINE_START() {
	//pulsePin(start_debug_output_pin);
	bit_index = 0;
}

static void pulsePin(uint8_t pin) {
	digitalWriteFast(pin, HIGH);
	delayMicroseconds(1);
	digitalWriteFast(pin, LOW);
}

void porta_isr(void) {
	uint32_t isfr = PORTA_ISFR;
	PORTA_ISFR = PORTA_ISFR;
	ISR_SERVICE_ROUTINE_DATA();
}
