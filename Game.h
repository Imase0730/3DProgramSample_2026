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

    // 入力レイアウト
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

    // 頂点シェーダー
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;

    // ピクセルシェーダー
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;

    // 定数バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;

    // 頂点バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;

    // インデックスバッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

    // ラスタライザーステート
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerState;

    // 深度ステンシル
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;

    // ブレンドステート
    Microsoft::WRL::ComPtr<ID3D11BlendState> m_blendState;

    // 定数バッファのデータ
    struct ConstantBufferData
    {
        DirectX::XMMATRIX worldViewProjection;   // ワールド行列×ビュー行列×プロジェクション行列
    };

    // ビュー行列を作成する関数
    static DirectX::SimpleMath::Matrix CreateViewMatrix(
        DirectX::SimpleMath::Vector3 eyePos,
        DirectX::SimpleMath::Vector3 focusPos,
        DirectX::SimpleMath::Vector3 upVector
    )
    {
        DirectX::SimpleMath::Matrix view = {};

        // カメラの向き
        DirectX::SimpleMath::Vector3 eyeDirection = eyePos - focusPos;

        // ベクトルを正規化する
        eyeDirection.Normalize();
        upVector.Normalize();

        // カメラ座標系の角軸の向きを算出する
        DirectX::SimpleMath::Vector3 zaxis = eyeDirection;           // Z軸
        DirectX::SimpleMath::Vector3 xaxis = upVector.Cross(zaxis);  // X軸
        DirectX::SimpleMath::Vector3 yaxis = zaxis.Cross(xaxis);     // Y軸

        // カメラ行列の逆行列を求める（正規直交行列の逆行列は転置行列）
        // |   1   0   0  0 | | RXx RYx RZx 0 |
        // |   0   1   0  0 | | RXy RYy RZy 0 | 
        // |   0   0   1  0 | | RXz RYz RZz 0 |
        // | -Tx -Ty -Tz  1 | |   0   0   0 1 |

        // 回転
        view._11 = xaxis.x;
        view._21 = xaxis.y;
        view._31 = xaxis.z;

        view._12 = yaxis.x;
        view._22 = yaxis.y;
        view._32 = yaxis.z;

        view._13 = zaxis.x;
        view._23 = zaxis.y;
        view._33 = zaxis.z;

        // 平行移動量は下記式から内積を使って求める
        //                 | RXx RYx RZx |
        // | -Tx -Ty -Tz | | RXy RYy RZy | 
        //                 | RXz RYz RZz |

        // 平行移動量
        view._41 = -xaxis.Dot(eyePos);
        view._42 = -yaxis.Dot(eyePos);
        view._43 = -zaxis.Dot(eyePos);

        return view;
    }

#if 0
    // ビュー行列を作成する関数
    static DirectX::XMMATRIX CreateViewMatrix(
        DirectX::XMVECTOR eyePos,
        DirectX::XMVECTOR focusPos,
        DirectX::XMVECTOR upVector
    )
    {
        // カメラへの向き
        DirectX::XMVECTOR eyeDirection = DirectX::XMVectorSubtract(eyePos, focusPos);

        // ----- カメラ座標系の角軸の向きを算出する ----- //

        // Z軸
        DirectX::XMVECTOR R2 = DirectX::XMVector3Normalize(eyeDirection);

        // X軸
        DirectX::XMVECTOR R0 = DirectX::XMVector3Cross(upVector, R2);
        R0 = DirectX::XMVector3Normalize(R0);

        // Y軸
        DirectX::XMVECTOR R1 = DirectX::XMVector3Cross(R2, R0);

        // カメラ行列の逆行列を求める（正規直交行列の逆行列は転置行列）
        // |   1   0   0  0 | | RXx RYx RZx 0 |
        // |   0   1   0  0 | | RXy RYy RZy 0 | 
        // |   0   0   1  0 | | RXz RYz RZz 0 |
        // | -Tx -Ty -Tz  1 | |   0   0   0 1 |

        // 平行移動量は下記式から内積を使って求める
        //                 | RXx RYx RZx |
        // | -Tx -Ty -Tz | | RXy RYy RZy | 
        //                 | RXz RYz RZz |

        DirectX::XMVECTOR NegEyePosition = DirectX::XMVectorNegate(eyePos);

        DirectX::XMVECTOR D0 = DirectX::XMVector3Dot(R0, NegEyePosition);
        DirectX::XMVECTOR D1 = DirectX::XMVector3Dot(R1, NegEyePosition);
        DirectX::XMVECTOR D2 = DirectX::XMVector3Dot(R2, NegEyePosition);

        DirectX::XMMATRIX view = {};
        view.r[0] = DirectX::XMVectorSelect(D0, R0, DirectX::g_XMSelect1110.v);
        view.r[1] = DirectX::XMVectorSelect(D1, R1, DirectX::g_XMSelect1110.v);
        view.r[2] = DirectX::XMVectorSelect(D2, R2, DirectX::g_XMSelect1110.v);
        view.r[3] = DirectX::g_XMIdentityR3.v;

        view = DirectX::XMMatrixTranspose(view);

        return view;
    }
#endif

    // 射影行列を作成する関数
    static DirectX::XMMATRIX CreatePerspectiveMatrix(
        float fovY, float aspectRatio, float nearPlane, float farPlane
    )
    {
        // 余接（コタンジェント）を求める関数
        auto cot = [](float a) { return cos(a) / sin(a); };

        float yScale = cot(fovY / 2.0f);
        float xScale = yScale / aspectRatio;

        DirectX::XMMATRIX proj = {};

        proj.r[0] = DirectX::XMVectorSet(xScale,   0.0f,                                          0.0f,  0.0f);
        proj.r[1] = DirectX::XMVectorSet(  0.0f, yScale,                                          0.0f,  0.0f);
        proj.r[2] = DirectX::XMVectorSet(  0.0f,   0.0f,             farPlane / (nearPlane - farPlane), -1.0f);
        proj.r[3] = DirectX::XMVectorSet(  0.0f,   0.0f, nearPlane * farPlane / (nearPlane - farPlane),  0.0f);

        return proj;
    }

};
