//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

#include "ImaseLib/Matrix.h"

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    // TODO: Provide parameters for swapchain format, depth/stencil format, and backbuffer count.
    //   Add DX::DeviceResources::c_AllowTearing to opt-in to variable rate displays.
    //   Add DX::DeviceResources::c_EnableHDR for HDR10 display.
    m_deviceResources->RegisterDeviceNotify(this);
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */

#ifdef _DEBUG
    // Fontの変更
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/ARIAL.ttf", 16.0f);
#endif

    // デバッグカメラの作成
    m_debugCamera = std::make_unique<Imase::DebugCamera>(width, height);
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

    // TODO: Add your game logic here.

    // キー情報を取得
    auto kb = Keyboard::Get().GetState();

#ifdef _DEBUG
    // Debug

    // ImGuiの更新処理
    Imase::DXTK_ImGui::Update(m_deviceResources->GetWindow());

    ImGui::Begin("Light & Material");

    // ----- ImGuiのウインドウに項目を追加する ----- //

    //std::vector<float> v;

    //ImGui::SeparatorText("LIGHT SETTING:");

    //// ライトの向き
    //v = { m_lightDirection.x, m_lightDirection.y, m_lightDirection.z };
    //ImGui::DragFloat3("LightDirection", v.data(), 0.01f);
    //m_lightDirection = { v[0], v[1], v[2] };
    //m_lightDirection.Normalize();

    //// アンビエントライト色
    //v = { m_ambientLightColor.x, m_ambientLightColor.y, m_ambientLightColor.z };
    //ImGui::ColorEdit3("AmbientLightColor", v.data());
    //m_ambientLightColor = { v[0], v[1], v[2] };

    //// ライトのディフューズ色
    //v = { m_lightDiffuseColor.x, m_lightDiffuseColor.y, m_lightDiffuseColor.z };
    //ImGui::ColorEdit3("LightDiffuseColor", v.data());
    //m_lightDiffuseColor = { v[0], v[1], v[2] };

    //// ライトのスペキュラー色
    //v = { m_lightSpecularColor.x, m_lightSpecularColor.y, m_lightSpecularColor.z };
    //ImGui::ColorEdit3("LightSpecularColor", v.data());
    //m_lightSpecularColor = { v[0], v[1], v[2] };

    //ImGui::SeparatorText("MATERIAL SETTING:");

    //// ディフューズ色
    //v = { m_diffuseColor.x, m_diffuseColor.y, m_diffuseColor.z };
    //ImGui::ColorEdit3("DiffuseColor", v.data());
    //m_diffuseColor = { v[0], v[1], v[2] };

    //// スペキュラー色
    //v = { m_specularColor.x, m_specularColor.y, m_specularColor.z };
    //ImGui::ColorEdit3("SpecularColor", v.data());
    //m_specularColor = { v[0], v[1], v[2] };

    //// スペキュラーパワー
    //ImGui::DragFloat("SpecuarPower", &m_specularPower, 1.0f, 10.0f, 100.0f);

    //// エミッシブ色
    //v = { m_emissiveColor.x, m_emissiveColor.y, m_emissiveColor.z };
    //ImGui::ColorEdit3("EmissiveColor", v.data());
    //m_emissiveColor = { v[0], v[1], v[2] };

    // --------------------------------------------- //

    // デバッグカメラの更新
    m_debugCamera->Update(!ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows));

    ImGui::End();

//    ImGui::ShowDemoWindow();
#else
    // Release

    // デバッグカメラの更新
    m_debugCamera->Update();

#endif // _DEBUG

}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();

    // TODO: Add your rendering code here.
    context;

    // ビュー行列を取得する
    SimpleMath::Matrix view = m_debugCamera->GetCameraMatrix();

    //view = Imase::CreateViewMatrix(SimpleMath::Vector3(0, 0, 5), SimpleMath::Vector3(0, 0, 0), SimpleMath::Vector3::Up);

    // グリッドの床の描画
    m_gridFloor->Render(context, view, m_proj);

    // ----- ポリゴンの描画 ----- //

    // 定数バッファの更新
    {
        ConstantBufferData data = {};

        // ワールド行列×ビュー行列×プロジェクション行列を設定する
        SimpleMath::Matrix m = view * m_proj;

        // シェーダーへ列優先行列を渡すため転置する
        data.worldViewProjection = XMMatrixTranspose(m);

        D3D11_MAPPED_SUBRESOURCE mapped;
        context->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

        // CPU側のバッファを書き換える
        memcpy(mapped.pData, &data, sizeof(data));

        context->Unmap(m_constantBuffer.Get(), 0);
    }

    // 頂点バッファの設定
    ID3D11Buffer* buffers[] = { m_vertexBuffer.Get() };
    UINT stride = sizeof(XMFLOAT3);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, buffers, &stride, &offset);

    // インデックスバッファの設定
    context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

    // 入力レイアウトの設定
    context->IASetInputLayout(m_inputLayout.Get());

    // トポロジーの設定
    context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 定数バッファの設定
    ID3D11Buffer* cBuffer[] = { m_constantBuffer.Get() };
    context->VSSetConstantBuffers(0, 1, cBuffer);

    // 頂点シェーダーの設定
    context->VSSetShader(m_vertexShader.Get(), nullptr, 0);

    // ピクセルシェーダーの設定
    context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

    // ラスタライザーステートの設定
    context->RSSetState(m_rasterizerState.Get());

    // 深度ステンシルバッファの設定
    context->OMSetDepthStencilState(m_depthStencilState.Get(), 0);

    // ブレンドステートの設定
    context->OMSetBlendState(m_blendState.Get(), nullptr, 0xffffffff);

    // 描画
    context->DrawIndexed(3, 0, 0);

    // デバッグフォントの描画
    m_debugFont->Render(m_states.get());

#ifdef _DEBUG
    // ImGuiの描画処理
    Imase::DXTK_ImGui::Render();
#endif // _DEBUG

    m_deviceResources->PIXEndEvent();

    // Show the new frame.
    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    const auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
    const auto r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnDisplayChange()
{
    m_deviceResources->UpdateColorSpace();
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 1280;
    height = 720;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();
    auto context = m_deviceResources->GetD3DDeviceContext();

    // TODO: Initialize device dependent objects here (independent of window size).
    device;

#ifdef _DEBUG
    int w, h;
    GetDefaultSize(w, h);
    // ImuGuiの初期化
    Imase::DXTK_ImGui::Initialize(m_deviceResources->GetWindow(), device, context, w, h);
#endif // _DEBUG

    // コモンステートの作成
    m_states = std::make_unique<CommonStates>(device);

    // デバッグフォントの作成
    m_debugFont = std::make_unique<Imase::DebugFont>(device, context
        , L"Resources/Font/SegoeUI_18.spritefont");

    // グリッドの床の作成
    m_gridFloor = std::make_unique<Imase::GridFloor>(
        device, context, m_states.get());

    // ----- 頂点シェーダー ＆ 入力レイアウト ----- //
    {
        // 頂点シェーダーの読み込み
        std::vector<uint8_t> data = DX::ReadData(L"Resources/Shaders/VertexShader.cso");

        // 頂点シェーダーの作成
        DX::ThrowIfFailed(
            device->CreateVertexShader(data.data(), data.size(), nullptr, m_vertexShader.ReleaseAndGetAddressOf())
        );

        // 入力レイアウトの作成
        D3D11_INPUT_ELEMENT_DESC layout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        DX::ThrowIfFailed(
            device->CreateInputLayout(layout, ARRAYSIZE(layout), data.data(), data.size(), m_inputLayout.ReleaseAndGetAddressOf())
        );
    }

    // ----- ピクセルシェーダー ----- //
    {
        // ピクセルシェーダーの読み込み
        std::vector<uint8_t> data = DX::ReadData(L"Resources/Shaders/PixelShader.cso");

        // ピクセルシェーダーの作成
        DX::ThrowIfFailed(
            device->CreatePixelShader(data.data(), data.size(), nullptr, m_pixelShader.ReleaseAndGetAddressOf())
        );
    }

    // ----- 定数バッファ ----- //
    {
        // 定数バッファの作成
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(ConstantBufferData);
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        DX::ThrowIfFailed(
            device->CreateBuffer(&desc, nullptr, m_constantBuffer.ReleaseAndGetAddressOf())
        );
    }

    // ----- 頂点バッファ ----- //
    {
        // 頂点データ
        XMFLOAT3 vertices[] =
        {
            {  0.0f, 1.0f, 0.0f },   // 0
            { -1.0f, 0.0f, 0.0f },   // 1
            {  1.0f, 0.0f, 0.0f },   // 2
        };

        // 頂点バッファの作成
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(vertices);
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA data = {};
        data.pSysMem = vertices;

        DX::ThrowIfFailed(
            device->CreateBuffer(&desc, &data, m_vertexBuffer.ReleaseAndGetAddressOf())
        );
    }

    // ----- インデックスバッファ ----- //
    {
        // インデックスデータ
        UINT16 indices[] = { 0, 1, 2 };

        // インデックス頂点バッファの作成
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(indices);
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA data = {};
        data.pSysMem = indices;

        DX::ThrowIfFailed(
            device->CreateBuffer(&desc, &data, m_indexBuffer.ReleaseAndGetAddressOf())
        );
    }

    // ----- ラスタライザーステート ----- //
    {
        // ラスタライザーステートの作成
        D3D11_RASTERIZER_DESC desc = {};
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = D3D11_CULL_BACK;
        desc.FrontCounterClockwise = TRUE;
        desc.DepthBias = 0;
        desc.DepthBiasClamp = 0.0f;
        desc.SlopeScaledDepthBias = 0.0f;
        desc.DepthClipEnable = TRUE;
        desc.ScissorEnable = FALSE;
        desc.MultisampleEnable = FALSE;
        desc.AntialiasedLineEnable = FALSE;
        DX::ThrowIfFailed(
            device->CreateRasterizerState(&desc, m_rasterizerState.ReleaseAndGetAddressOf())
        );
    }

    // ----- 深度ステンシルステート ----- //
    {
        // 深度ステンシルステートの作成
        D3D11_DEPTH_STENCIL_DESC desc = {};
        desc.DepthEnable = TRUE;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        desc.DepthFunc = D3D11_COMPARISON_LESS;
        desc.StencilEnable = FALSE;
        DX::ThrowIfFailed(
            device->CreateDepthStencilState(&desc, m_depthStencilState.ReleaseAndGetAddressOf())
        );
    }

    // ----- ブレンドステート ----- //
    {
        // ブレンドステートの作成
        D3D11_BLEND_DESC desc = {};
        desc.AlphaToCoverageEnable = FALSE;
        desc.IndependentBlendEnable = FALSE;
        desc.RenderTarget[0].BlendEnable = FALSE;
        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        DX::ThrowIfFailed(
            device->CreateBlendState(&desc, m_blendState.ReleaseAndGetAddressOf())
        );
    }

}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.

    // 画面サイズを取得する
    int w, h;
    GetDefaultSize(w, h);

    // 射影行列の作成
    m_proj = Imase::CreatePerspectiveMatrix(
        // 画角　　　　　　　　　　アスペクト比
        XMConvertToRadians(45.0f), static_cast<float>(w) / static_cast<float>(h),
        // Near Far
        0.1f, 100.0f);

}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
