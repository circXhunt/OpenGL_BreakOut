#include "game_object.h"

#ifndef COLLISION_H
#define COLLISION_H

enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

typedef std::tuple<GLboolean, Direction, glm::vec2> CollisionResult;
class Collision
{
public:
    static GLboolean CheckCollision(GameObject& one, GameObject& two) // AABB - AABB collision
    {
        // x轴方向碰撞？
        bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
            two.Position.x + two.Size.x >= one.Position.x;
        // y轴方向碰撞？
        bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
            two.Position.y + two.Size.y >= one.Position.y;
        // 只有两个轴向都有碰撞时才碰撞
        return collisionX && collisionY;
    }
    static CollisionResult CheckCollision(BallObject& one, GameObject& two) // AABB - Circle collision
    {
        // 获取圆的中心 
        glm::vec2 center(one.Position + one.Radius);
        // 计算AABB的信息（中心、半边长）
        glm::vec2 aabb_half_extents(two.Size.x / 2, two.Size.y / 2);
        glm::vec2 aabb_center(
            two.Position.x + aabb_half_extents.x,
            two.Position.y + aabb_half_extents.y
        );
        // 获取两个中心的差矢量
        glm::vec2 difference = center - aabb_center;
        glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
        // AABB_center加上clamped这样就得到了碰撞箱上距离圆最近的点closest
        glm::vec2 closest = aabb_center + clamped;
        // 获得圆心center和最近点closest的矢量并判断是否 length <= radius
        difference = closest - center;
        if (glm::length(difference) < one.Radius) {
            return std::make_tuple(GL_TRUE, VectorDirection(difference), difference);
        }
        else
        {
            return std::make_tuple(GL_FALSE, UP, glm::vec2(0, 0));
        }
    }
    static Direction VectorDirection(glm::vec2 target)
    {
        glm::vec2 compass[] = {
            glm::vec2(0.0f, 1.0f),  // 上
            glm::vec2(1.0f, 0.0f),  // 右
            glm::vec2(0.0f, -1.0f), // 下
            glm::vec2(-1.0f, 0.0f)  // 左
        };
        GLfloat max = 0.0f;
        GLuint best_match = -1;
        for (GLuint i = 0; i < 4; i++)
        {
            GLfloat dot_product = glm::dot(glm::normalize(target), compass[i]);
            if (dot_product > max)
            {
                max = dot_product;
                best_match = i;
            }
        }
        return (Direction)best_match;
    }
};

#endif // !COLLISION_H
