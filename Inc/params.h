/*
 * params.h
 *
 *  Created on: May 2, 2019
 *      Author: blue
 */

#ifndef INC_PARAMS_H_
#define INC_PARAMS_H_



/*============================================================
    各種定数（パラメータ）設定
============================================================*/
/*------------------------------------------------------------
    走行系
------------------------------------------------------------*/
//----走行パルス関連----
#define PULSE_SEC_HALF      208   //半区画走行用パルス。2倍すると1区画分に 208
#define PULSE_ROT_R90       158   //右90度回転用パルス数 145
#define PULSE_ROT_L90       158   //左90度回転用パルス数 145
#define PULSE_ROT_180       316   //180度回転用パルス数 289
#define PULSE_SETPOS_BACK   300   //後ろ壁に当てるために下がるパルス数
#define PULSE_SETPOS_SET    92    //後ろ壁から中央までのパルス数
//----テーブルカウンタ関連----
#define MAX_T_CNT PULSE_SEC_HALF  //テーブルカウンタの最大値
#define MIN_T_CNT 0               //テーブルカウンタの最小値
//----タイマ関連----
#define DEFAULT_INTERVAL  3000    //デフォルトのインターバル


//----動作方向関連----
#define MT_FWD_L  GPIO_PIN_SET    //CW/CCWで前に進む出力（左）
#define MT_BACK_L GPIO_PIN_RESET  //CW/CCWで後ろに進む出力（左）
#define MT_FWD_R  GPIO_PIN_RESET  //CW/CCWで前に進む出力（右）
#define MT_BACK_R GPIO_PIN_SET    //CW/CCWで後ろに進む出力（右）



/*------------------------------------------------------------
    センサ系
------------------------------------------------------------*/
//----壁判断閾値（しきい値）----
#define WALL_BASE_FR  30    //前壁右センサ
#define WALL_BASE_FL  30    //前壁左センサ
#define WALL_BASE_R   100   //右壁センサ
#define WALL_BASE_L   100   //左壁センサ

//----制御閾値（しきい値）----
#define CTRL_BASE_L   100   //左制御閾値
#define CTRL_BASE_R   100   //右制御閾値
#define CTRL_MAX      350   //制御量上限値
#define CTRL_CONT     0.3F  //比例制御係数

//----赤外線（赤色）LED発光待機時間（単位はマイクロ秒）
#define IR_WAIT_US  15



/*------------------------------------------------------------
    探索系
------------------------------------------------------------*/
//----ゴール座標----
#define GOAL_X 7  //7
#define GOAL_Y 7  //7


#endif /* INC_PARAMS_H_ */
