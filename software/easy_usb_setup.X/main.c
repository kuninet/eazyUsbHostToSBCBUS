#include <xc.h>

// CONFIGビットの設定
#pragma config FOSC = INTOSC    // 内部クロック
#pragma config WDTE = OFF       // ウォッチドッグタイマー無効
#pragma config PWRTE = OFF      // 電源タイマ無効
#pragma config MCLRE = ON       // MCLRピン有効
#pragma config CP = OFF         // プログラムメモリ保護無効
#pragma config BOREN = ON       // BOR有効
#pragma config CLKOUTEN = OFF   // CLKOUTピン無効
#pragma config IESO = OFF       // 内部/外部スイッチオーバー無効
#pragma config FCMEN = OFF      // フェイルセーフクロックモニタ無効

#define _XTAL_FREQ 8000000         // 実際の動作クロックに合わせる（8MHz）
#define CS_PIN LATAbits.LATA5      // チップセレクトピン（RA5）

void SPI_Init_Master() {
    // クロックを8MHzに設定
    OSCCONbits.IRCF = 0b1110;  // 8MHz内部クロック
    OSCCONbits.SCS = 0b10;    // 内部クロック使用

    // アナログピンをデジタルに設定
    ANSELAbits.ANSA0 = 0; // RA0 as digital (SDO)
    ANSELAbits.ANSA1 = 0; // RA1 as digital (SCK)
    ANSELAbits.ANSA2 = 0; // RA2 as digital (SDI)

    // SPIピンの方向設定
    TRISAbits.TRISA0 = 0; // SDO (出力)
    TRISAbits.TRISA1 = 0; // SCK (出力)
    TRISAbits.TRISA2 = 1; // SDI (入力)

    // チップセレクトピン設定（RA5）
    TRISAbits.TRISA5 = 0; // CS出力
    CS_PIN = 1;           // 初期状態は非アクティブ（HIGH）

    // SPI機能の設定
    APFCONbits.SDOSEL = 0; // SDOをRA0に設定
    SSP1STAT = 0x40;       // CKE=1（クロックエッジ）
    SSP1CON1 = 0x20;       // SPI Master, クロック=FOSC/4（=2MHz）
}

void SPI_Start_Communication() {
    CS_PIN = 0;  // 通信開始（チップセレクト LOW）
}

void SPI_End_Communication() {
    CS_PIN = 1;  // 通信終了（チップセレクト HIGH）
}

void SPI_Write(unsigned char data) {
    SSP1BUF = data;              // データ送信開始
    while (!SSP1STATbits.BF);    // 送信完了を待機（BF=1 になるまで）
}

void main() {


    SPI_Init_Master();     // SPI初期化
    __delay_us(100);       // 電源投入後の安定待ち
    

    while (1) {
        SPI_Start_Communication(); // CS = LOW

        SPI_Write(0x84);           // 最初のデータ送信
        __delay_ms(1);             // 
        SPI_Write(0x40);           // 次のデータ送信

        SPI_End_Communication();   // CS = HIGH

        __delay_ms(1000);          // 次の送信まで待機
    }
}
