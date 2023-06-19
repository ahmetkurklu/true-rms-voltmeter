# true-rms-voltmeter

## Load specifications
We want to measure the average and RMS (Root Mean Square) value of an arbitrary waveform voltage. To avoid disturbing the operation of the electrical device on which we want to measure this voltage, the input impedance should be greater than 100kΩ (ideally 1MΩ). The voltage to be measured will have an amplitude of ±100Vmax. We aim for an accuracy of approximately 5%. Finally, we want to display these measurements on an LCD screen every second.

## Operating principle

![image](https://github.com/ahmetkurklu/true-rms-voltmeter/assets/24780090/4b925642-c51d-439b-8c95-622794b8d240)

The analog part is used to adapt the voltage to be measured because it can be alternating and therefore have a negative component that the Arduino cannot handle. The Arduino Uno handles the digital part. It must retrieve the output signal from the analog part and perform calculations to display the average and effective values of the input voltage.

### Analog part

The analog part serves, as mentioned earlier, to adapt the voltage signal, and thus the output voltage of this analog part should be between 0 and Vref. It is important to have a high input impedance and a low output impedance. Ideally, multiple voltage ranges and overvoltage protection should be implemented.

Our goal is to have the following relationship:
Vs = α*Ve + β^2

Where:
α is the voltage divider coefficient,
β = Vref/2, and we choose Vref = 5V,
Vs is the output voltage of the analog part,
Ve is the input voltage to be measured.

We have chosen to use an operational amplifier (OP-AMP) circuit combined with a voltage divider for each voltage range (3 ranges: 3V, 30V, and 100V). The circuit diagram is as follows:

![image](https://github.com/ahmetkurklu/true-rms-voltmeter/assets/24780090/ff099065-6e26-4524-9b77-aaa0b038934e)

Here we can clearly see the 3 voltage ranges. Due to not having the appropriate resistances, the voltage ranges are set to 100V, 27.5V, and 2.95V. Unfortunately, we were unable to find a solution to switch between the voltage ranges dynamically. Therefore, to change the voltage range, you will need to manually disconnect and reconnect a cable between the voltage divider and the input of the OP-AMP circuit.

The purpose of the OP-AMP circuit is to amplify the voltage so that only the positive component is present while preserving the negative component of the input voltage. The adder circuit adds the output voltage from the voltage divider (V1 in the diagram, which will be a maximum of 2.5V if the voltage range is selected correctly) with the voltage Vref/2, which is 2.5V. The output voltage of the analog part, Vs, is then sent to the A0 pin of the Arduino board. Therefore, the output of the analog part is a reduced and amplified signal, as shown in the diagram:

![image](https://github.com/ahmetkurklu/true-rms-voltmeter/assets/24780090/158b7ac0-a197-4a5b-8262-774427c04279)


### Numerical part

The objectives of this part are to perform voltage acquisition using the ADC (Analog-to-Digital Converter) of the Arduino UNO at a rate of Te=1ms, and after 1000 acquisitions, calculate the average and effective values of the voltage. Finally, these values will be displayed on an LCD screen.

To implement the digital part, we first focused on the sampling period. To achieve this, we decided to use interrupts and TIMER1. The principle is that TIMER1 will count at a frequency that we set, and every time TIMER1 overflows, which means it reaches its maximum counting value, it will trigger an interrupt that resets TIMER1 to its initial state before counting again.

The CPU of the Arduino UNO has a frequency of 16MHz, and TIMER1 is a 16-bit timer, so it can count up to a maximum of 65536. We want a sampling period Te of 1ms, which corresponds to a frequency of 1kHz. Since we want interrupts on overflow, we need to pre-fill TIMER1 with a value calculated using the following formula:
X = 65536 - 16MHz/Prescaler/DesiredFreq

With a prescaler of 1 and a frequency of 1kHz, we obtain a value of 49536.

We tested the signal's inversion period using an oscilloscope and obtained the following waveform, which clearly shows a Te period of 1ms.

![image](https://github.com/ahmetkurklu/true-rms-voltmeter/assets/24780090/9fe22fa8-d517-4d0d-b3ac-cd0a3dacd76a)


To implement the calibration functionality, we used buttons declared as inputs and conditional statements (if statements). When the program detects that a button is pressed, it modifies the values of the coefficients applied to the measured voltage.

To acquire the voltage value, we used a flag variable that starts at 0 and changes to 1 with each interrupt. When the flag is set to 1, we use the analogRead() function in the main program to read the digital value from the analog pin. We then use this formula to obtain the voltage value.

![image](https://github.com/ahmetkurklu/true-rms-voltmeter/assets/24780090/e7f41d8a-63f4-4114-b982-745b8988373a)

Once Vs is obtained, we apply the appropriate coefficient based on the selected range, and then calculate the average or RMS value using the corresponding formula. We store the calculated results in the variables ResultatMoy (average) and ResultatEff (RMS).

For the display, we took the base program provided in the lab instructions and modified it to show the results. When the number of acquisitions reaches 1000, we divide ResultatMoy and ResultatEff by 1000, and then display the results on the LCD.

![image](https://github.com/ahmetkurklu/true-rms-voltmeter/assets/24780090/adfe1aa8-bf72-4469-aeb3-28ffefc72093)


## Results
Thus, we have achieved the desired result (displaying the average and RMS value of the input voltage). However, not all the requirements of the specifications have been met: there is no protection against overvoltage, and the range selection is not automatic, requiring a manual cable change.
