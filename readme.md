# Arduino Casino Simulator 🎰

This project is a **roulette and slot machine simulator/game built on Arduino**.

I built this project using the following components:
- Arduino Uno R3  
- 0.96" OLED display (4-pin, blue screen)  
- LCD1602 display  
- 5V magnetic buzzer  
- 1 RGB LED (4 pins)  
- 3 × 220Ω resistors  
- 1 push button  
- 2kΩ resistor (used instead of a potentiometer)  
- Jumper wires (male-to-male and female-to-male)

I used **all Arduino pins except D0, D1, D2, and D3**.

For the LCD contrast, I did not use a potentiometer.  
Instead, I connected a **2kΩ resistor between V0 and GND** to lower the contrast, since I did not have a potentiometer available.

![2k Ohm Resistor](images/2k%20ohm%20resistor.jpeg)



## Games Included

This project includes **two games**:
1. Roulette  
2. Slot Machine  



## Main Menu

When the Arduino starts, a **main menu** appears asking which game you want to play:
- **Short press** → Roulette  
- **Long press** → Slot Machine  

![Casino Main Menu](images/casino%20main%20menu.jpeg)



## Gameplay & Session Tracking

During gameplay, the **LCD1602 display shows the number of wins and losses** for the current session.

After **4 games**, the system asks:
- **Short press** → Play again  
- **Long press** → Go back to the main menu  

![Session Reset](images/session%20reset.jpeg)



# Sound & Light Output

- **Win**
  - Buzzer plays a “ding ding ding” sound  
  - RGB LED flashes different colors  

- **Loss**
  - Buzzer plays a sad sound  
  - RGB LED lights up red  

This feedback makes the game more interactive and enjoyable.



