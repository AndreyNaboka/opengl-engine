#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>
#include <algorithm>
#include <cmath>

namespace MathUtils
{
    inline int add(const int a, const int b) { return a + b; }
    // Интерполяция кватернионов с корректной обработкой противоположных направлений
    inline glm::quat slerpShortest(const glm::quat &q1, const glm::quat &q2, float t)
    {
        float dot = glm::dot(q1, q2);
        glm::quat q3 = q2;
        if (dot < 0.0f)
        {
            dot = -dot;
            q3 = -q2;
        }
        dot = glm::clamp(dot, -1.0f, 1.0f);
        float theta = std::acos(dot) * t;
        glm::quat q4 = (q3 - q1 * dot);
        q4 = glm::normalize(q4);
        return q1 * std::cos(theta) + q4 * std::sin(theta);
    }

    // Вычисление ограничивающей сферы по точкам
    inline void computeBoundingSphere(const std::vector<glm::vec3> &points, glm::vec3 &center, float &radius)
    {
        if (points.empty())
        {
            center = glm::vec3(0.0f);
            radius = 0.0f;
            return;
        }

        // Итеративный алгоритм для точного вычисления
        center = points[0];
        radius = 0.0f;

        for (size_t i = 1; i < points.size(); ++i)
        {
            glm::vec3 toPoint = points[i] - center;
            float distSq = glm::dot(toPoint, toPoint);
            if (distSq > radius * radius)
            {
                float dist = std::sqrt(distSq);
                float newRadius = (radius + dist) * 0.5f;
                center += toPoint * ((dist - radius) / (2.0f * dist));
                radius = newRadius;
            }
        }

        // Финальная коррекция для всех точек
        for (const auto &p : points)
        {
            float dist = glm::distance(center, p);
            if (dist > radius)
            {
                radius = dist;
            }
        }
    }

    // Проверка видимости сферы в камере (фрустум кэллинг)
    struct Frustum
    {
        glm::vec4 planes[6]; // left, right, bottom, top, near, far

        void update(const glm::mat4 &viewProj)
        {
            // Извлечение плоскостей из матрицы вида-проекции
            glm::mat4 vp = viewProj;

            // Left plane
            planes[0] = vp[3] + vp[0];
            // Right plane
            planes[1] = vp[3] - vp[0];
            // Bottom plane
            planes[2] = vp[3] + vp[1];
            // Top plane
            planes[3] = vp[3] - vp[1];
            // Near plane
            planes[4] = vp[3] + vp[2];
            // Far plane
            planes[5] = vp[3] - vp[2];

            // Нормализация
            for (int i = 0; i < 6; ++i)
            {
                float len = glm::length(glm::vec3(planes[i]));
                if (len > 0.0f)
                    planes[i] /= len;
            }
        }

        bool sphereVisible(const glm::vec3 &center, float radius) const
        {
            for (int i = 0; i < 6; ++i)
            {
                float dist = glm::dot(glm::vec3(planes[i]), center) + planes[i].w;
                if (dist < -radius)
                    return false; // сфера полностью за плоскостью
            }
            return true;
        }
    };
}