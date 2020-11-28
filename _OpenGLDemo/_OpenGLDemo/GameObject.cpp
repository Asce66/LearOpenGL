#include "GameObject.h"

GameObject::GameObject() :
	Position(glm::vec2(0)), Size(glm::vec2(1)), Color(glm::vec3(1)), Velocity(glm::vec2(0)), IsDestroied(false), IsSolid(false), Rotation(0.0f)
{
}

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D& sprite, glm::vec3 color, glm::vec2 velocity) :
	Position(pos), Size(size), Sprite(sprite), Color(color), Velocity(velocity), IsDestroied(false), IsSolid(false), Rotation(0.0f)
{
}

void GameObject::Draw(SpriteRenderer& renderer)
{
	renderer.DrawSprite(Sprite, Position, Size, Rotation, Color);
}
