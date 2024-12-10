# SMART FARM

## MADE BY KELOMPOK 22
- [Raihan Muhammad Ihsan](https://github.com/patuyyy) (2206028232)
- [Faruq Sami Ramadhan](https://github.com/Faruqsamr) (2206026675)
- [Salahuddin Zidane Alghifari](https://github.com/zidane06-sa) (2206028200)
- [Rifqi Ramadhan](https://github.com/Rifqi-Ramadhan) (2206062964)


## TABLE OF CONTENTS
- [INTRODUCTION](#introduction)
- [HARDWARE DESIGN AND IMPLEMENTATION](#hardware-design-and-implementation)
- [SOFTWARE IMPLEMENTATION](#software-implementation)
- [TEST RESULTS AND PERFORMANCE EVALUATION](#test-results-and-performance-evaluation)
- [CONCLUSION AND FUTURE WORK](#conclusion-and-future-work)


## INTRODUCTION

### PROBLEM
Manajemen sumber daya air dan pemantauan kondisi lingkungan merupakan tantangan utama dalam pertanian modern, karena memastikan tanaman mendapatkan perawatan optimal sering terkendala oleh kesulitan memantau suhu, kelembapan, dan kondisi tanah secara real-time. Penyiraman air yang dilakukan secara manual atau berdasarkan perkiraan cenderung kurang efisien, berisiko membuang air atau menyebabkan kekurangan pasokan air pada tanaman, yang akhirnya menurunkan produktivitas hasil panen. Selain itu, kurangnya automasi dalam sistem irigasi menyulitkan pengelolaan waktu dan sumber daya, terutama ketika tanah sangat kering atau tangki air habis, karena pengecekan manual tidak praktis dan membuang waktu. Hal ini meningkatkan risiko kerusakan tanaman, terutama di wilayah dengan cuaca tidak menentu, sehingga diperlukan solusi berbasis teknologi yang mengintegrasikan pemantauan lingkungan, pengelolaan air, dan automasi penyiraman untuk meningkatkan efisiensi dan efektivitas pertanian.

### SOLUTION
Proyek "Smart Farm" menawarkan solusi berbasis Internet of Things (IoT) untuk mengatasi tantangan dalam pertanian modern dengan menggunakan mikrokontroler ESP32 sebagai pusat pengendali untuk monitoring tanaman dan air dalam tangki, serta automasi penyiraman tanaman. Sistem ini memanfaatkan soil moisture sensor untuk memantau kelembaban tanah, dengan data yang dianalisis untuk menentukan kebutuhan air tanaman, sementara mikrokontroler mengontrol sistem sprinkler yang dioperasikan melalui driver motor L298N untuk penyiraman otomatis. Penyiraman dapat dijadwalkan pengguna melalui aplikasi Blynk atau diaktifkan otomatis berdasarkan deteksi tanah kering oleh sensor kelembaban. Selain itu, sensor ultrasonik HC-SR04 digunakan untuk memantau ketinggian air dalam tangki, dengan LED peringatan menyala saat tangki kosong sebagai notifikasi bagi pengguna untuk mengisi ulang. Dengan mengintegrasikan ESP32 dan berbagai sensor dalam rangkaian IoT, sistem ini menciptakan manajemen pertanian yang lebih efisien, hemat air, dan responsif terhadap kebutuhan tanaman, serta memberikan solusi praktis untuk meningkatkan produktivitas sekaligus mengurangi pemborosan sumber daya.


## HARDWARE DESIGN AND IMPLEMENTATION
Proyek Smart Farm menggunakan beberapa komponen utama yang terintegrasi dengan mikrokontroler ESP32. Komponen tersebut meliputi sensor kelembaban tanah (soil moisture sensor) untuk memantau kondisi tanah, modul driver motor L298N untuk mengendalikan pompa DC sebagai sprinkler, dan sensor ultrasonik HC-SR04 untuk memantau ketinggian air dalam tangki. Indikator LED built-in ESP32 berfungsi sebagai notifikasi saat tangki air kosong. ESP32 dihubungkan ke suplai daya stabil, sedangkan pompa DC mendapatkan daya dari sumber eksternal melalui L298N. Semua komponen dihubungkan sesuai dengan pin GPIO ESP32, dengan pertimbangan koneksi VCC, GND, dan sinyal data sesuai skema rangkaian.


## SOFTWARE IMPLEMENTATION
Software pada proyek ini diimplementasikan menggunakan platform Arduino IDE dan PlatformIO serta melibatkan integrasi dengan aplikasi Blynk. Program mencakup beberapa modul yaitu pembacaan data sensor untuk kelembaban tanah dan level air menggunakan HC-SR04, logika automisasi yang menyalakan pompa saat tanah kering atau sesuai jadwal yang diatur oleh pengguna, dan pengendalian LED built-in untuk notifikasi tangki kosong. Koneksi ke aplikasi Blynk dilakukan melalui koneksi Wi-Fi ESP32, memungkinkan monitoring data secara real-time dan penjadwalan penyiraman melalui interface aplikasi. Program juga memiliki kondisi fail-safe, seperti mematikan pompa jika tangki kosong.

### SOFTWARE USED

- [Arduino IDE](https://www.arduino.cc/en/software)
- [PlatformIO](https://platformio.org/)
- [Blynk](https://blynk.io/)


## TEST RESULTS AND PERFORMANCE EVALUATION

### TEST RESULTS

#### SENSOR TEST
Hasil dari pengujian membuktikan bahwa sistem dan sensor-sensor yang digunakan dapat bekerja sesuai fungsinya. Setiap sensor dicoba pada keadaan tertentu yang berbeda-beda dan menampilkan outputnya pada serial monitor. Diuji juga pada saat soil moisture sensor membaca kelembaban tanah yang rendah atau kering, maka driver motor l298n akan membuat pompa DC berhenti bekerja.


#### USER INTERFACE TEST
Pengujian pada dashboard aplikasi blynk sudah menampilkan data real-time yang digunakan untuk memonitoring kelembaban tanah dan air dalam tangki. Penjadwalan untuk mengaktifkan pompa DC juga telah dilakukan dan akan aktif selama satu menit sesuai penjadwalan yang diinput oleh user.


### PERFORMANCE EVALUATION
Berdasarkan Hasil pengujian yang kami lakukan, setiap sensor berhasil memberikan output yang sesuai dengan nilai sebenarnya. Namun pengujian yang dilakukan belum menggunakan situasi yang sebenarnya. Walau soil moisture sensor sudah dapat membaca kelembaban tanah, tetapi sensor tersebut masih belum diuji pada tanah yang benar benar kering. Sensor ultrasonik HC-SR04 digunakan dikarenakan keterbatasan sensor yang kelompok kami miliki, walau dapat menghitung suatu objek bahkan air, tetapi sensor ini belum kami uji jika berada dalam tangki yang sebenarnya. Aplikasi Blynk berhasil menampilkan interface pengguna yang intuitif untuk monitoring data secara real-time dan penjadwalan automasi penyiraman. Namun penggunakan PWM hanya dapat dilakukan pada PlatformIO

## CONCLUSION AND FUTURE WORK

### CONCLUSION
Proyek “Smart Farm” berhasil merancang dan mengimplementasikan sistem Internet of Things (IoT) yang memungkinkan automasi dan monitoring lingkungan pertanian secara efisien. Sistem ini menggunakan mikrokontroler ESP32 sebagai pusat pengendali, mengintegrasikan sensor kelembaban tanah, modul driver motor L298N, dan sensor ultrasonik HC-SR04 untuk memastikan penyiraman tanaman dan manajemen air dalam tangki berlangsung otomatis dan optimal. Aplikasi Blynk menyediakan interface pengguna yang intuitif untuk monitoring data secara real-time dan penjadwalan automasi penyiraman. Hasil pengujian menunjukkan bahwa semua komponen dapat berfungsi sesuai desain, meskipun pengujian pada kondisi lapangan yang sebenarnya masih terbatas. Beberapa tantangan, seperti penggunaan PWM yang belum sepenuhnya stabil pada Platform Arduino IDE, memberikan peluang untuk pengembangan lebih lanjut. Sistem ini memiliki potensi besar untuk diterapkan secara luas di sektor pertanian, meningkatkan efisiensi penggunaan sumber daya air dan waktu, serta memberikan kontribusi nyata pada modernisasi praktik pertanian.


### FUTURE WORK
Meskipun proyek Smart Farm telah menunjukkan keberhasilan dalam integrasi IoT untuk automasi dan monitoring pertanian, beberapa aspek masih dapat ditingkatkan di masa depan. Pertama, pengujian lebih lanjut diperlukan pada kondisi lapangan yang sebenarnya, seperti pada tanah yang benar-benar kering atau dalam tangki air berukuran nyata, untuk memastikan keandalan sistem dalam situasi dunia nyata. Selain itu, penggunaan sensor yang lebih spesifik, seperti sensor ultrasonik dengan presisi lebih tinggi atau sensor kelembaban tanah yang lebih akurat, dapat meningkatkan performa sistem.

Implementasi protokol komunikasi yang lebih aman dan efisien, seperti MQTT atau HTTP, dapat diintegrasikan untuk menggantikan metode koneksi saat ini, memberikan pengiriman data yang lebih andal dan aman. Pengembangan antarmuka aplikasi juga dapat dilakukan, termasuk memperbaiki fungsi PWM dan menyediakan lebih banyak opsi kontrol bagi pengguna. Akhirnya, proyek ini dapat diperluas dengan penambahan fitur seperti integrasi dengan prediksi cuaca berbasis API atau sistem pemberian pupuk otomatis untuk mendukung pertanian yang lebih cerdas dan terintegrasi. Dengan langkah-langkah ini, Smart Farm berpotensi menjadi solusi yang lebih komprehensif dan berdaya guna di sektor pertanian modern.

