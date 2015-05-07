# What is it #
The KeaDrone is a **three axis** stabilizer/lock used for RC helicopters (Accelerometer **AND** Gyro) . In optimum it should be able to hover your helicopter at a fixed place, even with changing winds.
It can be mounted on several receivers or it can be used with the add-on KeaTalker so you can control the unit directly from your PC or Notebook.

<img width='300' src='http://keadrone.googlecode.com/files/KeaDrone_V0R0_PCB-Top.png'>
PCB Size: 4.7x2.1 cm (1.85x0.82 inches) 24grams including GPS<br>
<br>
It is intended for both the do-it-yourselfer or the regular user that likes Plug-And-Play which will be a preprogrammed version.<br>
<br>
By itself, it can be used to develop a three axis inertial motion sensor (IMU) controller. With the addition of a GPS receiver (connector already present), it can be used to develop a UAV controller for an RC car, plane, or boat since it features multiple outputs for Servo's and a 2 times direct electrical motor controller.<br>
You can program the controller with every in-circuit programmer like PicKit2 (~30$).<br>
<br>
<hr />
This project is fully open-source and based on a self developed PCB. Most of the parts come from SparkFun like GPS, Gyro, Accelerator sensors and Zigbee.<br>
<br>
The PCB schematic will be posted soon and it will be possible to buy the PCB pre-assembled or in DIY package. But first, i have to get it working at least a little bit.<br>
And no worries, I do not need to make money out of this hobby project :). So it will be cheap and the basic setup (gyro+accelerometers+PCB) should be less than 50$ for the all-in package (PCB/IC's/ etc.), don't pin me on the price, it is just to give you an indication.<br>
<br>
For the ones that knows the "E-Sky Lama V3", the custom developed PCB has the exact same sizes of the controller unit. This means you can (re)place the original PCB really like "plug and play". No need to solder anything.<br>
<br>
<hr />
<h1>Where to start</h1>
<ul><li>Get a (pre-assembled) KeaDrone PCB (<a href='http://keadrone.freeforums.org/keadrone-pcb-t3.html'>HERE</a>)<br>
</li><li>If you haven't got a Helicopter yet, buy a E-Sky Lama V3 (~70$) <a href='http://www.google.nl/search?q=E-Sky+Lama+V3'>SEARCH</a> shops.<br>
</li><li>Install it (very easy, no soldering: just connectors)<br>
</li><li>Help the embedded firmware development or PC software development by joining. Send me a private message (PM) though the Forum (my username: willyp ) that you want to assist.</li></ul>

<h1>WANTED</h1>
I can use some <b>serious</b> help with this project:<br>
<ul><li>Programmers (Embedded C for PIC24H hardware, C# for PC Application)<br>
</li><li>Project description/issues maintainer<br>
</li><li>And more things...</li></ul>

<h1>Ultimate project goal</h1>

<ul><li>Though 3G mobile phone, you have live data from the helicopter on your PC like: Images/Video, altitude, temperature, speed and so on.<br>
</li><li>Use the helicopter for surveillance and make a landing plate where the helicopter can recharge itself (by ether Induction or touching wires)<br>
</li><li>Let your helicopter fly though way-points created e.g. in Google Maps.<br>
</li><li>Use a program for this that also calculates the estimate flight time for your way-points to make sure it can always come back before battery runs out.<br>
</li><li>Avoid crashing to objects By means of infrared AND/OR ultrasound sensors</li></ul>

<h2>First project milestone:</h2>
<ul><li>Try to hover the helicopter without the need of the human controls (yes, it sounds Soooo easy, but it isn't)</li></ul>

<h1>Why this project</h1>
There where three reasons why I started this project.<br>
<br>
The first thing is that I want to mount a wireless camera to the helicopter. But since the original Lama V3 includes only a simple Yaw lock, you need to continuously correct the helicopter pitch and roll movements outside (Pitch = Forward/Backward, Roll = Left/Right, Yaw = rotation against the center of the helicopter).<br>
So you can't get stable video's or even look at the video screen while you're flying since you continuously need to focus on the helicopter stability.<br>
<br>
The second thing was that I want to move the helicopter according to a predefined route (like, go around the house for inspection). For this I need GPS, which is included on the KeaDrone as a add-on.<br>
<br>
The last thing, what actually caused that I seriously started this project, was that I had to learn a new PCB CAD program for my job called "Altium Designer". And instead of making dummy PCB's from the given tutorials from Altium Designer, I created this KeaDrone PCB.<br>
<br>
This caused that I learned all of the ins and outs from Altium like: Creating a custom component PCB/Schematic, using SVN for Hardware, using the integrated library's, auto-router options and much more. And off course I learned the whole work-flow: from start (schematic) to the end (Gerber files).<br>
<br>
<h1>Why E-Sky Lama V3</h1>
I chose this helicopter as a reference for this project because of it's low price. Around 70$ you have a fully ready to fly kit incl. remote, motors, etc.<br>
And since it is sold a lot, you can find a lot of (cheap) spares too on many shops (even Ebay!). This makes it for me an ideal candidate to start experimenting without the need of a lot of money.<br>
<br>
So the main advantages are:<br>
<ul><li>Cheap spare parts<br>
</li><li>Easy to master/fly<br>
</li><li>Can lift some weight<br>
</li><li>Reasonable flight time (around 10 min.)<br>
</li><li>Looks nice</li></ul>

<img src='http://keadrone.googlecode.com/files/img_lama-v3.jpg' />

<h1>The Kea Family PCB's</h1>
The KeaDrone can be seen as the "MainBoard" for flying a helicopter. The intention of the Kea Family PCB's is that it can be easily attached to each other.<br>
For now we only have the MainBoard "KeaDrone" and the wireless unit "KeaTalker" which allows you to connect your helicopter wireless to your PC/Notebook.<br>
<br>
<h1>KeaDrone hardware features</h1>
The MainBoard comes with a (40MIPS) PIC24HJ64GP204, a 2x times two axis high accuracy gyro (LPY5150) and a 3axis Accelerometer (ADXL345B). The power (current) of both the motor's drivers can be accurately measured at high speed (over 1000 times the second).<br>
It can be used to detect a rotor-block (e.g. in case of a helicopter crash) and therefore prevents the engines from blowing.<br>
<br>
The used processor includes a Peripheral Pin Select feature where you can remap <b>every</b> pin according to your needs. It allows you to select in <b>software</b> what each pin features like, SPI/I2C/UART/PWM/Etc. Since a break-out header is included in the design, you can always choose whatever you need on this breakout.<br>
<br>
<h1>Why PIC24HJ64GP204</h1>
Because it is fast, easy and cheap to program, it uses less power than the 30F 5V family (50% less!) and there is a free C compiler from Microchip available. And most of all, it has the Peripheral pin select feature which is an excellent tool in my opinion for development boards.<br>
<br>
<h1>What is with the "Kea" thing?</h1>
Kea is a type of bird. Kea's are known for their intelligence and curiosity, both vital to their survival in a harsh mountain environment. Kea can solve logical puzzles, such as pushing and pulling things in a certain order to get to food, and will work together to achieve a certain objective.<br>
<br>
More information can be found at <a href='http://en.wikipedia.org/wiki/Kea'>WikiPedia</a>

<h1>Donations</h1>
If you want to contribute, please feel free to<br>
<a href='https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=FT6FBEB5RZGSG&lc=US&item_name=KeaDrone&currency_code=EUR&bn=PP%2dDonationsBF%3abtn_donate_SM%2egif%3aNonHosted'>donate</a>. It will support the project!<br>
<br>
<a href='https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=FT6FBEB5RZGSG&lc=US&item_name=KeaDrone&currency_code=EUR&bn=PP%2dDonationsBF%3abtn_donate_SM%2egif%3aNonHosted'><img src='https://www.paypal.com/en_US/i/btn/btn_donate_SM.gif' /></a>


<hr />

This below is what I have to write out for the front-page<br>
<ol><li>The KeaDrone PCB<br>
</li><li>What is it?<br>
</li><li>How?<br>
</li><li>Why?<br>
</li><li>KeaDrone hardware specs<br>
</li><li>The KeaTalker PCB features<br>
</li><li>Where to start?<br>
</li><li>Why E-Sky Lama V3</li></ol>


More info about my old projects can be found at <a href='http://www.wnpd.nl'>http://www.wnpd.nl</a>