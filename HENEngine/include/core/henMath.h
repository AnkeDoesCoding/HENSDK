#ifndef _HENMATH_H_
#define _HENMATH_H_

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

namespace hen::math
{
    using Vec2 = glm::vec2;
    using Vec3 = glm::vec3;
    using Vec4 = glm::vec4;

    using Matrix2 = glm::mat2;
    using Matrix3 = glm::mat3;
    using Matrix4 = glm::mat4;

    using Quat = glm::quat;

    // FLOAT

    inline float Clamp(float x, float min, float max)
    {
        return glm::clamp(x, min, max);
    }

    inline float Cos(float x)
    {
        return glm::cos(x);
    }

    inline float Degrees(float radians)
    {
        return glm::degrees(radians);
    }

    inline float Radians(float degrees)
    {
        return glm::radians(degrees);
    }

    inline float Asin(float angle)
    {
        return glm::asin(angle);
    }

    inline float Atan(float angle, float angle2)
    {
        return glm::atan(angle, angle2);
    }

    // VEC3

    inline Vec3 Cross(Vec3 vectorX, Vec3 vectorY)
    {
        return glm::cross(vectorX, vectorY);
    }

    inline Vec3 Normalise(Vec3 vector)
    {
        return glm::normalize(vector);
    }

    inline float Length(Vec3 vector)
    {
        return glm::length(vector);
    }

    inline Vec3 Degrees(Vec3 radians)
    {
        return glm::degrees(radians);
    }

    inline Vec3 Radians(Vec3 degrees)
    {
        return glm::radians(degrees);
    }

    inline Vec3 EulerAngles(Quat quat)
    {
        return glm::eulerAngles(quat);
    }

    // MATRIX4

    inline Matrix4 LookAt(Vec3 eye, Vec3 center, Vec3 up)
    {
        return glm::lookAt(eye, center, up);
    }

    inline Matrix4 Perspective(float fov, float aspect, float near, float far)
    {
        return glm::perspective(fov, aspect, near, far);
    }

    inline Matrix4 Scale(Matrix4 matrix, Vec3 vector)
    {
        return glm::scale(matrix, vector);
    }

    inline Matrix4 ToMatrix4(Quat quat)
    {
        return glm::toMat4(quat);
    }

    inline Matrix4 Translate(Matrix4 matrix, Vec3 vector)
    {
        return glm::translate(matrix, vector);
    }

    inline Matrix4 Transpose(Matrix4 matrix)
    {
        return glm::transpose(matrix);
    }

    inline Matrix4 Inverse(Matrix4 matrix)
    {
        return glm::inverse(matrix);
    }

    inline Matrix4 MakeMatrix4(const double* data)
    {
        return glm::make_mat4(data);
    }
}

#endif // !_HENMATH_H_