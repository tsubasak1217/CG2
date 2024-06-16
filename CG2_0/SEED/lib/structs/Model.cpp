#include "Model.h"
#include "MatrixFunc.h"
#include "SEED.h"

Model::Model(const std::string& directoryPath, const std::string& filename){
    vertices_ = LoadObjFile(directoryPath, filename);
}

void Model::Draw(uint32_t textureGH){
    SEED::DrawModel(*this, textureGH);
}

void Model::UpdateMatrix(){
    worldMat_ = AffineMatrix(scale_, rotation_, translation_);
    scale_ = {1.0f,1.0f,1.0f};
    rotation_ = {0.0f,0.0f,0.0f};
    translation_ = {0.0f,0.0f,0.0f};
    worldMat_ = IdentityMat4();
}
