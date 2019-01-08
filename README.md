SelfDrivingCar
===

作者：張頌宇

此為Linkit 7697版本

使用說明
---
先在根目錄下新增一個名叫"config.h"檔案如下：
```
#ifndef CONFIG_H
#define CONFIG_H

#define DEBUG true
#define DEBUG_FETCH true
#define DEBUG_UPLOAD false

#define WIFI_SSID "<wifi名稱>"
#define WIFI_PASS "<wifi密碼>"

#define GAME_SERVER_ON true
#define GAME_SERVER "<主機名稱>"
#define GAME_ID "<車子ID>"
#define GAME_TEAM "B"

#define TEAM_MODE true

#define fronttrig 2
#define frontecho 3
#define lefttrig 4
#define leftecho 5
#define rightttrig 6
#define righttecho 7

#define timerpinin 8
#define timerpinout 9

#define R_LED 10
#define G_LED 11
#define B_LED 12

#define leftpin1 14
#define leftpin2 15
#define rightpin1 16
#define rightpin2 17

#endif
```

使用Arduino IDE即可燒錄執行