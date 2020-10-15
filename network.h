//=============================================================================
//
// ネットワークの説明[network.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _NETWORK_H_
#define _NETWORK_H_	 // ファイル名を基準を決める

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "keyboard.h"
#include "manager.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define SCRIPT_NETWORK "network.ini"
#define MAX_COIN 20

//=============================================================================
// 列挙型定義
//=============================================================================
typedef enum
{
	NUM_KEY_W = 0,
	NUM_KEY_A,
	NUM_KEY_S,
	NUM_KEY_D,
	NUM_KEY_SPACE,
	NUM_KEY_M
} NUM_KEY;

typedef enum
{
	SOCKETTYPE_CLIENT = 0,
	SOCKETTYPE_GAME,
	SOCKETTYPE_MAX
} SOCKETTYPE;

typedef enum
{
	RECVDATA_STICK_H = NUM_KEY_M,
	RECVDATA_STICK_V,
	RECVDATA_ROT,
	RECVDATA_POS_X,
	RECVDATA_POS_Y,
	RECVDATA_POS_Z,
	RECVDATA_RANK,
	RECVDATA_MAX
} RECVDATA;

typedef enum
{
	STICKTYPE_H = 0,
	STICKTYPE_V,
	STICKTYPE_MAX
} STICKTYPE;

//=============================================================================
// 前方宣言
//=============================================================================
class CItem;
class CBalloonNum;

//=============================================================================
// 構造体定義
//=============================================================================
typedef struct
{
	//int m_nId = -1;										// ID番号
	int m_aKeyState[NUM_KEY_M] = {};				// キーボードのプレス状態
} PLAYERSTATE;

typedef struct
{
	D3DXVECTOR3 pos;			// 位置
	bool bCreate;				// 生成フラグ
} WAITEVENT;

typedef struct
{
	int nId;			// ID
	int nType;			// 種類
	bool bCreate;				// 生成フラグ
} WAITEVENT_SELECT;

typedef struct
{
	int nType;
	bool bReady;
	CBalloonNum *pBalloonNum;
} CHARACTERSELECT;

//=============================================================================
// クラス
//=============================================================================
class CNetwork
{
public:
	/* 関数 */
	CNetwork();
	~CNetwork();
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);

	static HRESULT LoadConfiguration(void);

	HRESULT Build(void);
	HRESULT Connect(void);

	PLAYERSTATE GetKeystate(void);

	bool SendTCP(const char* data, int nSize);
	bool SendUDP(const char* data, int nSize);
	bool DataRecv(SOCKETTYPE type, char* data, int nSize);

	bool KeyData(void);
	SOCKET GetSocket(void) { return m_sockClient; }
	bool GetPressKeyboard(int nId, int nKey);
	bool GetTriggerKeyboard(int nId, int nKey);
	int GetId(void) { return m_nId; }
	float GetRot(const int &nId) { return m_fRot[nId]; }

	bool UpdateUDP(void);
	bool UpdateTCP(void);
	void StartUpdate(void);
	void StopUpdate(void);
	void CloseTCP(void);

	D3DXVECTOR3 GetPosition(int nIndex) { return m_playerPos[nIndex]; }
	int GetRank(int nIndex) { return m_nRank[nIndex]; }
	bool GetDie(int nIndex) { return m_bDie[nIndex]; }
	bool CheckCharacterReady(int nIndex);

	void Create(void);

	void ResetCoin(void);
	void UpdateCharacterState(void);

private:
	SOCKET createServerSocket(unsigned short port);
	static void ConvertStringToFloat(char* text,const char* delimiter, float* pResult);

	// ネットワーク接続関連
	SOCKET m_sockClient;								// ソケット(クライアント)
	SOCKET m_sockClientToServer;						// ソケット(UDP送信用)
	SOCKET m_sockServerToClient;						// ソケット(UDP受信用)
	struct sockaddr_in m_addrServer;					// ソケットアドレス(サーバ)の情報(TCP送受信)
	struct sockaddr_in m_addrClientToServer;			// キー送信用ソケット(UDP送信用)
	fd_set m_readfds;
	static char aIp[32];					// IPアドレス
	static int nPort;						// ポート番号

	PLAYERSTATE keystate;

	// 受信情報関連
	bool m_aKeyState[MAX_PLAYER][NUM_KEY_M] = {};				//キーボードの入力情報ワーク
	bool m_aKeyStateOld[MAX_PLAYER][NUM_KEY_M] = {};			// 前のキーボード入力情報ワーク
	bool m_aKeyStateTrigger[MAX_PLAYER][NUM_KEY_M] = {};		// Trigger

	int m_nId;								// ID
	int m_nStick[STICKTYPE_MAX];
	float m_fRot[MAX_PLAYER];				// 回転情報
	bool m_bTimeout;						// タイムアウトフラグ
	D3DXVECTOR3 m_playerPos[MAX_PLAYER];	// プレイヤー位置情報
	bool m_bDie[MAX_PLAYER];				// 1つ前の死亡フラグ
	int m_nRank[MAX_PLAYER];
	CItem *m_apItem[MAX_COIN];				// コインのポインタ

	// マルチスレッド関連
	std::thread m_th;					// スレッド
	bool m_bUpdate;						// 更新フラグ
	WAITEVENT m_thunderEvent;			// 雷待機
	WAITEVENT m_pointcircleEvent;		// ポイントサークル待機
	WAITEVENT_SELECT m_selectBalloon;	// キャラクタ選択UI

	CHARACTERSELECT m_selectState[MAX_PLAYER];		// 選択状況

	D3DXVECTOR3 m_samplePos[MAX_PLAYER];
};
#endif // !_NETWORK_H_