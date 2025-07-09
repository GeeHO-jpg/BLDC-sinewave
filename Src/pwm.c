/*
 * pwm.c
 *
 *  Created on: May 30, 2025
 *      Author: _TTTXN
 */


#include "pwm.h"

uint32_t period,prescale,clk,pwm;
float field_mech,speed_rpm;



static float sin_lut[LUT_SIZE];

static void init_sin_lut(void) {
  for(int i=0;i<LUT_SIZE;i++)
    sin_lut[i] = sinf(2.0f*M_PI*i/LUT_SIZE);
}

static float fast_sin(float x) {
  // x in [0,2π)
  float idx_f = x*(LUT_SIZE/(2.0f*M_PI));
  int idx = ((int)idx_f) & (LUT_SIZE-1);
  int idx2 = (idx+1) & (LUT_SIZE-1);
  float frac = idx_f - (int)idx_f;
  return sin_lut[idx] + (sin_lut[idx2] - sin_lut[idx])*frac;
}


void set_start(){

	period = htim1.Init.Period ;
	prescale = htim1.Init.Prescaler ;
	clk = HAL_RCC_GetPCLK1Freq();
	pwm = clk/((period + 1) * (prescale + 1));

	init_sin_lut();

	//yaw
	HAL_TIM_PWM_Start(&htim14, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim17, TIM_CHANNEL_1);
	//pitch
	HAL_TIM_PWM_Start(&htim15, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
	//roll
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim15, TIM_CHANNEL_2);
}


void set_duty(TIM_HandleTypeDef *tim,uint32_t channel,int percen_duty){

	uint16_t duty =  percen_duty * 0.01 * period;

	__HAL_TIM_SET_COMPARE(tim, channel,duty);
}


void PWM_Update_2(
    float theta,
    AxisControl_t *parameter,
    TIM_HandleTypeDef *htimA, uint32_t channelA,
    TIM_HandleTypeDef *htimB, uint32_t channelB,
    TIM_HandleTypeDef *htimC, uint32_t channelC
) {
    // 1. torque scale พรีคอมพ์ได้ แต่ sys เปลี่ยนได้ก็คำนวนในนี้รอบเดียว
	parameter->torque = period * (parameter->percent_torque * 0.01f);


    // 2. wrap theta ให้อยู่ [0, TWO_PI)
    float ta = theta;
    if      (ta >= TWO_PI) ta -= TWO_PI;
    else if (ta <  0.0f )  ta += TWO_PI;

    // 3. สร้างมุมสำหรับ phase B และ C แล้ว wrap
    float tb = ta - ANG_120;
    if (tb < 0.0f) tb += TWO_PI;

    float tc = ta + ANG_120;  // = theta - 4π/3 mod 2π
    if (tc >= TWO_PI) tc -= TWO_PI;

    // 4. lookup sine จาก LUT
    float Va = fast_sin(ta);
    float Vb = fast_sin(tb);
    float Vc = fast_sin(tc);

    // 5. คำนวน duty และอัพเดต CCR
    uint16_t dutyA = (uint16_t)((Va * 0.5f + 0.5f) * parameter->torque);
    uint16_t dutyB = (uint16_t)((Vb * 0.5f + 0.5f) * parameter->torque);
    uint16_t dutyC = (uint16_t)((Vc * 0.5f + 0.5f) * parameter->torque);

    __HAL_TIM_SET_COMPARE(htimA, channelA, dutyA);
    __HAL_TIM_SET_COMPARE(htimB, channelB, dutyB);
    __HAL_TIM_SET_COMPARE(htimC, channelC, dutyC);
}


/**
 * @brief  สร้างสนามหมุนให้มอเตอร์หมุน open-loop
 * @param  speed_rpm       ความเร็วรอบที่ต้องการ (RPM)
 * @param  direction       ทิศทาง DIR_CW หรือ DIR_CCW
 * @param  percent_torque  แรงบิดเป็น % (0…100)
 * @param  dt              เวลาตั้งแต่เรียกครั้งก่อน [s]
 * @param  param           pointer ไปยัง AxisControl_t (มี percent_torque, torque)
 * @param  htimA–C, chA–C  Timer+Channel สำหรับ PWM เฟส A–C
 */
void MotorSpinOpenLoop(
    float speed_rpm,
    float dt,
    AxisControl_t       *param,
    TIM_HandleTypeDef   *htimA, uint32_t chA,
    TIM_HandleTypeDef   *htimB, uint32_t chB,
    TIM_HandleTypeDef   *htimC, uint32_t chC
) {
    static float mech_angle = 0.0f;  // เก็บมุมกลสะสม

    // 1) อัปเดตมุมกล (mechanical angle)
    float omega = speed_rpm * TWO_PI / 60.0f;       // [rad/s]
    mech_angle +=  omega * dt;           // + หรือ – ขึ้นอยู่กับ direction
    mech_angle = fmodf(mech_angle, TWO_PI);
    if (mech_angle < 0) mech_angle += TWO_PI;

    // 2) แปลงเป็นมุมไฟฟ้า (electrical angle)
    float theta_e = mech_angle * POLE_PAIRS;
    theta_e = fmodf(theta_e, TWO_PI);
    if (theta_e < 0) theta_e += TWO_PI;



    // 4) เรียกอัปเดต PWM 3 เฟส
    PWM_Update_2(
      theta_e, param,
      htimA, chA,
      htimB, chB,
      htimC, chC
    );
}





