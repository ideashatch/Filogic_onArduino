#include <Arduino.h>

// TB6612电机驱动测试
// 通过设置 GPIO35 GPIO36 的PWM值设置左右两个电机的 '速度'
// 但是具体电机是什么速度, 我们可通过电机上自带的霍尔编码器来测试
// 测试在100毫秒时间内，霍尔编码器得到的脉冲数

#define PWM_FREQUENCY     20000
#define INT_ENABLE        1

//TB6612引脚定义
const int right_R1 = 11;    
const int right_R2 = 8;
const int PWM_R = 36;
const int left_L1 = 9;
const int left_L2 = 10;
const int PWM_L = 35;

const int PinA_left = 15;     //定义检测左电机脉冲的引脚为 GPIO_B_3
const int PinA_right = 37;    //定义检测右电机脉冲的引脚为 GPIO_R_8

int times = 0, newtime = 0, d_time = 100;     //时间，最新的时间，时间间隔
int valA = 0, valB = 0, flagA = 0, flagB = 0; //变量valA和valB用于计算脉冲数

void setup() 
{
  Serial.begin(115200);          //串口波特率为115200

  pinMode(right_R1, OUTPUT);    //TB6612的引脚都设置为输出
  pinMode(right_R2, OUTPUT);
  pinMode(PWM_R, OUTPUT);
  digitalWrite(right_R1, LOW);
  digitalWrite(right_R2, LOW);

  pinMode(left_L1, OUTPUT);
  pinMode(left_L2, OUTPUT);
  pinMode(PWM_L, OUTPUT);
  digitalWrite(left_L1, LOW);
  digitalWrite(left_L2, LOW);

  pinMode(PinA_left, INPUT);    //设置检测脉冲的引脚为输入状态
  pinMode(PinA_right, INPUT);
#if INT_ENABLE
  //外部中断，用于计算车轮转速
  attachInterruptWithDebounce(PinA_left, Code_left, CHANGE, 0);  //PinA_left引脚的电平发生改变触发外部中断，执行子函数 Code_left
  attachInterruptWithDebounce(PinA_right, Code_right, CHANGE, 0); //PinA_right引脚的电平发生改变触发外部中断，执行子函数 Code_right
#endif
}

void loop() 
{
  int readLVal, readRVal;

  //两电机都正转
  digitalWrite(right_R1, HIGH);
  digitalWrite(right_R2, LOW);
  digitalWrite(left_L1, HIGH);
  digitalWrite(left_L2, LOW);
  analogWrite(PWM_R, (1000*1000)/PWM_FREQUENCY, 80);
  analogWrite(PWM_L, (1000*1000)/PWM_FREQUENCY, 160);

  newtime = times = millis();         //使变量newtime和times都等于程序运行到这的时间
  while ((newtime - times) < d_time)  //如果小于设定时间d_time，就一直循环
  {
#if !INT_ENABLE
    readLVal = digitalRead(PinA_left);
    readRVal = digitalRead(PinA_right);

    if( (readLVal + flagA) == 1 ) {   //偵測到電位的變化
      valA++;
      flagA = !flagA;
    }
    if( (readRVal + flagB) == 1 ) {
      valB++;
      flagB = !flagB;
    }
#endif
    newtime = millis(); //newtime等于程序运行到这的时间    
  }

  Serial.print(valA); Serial.print(',');
  Serial.println(valB);
  valA = valB = 0;
}
#if INT_ENABLE
/////////////////////霍尔计数/////////////////////////
//左测速码盘计数
void Code_left() 
{
  valA ++;
}
//右测速码盘计数
void Code_right() 
{
  valB ++;
}
#endif
