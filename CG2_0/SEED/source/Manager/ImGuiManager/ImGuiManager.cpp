#include "ImGuiManager.h"
#include "SEED.h"

void ImGuiManager::Initialize(SEED* pSEED)
{

    pSEED_ = pSEED;
    pDxManager_ = pSEED_->GetDxManager();

    /*===========================================================================================*/
    /*                                        ImGuiの初期化                                       */
    /*===========================================================================================*/

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(pSEED_->hwnd);
    ImGui_ImplDX12_Init(
        pDxManager_->device,
        pDxManager_->swapChainDesc.BufferCount,
        pDxManager_->rtvDesc.Format,
        pDxManager_->srvDescriptorHeap,
        pDxManager_->srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
        pDxManager_->srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()
    );
}

void ImGuiManager::Finalize()
{
    // ImGuiの終了処理
#ifdef _DEBUG
    // 後始末
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
#endif
}

void ImGuiManager::Draw()
{
    //pDxManager_->commandList->SetDescriptorHeaps(1, &(pDxManager_->srvDescriptorHeap));
    // ImGuiの描画
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), pDxManager_->commandList);
}

void ImGuiManager::Begin()
{
#ifdef _DEBUG
    // ImGuiフレーム開始
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
#endif
}

void ImGuiManager::End()
{
#ifdef _DEBUG
    // 描画前準備
    ImGui::Render();
    Draw();
#endif
}
