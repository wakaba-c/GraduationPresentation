//=============================================================================
//
// �l�b�g���[�N�̐���[network.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _NETWORK_H_
#define _NETWORK_H_	 // �t�@�C������������߂�

//=============================================================================
// �C���N���[�h�t�@�C��
//=============================================================================
#include "main.h"
#include "inputKeyboard.h"
#include "manager.h"

//=============================================================================
// �}�N����`
//=============================================================================
#define SCRIPT_NETWORK "network.ini"
#define MAX_COIN 20
#define MAX_PLAYER 4

//=============================================================================
// �񋓌^��`
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
	RECVDATA_STICK_H = NUM_KEY_M,		// �X�e�B�b�N
	RECVDATA_STICK_V,					// �X�e�B�b�N
	RECVDATA_ROT,						// Y����]
	RECVDATA_POS_X,						// X��
	RECVDATA_POS_Y,						// Y��
	RECVDATA_POS_Z,						// Z��
	RECVDATA_RANK,						// ����
	RECVDATA_FLAG,						// �`�F�b�N�|�C���g
	RECVDATA_ROUND,						// �����
	RECVDATA_MAX
} RECVDATA;

typedef enum
{
	STICKTYPE_H = 0,
	STICKTYPE_V,
	STICKTYPE_MAX
} STICKTYPE;

//=============================================================================
// �O���錾
//=============================================================================
class CItem;
class CBalloonNum;
class CEnemy;

//=============================================================================
// �\���̒�`
//=============================================================================
typedef struct
{
	//int m_nId = -1;										// ID�ԍ�
	int m_aKeyState[NUM_KEY_M] = {};				// �L�[�{�[�h�̃v���X���
} PLAYERSTATE;

typedef struct
{
	D3DXVECTOR3 pos;			// �ʒu
	bool bCreate;				// �����t���O
} WAITEVENT;

typedef struct
{
	int nId;			// ID
	int nType;			// ���
	bool bCreate;				// �����t���O
} WAITEVENT_SELECT;

typedef struct
{
	int nType;
	bool bReady;
	CBalloonNum *pBalloonNum;
} CHARACTERSELECT;

//=============================================================================
// �N���X
//=============================================================================
class CNetwork
{
public:
	/* �֐� */
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
	int GetLastRank(int nIndex) { return m_nLastRank[nIndex]; }
	int GetPlayerType(int nIndex) { return m_nType[nIndex]; }
	int GetRound(int nIndex) { return m_nNumRound[nIndex]; }
	int GetFlag(int nIndex) { return m_nNumFlag[nIndex]; }
	bool GetDie(int nIndex) { return m_bDie[nIndex]; }
	bool CheckCharacterReady(int nIndex);

	void Create(void);

	void ResetCoin(void);
	void UpdateCharacterState(void);

	void InitGame(void);

private:
	SOCKET createServerSocket(unsigned short port);
	static void ConvertStringToFloat(char* text,const char* delimiter, float* pResult);

	// �l�b�g���[�N�ڑ��֘A
	SOCKET m_sockClient;								// �\�P�b�g(�N���C�A���g)
	SOCKET m_sockClientToServer;						// �\�P�b�g(UDP���M�p)
	SOCKET m_sockServerToClient;						// �\�P�b�g(UDP��M�p)
	struct sockaddr_in m_addrServer;					// �\�P�b�g�A�h���X(�T�[�o)�̏��(TCP����M)
	struct sockaddr_in m_addrClientToServer;			// �L�[���M�p�\�P�b�g(UDP���M�p)
	fd_set m_readfds;
	static char aIp[32];					// IP�A�h���X
	static int nPort;						// �|�[�g�ԍ�

	PLAYERSTATE keystate;

	// ��M���֘A
	bool m_aKeyState[MAX_PLAYER][NUM_KEY_M] = {};				//�L�[�{�[�h�̓��͏�񃏁[�N
	bool m_aKeyStateOld[MAX_PLAYER][NUM_KEY_M] = {};			// �O�̃L�[�{�[�h���͏�񃏁[�N
	bool m_aKeyStateTrigger[MAX_PLAYER][NUM_KEY_M] = {};		// Trigger

	int m_nId;								// ID
	int m_nStick[STICKTYPE_MAX];
	float m_fRot[MAX_PLAYER];				// ��]���
	bool m_bTimeout;						// �^�C���A�E�g�t���O
	D3DXVECTOR3 m_playerPos[MAX_PLAYER];	// �v���C���[�ʒu���
	bool m_bDie[MAX_PLAYER];				// 1�O�̎��S�t���O
	int m_nRank[MAX_PLAYER];				// ���݂̃����L���O
	int m_nNumFlag[MAX_PLAYER];				// �`�F�b�N�|�C���g
	int m_nNumRound[MAX_PLAYER];			// �����
	CItem *m_apItem[MAX_COIN];				// �R�C���̃|�C���^
	CEnemy *m_pEnemy[MAX_PLAYER];			// ���҂̃|�C���^
	int m_nLastRank[MAX_PLAYER];			// �ŏI����
	int m_nType[MAX_PLAYER];				// �v���C���[�̎��
	bool m_bEndGame;

	// �}���`�X���b�h�֘A
	std::thread m_th;					// �X���b�h
	bool m_bUpdate;						// �X�V�t���O
	WAITEVENT m_StartSignal;			// �X�^�[�g�̍��}
	WAITEVENT_SELECT m_selectBalloon;	// �L�����N�^�I��UI

	CHARACTERSELECT m_selectState[MAX_PLAYER];		// �I����

	D3DXVECTOR3 m_samplePos[MAX_PLAYER];
};
#endif // !_NETWORK_H_