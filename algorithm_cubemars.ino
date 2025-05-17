/*
  CAN Send Example

  This will setup the CAN controller(MCP2515) to send CAN frames.
  Transmitted frames will be printed to the Serial port.
  Transmits a CAN standard frame every 2 seconds.

  MIT License
  https://github.com/codeljo/AA_MCP2515
*/
#define MAX_VELO_RPM 45
#define GEAR_RATIO 6
#define ACCEL_RAD_S_S 0.2
#define DT_MS 300
#define ANGLE_OF_ATTACK 15
#define SEED 2132138

#define EN_CAN 1

#include "mcp2515.h"
struct can_frame canMsg;
struct can_frame canMsg1;
MCP2515 mcp2515(10);


#define TIMING_CYCLE 100
#define TIMING_TOLERANCE 10

#define SERIAL_TIMING 0
#define COMMAND_TIMING 20
#define QUERY_TIMING 40
#define PRINT_TIMING 80

#define DT_CYCLES DT_MS/TIMING_CYCLE

struct Motor {
  int16_t angle;
  int16_t velocity;
  int16_t current;
  uint8_t temp;

  uint16_t encoder;
  uint16_t encoderRaw;
  int16_t encoderOffset;
};

enum {
    CAN_PACKET_SET_DUTY = 0,
    CAN_PACKET_SET_CURRENT,
    CAN_PACKET_SET_CURRENT_BRAKE,
    CAN_PACKET_SET_RPM,
    CAN_PACKET_SET_POS,
    CAN_PACKET_SET_ORIGIN_HERE,
    CAN_PACKET_SET_POS_SPD
} CAN_PACKET_ID;

void printMotor(Motor m, char c = '?');
void setVelocity(uint8_t controller_id, float velocity_rpm);
void setAngle(uint8_t controller_id, float velocity_rpm, float angle_deg, float RPA);

Motor motorA;
Motor motorB;

Motor motors[2] = {motorA, motorB};

double heading = 0;

// String inst;

int commandLoop = 0;
int printLoopCount = 0;

const char endCharA = 'A';
const char endCharB = 'B';
const char purge = '_';

float valueA = 0;
float valueB = 0;

int pointCount = 0;

char mode = 'm';

bool serialRead = 0;
bool commandSent = 0;
bool querySent = 0;
bool imuRecv = 0;
bool printSent = 0;

void comm_can_transmit_eid(uint32_t id, const uint8_t *data, uint8_t len) {
    canMsg1.can_id = CAN_EFF_FLAG + id;
    canMsg1.can_dlc = len;
    memcpy(canMsg1.data, data, len);
    mcp2515.sendMessage(&canMsg1);
}

void buffer_append_int32(uint8_t *buffer, int32_t number, int32_t *index) {
    buffer[(*index)++] = number >> 24;
    buffer[(*index)++] = number >> 16;
    buffer[(*index)++] = number >> 8;
    buffer[(*index)++] = number;
}

//int16数据位整理
void buffer_append_int16(uint8_t *buffer, int16_t number, int32_t *index) {
    buffer[(*index)++] = number >> 8;
    buffer[(*index)++] = number;
}

const uint8_t can_test[8] = { 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA };

void setup() {
  Serial.begin(115200);

      if (mcp2515.reset() == MCP2515::ERROR_OK) {
        Serial.print("CAN init ok!\r\n");
        mcp2515.setBitrate(CAN_1000KBPS, MCP_8MHZ);
        mcp2515.setNormalMode();

        comm_can_transmit_eid(0x00, can_test, 8);
    } else Serial.print("CAN init fault!\r\n");

  pinMode(7, OUTPUT);
  // lsm6ds3trc.configInt1(false, false, true); // accelerometer DRDY on INT1
  // lsm6ds3trc.configInt2(false, true, false); // gyro DRDY on INT2


  randomSeed(SEED);
}

void loop() {
  
  long time = millis();
  int cyclePosition = time%TIMING_CYCLE;

  if(cyclePosition > SERIAL_TIMING && cyclePosition <= SERIAL_TIMING + TIMING_TOLERANCE){
    if(Serial.available()){
      char inChar = Serial.read();
      // Serial.println("--");
      // Serial.println(inChar);
      // Serial.println((int)inChar);
      // Serial.println("--");
      if(inChar >= 48 && inChar <= 57){
        // inst += inChar;
      }else if(inChar == 'p'){
        mode = 'p';
        // setCurrent(0x141,0);
        // setCurrent(0x142,0);
      }else if(inChar == 'a'){
        mode = 'a';
        // setCurrent(0x141,0);
        // setCurrent(0x142,0);
      }else if(inChar == 'v'){
        mode = 'v';
        // setCurrent(0x141,0);
        // setCurrent(0x142,0);
      }else if(inChar == 'r'){
        mode = ' ';
        // setCurrent(0x141,0);
        // setCurrent(0x142,0);
      }else if(mode == 'p' && inChar == endCharA){
        // valueA = atof(inst.c_str());
        // setAngleSingle(0x141,MAX_VELO_RPM,(int16_t)(valueA*100));
        // inst = "";
      }else if(mode == 'p' && inChar == endCharB){
        // valueB = atof(inst.c_str());
        // setAngleSingle(0x142,MAX_VELO_RPM,(int16_t)(valueB*100));
        // inst = "";
      }else if(mode == 'v' && inChar == endCharA){
        // valueA = atof(inst.c_str());
        // setVelocity(0x141,(int32_t)(valueA*100));
        // inst = "";
      }else if(mode == 'v' && inChar == endCharB){
        // valueB = atof(inst.c_str());
        // setVelocity(0x142,(int32_t)(valueB*100));
        // inst = "";
      }else if(inChar == purge){
        // inst = "";
      }else if(inChar == 'd'){
        Serial.print("#");
        Serial.print(motors[0].encoder);
        Serial.print("=");
        Serial.print(motors[1].encoder);
        Serial.print("=");
      }
    }
  }else if(cyclePosition > COMMAND_TIMING && cyclePosition <= COMMAND_TIMING + TIMING_TOLERANCE){
    if(!commandSent){
      if(mode == 'a'){
        if(commandLoop > DT_CYCLES){
          heading += (random(65536)/65536.0) * ANGLE_OF_ATTACK - (ANGLE_OF_ATTACK/2);
          double heading_rad = heading * 3.14159 / 180;
          setVelocity(0x68,sin(heading_rad)*MAX_VELO_RPM*GEAR_RATIO);
          setVelocity(0x0A,cos(heading_rad)*MAX_VELO_RPM*GEAR_RATIO);
          // printMotor(motorA,'a');
          // printMotor(motorB,'b');
          commandLoop = 0;
          pointCount ++;
          digitalWrite(7, HIGH);
        }else{
          digitalWrite(7, LOW);
        }
        commandLoop ++;

      }
      
      commandSent = 1;
    }
  }else if(cyclePosition > QUERY_TIMING && cyclePosition <= QUERY_TIMING + TIMING_TOLERANCE){
    if(!querySent){
      querySent = 1;
    }
  }else if(cyclePosition > PRINT_TIMING && cyclePosition <= PRINT_TIMING + TIMING_TOLERANCE){
    if(!printSent){
      if(printLoopCount > 1){

        double heading_rad = heading * 3.14159 / 180;
        // Serial.print("Heading:");
        // Serial.print(heading_rad);
        // Serial.print(" A Des:[");
        // Serial.print(sin(heading_rad)*MAX_VELO_RPM);
        // Serial.print("] Act:[");
        // Serial.print(motors[0].velocity/10.0);
        // Serial.print("] B Des:");
        // Serial.print(cos(heading_rad)*MAX_VELO_RPM);
        // Serial.print("] Act:[");
        // Serial.print(motors[1].velocity/10.0);

        // Serial.print("[Immediate] [");
        // Serial.print(accel.acceleration.x + xOffset);
        // Serial.print("] [");
        // Serial.print(accel.acceleration.y + yOffset);
        // Serial.print("] [");
        // Serial.print(accel.acceleration.z + zOffset);

        // Serial.print("] [Integral G] X:");
        // Serial.print(accelX);
        // Serial.print(" Y:");
        // Serial.print(accelY);
        // Serial.print(" Z:");
        // Serial.print(accelZ);

        // Serial.print(" | PC:");
        // Serial.println(pointCount);

        printLoopCount = 0;
      }
      printLoopCount ++;
      printSent = 1;
    }
  }else{
    serialRead = 0;
    commandSent = 0;
    querySent = 0;
    printSent = 0;
  }

  getFeedback();
}


void printMotor(Motor m, char c = '?'){
  Serial.print("Motor ");
  Serial.print(c);
  Serial.print("| Angle: ");
  Serial.print(m.angle);
  Serial.print("| Velocity");
  Serial.print(m.velocity);
  Serial.print("| Temp");
  Serial.println(m.temp);
}

void setVelocity(uint8_t controller_id, float velocity_rpm){
    int32_t send_index = 0;
    uint8_t buffer[4];
    buffer_append_int32(buffer, (int32_t)velocity_rpm, &send_index);
    comm_can_transmit_eid(controller_id | ((uint32_t)CAN_PACKET_SET_RPM << 8), buffer, send_index);
}

void setAngleSingle(uint8_t controller_id, float velocity_rpm, float angle_deg, float RPA){
    // uint8_t data[8] = { 
    //     0xA6, 
    //     0x00, 
    //     (uint8_t)(velocity_dps), 
    //     (uint8_t)(velocity_dps>>8), 
    //     (uint8_t)(angle_deg_hundreth), 
    //     (uint8_t)(angle_deg_hundreth>>8), 
    //     0x00, 
    //     0x00};

    // // uint8_t data[8] = {0xA2, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00};
    // CANFrame frame(canID, data, sizeof(data));
    // // frame.print("TX");
    // CAN.write(frame);
    int32_t send_index = 0;
    int32_t send_index1 = 4;
    uint8_t buffer[8];
    buffer_append_int32(buffer, (int32_t)(angle_deg * 10000.0), &send_index);
    buffer_append_int16(buffer, velocity_rpm / 10.0, &send_index1);
    buffer_append_int16(buffer, RPA / 10.0, &send_index1);
    comm_can_transmit_eid(controller_id | ((uint32_t)CAN_PACKET_SET_POS_SPD << 8), buffer, send_index1);
}

// void setCurrent(uint16_t canID, int16_t current_hundreth){
//     uint8_t data[8] = { 
//         0xA1, 
//         0x00, 
//         0x00, 
//         0x00, 
//         (uint8_t)(current_hundreth), 
//         (uint8_t)(current_hundreth>>8), 
//         0x00, 
//         0x00};

//     // uint8_t data[8] = {0xA2, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00};
//     CANFrame frame(canID, data, sizeof(data));
//     // frame.print("TX");
//     CAN.write(frame);
// }

void getFeedback(){
  // uint8_t data[8] = {0,0,0,0,0,0,0,0};
  // CANFrame frame(0x140,data,8);
  // while (CAN.read(frame) == CANController::IOResult::OK) {
  //     frame.getData(data, 8);
  //     uint16_t id = frame.getId()-0x241;
  //     if(id == 0 || id == 1){
  //       if(data[0] == 0xA1 || data[0] == 0xA2 || data[0] == 0xA6 || data[0] == 0x9C){
  //           motors[id].angle = (int16_t)(data[6] | (data[7]<<8)); 
  //           motors[id].velocity = (int16_t)(data[4] | (data[5]<<8));
  //           motors[id].current = (int16_t)(data[2] | (data[3]<<8));
  //           motors[id].temp = data[1];
  //           // if(data[0] == 0x9C){
  //           //   Serial.println("9C");
  //           // }
  //       }else if(data[0] == 0x90){
  //           motors[id].encoder = (int16_t)(data[2] | (data[3]<<8)); 
  //           motors[id].encoderRaw = (int16_t)(data[4] | (data[5]<<8));
  //           motors[id].encoderOffset = (int16_t)(data[6] | (data[7]<<8));
  //           // frame.print("90");
  //       }else{
  //           // frame.print("WUT");
  //       }
  //     }
  //     digitalWrite(7,HIGH);
  // }

}
