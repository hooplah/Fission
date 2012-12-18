#include "PlayerControlComponent.h"

#include <Core/InputManager.h>
#include <Core/GameObject.h>
#include <Rendering/SpriteComponent.h>
#include <Physics/RigidBodyComponent.h>
#include <Scene/SceneManager.h>
#include <Logic/WeaponComponent.h>
#include <Logic/ProjectileComponent.h>

#include "Villain.h"

//move states
enum
{
    STAND,
    MOVE,
    TURN,
    PUNCH,
    JUMP,
};

//directions
enum
{
    LEFT,
    RIGHT
};

PlayerControlComponent::PlayerControlComponent(GameObject *object, std::string name) : HPComponent(object, name)
{
    mOnGround = false; // Floating by default
    mContactCount = 0;

    mGameObject->getComponent<SpriteComponent>()->setFrameLoop(0,0);
    mMoveState = STAND;
    mDirection = LEFT;

    mLightning = NULL;
}

PlayerControlComponent::~PlayerControlComponent()
{
    //dtor
}

bool PlayerControlComponent::update(float dt)
{
    SpriteComponent *sprite = mGameObject->getComponent<SpriteComponent>();
    RigidBodyComponent *body = mGameObject->getComponent<RigidBodyComponent>();

    if (mContactClock.getElapsedTime().asMilliseconds() < 300 && mGhostClock.getElapsedTime().asMilliseconds() >= 200)
        mOnGround = true;
    else if (mContactCount == 0)
        mOnGround = false;

    float yVel = body->getBody()->GetLinearVelocity().y;

    if (InputManager::get()->getKeyDown(sf::Keyboard::A))
    {
        if (!mOnGround) //not on the ground
        {
            mDirection = LEFT;
            body->getBody()->SetLinearVelocity(b2Vec2(-5,yVel));
        }
        else
        {
            if (mDirection == LEFT)
            {
                sprite->setFrameLoop(6,9);
                sprite->setLoopAnim(true);
                body->getBody()->SetLinearVelocity(b2Vec2(-8,yVel));
                mMoveState = MOVE;
            }
            else //do turning animation
            {
                mMoveState = TURN;
            }
        }
    }
    else if (InputManager::get()->getKeyDown(sf::Keyboard::D))
    {
        if (!mOnGround)
        {
            mDirection = RIGHT;
            body->getBody()->SetLinearVelocity(b2Vec2(5,yVel));
        }
        else
        {
            if (mDirection == RIGHT)
            {
                sprite->setFrameLoop(0,3);
                sprite->setLoopAnim(true);
                body->getBody()->SetLinearVelocity(b2Vec2(8,yVel));
                mMoveState = MOVE;
            }
            else //do turning animation
            {
                mMoveState = TURN;
            }
        }
    }
    else if (InputManager::get()->getKeyDown(sf::Keyboard::Space) && mMoveState == STAND)
    {
        if (!mLightning)
        {
            mLightning = SceneManager::get()->createGameObject();
            SpriteComponent *lSprite = new SpriteComponent(mLightning, "sprite", "Content/Textures/lightning.png", 4, 1);
            lSprite->setLoopAnim(true);
            lSprite->setAnimDelay(150);
            mLightning->addComponent(lSprite);
        }

        if (mDirection == LEFT)
        {
            sprite->setFrameLoop(14,15);
            sprite->setLoopAnim(false);
            mGameObject->getComponent<WeaponComponent>()->fire(180);
            mLightning->getComponent<SpriteComponent>()->setFrameLoop(2,3);
            mLightning->setPosition(mGameObject->getPosition()+sf::Vector2f(-2.6,0.5));
        }
        else if (mDirection == RIGHT)
        {
            sprite->setFrameLoop(12,13);
            sprite->setLoopAnim(false);
            mGameObject->getComponent<WeaponComponent>()->fire(0);
            mLightning->getComponent<SpriteComponent>()->setFrameLoop(0,1);
            mLightning->setPosition(mGameObject->getPosition()+sf::Vector2f(2.6,0.5));
        }

        body->getBody()->SetLinearVelocity(b2Vec2(0,yVel));
    }
    else if (mMoveState != TURN && mOnGround)
    {
        if (mDirection == LEFT)
            sprite->setFrameLoop(6,6);
        else if (mDirection == RIGHT)
            sprite->setFrameLoop(0,0);
        body->getBody()->SetLinearVelocity(b2Vec2(0,yVel));
        mMoveState = STAND;
    }

    if ((InputManager::get()->getKeyUp(sf::Keyboard::Space) || !mOnGround || mMoveState != STAND) && mLightning)
    {
        mLightning->kill();
        mLightning = NULL;
    }

    if (InputManager::get()->getKeyState(sf::Keyboard::W) == ButtonState::PRESSED && mOnGround) //jump!
    {
        mMoveState = JUMP;
        body->getBody()->SetLinearVelocity(b2Vec2(0,12));
    }

    // Fall through platforms
    if (InputManager::get()->getKeyDown(sf::Keyboard::S))
    {
        mGhostClock.restart();
        if (mOnGround)
        {
            body->getBody()->SetLinearVelocity(b2Vec2(0,-10));
        }
    }

    //temporary states that aren't controlled with the keyboard
    if (mMoveState == TURN)
    {
        if (mDirection == LEFT)
        {
            sprite->setFrameLoop(21,23);
            sprite->setLoopAnim(false);
            if (sprite->getAnimFinished())
            {
                mMoveState = STAND;
                mDirection = RIGHT;
            }
        }
        else if (mDirection == RIGHT)
        {
            sprite->setFrameLoop(18,20);
            sprite->setLoopAnim(false);
            if (sprite->getAnimFinished())
            {
                mMoveState = STAND;
                mDirection = LEFT;
            }
        }
    }
    else if (mMoveState == JUMP)
    {
        if (mOnGround)
            mMoveState = STAND;
    }

    if (!mOnGround)
    {
        if (mDirection == LEFT)
        {
            if (body->getBody()->GetLinearVelocity().y >= 0) //going up
            {
                sprite->setFrameLoop(30,30);
                sprite->setLoopAnim(false);
            }
            else if (body->getBody()->GetLinearVelocity().y < 0) //going down
            {
                sprite->setFrameLoop(31,34);
                sprite->setLoopAnim(false);
            }
        }
        else if (mDirection == RIGHT)
        {
            if (body->getBody()->GetLinearVelocity().y >= 0) //going up
            {
                sprite->setFrameLoop(24,24);
                sprite->setLoopAnim(false);
            }
            else if (body->getBody()->GetLinearVelocity().y < 0) //going down
            {
                sprite->setFrameLoop(25,28);
                sprite->setLoopAnim(false);
            }
        }
    }

    return true;
}

void PlayerControlComponent::onRender(sf::RenderTarget *target, sf::RenderStates states)
{
}

void PlayerControlComponent::onPreSolve(GameObject *object, b2Contact* contact, const b2Manifold* oldManifold)
{
    if (object->getType() == CHARACTER)
        contact->SetEnabled(false);

    // Don't collide with structures that we are jumping up to or are going down from
    if (object->getType() == PLATFORM && (mGameObject->getPosition().y <= object->getPosition().y ||
                                           mGhostClock.getElapsedTime().asMilliseconds() < 400))
    {
        contact->SetEnabled(false);

        if (mGameObject->getPosition().y <= object->getPosition().y)
            mGhostClock.restart();
    }
}

void PlayerControlComponent::onContactBegin(GameObject *object)
{
    ProjectileComponent *proj = object->getComponent<ProjectileComponent>();
    if (proj) //it's a projectile
    {
        damage(proj->getDamage());
    }

    if (object->getComponent<RigidBodyComponent>())
    {
        mContactClock.restart(); //restart the contact clock
        mContactCount++;
    }
}

void PlayerControlComponent::onContactEnd(GameObject *object)
{
    if (object->getComponent<RigidBodyComponent>())
        mContactCount--;
    if (mContactCount < 0)
        mContactCount = 0;
}
