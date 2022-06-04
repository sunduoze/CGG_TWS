目前问题，烧录后可以运行，断电后无法运行




命令：
scan_beacon            ：扫描指定mac地址的beacon信号，成功则返回scaned
get_rssi               ：扫描指定mac地址的rssi，成功则返回信号强度
set_addr:60C0BF29DA01  ：设定被指定的mac地址，冒号后为地址，长度为12，成功则返回0x + 地址

[00:40:04.420]收←◆SimpleBLEObserver_SerialPrint Start init.

[00:40:12.963]发→◇scan_beacon
□
[00:40:13.228]收←◆scaned

[00:40:16.185]发→◇get_rssi
□
[00:40:16.192]收←◆rssi 68

[00:40:20.044]发→◇set_addr:123456789012
□
[00:40:20.049]收←◆0x123456789012

[00:40:21.360]发→◇set_addr:60C0BF29DA01
□
[00:40:21.367]收←◆0x60C0BF29DA01
