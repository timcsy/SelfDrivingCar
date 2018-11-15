#include <Arduino.h>
#include "controller.h"
#include "fetch.h"
#include "config.h"

#define CMD_MAX 1024
#define RECORD_MAX 50

struct Command {
	int Vr;
	int Vl;
	int dt;
};

struct Record {
	float F;
	float R;
	float L;
	int Vr;
	int Vl;
	int x;
	int y;
	char C[25];
};

StaticJsonBuffer<65536> jsonBuffer;

// Drive------------------------------------------------------------------------------
int Vr = 0, Vl = 0, dt = -1; // dt is milliseconds (ms)
int Vrc = 0, Vlc = 0; // corrected velocity
unsigned long last_time = millis();
char Control_id[25] = {0};
float F = 0, last_F[2] = {0, 0}, R = 0, last_R[2] = {0, 0}, L = 0, last_L[2] = {0, 0};
int stuck = 0;
//------------------------------------------------------------------------------------

bool stopped = false;

// Command----------------------------------------------------------------------------
Command cmdArray[CMD_MAX]; // Queue of Commands
int cmd_index = 0, cmd_num = 0;
//------------------------------------------------------------------------------------

// Record-----------------------------------------------------------------------------
Record recordArray[RECORD_MAX]; // Round Queue of Records
int last_record_index = 0;
int record_index = 0;
StaticJsonBuffer<4096> recordBuffer;
//------------------------------------------------------------------------------------

// Game Server------------------------------------------------------------------------
int last_x = -1, x = -1, next_x = -1, last_y = -1, y = -1, next_y = -1;
String message;
//------------------------------------------------------------------------------------

void controller(String data) {
	String op;
	while (true) {
		get_position();

		if (stopped) {
			stopped = false;
			fetch(POST, "/api/v1/controls", "{\"op\":\"hook\"}");
		}
		jsonBuffer.clear();
		JsonObject& res = jsonBuffer.parseObject(data);
		if (!res.success()) continue;
		const char * _id = res["_id"];
		if (strcmp(Control_id, _id) != 0) {
			strcpy(Control_id, _id);
			String _op = res["op"];
			op = _op;

			if (op == "drive") data = drive(res["cmd"]);
			else data = hook();
		} else if (op == "drive") data = upload();
		else data = hook();
	}
}

void controller_setup() {
	maze_setup();
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

	// if (F < R && L < R && R > 20) {Vrc = -Vav; Vlc = Vav;}
	// else if (F > L && R > L && L < 20) {Vrc = -Vav; Vlc = Vav;}
	// else if (F < R && F < L && F < 20) {Vrc = -Vav; Vlc = Vav;}
	// else if (F > (R + L) / 2 && F > 20) {Vrc = Vav; Vlc = Vav;}
	// else if (F < L && R < L && L > 20) {Vrc = Vav; Vlc = -Vav;}
	// else if (F > R && L > R && R < 20) {Vrc = Vav; Vlc = -Vav;}
	// else if (F < (R + L) / 2 && F < 20) {Vrc = -Vav; Vlc = Vav;}
	// else {Vrc = -Vav; Vlc = Vav;}

	// if (F < 20 && L < 10) {Vrc = -Vav; Vlc = Vav;}
	// else if (L < 10) {Vrc = -Vav; Vlc = Vav;}
	// else if (F < 20) {Vrc = -Vav; Vlc = Vav;}
	// else if (F > 20) {Vrc = Vav; Vlc = Vav;}
	// else if (F < 20 && R < 20) {Vrc = Vav; Vlc = -Vav;}
	// else if (R < 20) {Vrc = Vav; Vlc = -Vav;}
	// else {Vrc = -Vav; Vlc = Vav;}

	// if (F < R && L < R) {Vrc = -Vav; Vlc = Vav;}
	// else if (F > L && R > L) {Vrc = -Vav; Vlc = Vav;}
	// else if (F < R && F < L) {Vrc = -Vav; Vlc = Vav;}
	// else if (F > (R + L) / 2) {Vrc = Vav; Vlc = Vav;}
	// else if (F < L && R < L) {Vrc = Vav; Vlc = -Vav;}
	// else if (F > R && L > R) {Vrc = Vav; Vlc = -Vav;}
	// else if (F < (R + L) / 2) {Vrc = -Vav; Vlc = Vav;}
	// else {Vrc = -Vav; Vlc = Vav;}

	// if (F < R && L < R && R > 20) {Vrc = -Vav; Vlc = Vav;}
	// else if (F > L && R > L && L < 20) {Vrc = -Vav; Vlc = Vav;}
	// else if (F < R && F < L && F < 20) {Vrc = -Vav; Vlc = Vav;}
	// else if (F > (R + L) / 2 && F > 20) {Vrc = Vav; Vlc = Vav;}
	// else if (F < L && R < L && L > 20) {Vrc = Vav; Vlc = -Vav;}
	// else if (F > R && L > R && R < 20) {Vrc = Vav; Vlc = -Vav;}
	// else if (F < (R + L) / 2 && F < 20) {Vrc = -Vav; Vlc = Vav;}
	// else {Vrc = -Vav; Vlc = Vav;}

	// if (F < 0.02 * Vav) { // if there is an obstacle in the front
	// 	Vrc = -Vav; Vlc = Vav;
	// }	else if (R < 0.02 * Vav) {
	// 	Vlc = -Vav; Vrc = Vav; // if there is no right space, then turn left
	// } else if (L < 0.02 * Vav) { // if there is no right space, then turn left
	// 	Vlc = Vav; Vrc = -Vav;
	// }

	if ((-0.1 < last_F[1] - last_F[0] || last_F[1] - last_F[0] < 0.1) &&
			(-0.1 < last_R[1] - last_R[0] || last_R[1] - last_R[0] < 0.1) &&
			(-0.1 < last_L[1] - last_L[0] || last_L[1] - last_L[0] < 0.1)) { // it it is stuck
		stuck++;
	} else {
		stuck = 0;
	}
	if (stuck > 5) {stuck = 0; Vrc = -Vav; Vlc = Vav;}
}

String hook() {
	String data = fetch(GET, "/api/v1");
	return data;
}

String drive(JsonArray& cmd) {
	int i;
	for (i = 0; i < cmd.size() && i < CMD_MAX; i++) {
		cmdArray[i].Vr = cmd[i]["Vr"];
		cmdArray[i].Vl = cmd[i]["Vl"];
		cmdArray[i].dt = cmd[i]["dt"];
	}
	cmd_index = 0;
	cmd_num = i;
	if (i) {
		Vr = cmdArray[0].Vr;
		Vl = cmdArray[0].Vl;
		dt = cmdArray[0].dt;
	}
	String data = upload();
	correct();
	return data;
}

void stop() {
	Vr = 0;
	Vl = 0;
	stopped = true;
}

void drive_control() {
	unsigned long time = millis();
	if (dt >= 0 && time - last_time >= dt) {
		last_time = time;
		if (cmd_index < cmd_num) {
			Vr = cmdArray[cmd_index].Vr;
			Vl = cmdArray[cmd_index].Vl;
			dt = cmdArray[cmd_index].dt;
			cmd_index++;
		} else if (cmd_index == cmd_num) {
			cmd_index++;
			stop();
		}
	}
	correct();
	analogWrite(rightpin1, (Vrc > 0)? Vrc: 0);
	analogWrite(rightpin2, (Vrc < 0)? -Vrc: 0);
	analogWrite(leftpin1, (Vlc > 0)? Vlc: 0);
	analogWrite(leftpin2, (Vlc < 0)? -Vlc: 0);
}

void saveToBuffer() {
	recordArray[record_index].F = F;
	recordArray[record_index].L = L;
	recordArray[record_index].R = R;
	recordArray[record_index].Vr = Vrc;
	recordArray[record_index].Vl = Vlc;
	recordArray[record_index].x = x;
	recordArray[record_index].y = y;
	strcpy(recordArray[record_index].C, Control_id);

	record_index = (record_index + 1) % RECORD_MAX;
}

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
		else F = last_F[1];
	}
	
	if (R < 1000 && R != 0) {
		last_R[0] = last_R[1];
		last_R[1] = R;
	} else { // extremely cases(eps or inf)
		if (last_R[0] < 10 && last_R[1] < 10) R = -1;
		else R = last_R[1];
	}
	
	if (L < 1000 && L != 0) {
		last_L[0] = last_L[1];
		last_L[1] = L;
	} else { // extremely cases(eps or inf)
		if (last_L[0] < 10 && last_L[1] < 10) L = -1;
		else L = last_L[1];
	}

	saveToBuffer();
}

String upload() {
	int last = last_record_index, cur = record_index;
	recordBuffer.clear();
	JsonObject& req = recordBuffer.createObject();
	JsonArray& records = req.createNestedArray("records");
	for (int i = last; i != cur; i = (i + 1) % RECORD_MAX) {
		if (recordArray[i].C[0]) {
			JsonObject& rec = recordBuffer.createObject();
			rec["F"] = recordArray[i].F;
			rec["R"] = recordArray[i].R;
			rec["L"] = recordArray[i].L;
			rec["Vr"] = recordArray[i].Vr;
			rec["Vl"] = recordArray[i].Vl;
			rec["x"] = recordArray[i].x;
			rec["y"] = recordArray[i].y;
			rec["C"] = recordArray[i].C;
			records.add(rec);
		}
	}
	String content;
	req.printTo(content);
	Serial.println(content);
	String data = fetch(POST, "/api/v1/records", content);
	last_record_index = cur;
	return data;
}

void light(int red, int green, int blue) {
	analogWrite(R_LED, red);
	analogWrite(G_LED, green);
	analogWrite(B_LED, blue);
}

void connection_setup() {
	game_server_setup(GAME_SERVER, GAME_ID, GAME_TEAM);
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

void get_position() {
	String res = fetch_game_server("position");
	game_server_parser(res);
}