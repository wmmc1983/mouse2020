/*
 * interrupt.c
 *
 *  Created on: May 2, 2019
 *      Author: blue
 */


#include "global.h"



void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){

  /*==========================================================
      走行用タイマ割り込み
  ==========================================================*/

  /**********
  ---走行の仕組み---
  ステッピングモータの動作制御は16bitタイマで行われている。各16bitタイマの設定は，
  ・カウント開始からCCR1までの間は出力ピンがLowになる
  ・CCR1に達すると出力ピンがHighになる
  ・ARRに達すると割り込みを生成+タイマカウンタをリセット
  となっている（drive_init函数参照）
  モータドライバの（RefをHighにした状態で）ClockをHighにすることで一定角度回転し，
  Lowに戻した後またHighにすることでまた一定角度回転する。
  Clockにはタイマの出力ピンを繋いであるので，タイマの周期を速くするほど回転も速くなる。
  このプログラムではカウント開始からCCR1の間を一定にして（モータドライバがHighを認識する分長さが必要），
  カウント開始からARRの間を調整することで速度を変化させている。
  加減速等の状態はMF.FLAG構造体（global.hで定義）で管理されている。
  加減速について，事前にExcelで計算した時間（割り込み）ごとのARRの値をtable[]配列に記録しておく。
  （配列の値はtable.hにExcelからコピー&ペーストをして設定する。table[]の定義はdrive.h参照）
  今加減速のどの段階なのか（table[]の要素番号・インデックス）はt_cnt_l,t_cnt_rで記録している。
  **********/

  if (htim->Instance == htim16.Instance) {
    /*--------------------------------------------------------------------
        16ビットタイマーTIM16の割り込み処理，左モータの管理をおこなう
    --------------------------------------------------------------------*/

    pulse_l++;                        //左パルスのカウンタをインクリメント

    //====加減速処理====
    //----減速処理----
    if(MF.FLAG.DECL){                 //減速フラグが立っている場合
      t_cnt_l = max(t_cnt_l - 1, min_t_cnt);
    }
    //----加速処理----
    else if(MF.FLAG.ACCL){            //加速フラグが立っている場合
      t_cnt_l = min(t_cnt_l + 1, max_t_cnt);
    }

    //----デフォルトインターバル----
    if(MF.FLAG.DEF){                  //デフォルトインターバルフラグが立っている場合
      __HAL_TIM_SET_AUTORELOAD(&htim16, DEFAULT_INTERVAL - dl );  //デフォルトインターバルに制御を加えた値に設定
    }
    //----それ以外の時はテーブルカウンタの指し示すインターバル----
    else {
      __HAL_TIM_SET_AUTORELOAD(&htim16, table[t_cnt_l] - dl);     //左モータインターバル設定
    }


  } /* if (htim->Instance == htim16.Instance) */

  else if (htim->Instance == htim17.Instance) {
    /*--------------------------------------------------------------------
        16ビットタイマーTIM17の割り込み処理，右モータの管理をおこなう
    --------------------------------------------------------------------*/

    pulse_r++;                        //右パルスのカウンタをインクリメント

    //====加減速処理====
    //----減速処理----
    if(MF.FLAG.DECL){                 //減速フラグが立っている場合
      t_cnt_r = max(t_cnt_r - 1, min_t_cnt);
    }
    //----加速処理----
    else if(MF.FLAG.ACCL){            //加速フラグが立っている場合
      t_cnt_r = min(t_cnt_r + 1, max_t_cnt);
    }

    //----デフォルトインターバル----
    if(MF.FLAG.DEF){                  //デフォルトインターバルフラグが立っている場合
      __HAL_TIM_SET_AUTORELOAD(&htim17, DEFAULT_INTERVAL - dr);   //デフォルトインターバルに制御を加えた値に設定
    }
    //----それ以外の時はテーブルカウンタの指し示すインターバル----
    else {
      __HAL_TIM_SET_AUTORELOAD(&htim17, table[t_cnt_r] - dr);     //右モータインターバル設定
    }


  }  /* if (htim->Instance == htim17.Instance) */

  else if (htim->Instance == htim6.Instance) {
    /*==========================================================
        タスク実行用タイマ割り込み
    ==========================================================*/
    /*----------------------------------------------------------
        16ビットタイマーTIM6の割り込み処理，センサ値の取得，制御比例値の計算をおこなう
    ----------------------------------------------------------*/

    switch(tp){

      //----センサ処理----
      case 0:
        //右センサ値の取得
        HAL_GPIO_WritePin(IR_R_GPIO_Port, IR_R_Pin, GPIO_PIN_SET);      //発光部LEDをON
        tim6_wait_us(IR_WAIT_US);                                       //光が強まるまで少し待機
        ad_r = get_sensor_value_r();                                    //センサ値を記録
        HAL_GPIO_WritePin(IR_R_GPIO_Port, IR_R_Pin, GPIO_PIN_RESET);    //発光部LEDをOFF
        //左センサ値の取得
        HAL_GPIO_WritePin(IR_L_GPIO_Port, IR_L_Pin, GPIO_PIN_SET);      //発光部LEDをON
        tim6_wait_us(IR_WAIT_US);                                       //光が強まるまで少し待機
        ad_l = get_sensor_value_l();                                    //センサ値を記録
        HAL_GPIO_WritePin(IR_L_GPIO_Port, IR_L_Pin, GPIO_PIN_RESET);    //発光部LEDをOFF
        break;

      case 1:
        //正面センサ値の取得
        HAL_GPIO_WritePin(IR_FR_GPIO_Port, IR_FR_Pin, GPIO_PIN_SET);    //発光部LEDをON
        tim6_wait_us(IR_WAIT_US);                                       //光が強まるまで少し待機
        ad_fr = get_sensor_value_fr();                                  //センサ値を記録
        HAL_GPIO_WritePin(IR_FR_GPIO_Port, IR_FR_Pin, GPIO_PIN_RESET);  //発光部LEDをOFF
        HAL_GPIO_WritePin(IR_FL_GPIO_Port, IR_FL_Pin, GPIO_PIN_SET);    //発光部LEDをON
        tim6_wait_us(IR_WAIT_US);                                       //光が強まるまで少し待機
        ad_fl = get_sensor_value_fl();                                  //センサ値を記録
        HAL_GPIO_WritePin(IR_FL_GPIO_Port, IR_FL_Pin, GPIO_PIN_RESET);  //発光部LEDをOFF
        break;

      //----制御処理----
      case 2:
        //制御フラグがあれば制御
        if(MF.FLAG.CTRL){
          //比例制御値を一次保存する変数を宣言し0で初期化
          int16_t dl_tmp = 0, dr_tmp = 0;
          //基準値からの差を見る
          dif_l = (int32_t) ad_l - base_l;
          dif_r = (int32_t) ad_r - base_r;

          if(CTRL_BASE_L < dif_l){              //制御の判断
            dl_tmp += CTRL_CONT * dif_l;        //比例制御値を決定
            dr_tmp += -1 * CTRL_CONT * dif_l;   //比例制御値を決定
          }
          if(CTRL_BASE_R < dif_r){
            dl_tmp += -1 * CTRL_CONT * dif_r;   //比例制御値を決定
            dr_tmp += CTRL_CONT * dif_r;        //比例制御値を決定
          }

          //一次保存した制御比例値をdlとdrに反映させる
          dl = max(min(CTRL_MAX, dl_tmp), -1 * CTRL_MAX);
          dr = max(min(CTRL_MAX, dr_tmp), -1 * CTRL_MAX);
        }else{
          //制御フラグがなければ制御値0
          dl = dr = 0;
        }
        break;
    }

    //====タスクポインタを進める====
    tp = (tp+1) % 3;

  } /* if (htim->Instance == htim6.Instance) */

} /* HAL_TIM_PeriodElapsedCallback */



//+++++++++++++++++++++++++++++++++++++++++++++++
//tim6_wait_us
// 1us毎にカウントアップするTIM6を使ってusマイクロ秒処理を止める関数。
// （whileループ中にオーバーフローが起こると機能しないのでTIM6タイマ更新割り込みハンドラ内のみで使用することを推奨する）
// 引数：us …… 処理を止めたいマイクロ秒
// 戻り値：無し
//+++++++++++++++++++++++++++++++++++++++++++++++
void tim6_wait_us(uint32_t us){
  uint32_t dest = __HAL_TIM_GET_COUNTER(&htim6) + us;
  while(__HAL_TIM_GET_COUNTER(&htim6) < dest);
}
