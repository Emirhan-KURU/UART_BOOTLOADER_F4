#UART_BOOTLOADER_F4

Butona basılı durumda işlemci resetlendiğinde boot moduna girilir. Bu modda 
uart üzerinden gönderilen Intel HEX formatlı dosya işlemcide parse edilerek ilgili flash
adreslerine yazılır. SOnrasında atlama yapılarak yazılan kod çalıştırılır.
Butona basılmadan reset durumunda ise direkt kullanıcı koduna atlama işlemi yapılır.

pyuartstm.py arayüzü kullanılarak istenilen baud rate de intel hex kodu uart üzerinden gönderilebilir.

![Uploading image.png…]()
