#pragma once

#include "SimpleMath.h"

namespace Imase
{
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

        proj.r[0] = DirectX::XMVectorSet(xScale, 0.0f, 0.0f, 0.0f);
        proj.r[1] = DirectX::XMVectorSet(0.0f, yScale, 0.0f, 0.0f);
        proj.r[2] = DirectX::XMVectorSet(0.0f, 0.0f, farPlane / (nearPlane - farPlane), -1.0f);
        proj.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, nearPlane * farPlane / (nearPlane - farPlane), 0.0f);

        return proj;
    }

}