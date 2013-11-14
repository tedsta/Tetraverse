#include "PlayerSystem.h"

#include "Fission/Core/Math.h"
#include "Fission/Rendering/RenderSystem.h"
#include "Fission/Rendering/TransformComponent.h"
#include "Fission/Network/IntentComponent.h"
#include "GridComponent.h"
#include "PlayerComponent.h"
#include "PhysicsComponent.h"
#include "InventoryComponent.h"

#include <iostream>

PlayerSystem::PlayerSystem(EventManager *eventManager, RenderSystem* rndSys) :
    System(eventManager, TransformComponent::Type|PlayerComponent::Type|PhysicsComponent::Type|IntentComponent::Type),
    mRndSys(rndSys)
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
    auto trans = static_cast<TransformComponent*>(entity->getComponent(TransformComponent::Type));
    auto player = static_cast<PlayerComponent*>(entity->getComponent(PlayerComponent::Type));
    auto phys = static_cast<PhysicsComponent*>(entity->getComponent(PhysicsComponent::Type));
    auto intent = static_cast<IntentComponent*>(entity->getComponent(IntentComponent::Type));
    //auto skel = static_cast<SkeletonComponent*>(entity->getComponent(SkeletonComponent::Type));
    auto inventory = reinterpret_cast<InventoryComponent*>(entity->getComponent(InventoryComponent::Type));

	//skel.Apply(skel.FindAnimation("running"), player->manimTime, true)
	//skel.Update(dt)

	if (!player->mStupidMode)
    {
		if (intent->isIntentActive("left") && phys->getVelocity().x > -250)
		{
			phys->setVelocityX(-100);
			//skel.skel.FlipX = true
			player->mAnimTime += dt;
		}
        else if (intent->isIntentActive("right") && phys->getVelocity().x < 2500)
        {
			phys->setVelocityX(100);
			//skel.skel.FlipX = false
			player->mAnimTime += dt;
		}
		else
			phys->setVelocityX(0);
	}
	else
		phys->setVelocityX(100);

	if (!player->mStupidMode)
    {
		if (intent->isIntentActive("up") && phys->getVelocity().y > -250)
			phys->setVelocityY(-175);
	}
	else
    {
		if ((phys->getDirCollision(LEFT) || phys->getDirCollision(RIGHT)) && phys->getVelocity().y > -250)
			phys->setVelocityY(-175);
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
	}

	if (intent->isIntentActive("stupidmode"))
		player->mStupidMode = !player->mStupidMode;

    if (intent->isIntentActive("0"))
    {
        if (intent->isIntentActive("use"))
            player->mLeftHand = 0;
        else if (intent->isIntentActive("interact"))
            player->mRightHand = 0;
    }
    else if (intent->isIntentActive("1"))
    {
        if (intent->isIntentActive("use"))
            player->mLeftHand = 1;
        else if (intent->isIntentActive("interact"))
            player->mRightHand = 1;
    }
    else if (intent->isIntentActive("2"))
    {
        if (intent->isIntentActive("use"))
            player->mLeftHand = 2;
        else if (intent->isIntentActive("interact"))
            player->mRightHand = 2;
    }
    else if (intent->isIntentActive("3"))
    {
        if (intent->isIntentActive("use"))
            player->mLeftHand = 3;
        else if (intent->isIntentActive("interact"))
            player->mRightHand = 3;
    }
    else if (intent->isIntentActive("4"))
    {
        if (intent->isIntentActive("use"))
            player->mLeftHand = 4;
        else if (intent->isIntentActive("interact"))
            player->mRightHand = 4;
    }
    else if (intent->isIntentActive("5"))
    {
        if (intent->isIntentActive("use"))
            player->mLeftHand = 5;
        else if (intent->isIntentActive("interact"))
            player->mRightHand = 5;
    }
    else if (intent->isIntentActive("6"))
    {
        if (intent->isIntentActive("use"))
            player->mLeftHand = 6;
        else if (intent->isIntentActive("interact"))
            player->mRightHand = 6;
    }
    else if (intent->isIntentActive("7"))
    {
        if (intent->isIntentActive("use"))
            player->mLeftHand = 7;
        else if (intent->isIntentActive("interact"))
            player->mRightHand = 7;
    }
    else if (intent->isIntentActive("8"))
    {
        if (intent->isIntentActive("use"))
            player->mLeftHand = 8;
        else if (intent->isIntentActive("interact"))
            player->mRightHand = 8;
    }
    else if (intent->isIntentActive("9"))
    {
        if (intent->isIntentActive("use"))
            player->mLeftHand = 9;
        else if (intent->isIntentActive("interact"))
            player->mRightHand = 9;
    }

	if (phys->getGrid())
    {
		auto pt = reinterpret_cast<TransformComponent*>(phys->getGrid()->getComponent(TransformComponent::Type));
		auto grid = reinterpret_cast<GridComponent*>(phys->getGrid()->getComponent(GridComponent::Type));

		if (intent->isIntentActive("use"))
        {
			sf::Vector2f mousePos = intent->getMousePos();
			mousePos += mRndSys->getView().getCenter();
			mousePos -= mRndSys->getView().getSize()/2.f;
			sf::Vector2f pos = grid->getTilePos(pt, mousePos);
            inventory->useItem(player->mLeftHand, grid, pos.x, pos.y);
			//if (length(mousePos - trans->getPosition()) < 16*10) //16 pixels per tile, 10 tiles
				//grid->setTile(int(pos.x), int(pos.y), Tile(0, 0, 0, 2), -1);
		}
		if (intent->isIntentActive("interact"))
		{
			sf::Vector2f mousePos = intent->getMousePos();
			mousePos += mRndSys->getView().getCenter();
			mousePos -= mRndSys->getView().getSize()/2.f;
			sf::Vector2f pos = grid->getTilePos(pt, mousePos);
			inventory->useItem(player->mRightHand, grid, pos.x, pos.y);
			//if (length(mousePos - trans->getPosition()) < 16*10) //16 pixels per tile, 10 tiles
				//grid->setTile(int(pos.x), int(pos.y), Tile(0, 0, 0, 1), -1);
		}
		if (intent->isIntentActive("test"))
		{
			sf::Vector2f mousePos = intent->getMousePos();
			mousePos += mRndSys->getView().getCenter();
			mousePos -= mRndSys->getView().getSize()/2.f;
			sf::Vector2f pos = grid->getTilePos(pt, mousePos);
            pos.x = grid->wrapX(pos.x);
            if(grid->getTile(int(pos.x),int(pos.y)).mWire == 0){
                grid->setTile(int(pos.x), int(pos.y), Tile(0, 0, 0, 1,0), -1);
            }
            else if(grid->getTile(int(pos.x),int(pos.y)).mWire == 1){
                grid->setTile(int(pos.x), int(pos.y), Tile(0, 0, 0, 128,128), -1);
            }
            else{
                grid->setTile(int(pos.x), int(pos.y), Tile(0, 0, 0, 0,0), -1);
            }
		}
	}
}

void PlayerSystem::end(const float dt)
{
}
