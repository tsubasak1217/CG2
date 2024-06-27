#include "DxFunc.h"
#include <windows.h>
#include <format>
#include <cassert>

// ログを出力する関数--------------------------------------------------------------------------------------
void Log(const std::string& message) {
    OutputDebugStringA(message.c_str());
}

void Log(const std::wstring& message) {
    OutputDebugStringA(ConvertString(message).c_str());
}



// 文字列を変換する関数--------------------------------------------------------------------------------------
std::wstring ConvertString(const std::string& str) {
    if(str.empty()) {
        return std::wstring();
    }

    auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
    if(sizeNeeded == 0) {
        return std::wstring();
    }
    std::wstring result(sizeNeeded, 0);
    MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
    return result;
}

std::string ConvertString(const std::wstring& str) {
    if(str.empty()) {
        return std::string();
    }

    auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
    if(sizeNeeded == 0) {
        return std::string();
    }
    std::string result(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
    return result;
}


//--------------------------------- HLSLをコンパイルする関数------------------------------------------//
IDxcBlob* CompileShader(
    const std::wstring& filePath,
    const wchar_t* profile,
    IDxcUtils* dxcUtils,
    IDxcCompiler3* dxcCompiler,
    IDxcIncludeHandler* includeHandler
) {

    // これからシェーダーをコンパイルする旨をログに出す
    Log(ConvertString(std::format(L"Begin CompileShader, path: {}, profile : {}\n", filePath, profile)));

    /*------------------------ ファイルを読み込む --------------------------*/

    // hlslファイルを読む
    IDxcBlobEncoding* shaderSource = nullptr;
    HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
    // 読めなかったら止める
    assert(SUCCEEDED(hr));
    // 読み込んだファイルの内容を設定する
    DxcBuffer shaderSourceBuffer{};
    shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
    shaderSourceBuffer.Size = shaderSource->GetBufferSize();
    shaderSourceBuffer.Encoding = DXC_CP_UTF8; // UTF8のコードであることを通知

    /*-------------------------- コンパイルする ----------------------------*/

    LPCWSTR arguments[] = {
    filePath.c_str(), // コンパイル対象のhlslファイル名
        L"-E", L"main", // エントリーポイントの指定。基本的にmain以外にはしない
        L"-T", profile, // ShaderProfileの設定
        L"-Zi", L"-Qembed_debug",// デバッグ用の情報を埋め込む
        L"-Od", //最適化を外しておく
        L"-Zpr", // メモリレイアウトは行優先
    };

    // 実際にShaderをコンパイルする
    IDxcResult* shaderResult = nullptr;
    hr = dxcCompiler->Compile(
        &shaderSourceBuffer,// 読み込んだファイル
        arguments,// コンパイルオプション
        _countof(arguments),// コンパイルオプションの数
        includeHandler,// includeが含まれた諸々
        IID_PPV_ARGS(&shaderResult) // コンパイル結果
    );

    // コンパイルエラーではなくdxcが起動できないなど致命的な状況
    assert(SUCCEEDED(hr));

    /*---------------------------- エラー確認 ------------------------------*/

    // 警告・エラーが出てたらログに出して止める
    IDxcBlobUtf8* shaderError = nullptr;
    shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
    if(shaderError != nullptr && shaderError->GetStringLength() != 0) {
        Log(shaderError->GetStringPointer());
        // 警告・エラーダメゼッタイ
        assert(false);
    }

    /*-------------------- コンパイル結果を受け取って返す ----------------------*/

    // コンパイル結果から実行用のバイナリ部分を取得
    IDxcBlob* shaderBlob = nullptr;
    hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
    assert(SUCCEEDED(hr));
    // 成功したログを出す
    Log(ConvertString(std::format(L"Compile Succeeded, path: {}, profile:{}\n", filePath, profile)));
    // もう使わないリソースを解放
    shaderSource->Release();
    shaderResult->Release();


    // 実行用のバイナリを返却
    return shaderBlob;

}

// ----------------------------------DescriptorHeap作成関数-----------------------------------------//

ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {

    // ディスクリプタヒープとディスクリプターを格納する変数
    ID3D12DescriptorHeap* descriptorHeap = nullptr;
    D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};

    descriptorHeapDesc.Type = heapType; // ヒープタイプ設定
    descriptorHeapDesc.NumDescriptors = numDescriptors; // ディスクリプターの数を設定
    descriptorHeapDesc.Flags = shaderVisible ?// visibleかどうか
        D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    // ディスクリプタヒープを作成
    HRESULT hr;
    hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));

    // ディスクリプタヒープが作れなかったので起動できない
    assert(SUCCEEDED(hr));

    return descriptorHeap;
}

void CreateDescriptorHeap(ID3D12Device* device, ID3D12DescriptorHeap* heap, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {

    // ディスクリプタヒープとディスクリプターを格納する変数
    D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};

    descriptorHeapDesc.Type = heapType; // ヒープタイプ設定
    descriptorHeapDesc.NumDescriptors = numDescriptors; // ディスクリプターの数を設定
    descriptorHeapDesc.Flags = shaderVisible ?// visibleかどうか
        D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    // ディスクリプタヒープを作成
    HRESULT hr;
    hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&heap));

    // ディスクリプタヒープが作れなかったので起動できない
    assert(SUCCEEDED(hr));
}

//------------------------------テクスチャを読み込む関数---------------------------------------//

DirectX::ScratchImage LoadTextureImage(const std::string& filePath) {

    DirectX::ScratchImage image{};
    DirectX::ScratchImage mipImages{};

    std::wstring filePathW = ConvertString(filePath);// wstring型に変換
    // ファイルを読み込む
    HRESULT hr = DirectX::LoadFromWICFile(
        filePathW.c_str(),
        DirectX::WIC_FLAGS_FORCE_SRGB,
        nullptr,
        image
    );
    assert(SUCCEEDED(hr));

    // ミップマップの作成
    if(image.GetMetadata().width > 1 && image.GetMetadata().height > 1) {
        hr = DirectX::GenerateMipMaps(
            image.GetImages(),
            image.GetImageCount(),
            image.GetMetadata(),
            DirectX::TEX_FILTER_SRGB,
            0,
            mipImages
        );
    } else{// サイズが1x1のときはここ
        return image;
    }

    assert(SUCCEEDED(hr));

    return mipImages;
}

// ------------------------------リソース作成に関する関数---------------------------------------//

// バッファ用
ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {

    //頂点リソース用のヒープの設定
    D3D12_HEAP_PROPERTIES uploadHeapProperties{};
    uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // UploadHeap

    //頂点リソースの設定
    D3D12_RESOURCE_DESC vertexResourceDesc{};
    // バッファリソース。 テクスチャの場合はまた別の設定をする
    vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    vertexResourceDesc.Width = sizeInBytes;// リソースのサイズ。
    // バッファの場合はこれらは1にする決まり
    vertexResourceDesc.Height = 1;
    vertexResourceDesc.DepthOrArraySize = 1;
    vertexResourceDesc.MipLevels = 1;
    vertexResourceDesc.SampleDesc.Count = 1;
    // バッファの場合はこれにする決まり
    vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    // リソースの作成
    ID3D12Resource* vertexResource = nullptr;
    HRESULT hr;

    assert(&uploadHeapProperties);
    assert(&vertexResourceDesc);

    hr = device->CreateCommittedResource(
        &uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
        &vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(&vertexResource)
    );

    assert(SUCCEEDED(hr));

    return vertexResource;
}


// テクスチャ用
ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata) {

    // metadataをもとにResourceの設定
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width = UINT(metadata.width); // Texture幅
    resourceDesc.Height = UINT(metadata.height); // Texture高さ
    resourceDesc.MipLevels = UINT16(metadata.mipLevels); // mipmap
    resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize); // 奥行き or Textureの配列数
    resourceDesc.Format = metadata.format; // Texture Format
    resourceDesc.SampleDesc.Count = 1; // サンプリングカウント。1固定。
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);// Textureの次元数

    // 利用するHeapの設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // デフォルト
    //heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK; // WriteBackポリシーでCPUアクセス可能
    //heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0; // プロセッサの近くに配置

    // リソースの作成
    ID3D12Resource* resource = nullptr;
    HRESULT hr;
    hr = device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&resource)
    );

    assert(SUCCEEDED(hr));

    return resource;
}

// 深度ステンシルのリソースを作成する関数
ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height) {

    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width = width;// テクスチャの幅
    resourceDesc.Height = height;// テクスチャの高さ
    resourceDesc.MipLevels = 1;// mipmapの数
    resourceDesc.DepthOrArraySize = 1;// 奥行き or 配列Tecstureの配列数
    resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;// DepthStencilとして利用可能なフォーマット
    resourceDesc.SampleDesc.Count = 1;// サンプリングカウント。1固定
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;// 二次元
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;// DepthStencilとして使う

    // 利用するHeapの設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//VRAM上に作る

    // 深度値のクリア設定
    D3D12_CLEAR_VALUE depthClearValue{};
    depthClearValue.DepthStencil.Depth = 1.0f;// 最大値でクリア
    depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;// Resourceと合わせる

    // Resourceの作成
    ID3D12Resource* resource = nullptr;
    HRESULT hr;
    hr = device->CreateCommittedResource(
        &heapProperties,// heapの設定
        D3D12_HEAP_FLAG_NONE,// その他特殊な設定は特になし
        &resourceDesc,// Resourceの設定
        D3D12_RESOURCE_STATE_DEPTH_WRITE,// 深度値を書き込む状態にしておく
        &depthClearValue,// 深度のクリア値
        IID_PPV_ARGS(&resource)// 作成するリソースポインタへのポインタ
    );
    assert(SUCCEEDED(hr));

    return resource;
}


//データを転送するUploadTextureData関数
[[nodiscard]]
ID3D12Resource* UploadTextureData(
    ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device,
    ID3D12GraphicsCommandList* commandList)
{
    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
    DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
    uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
    ID3D12Resource* intermediateResource = CreateBufferResource(device, intermediateSize);
    UpdateSubresources(commandList, texture, intermediateResource, 0, 0, UINT(subresources.size()), subresources.data());

    //
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = texture;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
    commandList->ResourceBarrier(1, &barrier);
    return intermediateResource;
}

// ディスクリプタのハンドルを取得する関数
D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
    D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
    descriptorHandle.ptr += (descriptorSize * index);
    return descriptorHandle;
}

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
    D3D12_GPU_DESCRIPTOR_HANDLE descriptorHandle = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
    descriptorHandle.ptr += (descriptorSize * index);
    return descriptorHandle;
}

ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
    // ファイルを開く
    std::ifstream file(directoryPath + "/" + filename);
    assert(file.is_open());// 失敗したらアサート

    // 開けたら必要な変数を用意
    ModelData modelData;
    std::vector<Vector4>positions;
    std::vector<Vector3>normals;
    std::vector<Vector2>texcoords;
    std::string line;

    while(std::getline(file, line)){

        // まずobjファイルの行の先頭の識別子を読む
        std::string identifer;
        std::istringstream s(line);
        s >> identifer;

        // identifer(識別子)に応じた処理を行う

        if(identifer == "v"){// 頂点位置-----------------------------

            Vector4 position;
            // x,y,zの順に格納
            s >> position.x >> position.y >> position.z;
            // zは1.0固定
            position.w = 1.0f;
            // 座標配列末尾にを要素を追加
            positions.push_back(position);

        } else if(identifer == "vt"){// 頂点テクスチャ座標------------

            Vector2 texcoord;
            // x,y,zの順に格納
            s >> texcoord.x >> texcoord.y;
            // テクスチャ座標配列末尾にを要素を追加
            texcoords.push_back(texcoord);

        } else if(identifer == "vn"){// 頂点法線--------------------

            Vector3 normal;
            // x,y,zの順に格納
            s >> normal.x >> normal.y >> normal.z;
            // 座標配列末尾にを要素を追加
            normals.push_back(normal);

        } else if(identifer == "f"){// 面

            VertexData vertices[3];

            for(int32_t faceVertex = 0; faceVertex < 3; faceVertex++){

                std::string vertexDefinition;
                s >> vertexDefinition;
                std::istringstream v(vertexDefinition);
                uint32_t elementIndices[3];

                for(int32_t element = 0; element < 3; element++){
                    std::string index;
                    std::getline(v, index, '/');
                    elementIndices[element] = std::stoi(index);
                }

                Vector4 position = positions[elementIndices[0] - 1];
                Vector2 texcoord = texcoords[elementIndices[1] - 1];
                Vector3 normal = normals[elementIndices[2] - 1];

                // 左手座標系に変換
                position.x *= -1.0f;
                normal.x *= -1.0f;
                texcoord.y = 1.0f - texcoord.y;

                vertices[faceVertex] = { position,texcoord,normal };
            }

            // 反対周りに格納
            modelData.vertices.push_back(vertices[2]);
            modelData.vertices.push_back(vertices[1]);
            modelData.vertices.push_back(vertices[0]);

        } else if(identifer == "mtllib"){// mtlファイルの場所

            std::string materialFilename;
            // ファイル名を格納
            s >> materialFilename;

            // mtlファイルを読み込む
            modelData.materialData = LoadMaterialTemplateFile(directoryPath, materialFilename);
        }
    }

    return modelData;
}

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
{

    // ファイルを開く
    std::ifstream file(directoryPath + "/" + filename);
    assert(file.is_open());// 失敗したらアサート

    MaterialData materialData;
    std::string line;

    while(std::getline(file, line)){

        // まずobjファイルの行の先頭の識別子を読む
        std::string identifer;
        std::istringstream s(line);
        s >> identifer;

        if(identifer == "map_Kd"){// ファイル名

            std::string textureFilename;
            // ファイル名を格納
            s >> textureFilename;

            materialData.textureFilePath_ = directoryPath + "/" + textureFilename;
        }
    }

    return materialData;
}

// Matrix4x4 を DirectX::XMMATRIX に変換する関数
DirectX::XMMATRIX ConvertToXMMATRIX(const Matrix4x4& matrix)
{
    return DirectX::XMMATRIX(
        matrix.m[0][0], matrix.m[0][1], matrix.m[0][2], matrix.m[0][3],
        matrix.m[1][0], matrix.m[1][1], matrix.m[1][2], matrix.m[1][3],
        matrix.m[2][0], matrix.m[2][1], matrix.m[2][2], matrix.m[2][3],
        matrix.m[3][0], matrix.m[3][1], matrix.m[3][2], matrix.m[3][3]
    );
}