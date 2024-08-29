#UART_BOOTLOADER_F4

Butona basılı durumda işlemci resetlendiğinde boot moduna girilir. Bu modda 
uart üzerinden gönderilen Intel HEX formatlı dosya işlemcide parse edilerek ilgili flash
adreslerine yazılır. SOnrasında atlama yapılarak yazılan kod çalıştırılır.
Butona basılmadan reset durumunda ise direkt kullanıcı koduna atlama işlemi yapılır.

pyuartstm.py arayüzü kullanılarak istenilen baud rate de intel hex kodu uart üzerinden gönderilebilir.

![image](https://github.com/user-attachments/assets/71888fd4-2bf9-4c4f-92f2-94e8941d58d1)

