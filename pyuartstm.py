import sys
import serial
import time
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QHBoxLayout, QLabel, QLineEdit, QPushButton, QFileDialog, QComboBox, QTextEdit, QMessageBox, QProgressBar

class UartSender(QWidget):
    def __init__(self):
        super().__init__()
        self.file_name = None  # Dosya seçilmediğinde None olacak
        self.initUI()

    def initUI(self):
        # Ana layout: yatay (horizontal)
        main_layout = QHBoxLayout()

        # Sol taraf: dosya seçimi ve gönderme seçenekleri
        left_layout = QVBoxLayout()

        # Dosya seçimi için Label ve Button
        self.file_label = QLabel('Seçilen Dosya: ')
        self.file_button = QPushButton('Dosya Seç')
        self.file_button.clicked.connect(self.select_file)

        # Dosya uzantısı girişi için Label ve LineEdit
        self.ext_label = QLabel('Dosya Uzantısı: ')
        self.ext_input = QLineEdit()

        # Baud rate girişi için Label ve LineEdit
        self.baud_label = QLabel('Baud Rate: ')
        self.baud_input = QLineEdit()
        self.baud_input.setText('115200')  # Varsayılan baud rate

        # Port seçimi için Label ve ComboBox
        self.port_label = QLabel('Port Seçimi: ')
        self.port_combo = QComboBox()
        self.port_combo.addItems(['/dev/ttyUSB0', '/dev/ttyUSB1', '/dev/ttyS0'])  # İhtiyaca göre portları ekleyin

        # Gönderme butonu
        self.send_button = QPushButton('Gönder')
        self.send_button.clicked.connect(self.send_file)

        # Oku butonu
        self.read_button = QPushButton('Oku')
        self.read_button.clicked.connect(self.read_data)

        # İlerleme çubuğu ve etiket
        self.progress_bar = QProgressBar()
        self.progress_bar.setTextVisible(True)
        self.progress_label = QLabel('Veri Gönderiliyor: 0%')

        # Sol layout elemanlarını ekle
        left_layout.addWidget(self.file_label)
        left_layout.addWidget(self.file_button)
        left_layout.addWidget(self.ext_label)
        left_layout.addWidget(self.ext_input)
        left_layout.addWidget(self.baud_label)
        left_layout.addWidget(self.baud_input)
        left_layout.addWidget(self.port_label)
        left_layout.addWidget(self.port_combo)
        left_layout.addWidget(self.send_button)
        left_layout.addWidget(self.read_button)
        left_layout.addWidget(self.progress_bar)
        left_layout.addWidget(self.progress_label)

        # Sağ taraf: Verilerin gösterileceği alan
        self.data_output = QTextEdit()
        self.data_output.setReadOnly(True)
        self.data_output.setMinimumWidth(600)  # Genişliği artır
        self.data_output.setMinimumHeight(400)  # Yüksekliği artır

        # Ana layout'a sol ve sağ layout'u ekle
        main_layout.addLayout(left_layout)
        main_layout.addWidget(self.data_output)

        self.setLayout(main_layout)
        self.setWindowTitle('UART Dosya Gönderici')
        self.show()

    def select_file(self):
        options = QFileDialog.Options()
        options |= QFileDialog.ReadOnly
        file_name, _ = QFileDialog.getOpenFileName(self, "Dosya Seç", "", "Tüm Dosyalar (*);; Hex Dosyaları (*.hex)", options=options)
        if file_name:
            self.file_label.setText(f'Seçilen Dosya: {file_name}')
            self.file_name = file_name

    def send_file(self):
        try:
            if not self.file_name:  # Dosya seçilmediyse
                self.show_error("Dosya seçilmedi. Lütfen bir dosya seçin.")
                return

            file_ext = self.ext_input.text()
            baud_rate = int(self.baud_input.text())
            port_name = self.port_combo.currentText()

            if not self.file_name.endswith(file_ext):
                raise ValueError("Dosya uzantısı hatalı!")

            with open(self.file_name, 'r') as file:
                lines = file.readlines()

            ser = serial.Serial(port_name, baud_rate)

            total_lines = len(lines)
            for idx, line in enumerate(lines):
                ser.write(line.encode())
                time.sleep(0.02)  # 20 ms bekleme süresi

                # İlerleme çubuğunu güncelle
                progress = int((idx + 1) / total_lines * 100)
                self.progress_bar.setValue(progress)
                self.progress_label.setText(f'Veri Gönderiliyor: {progress}%')
                QApplication.processEvents()  # GUI'nin güncellenmesi için

            # Dosya bittiğinde özel bir bit dizisi gönder
            ser.write(b'<EOF>')  # Özel dosya sonu karakter dizisi
            ser.close()

            self.progress_bar.setValue(100)
            self.progress_label.setText('Gönderildi!')
            print("Dosya başarıyla gönderildi.")

        except Exception as e:
            self.show_error(f"Hata: {e}")

    def read_data(self):
        try:
            if not self.file_name:  # Dosya seçilmediyse
                self.show_error("Dosya seçilmedi. Lütfen bir dosya seçin.")
                return

            # Dosyayı oku ve hex formatına çevir
            formatted_data = ""
            with open(self.file_name, 'r') as file:
                address = 0
                for line in file:
                    if line.startswith(':'):
                        # Intel HEX formatında veri kısmını çıkart
                        line = line.strip()
                        byte_count = int(line[1:3], 16)
                        address = int(line[3:7], 16)
                        record_type = int(line[7:9], 16)
                        data = line[9:-2]  # Son iki karakter kontrol toplamıdır

                        # Veriyi byte'lara ayır
                        hex_data = [data[i:i+2] for i in range(0, len(data), 2)]

                        # Formatlı bir biçimde hizalanmış şekilde ekrana yaz
                        formatted_data += f"{address:04X}    "  # Adres
                        for i in range(0, len(hex_data), 16):
                            line_data = ' '.join(hex_data[i:i+16])  # 16 byte'lık bloklar halinde veri
                            formatted_data += f"{line_data:<47}\n"  # 47 karakterlik hizalama
                            address += 16

            # Ekrana yazdır
            self.data_output.clear()
            self.data_output.append(formatted_data.upper())

        except Exception as e:
            self.show_error(f"Hata: {e}")

    def show_error(self, message):
        msg_box = QMessageBox()
        msg_box.setIcon(QMessageBox.Critical)
        msg_box.setWindowTitle("Hata")
        msg_box.setText(message)
        msg_box.exec_()

if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = UartSender()
    sys.exit(app.exec_())
