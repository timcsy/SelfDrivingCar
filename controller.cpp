#include <Arduino.h>
#include "controller.h"
#include "fetch.h"
#include "config.h"
#include "maze.h"
#include "server.h"

enum {LEFT = 0, FRONT = 1, RIGHT = 2};
enum {GR = 100, GL, GC, GS, GU, TR, TL, UT};

// Drive------------------------------------------------------------------------------
int Vr = 0, Vl = 0, dt = -1; // dt is milliseconds (ms)
int Vrc = 0, Vlc = 0; // corrected velocity
unsigned long last_time = millis();
float F = 0, last_F[2] = {0, 0}, R = 0, last_R[2] = {0, 0}, L = 0, last_L[2] = {0, 0}, ULTRA[3] = {0, 0, 0}, RATIO[3] = {1, 1, 1}, DIFF[3] = {0, 0, 0};
int stuck = 0;
int q100_L = 0, q100_R = 0;
//------------------------------------------------------------------------------------

bool stopped = false;

// Game Server------------------------------------------------------------------------
int last_x = -1, x = -1, next_x = -1, last_y = -1, y = -1, next_y = -1;
String message;
String sig;
//------------------------------------------------------------------------------------

void controller() {
	while (true) {
//    server_loop();
//		if (GAME_SERVER_ON) get_position();
    if (GAME_SERVER_ON) get_command();
    

		if (stopped) {
		  stopped = false;
		}

// straight ultrasound data
//    ultrasound();
//    Serial.print(L);
//    Serial.print(" ");
//    Serial.print(RATIO[LEFT]);
//    Serial.print(" ");
//    Serial.println(DIFF[LEFT]);
//    delay(100);

// adjust quarter
//    q100_R = quarter_adj(1, true);
//    delay(5000);
//    q100_L = quarter_adj(1, false);
//    delay(10000);

// fixed quarter
    q100_R = 8;
    q100_L = 8;

//-------------------------------------------------------------------------------------------------------------

// hard-coding (00 -> 07) might stuck at 04, 14, 22, 23
//    int path_00_07[15] = {GU, TR, GU, TR, GU, TL, GS, TL, GS, GU, TL, GU, TR, GU, UT};
//    hard_coding(path_00_07, 15);
//    delay(10000);

// hard-coding (07 -> 77)
//    int path_07_77[13] = {GU, TL, GR, GL, GC, GS, TL, GU, TR, GU, TL, GU, UT};
//    hard_coding(path_07_77, 13);
//    delay(10000);

// hard-coding (77 -> 70) might error at 76, 46
//    int path_77_70[17] = {GU, TR, GU, TL, GU, TL, GR, GC, TR, GS, GL, GC, TL, GU, TR, GU, UT};
//    hard_coding(path_77_70, 17);
//    delay(10000);

// hard-coding (70 -> 00) might stuck at 72, 12
//    int path_70_00[12] = {GL, GC, GS, TL, GU, TR, GU, TL, GU, TL, GU, UT};
//    hard_coding(path_70_00, 12);
//    delay(10000);

//-------------------------------------------------------------------------------------------------------------

// hard-coding (00 -> 70) might stuck at 04, 14
    light(0, 0, 255);
    int path_00_70[10] = {GU, TR, GU, TR, GU, TL, GU, TR, GU, UT};
    hard_coding(path_00_70, 10);
    light(0, 0, 0);
    if (TEAM_MODE) {
      fetch_game_server("send-to GlimGlam GO");
      sig = get_fetch_result();
      while (sig != "send-from yihsin GO") sig = get_fetch_result();
    }

// hard-coding (70 -> 77) not stable at 72, 52, 46
    light(0, 0, 255);
    int path_70_77[19] = {GL, GC, GS, TL, GS, GR, GC, TR, GU, TL, GR, GC, TR, GU, TR, GU, TL, GU, UT};
    hard_coding(path_70_77, 19);
    light(0, 0, 0);
    if (TEAM_MODE) {
      fetch_game_server("send-to GlimGlam GO");
      sig = get_fetch_result();
      while (sig != "send-from yihsin GO") sig = get_fetch_result();
    }

// hard-coding (77 -> 07) might error at 76, 46
    light(0, 0, 255);
    int path_77_07[10] = {GU, TR, GU, TL, GU, TR, GU, TR, GU, UT};
    hard_coding(path_77_07, 10);
    light(0, 0, 0);
    if (TEAM_MODE) {
      fetch_game_server("send-to GlimGlam GO");
      sig = get_fetch_result();
      while (sig != "send-from yihsin GO") sig = get_fetch_result();
    }

// hard-coding (07 -> 00) might error at 24, 25
    light(0, 0, 255);
    int path_07_00[15] = {GU, TL, GR, GC, TR, GU, TR, GU, TR, GU, TL, GU, TL, GU, UT};
    hard_coding(path_07_00, 15);
    light(0, 0, 0);
    if (TEAM_MODE) {
      fetch_game_server("send-to GlimGlam GO");
      sig = get_fetch_result();
      while (sig != "send-from yihsin GO") sig = get_fetch_result();
    }

//-------------------------------------------------------------------------------------------------------------

// hard-coding (07 -> 70) for QUIZ
//    int path_07_70[13] = {GL, GC, TL, GR, GC, TR, GS, GL, GC, TL, GU, TR, GU};
//    hard_coding(path_07_70, 13);
//    delay(10000);

// testing turn
//    turn(LEFT);
//    delay(5000);
//    turn(RIGHT);
//    delay(10000);

// go straight forward
//    if (R < 30) {
//      light(255, 0, 0);
//      straight(RIGHT, 255, 100);
//    }
//    else if (L < 30) {
//      light(0, 255, 0);
//      straight(LEFT, 255, 100);
//    }
//    else if (F > 12) {
//      light(0, 0, 255);
//      drive(255, 255, 100);
//    }
//    else {
//      light(255, 255, 255);
//      ultrasound();
//    }

// using parallel to walk straight
//    ultrasound();
//    drive(255, 255);
//    delay(100);
//    (last_R[1] > last_R[0])? parallel(RIGHT, true): parallel(RIGHT, false);
//    drive(0, 0);
//    delay(100);
    
// counterclockwise far to near
//    parallel(RIGHT, false);
//    delay(10000);

// counterclockwise adjust
//    q100_L = quarter_adj(3, false);
//    delay(10000);

// clockwiae far to near and near to far(fail)
//    parallel(LEFT);
//    delay(5000);
//    parallel(LEFT, true, true);
	}
}

void controller_setup() {
	maze_setup();
}

void hard_coding(int path[], int size) {
  ultrasound();
  for (int i = 0; i < size; i++) {
    switch (path[i]) {
      case GR: go_right(); break;
      case GL: go_left(); break;
      case GC: go_center(); break;
      case GS: go_step(); break;
      case GU: go_until(); break;
      case TR: turn(RIGHT); break;
      case TL: turn(LEFT); break;
      case UT: turn(RIGHT); turn(RIGHT); break; // U turn
    }
  }
}

void drive(int Vl, int Vr, int ms) {
  get_command();
  if (ms == -1) {
  	analogWrite(rightpin1, (Vr > 0)? Vr: 0);
  	analogWrite(rightpin2, (Vr < 0)? -Vr: 0);
  	analogWrite(leftpin1, (Vl > 0)? Vl: 0);
  	analogWrite(leftpin2, (Vl < 0)? -Vl: 0);
  } else {
    drive(Vl, Vr);
    delay(ms);
    drive(0, 0);
    ultrasound();
    delay(ms);
  }
}

void straight(int direction, int V, int ms, double threshold) {
  double U, ratio;
  U = ULTRA[direction]; ratio = RATIO[direction];

  if (U < 9.5 && ratio < 1 / threshold) {
    if (direction == RIGHT) drive(-V, 100 + (V - 100) * abs(1 / threshold - ratio), ms);
    else if (direction == LEFT) drive(100 + (V - 100) * abs(1 / threshold - ratio), -V, ms);
  } else if (9.5 < U && U < 10.5 && ratio < threshold) {
    if (direction == RIGHT) drive(-V, V, ms);
    else if (direction == LEFT) drive(V, -V, ms);
  } else if (9.5 < U && U < 10.5 && ratio > 1 / threshold) {
    if (direction == RIGHT) drive(V, -V, ms);
    else if (direction == LEFT) drive(-V, V, ms);
  } else if (U > 10.5 && ratio > threshold) {
    if (direction == RIGHT) drive(100 + (V - 100) * abs(ratio - threshold), -V, ms);
    else if (direction == LEFT) drive(-V, 100 + (V - 100) * abs(ratio - threshold), ms);
  }
  
  if (F > 12) drive(V, V, ms);
  else ultrasound();
}

void go_init() {
  ultrasound();
}
void go_right() {
  while(R < 30) straight(RIGHT, 255, 100);
}
void go_left() {
  while(L < 30) straight(LEFT, 255, 100);
}
void go_center() {
  for (int i = 0; i < 3; i++) drive(255, 255, 100);
}
void go_step() {
  for (int i = 0; i < 7; i++) drive(255, 255, 100);
}
void go_until() {
  while (F > 12) {
    if (R < 30) straight(RIGHT, 255, 100);
    else if (L < 30) straight(LEFT, 255, 100);
    else drive(255, 255, 100);
  }
}

int parallel(int direction, bool clockwise, bool leave, double threshold, int V) {
  int counter = 0;
  double U, ratio;
  U = ULTRA[direction]; ratio = leave? (1 / RATIO[direction]): RATIO[direction];
  while (true) {
    ultrasound();
    U = ULTRA[direction]; ratio = leave? (1 / RATIO[direction]): RATIO[direction];
    
    if (leave == false && U < 30 && threshold < ratio) {
      for (int i = 0; i < (clockwise? 1: 2); i++) { // this is only for my car
        clockwise? drive(V, -V, 100): drive(-V, V, 100);
        counter++;
      }
      return counter;
    } else if (leave == true && ratio < threshold) {
      return counter;
    } else {
      clockwise? drive(V, -V, 100): drive(-V, V, 100);
      counter++;
    }
  }
}

int quarter_adj(int round, bool clockwise, int ms, int V) { // if  clockwise, put R to the front side, if counterclockwise, put L to the front side
  int quarter = 0;
    for (int n = 1; n <= round; n++) {
      if (clockwise) {
        quarter += parallel(RIGHT, true);
        quarter += parallel(FRONT, true);
        quarter += parallel(LEFT, true);
      } else {
        quarter += parallel(LEFT, false);
        quarter += parallel(FRONT, false);
        quarter += parallel(RIGHT, false);
      }
      for (int i = 0; i < quarter / (3 * n); i++) {
        clockwise? drive(V, -V, ms): drive(-V, V, ms);
      }
    }
    quarter /= 3 * round;
    return quarter;
}

void turn(int direction) {
  while (R < 5) drive(-255, 255, 100);
  while (L < 5) drive(255, -255, 100);
  while (F < 10) drive(-255, -255, 100);
  if (direction == LEFT) {
    for (int i = 0; i < q100_L || F < 30; i++) {
      drive(-255, 255, 100);
    }
  } else if (direction == RIGHT) {
    for (int i = 0; i < q100_R || F < 30; i++) {
      drive(255, -255, 100);
    }
  }
}

void correct() {
	Vrc = Vr; Vlc = Vl;
	int Vav = (((Vr > 0)? Vr: -Vr) + ((Vl > 0)? Vl: -Vl)) / 2;
	if (Vr == 0 && Vl == 0) {Vrc = 0; Vlc = 0;} // if it stops then stop
	else if (F < 0.05 * Vav) { // if there is an obstacle in the front
		if (R > L) {Vrc = -Vav; Vlc = Vav;} // if there exist right space, then turn right
		else {Vlc = -Vav; Vrc = Vav;} // if there exist left space, then turn left
	}	else if (R < 0.02 * Vav) {
		Vlc = -Vav; Vrc = Vav; // if there is no right space, then turn left
	} else if (L < 0.02 * Vav) { // if there is no right space, then turn left
		Vlc = Vav; Vrc = -Vav;
	} else if (F < 200 && R < 200 && L < 200) { // the controlable range, drive in the middle
		if (R > L) {
			float c = (float)(R - L) / (R + L);
			Vrc = (1.0 - c) * Vr;
			if (Vr >= 50 && Vrc < 50) Vrc = 50;
			Vlc = Vl;
		}	else if (L > R) {
			float c = (float)(L - R) / (R + L);
			Vrc = Vr;
			Vlc = (1.0 - c) * Vl;
			if (Vl >= 50 && Vlc < 50) Vlc = 50;
		} else {Vrc = -Vav; Vlc = Vav;}
	}

	if ((-0.1 < last_F[1] - last_F[0] || last_F[1] - last_F[0] < 0.1) &&
			(-0.1 < last_R[1] - last_R[0] || last_R[1] - last_R[0] < 0.1) &&
			(-0.1 < last_L[1] - last_L[0] || last_L[1] - last_L[0] < 0.1)) { // it it is stuck
		stuck++;
	} else {
		stuck = 0;
	}
	if (stuck > 10) {
		stuck = 0;
		Vrc = -Vav; Vlc = Vav;
	}
}

void stop() {
	Vr = 0;
	Vl = 0;
	stopped = true;
}

// void drive_control() {
// 	unsigned long time = millis();
// 	if (dt >= 0 && time - last_time >= dt) {
// 		last_time = time;
// 		if (cmd_index < cmd_num) {
// 			Vr = cmdArray[cmd_index].Vr;
// 			Vl = cmdArray[cmd_index].Vl;
// 			dt = cmdArray[cmd_index].dt;
// 			cmd_index++;
// 		} else if (cmd_index == cmd_num) {
// 			cmd_index++;
// 			stop();
// 		}
// 	}
// 	correct();
// 	analogWrite(rightpin1, (Vrc > 0)? Vrc: 0);
// 	analogWrite(rightpin2, (Vrc < 0)? -Vrc: 0);
// 	analogWrite(leftpin1, (Vlc > 0)? Vlc: 0);
// 	analogWrite(leftpin2, (Vlc < 0)? -Vlc: 0);
// }

float distance(int trig, int echo) {
	float duration;
	pinMode(trig, OUTPUT);
	pinMode(echo, INPUT);
	digitalWrite(trig, HIGH);
	delayMicroseconds(10); // 給予 trig 10us TTL pulse，讓模組發射聲波
	digitalWrite(trig, LOW);
	duration = pulseIn(echo, HIGH, 20000); // 紀錄echo電位從high到low的時間，就是超音波來回的時間，若0.02秒內沒收到超音波則回傳0
	return duration / 29.0 / 2.0; // 聲速340m/s ，換算後約每29微秒走一公分，超音波來回所以再除2
}

void ultrasound() {
	F = distance(fronttrig, frontecho);
	L = distance(lefttrig, leftecho);
	R = distance(rightttrig, righttecho);

	if (F < 1000 && F != 0) {
		last_F[0] = last_F[1];
		last_F[1] = F;
	} else { // extremely cases(eps or inf)
		if (last_F[0] < 10 && last_F[1] < 10) F = -1;
		else F = last_F[1] + 1;
	}
	
	if (R < 1000 && R != 0) {
		last_R[0] = last_R[1];
		last_R[1] = R;
	} else { // extremely cases(eps or inf)
		if (last_R[0] < 10 && last_R[1] < 10) R = -1;
		else R = last_R[1] + 1;
	}
	
	if (L < 1000 && L != 0) {
		last_L[0] = last_L[1];
		last_L[1] = L;
	} else { // extremely cases(eps or inf)
		if (last_L[0] < 10 && last_L[1] < 10) L = -1;
		else L = last_L[1] + 1;
	}

  ULTRA[LEFT] = L;
  ULTRA[FRONT] = F;
  ULTRA[RIGHT] = R;

  RATIO[LEFT] = last_L[1] / last_L[0];
  RATIO[FRONT] = last_F[1] / last_F[0];
  RATIO[RIGHT] = last_R[1] / last_R[0];

  DIFF[LEFT] = last_L[1] - last_L[0];
  DIFF[FRONT] = last_F[1] - last_F[0];
  DIFF[RIGHT] = last_R[1] - last_R[0];
}

void light(int red, int green, int blue) {
	analogWrite(R_LED, red);
	analogWrite(G_LED, green);
	analogWrite(B_LED, blue);
}

void connection_setup() {
	if (GAME_SERVER_ON) game_server_setup(GAME_SERVER, GAME_ID, GAME_TEAM);
}

void game_server_parser(String res) {
	// position 暫時
	if (res.startsWith("position ")) {
		char str[128];
		strcpy(str, res.c_str());
		char * pch;
		pch = strtok(str, " "); // postion
		pch = strtok(NULL, " "); // x
		last_x = x;
		x = String(pch).toInt();
		pch = strtok(NULL, " "); // y
		last_y = y;
		y = String(pch).toInt();
	}
}

int m = 0;
void get_position() {
	if (millis() - last_time > 1000) {
		if (m % 3 == 0) fetch_game_server(String("send-to GlimGlam yihsin==Carrr==") + (m++ / 2) + "==times.");
    else if (m % 3 == 1) fetch_game_server(String("send-to yihsin GlimGlam==Carrr==") + (m++ / 2) + "==times.");
    else {
      fetch_game_server("position");
      m++;
    }
		String res = get_fetch_result();
    Serial.println(res);
		if (res) game_server_parser(res);
		last_time = millis();
	}
}

void get_command() {
  if (millis() - last_time > 1000) {
    String res = get_fetch_result();
    Serial.println(res);
    if (res == "send-from controller front") drive(255, 255, 100);
    if (res == "send-from controller right") drive(255, -255, 100);
    if (res == "send-from controller left") drive(-255, 255, 100);
    if (res == "send-from controller stop") {
      drive(0, 0);
      delay(1000);
    }
    last_time = millis();
  }
}
