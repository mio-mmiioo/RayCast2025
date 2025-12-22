#include "Model.h"
#include "Direct3D.h"

namespace Model
{
	std::vector<ModelData*> modelList;
}

int Model::Load(std::string fileName)
{
	ModelData* pModelData = new ModelData;
	pModelData->filename_ = fileName;
	pModelData->pFbx_ = nullptr;

	for (auto& itr : modelList)
	{
		if (itr->filename_ == fileName)
		{
			pModelData->pFbx_ = itr->pFbx_;
			break;
		}
	}
	
	if (pModelData->pFbx_ == nullptr)
	{
		pModelData->pFbx_ = new Fbx;
		pModelData->pFbx_->Load(fileName.c_str());
	}
	modelList.push_back(pModelData);
	return ((int)(modelList.size() - 1));
}

void Model::SetTransform(int hModel, Transform transform)
{
	modelList[hModel]->transform_ = transform;
}

void Model::Draw(int hModel)
{
	modelList[hModel]->pFbx_->Draw(modelList[hModel]->transform_);
}

void Model::Release()
{
	bool isReffered = false; // 参照されているか
	for (int i = 0; i < modelList.size(); i++)
	{
		isReffered = false;
		for (int j = i + 1; j < modelList.size(); j++)
		{
			if (modelList[i]->pFbx_ == modelList[j]->pFbx_)
			{
				isReffered = true;
				break;
			}
		}
		if (isReffered == false)
		{
			SAFE_DELETE(modelList[i]->pFbx_);
		}
		SAFE_DELETE(modelList[i]);
	}
	modelList.clear(); // 配列を空にする(念のため)
}

void Model::RayCast(int hModel, RayCastData& rayData)
{
	// 対象のモデルのトランスフォームを計算
	modelList[hModel]->transform_.Calculation();

	// ワールド行列を取得
	XMMATRIX worldMatrix = modelList[hModel]->transform_.GetWorldMatrix();
	// ワールド行列の逆行列
	XMMATRIX wInc = worldMatrix * -1.0f;

	//②レイの通過点を求める(ワールド空間でのレイの始点からdir方向に進む直線上の点を計算）
	//方向ベクトルをちょい伸ばした先の点
	//XMVECTOR vDirVec{ [rayData.startを始点として、rayData.dir方向にちょい進んだ点を計算（距離は考えなくてもいいよ）] };
	
	
	XMVECTOR vDirVec = { rayData.direction.x - rayData.start.x, rayData.direction.y - rayData.start.y, rayData.direction.z - rayData.start.z };
	
	//③rayData.startをモデル空間に変換（①をかける）
	//XMVECTOR vstart = [Rayのスタート座標をXMVECTOR化];
	
	XMVECTOR vStart = { rayData.start.x, rayData.start.y, rayData.start.z };
	//https://learn.microsoft.com/ja-jp/windows/win32/api/directxmath/
	//ここから、3次元ベクトルの変換関数を探す w=1のときの変換
	//vstart = [vstartをワールド逆行列で変換];
	//vStart = ;
	XMStoreFloat4(&rayData.start, vStart); //変換結果をrayData.startに格納

	//④（始点から方向ベクトルをちょい伸ばした先）通過点（②）に①をかける(モデル空間に変換）
	//vDirVec = [vDirVecをワールド逆行列で変換];
	//vDirVec = ;
	//⑤rayData.dirを③から④に向かうベクトルにする（位置と位置引き算＝ベクトル）
	//XMVEVCTOR dirAtLocal = [④ - ③]
	//	XMStoreFloat4(&rayData.dir, vDirVec); //変換結果をrayData.dirに格納
	XMVECTOR dirAtLocal = vDirVec - vStart;
	XMStoreFloat4(&rayData.direction, dirAtLocal);
	//指定したモデル番号のFBXにレイキャスト！
	modelList[hModel]->pFbx_->RayCast(rayData);
}

