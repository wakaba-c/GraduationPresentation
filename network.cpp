//=============================================================================
//
// �l�b�g���[�N���� [network.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "network.h"
#include "renderer.h"
#include "camera.h"
#include "player.h"
#include "game.h"
#include "fade.h"
#include "inputController.h"
#include "enemy.h"
#include "object.h"
#include "startSignal.h"
#include "speed.h"
#include "characterSelect.h"

//=============================================================================
// �ÓI�����o�ϐ�
//=============================================================================
char CNetwork::aIp[32] = {};
int CNetwork::nPort = 0;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CNetwork::CNetwork()
{
	m_nId = -1;
	m_bTimeout = false;

	m_selectBalloon.bCreate = false;
	m_selectBalloon.nId = -1;
	m_selectBalloon.nType = -1;

	// �����t���O�̏�����
	m_StartSignal.bCreate = false;
	m_bEndGame = false;

	for (int nCount = 0; nCount < MAX_PLAYER; nCount++)
	{
		m_selectState[nCount].bReady = false;
		m_selectState[nCount].nType = -1;
		m_selectState[nCount].pBalloonNum = NULL;
		m_nRank[nCount] = 0;
	}
}

//=============================================================================
// �f�X�g���N�^
//=============================================================================
CNetwork::~CNetwork()
{

}

//=============================================================================
// ������
//=============================================================================
HRESULT CNetwork::Init(void)
{
	HRESULT hr;
	hr = Build();					// �\�P�b�g�̐���

	OutputDebugString("�N���C�A���g�\�z����\n");

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
// �I��
//=============================================================================
void CNetwork::Uninit(void)
{
	// �ʃX���b�h�œ��삵�Ă���X�V�����̏I������
	StopUpdate();

	//�\�P�b�g(�N���C�A���g)�̊J��
	CloseTCP();

	// winsock2 �̏I������
	WSACleanup();
}

//=============================================================================
// �X�V  if ( setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (const char *),  sizeof(param) )
//=============================================================================
void CNetwork::Update(void)
{
	DWORD dwCurrentTime;		// ���ݎ���
	DWORD dwExecLastTime;		// �Ō�ɏ�����������
	DWORD dwFrameCount;
	DWORD dwFPSLastTime;

	dwCurrentTime = 0;
	dwExecLastTime = timeGetTime();		// �V�X�e���������~���b�P�ʂŎ擾

	dwFrameCount = 0;
	dwFPSLastTime = timeGetTime();		// �V�X�e���������~���b�P�ʂŎ擾

	while (m_bUpdate)
	{
		// DirectX�̏���
		dwCurrentTime = timeGetTime();		// �V�X�e���������擾

		if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))		// 1/60�b���ƂɎ��s
		{
			dwExecLastTime = dwCurrentTime;	// ��������������ۑ�

			int nError = -1;

			KeyData();

			fd_set readfds;
			FD_ZERO(&readfds);
			FD_SET(m_sockServerToClient, &readfds);
			FD_SET(m_sockClient, &readfds);

			struct timeval tv;

			// �w�肵���b���Ń^�C���A�E�g�����܂�
			tv.tv_sec = 0;
			tv.tv_usec = 0;

			nError = select(0, &readfds, NULL, NULL, &tv);
			if (nError == 0)
			{
				OutputDebugString("�^�C���A�E�g\n");
				continue;
			}

			if (CManager::GetMode() == CManager::MODE_GAME)
			{
				if (FD_ISSET(m_sockServerToClient, &readfds))
				{// ��M
					// �L�[���͏��̍X�V
					UpdateUDP();
				}
			}

			if (FD_ISSET(m_sockClient, &readfds))
			{// ��M
				// �T�[�o�[�R�}���h�̍X�V
				UpdateTCP();
			}
		}
	}
}

//=============================================================================
// �l�b�g���[�N�̐ݒ�f�[�^�ǂݍ���
//=============================================================================
HRESULT CNetwork::LoadConfiguration(void)
{
	FILE *pFile;
	char cReadText[128];		//����
	char cHeadText[128];		//��r
	char cDie[128];

	//�e�L�X�g�f�[�^���[�h
	pFile = fopen(SCRIPT_NETWORK, "r");

	if (pFile != NULL)
	{
		//�X�N���v�g������܂Ń��[�v
		while (strcmp(cHeadText, "SCRIPT") != 0)
		{
			fgets(cReadText, sizeof(cReadText), pFile);
			sscanf(cReadText, "%s", &cHeadText);
		}

		//�X�N���v�g��������
		if (strcmp(cHeadText, "SCRIPT") == 0)
		{
			//�G���h�X�N���v�g������܂�
			while (strcmp(cHeadText, "END_SCRIPT") != 0)
			{
				fgets(cReadText, sizeof(cReadText), pFile);
				sscanf(cReadText, "%s", &cHeadText);

				//���s
				if (strcmp(cReadText, "\n") != 0)
				{
					if (strcmp(cHeadText, "IP") == 0)
					{//�p�[�c�����̂Ƃ�
						sscanf(cReadText, "%s %s %s", &cDie, &cDie, aIp);					//�p�[�c�̐����擾
					}
					else if (strcmp(cHeadText, "PORT") == 0)
					{//�p�[�c���f���̃A�h���X���̂Ƃ�
						sscanf(cReadText, "%s %s %d", &cDie, &cDie, &nPort);					//�p�[�c�̐����擾
					}
				}
			}
		}

		//�t�@�C����
		fclose(pFile);
	}
	else
	{
		MessageBox(NULL, "�l�b�g���[�N�̐ݒ�f�[�^�Ǎ����s�I", "�x���I", MB_ICONWARNING);
	}

	return S_OK;
}

//=============================================================================
// �L�[�{�[�h�̓��͏�Ԃ̎擾
//=============================================================================
PLAYERSTATE CNetwork::GetKeystate(void)
{
	return keystate;
}

//=============================================================================
// ���M(TCP)
//=============================================================================
bool CNetwork::SendTCP(const char *data, int nSize)
{
	int nError;

	//�N���C�A���g�փf�[�^(ID)���M
	nError = send(m_sockClient, data, nSize, 0);
	if (nError < 0)
	{
		printf("���M�G���[ : %d\n", WSAGetLastError());
	}

	return false;
}

//=============================================================================
// ���M(UDP)
//=============================================================================
bool CNetwork::SendUDP(const char *data, int nSize)
{
	int nError;

	//�N���C�A���g�փf�[�^(ID)���M
	nError = sendto(m_sockClientToServer, data, nSize, 0, (struct sockaddr *)&m_addrClientToServer, sizeof(m_addrClientToServer));
	if (nError == SOCKET_ERROR)
	{
		char aError[256];
		memset(&aError, 0, sizeof(aError));
		sprintf(aError, "���M�G���[ : %d\n", WSAGetLastError());
		MessageBox(NULL, aError, "�x���I", MB_ICONWARNING);
	}

	return false;
}

//=============================================================================
// ��M
//=============================================================================
bool CNetwork::DataRecv(SOCKETTYPE type, char *data, int nSize)
{
	int nError;
	char aError[256];

	switch (type)
	{
	case SOCKETTYPE_CLIENT:
		//�T�[�o�[����f�[�^��M
		nError = recv(m_sockClient, data, nSize, 0);
		break;
	case SOCKETTYPE_GAME:
		//�T�[�o�[����f�[�^��M
		nError = recv(m_sockClientToServer, data, nSize, 0);
		break;
	}

	if (nError < 0)
	{
		sprintf(aError, "��M�G���[ : %d\n", WSAGetLastError());
		MessageBox(NULL, aError, "�x���I", MB_ICONWARNING);
		return false;
	}

	return true;
}

//=============================================================================
// �N���C�A���g�\�P�b�g�̍쐬
//=============================================================================
HRESULT CNetwork::Build(void)
{
	WSADATA wsaData;								//winsock�̏��
	int nError;										//�G���[�擾

	//winsock�̏�����
	nError = WSAStartup(MAKEWORD(2, 0), &wsaData);
	if (nError != 0)
	{
		switch (nError)
		{
		case WSASYSNOTREADY:
			MessageBox(NULL, "�l�b�g���[�N�T�u�V�X�e�����l�b�g���[�N�ւ̐ڑ��������ł��Ă��܂���!", "�x���I", MB_ICONWARNING);
			return E_FAIL;
			break;
		case WSAVERNOTSUPPORTED:
			MessageBox(NULL, "�v�����ꂽwinsock�̃o�[�W�������T�|�[�g����Ă��܂���I", "�x���I", MB_ICONWARNING);
			return E_FAIL;
			break;
		case WSAEINPROGRESS:
			MessageBox(NULL, "�u���b�L���O����̎��s���ł��邩�A �܂��̓T�[�r�X�v���o�C�_���R�[���o�b�N�֐����������Ă��܂��I", "�x���I", MB_ICONWARNING);
			return E_FAIL;
			break;
		case WSAEPROCLIM:
			MessageBox(NULL, "winsock�������ł���ő�v���Z�X���ɒB���܂����I", "�x���I", MB_ICONWARNING);
			return E_FAIL;
			break;
		case WSAEFAULT:
			MessageBox(NULL, "�������ł���lpWSAData �͗L���ȃ|�C���^�ł͂���܂���I", "�x���I", MB_ICONWARNING);
			return E_FAIL;
			break;
		}
	}

	// �\�P�b�g�쐬
	m_sockClient = socket(AF_INET, SOCK_STREAM, 0);

	if (m_sockClient == INVALID_SOCKET)
	{// �\�P�b�g�̍쐬�Ɏ��s�����Ƃ�
		char aError[64];
		sprintf(aError, "�\�P�b�g�̐����Ɏ��s!!\n �G���[�R�[�h : %d", WSAGetLastError());
		MessageBox(NULL, aError, "�x���I", MB_ICONWARNING);
		return E_FAIL;
	}

	// �\�P�b�g�쐬
	m_sockClientToServer = socket(AF_INET, SOCK_DGRAM, 0);

	if (m_sockClientToServer == INVALID_SOCKET)
	{// �\�P�b�g�̍쐬�Ɏ��s�����Ƃ�
		char aError[64];
		sprintf(aError, "�\�P�b�g�̐����Ɏ��s!!\n �G���[�R�[�h : %d", WSAGetLastError());
		MessageBox(NULL, aError, "�x���I", MB_ICONWARNING);
		return E_FAIL;
	}

	//�\�P�b�g�̐ݒ�
	m_addrServer.sin_family = AF_INET;
	m_addrServer.sin_port = htons(12345);
	m_addrServer.sin_addr.S_un.S_addr = inet_addr(aIp);

	bind(m_sockClientToServer, (struct sockaddr *)&m_addrServer, sizeof(m_addrServer));

	m_addrClientToServer.sin_family = AF_INET;
	m_addrClientToServer.sin_port = htons(10032);
	m_addrClientToServer.sin_addr.S_un.S_addr = inet_addr(aIp);

	// �\�P�b�g�쐬
	m_sockServerToClient = createServerSocket(nPort);

	return S_OK;
}

//=============================================================================
// �R�l�N�g
//=============================================================================
HRESULT CNetwork::Connect(void)
{
	int val = -1;
	unsigned int nStartTime = 0;
	SOCKET sock;

	// �\�P�b�g�̏�����
	sock = socket(AF_INET, SOCK_STREAM, 0);

	//�T�[�o�ɐڑ�
	val = connect(sock, (struct sockaddr *)&m_addrServer, sizeof(m_addrServer));
	if (val == SOCKET_ERROR)
	{
		char aError[64];
		sprintf(aError, "�l�b�g���[�N�G���[!\nerror : %d\n", WSAGetLastError());
		MessageBox(NULL, aError, "�x��!", MB_ICONWARNING);
		return E_FAIL;
	}

	m_sockClient = sock;

	// ID�̎擾
	SendTCP("LOAD_ID", sizeof("LOAD_ID"));
	if (!DataRecv(SOCKETTYPE_CLIENT, (char*)&m_nId, sizeof(int)))
	{
		return E_FAIL;
	}

	// �V�[�h�l�̎擾
	SendTCP("SEED", sizeof("SEED"));
	if (!DataRecv(SOCKETTYPE_CLIENT, (char*)&nStartTime, sizeof(nStartTime)))
	{
		return E_FAIL;
	}

	srand(nStartTime);				// �����̎��ݒ�

#ifdef _DEBUG
	char debug[256];
	sprintf(debug, "ID = %d\n", m_nId);
	OutputDebugString(debug);
	sprintf(debug, "SEED = %d\n", nStartTime);
	OutputDebugString(debug);
#endif // _DEBUG
	return S_OK;
}

//=============================================================================
// �L�[�f�[�^
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

		// ID, W�L�[, A�L�[, S�L�[, D�L�[, SPACE�L�[, �X�e�B�b�NH, �X�e�B�b�NV, ��]���, �ʒuX, �ʒuY, �ʒuZ, �X�R�A
		sprintf(data, "SAVE_KEY %d %d %d %d %d %d %.2f %.2f %.2f %.2f %.2f %.2f %d %d %.2f",
			m_nId,
			pKeyboard->GetPressKeyboard(DIK_W), pKeyboard->GetPressKeyboard(DIK_A),
			pKeyboard->GetPressKeyboard(DIK_S), pKeyboard->GetPressKeyboard(DIK_D), aKeyState[NUM_KEY_SPACE],		// �L�[���͏��
			stick_H,					// �X�e�B�b�NH
			stick_V,					// �X�e�B�b�NV
			rot.y,						// ��]
			pos.x, pos.y, pos.z,		// �ʒu
			nNumFlag,					// ���̃`�F�b�N�|�C���g
			nNumRound,					// ���݂̎����
			CSpeed::GetSpeed()
		);
		pNetwork->SendUDP(data, sizeof("SAVE_KEY") + 1024);
	}

	return true;
}

//=============================================================================
// �L�[�{�[�h�̃v���X��Ԏ擾
//=============================================================================
bool CNetwork::GetPressKeyboard(int nId, int nKey)
{
	return m_aKeyState[nId][nKey];
}

//=============================================================================
// �L�[�{�[�h�̃g���K�[��Ԏ擾
//=============================================================================
bool CNetwork::GetTriggerKeyboard(int nId, int nKey)
{
	return m_aKeyStateTrigger[nId][nKey];
}

//=============================================================================
// �L�����N�^�[���g�p�\���ǂ���
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
// �N���G�C�g����
//=============================================================================
void CNetwork::Create(void)
{
	if (m_StartSignal.bCreate)
	{
		for (int nCount = 0; nCount < MAX_PLAYER; nCount++)
		{
			if (nCount == m_nId) continue;

			if (m_pEnemy[nCount] != NULL)
			{
				// �v���C���[���f�����̓ǂݍ���
				switch (m_nType[nCount])
				{
				case 0:
					m_pEnemy[nCount]->LoadScript(SCRIPT_CAR01, CPlayer::ANIMATIONTYPE_MAX);
					break;
				case 1:
					m_pEnemy[nCount]->LoadScript(SCRIPT_CAR02, CPlayer::ANIMATIONTYPE_MAX);
					break;
				case 2:
					m_pEnemy[nCount]->LoadScript(SCRIPT_CAR03, CPlayer::ANIMATIONTYPE_MAX);
					break;
				case 3:
					m_pEnemy[nCount]->LoadScript(SCRIPT_CAR04, CPlayer::ANIMATIONTYPE_MAX);
					break;
				}
			}
		}

		CStartSignal::Create();
		m_StartSignal.bCreate = false;
	}

	if (m_bEndGame)
	{// �Q�[���I���t���O�������Ă����Ƃ�
		if (CFade::GetFade() == CFade::FADE_NONE)
		{// �t�F�[�h���Ă��Ȃ��Ƃ�
		 // �`���[�g���A����
			CFade::SetFade(CManager::MODE_RESULT, CFade::FADETYPE_NORMAL);
		}

		m_bEndGame = false;
	}
}

//=============================================================================
// �R�C���̃f�[�^�x�[�X�����Z�b�g
//=============================================================================
void CNetwork::ResetCoin(void)
{
}

//=============================================================================
// �L�����N�^�[�I����Ԃ̍X�V
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
	//		{// �I�𒆃v���C���[�\��UI�����݂��Ă��Ȃ��Ƃ�
	//		 // UI�̍쐬
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
// �Q�[��������������
//=============================================================================
void CNetwork::InitGame(void)
{
	for (int nCount = 0; nCount < MAX_PLAYER; nCount++)
	{
		m_pEnemy[nCount] = CEnemy::Create();
	}

	StartUpdate();

	char aData[64];
	sprintf(aData, "READY 1 %d", CCharacterSelect::GetCarType());
	SendTCP(aData, sizeof(aData));
	OutputDebugString("�L�����N�^�[�^�C�v���M����");
}

//=============================================================================
// �T�[�o�[�\�P�b�g���쐬����
//=============================================================================
SOCKET CNetwork::createServerSocket(unsigned short port)
{
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == SOCKET_ERROR)
	{// �\�P�b�g�̍쐬�Ɏ��s�����Ƃ�
		char aError[64];
		sprintf(aError, "�\�P�b�g�̐����Ɏ��s!!\n �G���[�R�[�h : %d", WSAGetLastError());
		MessageBox(NULL, aError, "�x���I", MB_ICONWARNING);
		return sock;
	}

	struct sockaddr_in adr;

	adr.sin_family = AF_INET;
	adr.sin_port = htons(port);
	adr.sin_addr.S_un.S_addr = INADDR_ANY;  // �T�[�o���Ŋ��蓖�Ă��Ă��� IP �������Őݒ�

	if (SOCKET_ERROR == bind(sock, (struct sockaddr *)&adr, sizeof(adr)))
	{
		char aError[64];
		sprintf(aError, "�\�P�b�g�̐����Ɏ��s!!\n �G���[�R�[�h : %d", WSAGetLastError());
		MessageBox(NULL, aError, "�x���I", MB_ICONWARNING);
		return sock;
	}

	return sock;
}

//=============================================================================
// �`���ϊ�(float)�֐�
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
// UDP���瑗���Ă�����̐���(�L�[���)
//=============================================================================
bool CNetwork::UpdateUDP(void)
{
	char aData[1024];
	float fData[RECVDATA_MAX + 1];
	char cPlayerData[MAX_PLAYER][128];		//��r
	char cDie[32];

	memset(&aData, 0, sizeof(aData));

	int nError = -1;
	nError = recv(m_sockServerToClient, aData, sizeof(aData), 0);

	if (nError == SOCKET_ERROR)
	{
		// �\�P�b�g�̍쐬�Ɏ��s�����Ƃ�
		char aError[64];
		sprintf(aError, "�T�[�o�[�ɐڑ����s!!\n �G���[�R�[�h : %d", WSAGetLastError());
		MessageBox(NULL, aError, "�x���I", MB_ICONWARNING);
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
			// ��]�ʂ̑��
			m_fRot[nCount] = fData[RECVDATA_ROT];

			// �ʒu�̑��
			m_playerPos[nCount] = D3DXVECTOR3(fData[RECVDATA_POS_X], fData[RECVDATA_POS_Y], fData[RECVDATA_POS_Z]);

			// �����N�̑��
			m_nRank[nCount] = (int)fData[RECVDATA_RANK];

			// �`�F�b�N�|�C���g
			m_nNumFlag[nCount] = (int)fData[RECVDATA_FLAG];

			// �����
			m_nNumRound[nCount] = (int)fData[RECVDATA_ROUND];

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
				m_pEnemy[nCount]->SetPosition(m_playerPos[nCount]);				// �ʒu�̎擾
				m_pEnemy[nCount]->SetRotation(D3DXVECTOR3(0.0f, m_fRot[nCount], 0.0f));
			}
		}
	}

	return true;
}

//=============================================================================
// �T�[�o�[���߂̏���
//=============================================================================
bool CNetwork::UpdateTCP(void)
{
	char aFunc[256];
	int nError;

	char cDataText[128];		//����
	char cHeadText[128];		//��r

	memset(cDataText, 0, 128);
	memset(cHeadText, 0, 128);

	memset(aFunc, 0, 256);
	nError = recv(m_sockClient, &aFunc[0], 256, 0);

	sscanf(aFunc, "%s %s", &cHeadText, &cDataText);
	OutputDebugString(aFunc);

	// �ڑ��m�F
	if (WSAGetLastError() == WSAEWOULDBLOCK)
	{
		printf("�܂����Ȃ�");
	}
	else if (nError == 0 || nError == -1) {
		//�ؒf������
		printf("�T�[�o�[�ƒʐM�ؒf!!!\n");
		closesocket(m_sockClient);
		m_sockClient = NULL;
		return false;
	}

	//���[�h��������
	if (strcmp(cHeadText, "KILL") == 0)
	{
		char aDie[64];
		int nDeath = -1;		// �N��?
		int nKill = -1;			// �N��?

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
			//{// �������L���[�������Ƃ�
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
		sscanf(aFunc, "%s %d %d %d %d", &aDie, &m_nLastRank[0], &m_nLastRank[1], &m_nLastRank[2], &m_nLastRank[3]);
		OutputDebugString(aFunc);

		m_bEndGame = true;
	}
	else if (strcmp(cHeadText, "GAME_START") == 0)
	{
		char aDie[64];
		sscanf(aFunc, "%s %d %d %d %d", &aDie, &m_nType[0], &m_nType[1], &m_nType[2], &m_nType[3]);

		m_StartSignal.bCreate = true;

		OutputDebugString(aFunc);
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
	else if (strcmp(cHeadText, "RANK") == 0)
	{
		char aDie[64];
		sscanf(aFunc, "%s %d %d %d %d", &aDie, &m_nLastRank[0], &m_nLastRank[1], &m_nLastRank[2], &m_nLastRank[3]);
		OutputDebugString(aFunc);
	}
	return true;
}

//=============================================================================
// �X�V�J�n
//=============================================================================
void CNetwork::StartUpdate(void)
{
	if (!m_bUpdate)
	{// �X�V�t���O���܂�Ă����Ƃ�
		// �}���`�X���b�h�ɂčX�V�J�n
		m_bUpdate = true;								// �X�V�t���O�𗧂Ă�
		std::thread th1(&CNetwork::Update, this);	// �X���b�h�̍쐬
		th1.detach();									// �X���b�h�̊Ǘ���؂藣��
	}
}

//=============================================================================
// �X�V��~
//=============================================================================
void CNetwork::StopUpdate(void)
{
	if (m_bUpdate)
	{// �X�V�t���O�������Ă�����
		m_bUpdate = false;
	}
}

//=============================================================================
// �ʐM�ؒf
//=============================================================================
void CNetwork::CloseTCP(void)
{
	closesocket(m_sockClient);
}