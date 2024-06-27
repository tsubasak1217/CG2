#include "Model.h"
#include "MatrixFunc.h"
#include "SEED.h"

Model::Model(const std::string& directoryPath, const std::string& filename){
    Initialize(directoryPath, filename);
}

void Model::Initialize(const std::string& directoryPath, const std::string& filename)
{
    modelData_ = LoadObjFile(directoryPath, filename);

    scale_ = { 1.0f,1.0f,1.0f };
    rotation_ = { 0.0f,0.0f,0.0f };
    translation_ = { 0.0f,0.0f,0.0f };
    worldMat_ = IdentityMat4();

    textureGH_ = SEED::LoadTexture(modelData_.materialData.textureFilePath_);
    enableLighting_ = true;
}

void Model::Draw(){
    SEED::DrawModel(*this);
}

void Model::UpdateMatrix(){
    worldMat_ = AffineMatrix(scale_, rotation_, translation_);
}
