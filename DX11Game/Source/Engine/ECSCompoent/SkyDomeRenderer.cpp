////==================================================================
////								SkyDomeRenderer.h
////	スカイドームレンダラー
////
////==================================================================
////	author :	AT12A 05 宇佐美晃之
////==================================================================
////	開発履歴
////
////	2021/01/15 スカイドームレンダラークラス作成
////
////===================================================================
//
//
////====== インクルード部 ======
//#include "SkyDomeRenderer.h"
//#include "../ECSSystem/RendererSystem.h"
//#include "../ECS/Entity/EntityManager.h"
//
//#include "Transform.h"
//#include "../Renderer/Camera.h"
//#include "../System/Texture.h"
//
//using namespace ECS;
//
////===== マクロ定義 =====
//
////===== グローバル変数 =====
//std::map<const std::string, MESH*, std::less<>> SkyDomeRenderer::m_meshPool;
//
//
////========================================
////
////	コンストラクタ
////
////========================================
//SkyDomeRenderer::SkyDomeRenderer()
//{
//	MakeSkyDome("Sky", 100);
//	SetLighting(false);
//	//SetRendererBlendState(BS_ALPHABLEND);
//
//	m_fLayer = VIEW_FAR_Z + 10;
//}
//
////========================================
////
////	デストラクタ
////
////========================================
//SkyDomeRenderer::~SkyDomeRenderer()
//{
//}
