#include "PlayerSystem.h"

#include <Fission/Core/Math.h>
#include <Fission/Rendering/RenderSystem.h>
#include <Fission/Rendering/TransformComponent.h>
#include <Fission/Network/IntentComponent.h>
#include <Fission/Network/Connection.h>

#include "GridComponent.h"
#include "PlayerComponent.h"
#include "PhysicsComponent.h"
#include "InventoryComponent.h"
#include "SkeletonComponent.h"
#include "ItemComponent.h"

#include <iostream>

PlayerSystem::PlayerSystem(EventManager *eventManager, RenderSystem* rndSys, Connection* conn, float lockStep) :
    System(eventManager, lockStep, TransformComponent::Type|PlayerComponent::Type|PhysicsComponent::Type|IntentComponent::Type),
    mRndSys(rndSys), mConnection(conn)
{
    //ctor
}

PlayerSystem::~PlayerSystem()
{
    //dtor
}

void PlayerSystem::begin(const float dt)
{
}

void PlayerSystem::processEntity(Entity *entity, const float dt)
{
    auto trans = reinterpret_cast<TransformComponent*>(entity->getComponent(TransformComponent::Type));
    auto player = reinterpret_cast<PlayerComponent*>(entity->getComponent(PlayerComponent::Type));
    auto phys = reinterpret_cast<PhysicsComponent*>(entity->getComponent(PhysicsComponent::Type));
    auto intent = reinterpret_cast<IntentComponent*>(entity->getComponent(IntentComponent::Type));
    auto skel = reinterpret_cast<SkeletonComponent*>(entity->getComponent(SkeletonComponent::Type));
    auto inventory = reinterpret_cast<InventoryComponent*>(entity->getComponent(InventoryComponent::Type));

	skel->update(dt);

    bool runLeft = skel->getCurrentAnimation() == "running" && skel->getSkeleton()->skeleton->flipX;
    bool runRight = skel->getCurrentAnimation() == "running" && !skel->getSkeleton()->skeleton->flipX;

    if (intent->isIntentActive("left") && skel->getCurrentAnimation() == "")
    {
        skel->setAnimation(skel->findAnimation("running"), player->mStupidMode);
        skel->getSkeleton()->skeleton->flipX = true;
        runLeft = true;
    }
    else if (intent->isIntentActive("right") && skel->getCurrentAnimation() == "")
    {
        skel->setAnimation(skel->findAnimation("running"), player->mStupidMode);
        skel->getSkeleton()->skeleton->flipX = false;
        runRight = true;
    }

    if (runLeft && phys->getBody()->getVelocity().x > -25)
    {
        phys->getBody()->setVelocity(sf::Vector2f(-10, phys->getBody()->getVelocity().y));
    }
    else if (runRight && phys->getBody()->getVelocity().x < 25)
    {
        phys->getBody()->setVelocity(sf::Vector2f(10, phys->getBody()->getVelocity().y));
    }
    else
        phys->getBody()->setVelocity(sf::Vector2f(0, phys->getBody()->getVelocity().y));

	if (!player->mStupidMode)
    {
		if (intent->isIntentActive("up") && phys->getBody()->getVelocity().y > -15)
			phys->getBody()->setVelocity(sf::Vector2f(phys->getBody()->getVelocity().x, -15));
	}
	else
    {
		//if ((phys->getDirCollision(LEFT) || phys->getDirCollision(RIGHT)) && phys->getVelocity().y > -250)
			//phys->setVelocityY(-175);
	}

	if (mRndSys)
    {
		if (length(player->mCam-trans->getPosition()) > 5)
        {
			sf::Vector2f dist = trans->getPosition() - player->mCam;
			sf::Vector2f dir = normalize(dist);
			float camMove = length(dist) * 4 * dt;
			player->mCam += dir*camMove;
		}

		mRndSys->getView().setCenter(player->mCam);

        if (intent->isIntentActive("zoomin"))
            mRndSys->getView().zoom(1.f+dt);
        if (intent->isIntentActive("zoomout"))
            mRndSys->getView().zoom(1.f-dt);
	}

	if (intent->isIntentActive("stupidmode"))
    {
        skel->setAnimation(skel->findAnimation("running"), false);
        player->mStupidMode = !player->mStupidMode;
    }

    if (intent->isIntentActive("0"))
    {
        if (intent->isIntentActive("useLeft"))
        {
            if (player->mLeftHand != 0)
                player->clearLeftCoords();
            player->mLeftHand = 0;
        }
        else if (intent->isIntentActive("useRight"))
        {
            if (player->mRightHand != 0)
                player->clearRightCoords();
            player->mRightHand = 0;
        }
    }
    else if (intent->isIntentActive("1"))
    {
        if (intent->isIntentActive("useLeft"))
        {
            if (player->mLeftHand != 1)
                player->clearLeftCoords();
            player->mLeftHand = 1;
        }
        else if (intent->isIntentActive("useRight"))
        {
            if (player->mRightHand != 1)
                player->clearRightCoords();
            player->mRightHand = 1;
        }

    }
    else if (intent->isIntentActive("2"))
    {
        if (intent->isIntentActive("useLeft"))
        {
            if (player->mLeftHand != 2)
                player->clearLeftCoords();
            player->mLeftHand = 2;
        }
        else if (intent->isIntentActive("useRight"))
        {
            if (player->mRightHand != 2)
                player->clearRightCoords();
            player->mRightHand = 2;
        }
    }
    else if (intent->isIntentActive("3"))
    {
        if (intent->isIntentActive("useLeft"))
        {
            if (player->mLeftHand != 3)
                player->clearLeftCoords();
            player->mLeftHand = 3;
        }
        else if (intent->isIntentActive("useRight"))
        {
            if (player->mRightHand != 3)
                player->clearRightCoords();
            player->mRightHand = 3;
        }
    }
    else if (intent->isIntentActive("4"))
    {
        if (intent->isIntentActive("useLeft"))
        {
            if (player->mLeftHand != 4)
                player->clearLeftCoords();
            player->mLeftHand = 4;
        }
        else if (intent->isIntentActive("useRight"))
        {
            if (player->mRightHand != 4)
                player->clearRightCoords();
            player->mRightHand = 4;
        }
    }
    else if (intent->isIntentActive("5"))
    {
        if (intent->isIntentActive("useLeft"))
        {
            if (player->mLeftHand != 5)
                player->clearLeftCoords();
            player->mLeftHand = 5;
        }
        else if (intent->isIntentActive("useRight"))
        {
            if (player->mRightHand != 5)
                player->clearRightCoords();
            player->mRightHand = 5;
        }
    }
    else if (intent->isIntentActive("6"))
    {
        if (intent->isIntentActive("useLeft"))
        {
            if (player->mLeftHand != 6)
                player->clearLeftCoords();
            player->mLeftHand = 6;
        }
        else if (intent->isIntentActive("useRight"))
        {
            if (player->mRightHand != 6)
                player->clearRightCoords();
            player->mRightHand = 6;
        }
    }
    else if (intent->isIntentActive("7"))
    {
        if (intent->isIntentActive("useLeft"))
        {
            if (player->mLeftHand != 7)
                player->clearLeftCoords();
            player->mLeftHand = 7;
        }
        else if (intent->isIntentActive("useRight"))
        {
            if (player->mRightHand != 7)
                player->clearRightCoords();
            player->mRightHand = 7;
        }
    }
    else if (intent->isIntentActive("8"))
    {
        if (intent->isIntentActive("useLeft"))
        {
            if (player->mLeftHand != 8)
                player->clearLeftCoords();
            player->mLeftHand = 8;
        }

        else if (intent->isIntentActive("useRight"))
        {
            if (player->mRightHand != 8)
                player->clearRightCoords();
            player->mRightHand = 8;
        }
    }
    else if (intent->isIntentActive("9"))
    {
        if (intent->isIntentActive("useLeft"))
        {
            if (player->mLeftHand != 9)
                player->clearLeftCoords();
            player->mLeftHand = 9;
        }
        else if (intent->isIntentActive("useRight"))
        {
            if (player->mRightHand != 9)
                player->clearRightCoords();
            player->mRightHand = 9;
        }
    }
    else if (phys->getPrimaryGrid()) // We're not setting any hands, so lets use them
    {
		auto pt = reinterpret_cast<TransformComponent*>(phys->getPrimaryGrid()->getComponent(TransformComponent::Type));
		auto grid = reinterpret_cast<GridComponent*>(phys->getPrimaryGrid()->getComponent(GridComponent::Type));

        if (player->mLeftHandState == BtnState::PRESSED)
            player->mLeftHandState = BtnState::DOWN;
        if (player->mLeftHandState == BtnState::RELEASED)
            player->mLeftHandState = BtnState::UP;

        if (intent->isIntentActive("useLeft"))
            player->mLeftHandState = BtnState::PRESSED;
        if (intent->isIntentActive("useLeftRelease"))
            player->mLeftHandState = BtnState::RELEASED;


        if (player->mRightHandState == BtnState::PRESSED)
            player->mRightHandState = BtnState::DOWN;
        if (player->mRightHandState == BtnState::RELEASED)
            player->mRightHandState = BtnState::UP;

        if (intent->isIntentActive("useRight"))
            player->mRightHandState = BtnState::PRESSED;
        if (intent->isIntentActive("useRightRelease"))
            player->mRightHandState = BtnState::RELEASED;

		if (inventory->getItemAt(player->mLeftHand) && player->mLeftHandState == inventory->getItemAt(player->mLeftHand)->getUseState())
        {
			sf::Vector2f mousePos = intent->getMousePos();
            mousePos = mRndSys->getWindow().mapPixelToCoords(sf::Vector2i(mousePos.x, mousePos.y), mRndSys->getView());
			sf::Vector2f pos = grid->getTilePos(mousePos);
			if (pos.y >= 0 && pos.y < grid->getSizeY())
            {
                player->pushLeftCoord(sf::Vector2f(pos.x, pos.y));
                if (player->getLeftCoordsCount() == inventory->getItemAt(player->mLeftHand)->getCoordCount())
                    inventory->useItem(player->mLeftHand, phys->getPrimaryGrid(), player->mLeftCoords);
            }
		}
		if (inventory->getItemAt(player->mRightHand) && player->mRightHandState == inventory->getItemAt(player->mRightHand)->getUseState())
		{
			sf::Vector2f mousePos = intent->getMousePos();
			mousePos = mRndSys->getWindow().mapPixelToCoords(sf::Vector2i(mousePos.x, mousePos.y), mRndSys->getView());
			sf::Vector2f pos = grid->getTilePos(mousePos);
			if (pos.y >= 0 && pos.y < grid->getSizeY())
            {
                player->pushRightCoord(sf::Vector2f(pos.x, pos.y));
                if (player->getRightCoordsCount() == inventory->getItemAt(player->mRightHand)->getCoordCount())
                    inventory->useItem(player->mRightHand, phys->getPrimaryGrid(), player->mRightCoords);
            }
		}
		if (intent->isIntentActive("interact"))
		{
			sf::Vector2f mousePos = intent->getMousePos();
			mousePos = mRndSys->getWindow().mapPixelToCoords(sf::Vector2i(mousePos.x, mousePos.y), mRndSys->getView());
			sf::Vector2f pos = grid->getTilePos(mousePos);
            grid->interact(pos.x, pos.y);
		}
	}
}

void PlayerSystem::end(const float dt)
{
}
