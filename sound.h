//=============================================================================
//
// サウンド処理 [sound.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _SOUND_H_
#define _SOUND_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"

//=============================================================================
// 構造体定義
//=============================================================================
typedef struct
{
	char *pFilename;			// ファイル名
	int nCntLoop;				// ループカウント
} SOUNDPARAM;

//=============================================================================
// サウンドファイル
//=============================================================================
typedef enum
{
	SOUND_LABEL_BGM000 = 0,		// 山の中
	SOUND_LABEL_SE_GRASS_1,		// 草の踏む音1
	SOUND_LABEL_SE_GRASS_2,		// 草の踏む音2
	SOUND_LABEL_SE_GRASS_3,		// 草の踏む音3
	SOUND_LABEL_SE_GRASS_4,		// 草の踏む音4
	SOUND_LABEL_SE_SAND_1,		// 砂の踏む音1
	SOUND_LABEL_SE_SAND_2,		// 砂の踏む音2
	SOUND_LABEL_SE_SAND_3,		// 砂の踏む音3
	SOUND_LABEL_SE_SAND_4,		// 砂の踏む音4
	SOUND_LABEL_SE_SLASHING,	// 鎌の斬撃音
	SOUND_LABEL_SE_BOMB,		// 爆発音
	SOUND_LABEL_SE_JUMP,		// ジャンプ音
	SOUND_LABEL_SE_LANDING,		// 着地音
	SOUND_LABEL_SE_PUNCH,		// ダメージ音
	SOUND_LABEL_SE_CHARGE,		// チャージ音
	SOUND_LABEL_SE_GET,			// 出現
	SOUND_LABEL_MAX
} SOUND_LABEL;

//=============================================================================
// クラス定義
//=============================================================================
class CSoundCallback : public IXAudio2VoiceCallback {
public:
	CSoundCallback() { }
	~CSoundCallback() { }
	void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32 BytesRequired) { }
	void STDMETHODCALLTYPE OnVoiceProcessingPassEnd(void) { }
	void STDMETHODCALLTYPE OnStreamEnd(void) { SetEvent(m_hEvent); }
	void STDMETHODCALLTYPE OnBufferStart(void *pBufferContext) { SetEvent(m_hEvent); }
	void STDMETHODCALLTYPE OnBufferEnd(void *pBufferContext) { }
	void STDMETHODCALLTYPE OnLoopEnd(void *pBufferContext) { }
	void STDMETHODCALLTYPE OnVoiceError(void *pBufferContext, HRESULT Error) { }
private:
	HANDLE m_hEvent = NULL;
};

class CSound
{
public:
	CSound();
	~CSound();
	HRESULT Init(HWND hWnd);
	void Uninit(void);
	void Update(void);

	void SetVolume(SOUND_LABEL label, float fVolume);

	HRESULT PlaySound(SOUND_LABEL label);
	void StopSound(SOUND_LABEL label);
	void StopSound(void);

private:
	HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD *pChunkSize, DWORD *pChunkDataPosition);
	HRESULT ReadChunkData(HANDLE hFile, void *pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);

	IXAudio2 *m_pXAudio2 = NULL;								// XAudio2オブジェクトへのインターフェイス
	IXAudio2MasteringVoice *m_pMasteringVoice = NULL;			// マスターボイス
	IXAudio2SubmixVoice *m_pSubmixVoice = NULL;					// サブミックスボイス
	IXAudio2SourceVoice *m_apSourceVoice[SOUND_LABEL_MAX] = {};	// ソースボイス

	BYTE *m_apDataAudio[SOUND_LABEL_MAX] = {};					// オーディオデータ
	DWORD m_aSizeAudio[SOUND_LABEL_MAX] = {};					// オーディオデータサイズ

	// 各音素材のパラメータ
	SOUNDPARAM m_aParam[SOUND_LABEL_MAX] =
	{
		{ "data/sound/bgm/karuizawa-birds-2_01.wav", -1 },					// タイトル
		{ "data/sound/se/grass/dash-lawn1_01.wav", 0 },						// 草の踏む音1
		{ "data/sound/se/grass/dash-lawn1_02.wav", 0 },						// 草の踏む音2
		{ "data/sound/se/grass/dash-lawn1_03.wav", 0 },						// 草の踏む音3
		{ "data/sound/se/grass/dash-lawn1_04.wav", 0 },						// 草の踏む音4
		{ "data/sound/se/sand/dash-soil1_01.wav", 0 },						// 砂の踏む音1
		{ "data/sound/se/sand/dash-soil1_02.wav", 0 },						// 砂の踏む音2
		{ "data/sound/se/sand/dash-soil1_03.wav", 0 },						// 砂の踏む音3
		{ "data/sound/se/sand/dash-soil1_04.wav", 0 },						// 砂の踏む音4
		{ "data/sound/se/sword-gesture.wav", 0 },							// 鎌の斬撃音
		{ "data/sound/se/bomb.wav", 0 },									// 爆発音
		{ "data/sound/se/jump.wav", 0 },									// ジャンプ音
		{ "data/sound/se/landing.wav", 0 },									// 着地音
		{ "data/sound/se/punch.wav", 0 },									// ダメージ音
		{ "data/sound/se/magic-circle.wav", 0 },							// チャージ音
		{ "data/sound/se/explosion000.wav", 0 },							// 決定音
	};
};
#endif