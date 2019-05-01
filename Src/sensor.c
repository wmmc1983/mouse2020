/*
 * sensor.c
 *
 *  Created on: May 2, 2019
 *      Author: blue
 */


#include "global.h"


//+++++++++++++++++++++++++++++++++++++++++++++++
//sensor_init
// センサ系の変数の初期化，ADコンバータの設定とセンサ値取得に使用するタイマの設定をする
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void sensor_init(void){

  //====センサ系の変数の初期化====
  tp = 0;
  ad_l = ad_r = ad_fr = ad_fl = 0;
  base_l = base_r = 0;


  /*------------------------------------
    TIM6 Interrupt Initialization
  ------------------------------------*/
  HAL_TIM_Base_Start_IT(&htim6);

}


//+++++++++++++++++++++++++++++++++++++++++++++++
//get_adc_value
// 指定されたチャンネルのアナログ電圧値を取り出す
// 引数1：hadc …… 電圧値を取り出すチャンネルが属すADCのHandler
// 引数2：channel …… 電圧値を取り出すチャンネル
// 戻り値：電圧値（12bit分解能）
//+++++++++++++++++++++++++++++++++++++++++++++++
int get_adc_value(ADC_HandleTypeDef *hadc, uint32_t channel){

  ADC_ChannelConfTypeDef sConfig = {0};

  sConfig.Channel = channel;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;

  HAL_ADC_ConfigChannel(hadc, &sConfig);

  HAL_ADC_Start(hadc);                    // AD変換を開始する
  HAL_ADC_PollForConversion(hadc, 100);   // AD変換終了まで待機する
  return HAL_ADC_GetValue(hadc);          // AD変換結果を取得する
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//get_sensor_value_r
// Sensor R の値を読み取る
// 引数：無し
// 戻り値：電圧値（12bit分解能）
//+++++++++++++++++++++++++++++++++++++++++++++++
int get_sensor_value_r(void){
  return get_adc_value(&hadc1, ADC_CHANNEL_3);
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//get_sensor_value_fr
// Sensor FR の値を読み取る
// 引数：無し
// 戻り値：電圧値（12bit分解能）
//+++++++++++++++++++++++++++++++++++++++++++++++
int get_sensor_value_fr(void){
  return get_adc_value(&hadc1, ADC_CHANNEL_1);
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//get_sensor_value_fl
// Sensor FL の値を読み取る
// 引数：無し
// 戻り値：電圧値（12bit分解能）
//+++++++++++++++++++++++++++++++++++++++++++++++
int get_sensor_value_fl(void){
  return get_adc_value(&hadc2, ADC_CHANNEL_4);
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//get_sensor_value_l
// Sensor L の値を読み取る
// 引数：無し
// 戻り値：電圧値（12bit分解能）
//+++++++++++++++++++++++++++++++++++++++++++++++
int get_sensor_value_l(void){
  return get_adc_value(&hadc2, ADC_CHANNEL_2);
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//get_base
// 壁制御用の基準値を取得する
// 引数：なし
// 戻り値：理想的な値を取得できたか　1:できた　0:できなかった
//+++++++++++++++++++++++++++++++++++++++++++++++
uint8_t get_base(){
  uint8_t res = 1;                  //理想的な値を取得できたか

  //----制御用の基準を取得----
  base_l = ad_l;                    //現在の左側のセンサ値で決定
  base_r = ad_r;                    //現在の右側のセンサ値で決定

  return res;                     //理想的な値を取得できたかを返す

}


//+++++++++++++++++++++++++++++++++++++++++++++++
//get_wall_info
// 壁情報を取得する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void get_wall_info(){

  //----壁情報の初期化----
  wall_info = 0x00;                 //壁情報を初期化
  //----前壁を見る----
  if(ad_fr > WALL_BASE_FR || ad_fl > WALL_BASE_FL){
    //AD値が閾値より大きい（=壁があって光が跳ね返ってきている）場合
    wall_info |= 0x88;                //壁情報を更新
  }
  //----右壁を見る----
  if(ad_r > WALL_BASE_R){
    //AD値が閾値より大きい（=壁があって光が跳ね返ってきている）場合
    wall_info |= 0x44;                //壁情報を更新
  }
  //----左壁を見る----
  if(ad_l > WALL_BASE_L){
    //AD値が閾値より大きい（=壁があって光が跳ね返ってきている）場合
    wall_info |= 0x11;                //壁情報を更新
  }

}
