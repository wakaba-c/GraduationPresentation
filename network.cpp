//=============================================================================
//
// ネットワーク処理 [network.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "network.h"
#include "renderer.h"
#include "camera.h"
#include "player.h"
#include "game.h"
#include "score.h"
#include "fade.h"
#include "inputController.h"
#include "enemy.h"
#include "object.h"

//=============================================================================
// 静的メンバ変数
//=============================================================================
char CNetwork::aIp[32] = {};
int CNetwork::nPort = 0;

//=============================================================================
// コンストラクタ
//=============================================================================
CNetwork::CNetwork()
{
	m_nId = -1;
	m_bTimeout = false;

	m_selectBalloon.bCreate = false;
	m_selectBalloon.nId = -1;
	m_selectBalloon.nType = -1;

	// 生成フラグの初期化
	m_thunderEvent.bCreate = false;
	m_pointcircleEvent.bCreate = false;

	for (int nCount = 0; nCount < MAX_PLAYER; nCount++)
	{
		m_selectState[nCount].bReady = false;
		m_selectState[nCount].nType = -1;
		m_selectState[nCount].pBalloonNum = NULL;
		m_nRank[nCount] = 0;
	}
}

//=============================================================================
// デストラクタ
//=============================================================================
CNetwork::~CNetwork()
{

}

//=============================================================================
// 初期化
//=============================================================================
HRESULT CNetwork::Init(void)
{
	HRESULT hr;
	hr = Build();					// ソケットの生成

	OutputDebugString("クライアント構築完了\n");

	for (int nCount = 0; nCount < MAX_PLAYER; nCount++)
	{
		m_fRot[nCount] = 0.0f;
		m_bDie[nCount] = false;
		m_pEnemy[nCount] = NULL;
		m_samplePos[nCount] = D3DXVECTOR3(250.0f * (nCount + 1), 600.0f, 0.0f);
	}

	m_bUpdate = false;

	return hr;
}

//=============================================================================
// 終了
//=============================================================================
void CNetwork::Uninit(void)
{
	// 別スレッドで動作している更新処理の終了処理
	StopUpdate();

	//ソケット(クライアント)の開放
	CloseTCP();

	// winsock2 の終了処理
	WSACleanup();
}

//=============================================================================
// 更新  if ( setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (const char *),  sizeof(param) )
//=============================================================================
void CNetwork::Update(void)
{
	DWORD dwCurrentTime;		// 現在時刻
	DWORD dwExecLastTime;		// 最後に処理した時刻
	DWORD dwFrameCount;
	DWORD dwFPSLastTime;

	dwCurrentTime = 0;
	dwExecLastTime = timeGetTime();		// システム時刻をミリ秒単位で取得

	dwFrameCount = 0;
	dwFPSLastTime = timeGetTime();		// システム時刻をミリ秒単位で取得

	while (m_bUpdate)
	{
		// DirectXの処理
		dwCurrentTime = timeGetTime();		// システム時刻を取得

		if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))		// 1/60秒ごとに実行
		{
			dwExecLastTime = dwCurrentTime;	// 処理した時刻を保存

			int nError = -1;

			KeyData();

			fd_set readfds;
			FD_ZERO(&readfds);
			FD_SET(m_sockServerToClient, &readfds);
			FD_SET(m_sockClient, &readfds);

			struct timeval tv;

			// 指定した秒数でタイムアウトさせます
			tv.tv_sec = 0;
			tv.tv_usec = 0;

			nError = select(0, &readfds, NULL, NULL, &tv);
			if (nError == 0)
			{
				OutputDebugString("タイムアウト\n");
				continue;
			}

			if (CManager::GetMode() == CManager::MODE_GAME)
			{
				if (FD_ISSET(m_sockServerToClient, &readfds))
				{// 受信
					// キー入力情報の更新
					UpdateUDP();
				}
			}

			if (FD_ISSET(m_sockClient, &readfds))
			{// 受信
				// サーバーコマンドの更新
				UpdateTCP();
			}
		}
	}
}

//=============================================================================
// ネットワークの設定データ読み込み
//=============================================================================
HRESULT CNetwork::LoadConfiguration(void)
{
	FILE *pFile;
	char cReadText[128];		//文字
	char cHeadText[128];		//比較
	char cDie[128];

	//テキストデータロード
	pFile = fopen(SCRIPT_NETWORK, "r");

	if (pFile != NULL)
	{
		//スクリプトが来るまでループ
		while (strcmp(cHeadText, "SCRIPT") != 0)
		{
			fgets(cReadText, sizeof(cReadText), pFile);
			sscanf(cReadText, "%s", &cHeadText);
		}

		//スクリプトだったら
		if (strcmp(cHeadText, "SCRIPT") == 0)
		{
			//エンドスクリプトが来るまで
			while (strcmp(cHeadText, "END_SCRIPT") != 0)
			{
				fgets(cReadText, sizeof(cReadText), pFile);
				sscanf(cReadText, "%s", &cHeadText);

				//改行
				if (strcmp(cReadText, "\n") != 0)
				{
					if (strcmp(cHeadText, "IP") == 0)
					{//パーツ総数のとき
						sscanf(cReadText, "%s %s %s", &cDie, &cDie, aIp);					//パーツの数を取得
					}
					else if (strcmp(cHeadText, "PORT") == 0)
					{//パーツモデルのアドレス情報のとき
						sscanf(cReadText, "%s %s %d", &cDie, &cDie, &nPort);					//パーツの数を取得
					}
				}
			}
		}

		//ファイル閉
		fclose(pFile);
	}
	else
	{
		MessageBox(NULL, "ネットワークの設定データ読込失敗！", "警告！", MB_ICONWARNING);
	}

	return S_OK;
}

//=============================================================================
// キーボードの入力状態の取得
//=============================================================================
PLAYERSTATE CNetwork::GetKeystate(void)
{
	return keystate;
}

//=============================================================================
// 送信(TCP)
//=============================================================================
bool CNetwork::SendTCP(const char *data, int nSize)
{
	int nError;

	//クライアントへデータ(ID)送信
	nError = send(m_sockClient, data, nSize, 0);
	if (nError < 0)
	{
		printf("送信エラー : %d\n", WSAGetLastError());
	}

	return false;
}

//=============================================================================
// 送信(UDP)
//=============================================================================
bool CNetwork::SendUDP(const char *data, int nSize)
{
	int nError;

	//クライアントへデータ(ID)送信
	nError = sendto(m_sockClientToServer, data, nSize, 0, (struct sockaddr *)&m_addrClientToServer, sizeof(m_addrClientToServer));
	if (nError == SOCKET_ERROR)
	{
		char aError[256];
		memset(&aError, 0, sizeof(aError));
		sprintf(aError, "送信エラー : %d\n", WSAGetLastError());
		MessageBox(NULL, aError, "警告！", MB_ICONWARNING);
	}

	return false;
}

//=============================================================================
// 受信
//=============================================================================
bool CNetwork::DataRecv(SOCKETTYPE type, char *data, int nSize)
{
	int nError;
	char aError[256];

	switch (type)
	{
	case SOCKETTYPE_CLIENT:
		//サーバーからデータ受信
		nError = recv(m_sockClient, data, nSize, 0);
		break;
	case SOCKETTYPE_GAME:
		//サーバーからデータ受信
		nError = recv(m_sockClientToServer, data, nSize, 0);
		break;
	}

	if (nError < 0)
	{
		sprintf(aError, "受信エラー : %d\n", WSAGetLastError());
		MessageBox(NULL, aError, "警告！", MB_ICONWARNING);
		return false;
	}

	return true;
}

//=============================================================================
// クライアントソケットの作成
//=============================================================================
HRESULT CNetwork::Build(void)
{
	WSADATA wsaData;								//winsockの情報
	int nError;										//エラー取得

	//winsockの初期化
	nError = WSAStartup(MAKEWORD(2, 0), &wsaData);
	if (nError != 0)
	{
		switch (nError)
		{
		case WSASYSNOTREADY:
			MessageBox(NULL, "ネットワークサブシステムがネットワークへの接続を準備できていません!", "警告！", MB_ICONWARNING);
			return E_FAIL;
			break;
		case WSAVERNOTSUPPORTED:
			MessageBox(NULL, "要求されたwinsockのバージョンがサポートされていません！", "警告！", MB_ICONWARNING);
			return E_FAIL;
			break;
		case WSAEINPROGRESS:
			MessageBox(NULL, "ブロッキング操作の実行中であるか、 またはサービスプロバイダがコールバック関数を処理しています！", "警告！", MB_ICONWARNING);
			return E_FAIL;
			break;
		case WSAEPROCLIM:
			MessageBox(NULL, "winsockが処理できる最大プロセス数に達しました！", "警告！", MB_ICONWARNING);
			return E_FAIL;
			break;
		case WSAEFAULT:
			MessageBox(NULL, "第二引数であるlpWSAData は有効なポインタではありません！", "警告！", MB_ICONWARNING);
			return E_FAIL;
			break;
		}
	}

	// ソケット作成
	m_sockClient = socket(AF_INET, SOCK_STREAM, 0);

	if (m_sockClient == INVALID_SOCKET)
	{// ソケットの作成に失敗したとき
		char aError[64];
		sprintf(aError, "ソケットの生成に失敗!!\n エラーコード : %d", WSAGetLastError());
		MessageBox(NULL, aError, "警告！", MB_ICONWARNING);
		return E_FAIL;
	}

	// ソケット作成
	m_sockClientToServer = socket(AF_INET, SOCK_DGRAM, 0);

	if (m_sockClientToServer == INVALID_SOCKET)
	{// ソケットの作成に失敗したとき
		char aError[64];
		sprintf(aError, "ソケットの生成に失敗!!\n エラーコード : %d", WSAGetLastError());
		MessageBox(NULL, aError, "警告！", MB_ICONWARNING);
		return E_FAIL;
	}

	//ソケットの設定
	m_addrServer.sin_family = AF_INET;
	m_addrServer.sin_port = htons(12345);
	m_addrServer.sin_addr.S_un.S_addr = inet_addr(aIp);

	bind(m_sockClientToServer, (struct sockaddr *)&m_addrServer, sizeof(m_addrServer));

	m_addrClientToServer.sin_family = AF_INET;
	m_addrClientToServer.sin_port = htons(10032);
	m_addrClientToServer.sin_addr.S_un.S_addr = inet_addr(aIp);

	// ソケット作成
	m_sockServerToClient = createServerSocket(nPort);

	return S_OK;
}

//=============================================================================
// コネクト
//=============================================================================
HRESULT CNetwork::Connect(void)
{
	int val = -1;
	unsigned int nStartTime = 0;
	SOCKET sock;

	// ソケットの初期化
	sock = socket(AF_INET, SOCK_STREAM, 0);

	//サーバに接続
	val = connect(sock, (struct sockaddr *)&m_addrServer, sizeof(m_addrServer));
	if (val == SOCKET_ERROR)
	{
		char aError[64];
		sprintf(aError, "ネットワークエラー!\nerror : %d\n", WSAGetLastError());
		MessageBox(NULL, aError, "警告!", MB_ICONWARNING);
		return E_FAIL;
	}

	m_sockClient = sock;

	// IDの取得
	SendTCP("LOAD_ID", sizeof("LOAD_ID"));
	if (!DataRecv(SOCKETTYPE_CLIENT, (char*)&m_nId, sizeof(int)))
	{
		return E_FAIL;
	}

	// シード値の取得
	SendTCP("SEED", sizeof("SEED"));
	if (!DataRecv(SOCKETTYPE_CLIENT, (char*)&nStartTime, sizeof(nStartTime)))
	{
		return E_FAIL;
	}

	srand(nStartTime);				// 乱数の種を設定

#ifdef _DEBUG
	char debug[256];
	sprintf(debug, "ID = %d\n", m_nId);
	OutputDebugString(debug);
	sprintf(debug, "SEED = %d\n", nStartTime);
	OutputDebugString(debug);
#endif // _DEBUG

	StartUpdate();
	return S_OK;
}

//=============================================================================
// キーデータ
//=============================================================================
bool CNetwork::KeyData(void)
{
	CInputKeyboard *pKeyboard = CManager::GetInputKeyboard();
	CInputController *pJoypad = CManager::GetInputController();
	CRenderer *pRenderer = CManager::GetRenderer();
	CCamera *pCamera = CManager::GetCamera();
	CPlayer *pPlayer = CGame::GetPlayer();
	PLAYERSTATE state;
	memset(&state, 0, sizeof(PLAYERSTATE));
	CNetwork *pNetwork = CManager::GetNetwork();
	float stick_H, stick_V;

	if (pPlayer == NULL)
	{
		return false;
	}

	if (pCamera == NULL)
	{
		return false;
	}

	if (pKeyboard == NULL)
	{
		return false;
	}

	if (pJoypad == NULL)
	{
		stick_H = 0.0f;
		stick_V = 0.0f;
	}

	D3DXVECTOR3 pos = pPlayer->GetPosition();
	int nNumFlag = CObject::GetPointNum();
	int nNumRound = pPlayer->GetNumRound();

	if (pNetwork != NULL)
	{
		D3DXVECTOR3 rot = pCamera->GetRotation();

		char data[1024];
		bool aKeyState[NUM_KEY_M] = {};

		if (pJoypad != NULL)
		{
			pJoypad->GetJoypadStickLeft(0, &stick_H, &stick_V);

			if (pKeyboard->GetPressKeyboard(DIK_SPACE) || pJoypad->GetControllerPress(0, JOYPADKEY_A) || pJoypad->GetControllerPress(0, JOYPADKEY_X))
			{
				aKeyState[NUM_KEY_SPACE] = true;
			}
		}
		else
		{
			if (pKeyboard->GetPressKeyboard(DIK_SPACE))
			{
				aKeyState[NUM_KEY_SPACE] = true;
			}
		}

		// ID, Wキー, Aキー, Sキー, Dキー, SPACEキー, スティックH, スティックV, 回転情報, 位置X, 位置Y, 位置Z, スコア
		sprintf(data, "SAVE_KEY %d %d %d %d %d %d %f %f %f %f %f %f %d %d",
			m_nId,
			pKeyboard->GetPressKeyboard(DIK_W), pKeyboard->GetPressKeyboard(DIK_A),
			pKeyboard->GetPressKeyboard(DIK_S), pKeyboard->GetPressKeyboard(DIK_D), aKeyState[NUM_KEY_SPACE],		// キー入力情報
			stick_H,					// スティックH
			stick_V,					// スティックV
			rot.y,						// 回転
			pos.x, pos.y, pos.z,		// 位置
			nNumFlag,					// 次のチェックポイント
			nNumRound					// 現在の周回回数
		);
		pNetwork->SendUDP(data, sizeof("SAVE_KEY") + 1024);
	}

	return true;
}

//=============================================================================
// キーボードのプレス状態取得
//=============================================================================
bool CNetwork::GetPressKeyboard(int nId, int nKey)
{
	return m_aKeyState[nId][nKey];
}

//=============================================================================
// キーボードのトリガー状態取得
//=============================================================================
bool CNetwork::GetTriggerKeyboard(int nId, int nKey)
{
	return m_aKeyStateTrigger[nId][nKey];
}

//=============================================================================
// キャラクターが使用可能かどうか
//=============================================================================
bool CNetwork::CheckCharacterReady(int nIndex)
{
	for (int nCount = 0; nCount < MAX_PLAYER; nCount++)
	{
		if (m_selectState[nCount].bReady)
		{
			if (m_selectState[nCount].nType == nIndex)
			{
				return false;
			}
		}
	}
	return true;
}

//=============================================================================
// クリエイト処理
//=============================================================================
void CNetwork::Create(void)
{
	if (m_thunderEvent.bCreate)
	{// 雷サークル作成処理
	}
	if (m_pointcircleEvent.bCreate)
	{// ポイントサークル作成処理
	}
}

//=============================================================================
// コインのデータベースをリセット
//=============================================================================
void CNetwork::ResetCoin(void)
{
}

//=============================================================================
// キャラクター選択状態の更新
//=============================================================================
void CNetwork::UpdateCharacterState(void)
{
	//CNetwork *pNetwork = CManager::GetNetwork();
	//char aData[256], aDie[64];
	//int nType[MAX_PLAYER];
	//memset(&aData, 0, sizeof(aData));

	//if (pNetwork != NULL)
	//{
	//	pNetwork->SendTCP("CHARACTERSELECT_STATE", sizeof("CHARACTERSELECT_STATE"));
	//	if (!pNetwork->DataRecv(SOCKETTYPE_CLIENT, aData, sizeof(aData)))
	//	{
	//		return;
	//	}

	//	sscanf(aData, "%s %d %d %d %d", &aDie , &nType[0], &nType[1], &nType[2], &nType[3]);

	//	for (int nCount = 0; nCount < MAX_PLAYER; nCount++)
	//	{
	//		if (nType[nCount] == -1)
	//		{
	//			continue;
	//		}

	//		m_selectState[nCount].bReady = true;
	//		m_selectState[nCount].nType = nType[nCount];

	//		if (m_selectState[nCount].pBalloonNum == NULL)
	//		{// 選択中プレイヤー表示UIが存在していないとき
	//		 // UIの作成
	//			m_selectState[nCount].pBalloonNum = CBalloonNum::Create(nCount);

	//			if (m_selectState[nCount].pBalloonNum != NULL)
	//			{
	//				m_selectState[nCount].pBalloonNum->SetPosition(m_samplePos[nType[nCount]]);
	//				m_selectState[nCount].pBalloonNum->Init();
	//			}
	//		}
	//	}
	//}
}

//=============================================================================
// ゲーム時初期化処理
//=============================================================================
void CNetwork::InitGame(void)
{
	for (int nCount = 0; nCount < MAX_PLAYER; nCount++)
	{
		m_pEnemy[nCount] = CEnemy::Create();
	}
}

//=============================================================================
// サーバーソケットを作成する
//=============================================================================
SOCKET CNetwork::createServerSocket(unsigned short port)
{
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == SOCKET_ERROR)
	{// ソケットの作成に失敗したとき
		char aError[64];
		sprintf(aError, "ソケットの生成に失敗!!\n エラーコード : %d", WSAGetLastError());
		MessageBox(NULL, aError, "警告！", MB_ICONWARNING);
		return sock;
	}

	struct sockaddr_in adr;

	adr.sin_family = AF_INET;
	adr.sin_port = htons(port);
	adr.sin_addr.S_un.S_addr = INADDR_ANY;  // サーバ側で割り当てられている IP を自動で設定

	if (SOCKET_ERROR == bind(sock, (struct sockaddr *)&adr, sizeof(adr)))
	{
		char aError[64];
		sprintf(aError, "ソケットの生成に失敗!!\n エラーコード : %d", WSAGetLastError());
		MessageBox(NULL, aError, "警告！", MB_ICONWARNING);
		return sock;
	}

	return sock;
}

//=============================================================================
// 形式変換(float)関数
//=============================================================================
void CNetwork::ConvertStringToFloat(char* text, const char* delimiter, float* pResult)
{
	char* tp;
	char* ctx;
	int nTemp = 1;

	tp = strtok_s(text, delimiter, &ctx);
	pResult[0] = (float)atof(tp);

	while (tp = strtok_s(NULL, ",", &ctx))
	{
		pResult[nTemp++] = (float)atof(tp);
	}
}

//=============================================================================
// UDPから送られてくる情報の整理(キー情報)
//=============================================================================
bool CNetwork::UpdateUDP(void)
{
	char aData[1024];
	float fData[RECVDATA_MAX];
	char cPlayerData[MAX_PLAYER][128];		//比較
	char cDie[32];

	int nError = -1;
	nError = recv(m_sockServerToClient, aData, sizeof(aData), 0);

	if (nError == SOCKET_ERROR)
	{
		// ソケットの作成に失敗したとき
		char aError[64];
		sprintf(aError, "サーバーに接続失敗!!\n エラーコード : %d", WSAGetLastError());
		MessageBox(NULL, aError, "警告！", MB_ICONWARNING);
	}
	else
	{
		sscanf(aData, "%s %s %s %s %s", &cDie, &cPlayerData[0], &cPlayerData[1], &cPlayerData[2], &cPlayerData[3]);

		for (int nCount = 0; nCount < MAX_PLAYER; nCount++)
		{
			ConvertStringToFloat(cPlayerData[nCount], ",", fData);

			for (int nCntkey = 0; nCntkey < NUM_KEY_M; nCntkey++)
			{
				m_aKeyStateOld[nCount][nCntkey] = m_aKeyState[nCount][nCntkey];
			}

			for (int nCntKey = 0; nCntKey < NUM_KEY_M; nCntKey++)
			{
				//m_aKeyState[nCount][nCntKey] = fData[nCntKey];
				m_aKeyState[nCount][nCntKey] = (fData[nCntKey] == 0) ? true : false;
			}

			for (int nCntKey = 0; nCntKey < NUM_KEY_M; nCntKey++)
			{
				m_aKeyStateTrigger[nCount][nCntKey] = (m_aKeyStateOld[nCount][nCntKey] ^ m_aKeyState[nCount][nCntKey]) & m_aKeyState[nCount][nCntKey];
			}
			// 回転量の代入
			m_fRot[nCount] = fData[RECVDATA_ROT];

			// 位置の代入
			m_playerPos[nCount] = D3DXVECTOR3(fData[RECVDATA_POS_X], fData[RECVDATA_POS_Y], fData[RECVDATA_POS_Z]);

			// ランクの代入
			m_nRank[nCount] = (int)fData[RECVDATA_RANK];

			m_nStick[STICKTYPE_H] = (int)fData[RECVDATA_STICK_H];
			m_nStick[STICKTYPE_V] = (int)fData[RECVDATA_STICK_V];
		}
	}

	for (int nCount = 0; nCount < MAX_PLAYER; nCount++)
	{
		if (nCount != m_nId)
		{
			if (m_pEnemy[nCount] != NULL)
			{
				m_pEnemy[nCount]->SetPosition(m_playerPos[nCount]);				// 位置の取得
				m_pEnemy[nCount]->SetRotation(D3DXVECTOR3(0.0f, m_fRot[nCount], 0.0f));
			}
		}
	}

	return true;
}

//=============================================================================
// サーバー命令の処理
//=============================================================================
bool CNetwork::UpdateTCP(void)
{
	char aFunc[256];
	int nError;

	char cDataText[128];		//文字
	char cHeadText[128];		//比較

	memset(cDataText, 0, 128);
	memset(cHeadText, 0, 128);

	memset(aFunc, 0, 256);
	nError = recv(m_sockClient, &aFunc[0], 256, 0);

	sscanf(aFunc, "%s %s", &cHeadText, &cDataText);
	OutputDebugString(aFunc);

	// 接続確認
	if (WSAGetLastError() == WSAEWOULDBLOCK)
	{
		printf("まだ来ない");
	}
	else if (nError == 0 || nError == -1) {
		//切断が発生
		printf("サーバーと通信切断!!!\n");
		closesocket(m_sockClient);
		m_sockClient = NULL;
		return false;
	}

	//ロードだったら
	if (strcmp(cHeadText, "KILL") == 0)
	{
		char aDie[64];
		int nDeath = -1;		// 誰が?
		int nKill = -1;			// 誰に?

		sscanf(aFunc, "%s %d %d", &aDie, &nDeath, &nKill);

		if (!m_bDie[nDeath])
		{
			//CPlayer *pPlayer = CGame::GetPlayer(nDeath);

			//if (pPlayer != NULL)
			//{
			//	pPlayer->Die();
			//}

			//m_bDie[nDeath] = true;

			//if (m_nId == nKill)
			//{// 自分がキラーだったとき
			//	pPlayer = CGame::GetPlayer(m_nId);
			//	if (pPlayer != NULL)
			//	{
			//		pPlayer->MpUp(CCharacter::GetStatus(pPlayer->GetCharacter()).nMaxMpUp_KnockDown);
			//	}
			//}
		}
	}
	else if (strcmp(cHeadText, "GAME_END") == 0)
	{
		char aDie[64];
		int nRank[MAX_PLAYER] = {};

		if (CFade::GetFade() == CFade::FADE_NONE)
		{// フェードしていないとき
			sscanf(aFunc, "%s %d %d %d %d", &aDie, &nRank[0], &nRank[1], &nRank[2], &nRank[3]);

			// チュートリアルへ
			CFade::SetFade(CManager::MODE_RESULT);
		}
	}
	else if (strcmp(cHeadText, "GAME_START") == 0)
	{
		if (CFade::GetFade() == CFade::FADE_NONE)
		{// フェードしていないとき
			// 初期化
			for (int nCount = 0; nCount < MAX_PLAYER; nCount++)
			{
				m_selectState[nCount].bReady = false;
				m_selectState[nCount].nType = -1;
			}

			// チュートリアルへ
			CFade::SetFade(CManager::MODE_GAME);
		}
	}
	else if (strcmp(cHeadText, "CHARACTER_SELECT") == 0)
	{

	}
	else if (strcmp(cHeadText, "CHARACTER_CANCEL") == 0)
	{

	}
	else if (strcmp(cHeadText, "HIT") == 0)
	{

	}
	else if (strcmp(cHeadText, "SET_COIN") == 0)
	{

	}
	else if (strcmp(cHeadText, "DELETE_COIN") == 0)
	{

	}
	else if (strcmp(cHeadText, "THUNDER") == 0)
	{
		char aDie[64];
		D3DXVECTOR3 pos;

		// 雷生成
		sscanf(aFunc, "%s %f %f %f", &aDie, &pos.x, &pos.y, &pos.z);
		m_thunderEvent.pos = pos;
		m_thunderEvent.bCreate = true;
	}
	else if (strcmp(cHeadText, "POINTCIRCLE") == 0)
	{
		char aDie[64];
		D3DXVECTOR3 pos;

		// ポイントサークル生成
		sscanf(aFunc, "%s %f %f %f", &aDie, &pos.x, &pos.y, &pos.z);
		m_pointcircleEvent.pos = pos;
		m_pointcircleEvent.bCreate = true;
	}

	return true;
}

//=============================================================================
// 更新開始
//=============================================================================
void CNetwork::StartUpdate(void)
{
	if (!m_bUpdate)
	{// 更新フラグが折れていたとき
		// マルチスレッドにて更新開始
		m_bUpdate = true;								// 更新フラグを立てる
		std::thread th1 (&CNetwork::Update, this);	// スレッドの作成
		th1.detach();									// スレッドの管理を切り離す
	}
}

//=============================================================================
// 更新停止
//=============================================================================
void CNetwork::StopUpdate(void)
{
	if (m_bUpdate)
	{// 更新フラグが立っていた時
		m_bUpdate = false;
	}
}

//=============================================================================
// 通信切断
//=============================================================================
void CNetwork::CloseTCP(void)
{
	closesocket(m_sockClient);
}