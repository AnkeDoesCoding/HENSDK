#ifndef _HENMATH_H_
#define _HENMATH_H_

#ifdef PLATFORM_LINUX
    #define _XM_NO_XMVECTOR_OVERLOADS_
#endif

#include "vendor/directxmath/include/DirectXMath.h"
#include "vendor/directxmath/include/DirectXPackedVector.h"

// the reason i am aliasing directxmath is because it is ugly

namespace hen::math
{
    // DirectXMath Aliases
    // -----------------------------------------------------------------------------------------------------

    using Vector = DirectX::XMVECTOR;

    using Matrix = DirectX::XMMATRIX;

    using Float2 = DirectX::XMFLOAT2;

    using Float3 = DirectX::XMFLOAT3;

    using Float4 = DirectX::XMFLOAT4;

    // VECTOR

    inline Vector Vector3TransformCoord(Vector lookAtVec, Matrix rotMatrix)
    {
        return DirectX::XMVector3TransformCoord(lookAtVec, rotMatrix);
    }

    inline Vector VectorAdd(Vector posVec, Vector lookAtVec)
    {
        return DirectX::XMVectorAdd(posVec, lookAtVec);
    }

    // MATRIX

    inline Matrix IdentityMatrix()
    {
        return DirectX::XMMatrixIdentity();
    }

    inline Matrix MatrixPerspectiveFovLH(float fieldOfView, float aspectRatio, float nearZ, float farZ)
    {
        return DirectX::XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, nearZ, farZ);
    }

    inline Matrix MatrixOrthographicLH(float viewWidth, float viewHeight, float nearZ, float farZ)
    {
        return DirectX::XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ);
    }

    inline Matrix MatrixRotationRollPitchYaw(float pitch, float yaw, float roll)
    {
        return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
    }

    inline Matrix MatrixLookAtLH(Vector posVec, Vector focusVec, Vector upVec)
    {
        return DirectX::XMMatrixLookAtLH( posVec,  focusVec,  upVec);
    }

    inline Matrix MatrixTranspose(Matrix matrix)
    {
        return DirectX::XMMatrixTranspose(matrix);
    }
}

#endif // !_HENMATH_H_