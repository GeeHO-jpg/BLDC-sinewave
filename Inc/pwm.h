/*
 * pwm.h
 *
 *  Created on: May 30, 2025
 *      Author: _TTTXN
 */

#ifndef INC_PWM_H_
#define INC_PWM_H_

#include "main.h"
#include "tim.h"
#include "math.h"

extern uint32_t period,prescale,clk,pwm;
extern float field_mech,speed_rpm;


#define tim_roll_a	htim15
#define tim_roll_b	htim1
#define tim_roll_c	htim1
#define roll_a		 TIM_CHANNEL_2
#define roll_b		 TIM_CHANNEL_1
#define roll_c		 TIM_CHANNEL_2

#define tim_pitch_a	htim3
#define tim_pitch_b	htim3
#define tim_pitch_c	htim15
#define pitch_a		 TIM_CHANNEL_3
#define pitch_b		 TIM_CHANNEL_4
#define pitch_c		 TIM_CHANNEL_1

#define tim_yaw_a	htim14
#define tim_yaw_b	htim16
#define tim_yaw_c	htim17
#define yaw_a		 TIM_CHANNEL_1
#define yaw_b		 TIM_CHANNEL_1
#define yaw_c		 TIM_CHANNEL_1


#define LUT_SIZE 4096
#define TWO_PI 2*M_PI
#define ANG_120 (TWO_PI / 3.0f)
#define ANG_240 (2.0f * ANG_120)


#define POLE_PAIRS  (14.0f/2.0f)   // 14 pole → 7 pole-pairs
#define RED2DEG (180.0f/M_PI)
#define DEG2RED (M_PI/180.f)



typedef struct{
    float percent_torque;
    float torque;

}AxisControl_t;


void set_start();
void set_duty(TIM_HandleTypeDef *tim,uint32_t channel,int percen_duty);
void PWM_Update_2(
    float theta,
    AxisControl_t *parameter,
    TIM_HandleTypeDef *htimA, uint32_t channelA,
    TIM_HandleTypeDef *htimB, uint32_t channelB,
    TIM_HandleTypeDef *htimC, uint32_t channelC
);
void MotorSpinOpenLoop(
    float speed_rpm,
    float dt,
    AxisControl_t       *param,
    TIM_HandleTypeDef   *htimA, uint32_t chA,
    TIM_HandleTypeDef   *htimB, uint32_t chB,
    TIM_HandleTypeDef   *htimC, uint32_t chC
);

//void set_duty(){
//	__HAL_TIM_SET_COMPARE();
//}

#endif /* INC_PWM_H_ */
