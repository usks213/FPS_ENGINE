//==================================================================
//												Object.h
//	全ての元となるオブジェクト
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/21	オブジェクトクラス作成
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include <memory>


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class Object
	{
		friend class ObjectManager;

	protected:
		// 自身のweakポインタ
		std::weak_ptr<Object> m_self;

	public:
		// コンストラクタ
		Object();
		// デストラクタ
		virtual ~Object();

		// 自身の破棄
		virtual void Destroy();

	protected:
		// オブジェクト生成時
		virtual void OnCreate(){}
		// オブジェクト破棄時
		virtual void OnDestroy(){}

	};

}
