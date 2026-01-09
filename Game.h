//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"

#include <memory>

#include "ImaseLib/DebugFont.h"
#include "ImaseLib/DebugCamera.h"
#include "ImaseLib/GridFloor.h"

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:

    Game() noexcept(false);
    ~Game() = default;

    Game(Game&&) = default;
    Game& operator= (Game&&) = default;

    Game(Game const&) = delete;
    Game& operator= (Game const&) = delete;

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    void OnDeviceLost() override;
    void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnDisplayChange();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const noexcept;

private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;

private:

    // 射影行列
    DirectX::SimpleMath::Matrix m_proj;

    // コモンステート
    std::unique_ptr<DirectX::CommonStates> m_states;

    // デバッグフォント
    std::unique_ptr<Imase::DebugFont> m_debugFont;

    // デバッグカメラ
    std::unique_ptr<Imase::DebugCamera> m_debugCamera;

    // グリッドの床
    std::unique_ptr<Imase::GridFloor> m_gridFloor;

    // 定数バッファのデータ
    struct ConstantBufferData
    {
        DirectX::XMMATRIX worldViewProjection;   // ワールド行列×ビュー行列×プロジェクション行列
    };

    // 定数バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;

    // 頂点バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;

    // インデックスバッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

    // ----- IA ----- //

    // 入力レイアウト
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

    // ----- VS ----- //

    // 頂点シェーダー
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;

    // ----- PS ----- //

    // ピクセルシェーダー
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;

    // ----- OM ----- //

    // ラスタライザーステート
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerState;

    // 深度ステンシル
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;

    // ブレンドステート
    Microsoft::WRL::ComPtr<ID3D11BlendState> m_blendState;

};
