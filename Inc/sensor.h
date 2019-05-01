/*
 * sensor.h
 *
 *  Created on: May 2, 2019
 *      Author: blue
 */

#ifndef INC_SENSOR_H_
#define INC_SENSOR_H_


/*============================================================
    各種定数・変数宣言
============================================================*/
//====変数====
#ifdef MAIN_C_                        //main.cからこのファイルが呼ばれている場合
  /*グローバル変数の定義*/
  //----その他----
  uint8_t tp;                       //タスクポインタ
  uint32_t ad_r, ad_fr, ad_fl, ad_l;            //A-D値格納
  uint16_t base_l, base_r;                //基準値を格納
  int16_t dif_l, dif_r;                 //AD値と基準との差
#else                           //main.c以外からこのファイルが呼ばれている場合
  extern uint8_t tp;
  extern uint32_t ad_r, ad_fr, ad_fl, ad_l;
  extern uint16_t base_l,  base_r;
  extern int16_t dif_l, dif_r;
#endif

/*============================================================
    関数プロトタイプ宣言
============================================================*/

void sensor_init(void);
int get_adc_value(ADC_HandleTypeDef*, uint32_t);
int get_sensor_value_r(void);
int get_sensor_value_fr(void);
int get_sensor_value_fl(void);
int get_sensor_value_l(void);


uint8_t get_base();         //センサ基準値を取得
void get_wall_info();       //壁情報を読む



#endif /* INC_SENSOR_H_ */
