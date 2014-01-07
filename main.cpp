#include <iostream>
#include <bitset>
#include <cstdlib>
#include <sstream>
#include <stack>

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <Sqrat/sqext.h>

#include <Fission/Core/Engine.h>
#include <Fission/Core/EntityManager.h>
#include <Fission/Core/ResourceManager.h>
#include <Fission/Core/ComponentTypeManager.h>

#include <Fission/Rendering/RenderManager.h>
#include <Fission/Rendering/SpriteRenderSystem.h>
#include <Fission/Rendering/DebugDisplay.h>
#include <Fission/Rendering/Sprite.h>
#include <Fission/Rendering/Transform.h>

#include <Fission/Input/InputSystem.h>

#include <Fission/Network/Connection.h>
#include <Fission/Network/IntentSystem.h>
#include <Fission/Network/Intent.h>

#include <Fission/Script/ScriptSystem.h>

#include "phys/PhysicsWorld.h"
#include "phys/RigidBody.h"
#include "phys/Shape.h"

#include "Components/RigidBody.h"

#include "Systems/PhysicsSystem.h"

#include "perlin/perlin.h"
#include <random>

void bindSquirrel(HSQUIRRELVM vm);

int main()
{
    std::cout << "Tetraverse\nTeam Fission\n\n";

    fsn::ResourceManager::init();

    fsn::ComponentTypeManager::add<fsn::Transform>();
    fsn::ComponentTypeManager::add<fsn::Sprite>();
    fsn::ComponentTypeManager::add<fsn::Intent>();
    fsn::ComponentTypeManager::add<RigidBody>();

    auto engine = new fsn::Engine;
    auto renderMgr = new fsn::RenderManager(800, 600, "Tetraverse", 5, fsn::ResourceManager::get()->getFont("Content/Fonts/font.ttf"));
    auto conn = new fsn::Connection(engine->getEventManager());

    auto inputSys = new fsn::InputSystem(engine->getEventManager(), 1.f/60.f, &renderMgr->getWindow());
    auto intentSys = new fsn::IntentSystem(engine->getEventManager(), 1.f/60.f, conn);
    auto spriteSys = new fsn::SpriteRenderSystem(engine->getEventManager(), renderMgr, 0.f);
    auto physSys = new PhysicsSystem(engine->getEventManager(), 1.f/60.f);

    engine->addSystem(inputSys);
    engine->addSystem(intentSys);
    engine->addSystem(spriteSys);
    engine->addSystem(physSys);

    auto entityMgr = engine->getEntityManager();

    auto player = entityMgr->getEntityRef(entityMgr->createEntity());
    player->addComponent(new fsn::Transform(sf::Vector2f(50, 50)));
    player->addComponent(new fsn::Sprite("Content/Textures/Tiles/dirt.png", 23, 23));

    float accum = 0;
    int frames = 0;

    sf::Clock clock;
    while (renderMgr->getWindow().isOpen())
    {
        float dt = clock.getElapsedTime().asSeconds();
        clock.restart();
        accum += dt;

        if (accum >= 1.f)
        {
            std::stringstream fps;
            std::string sFps;
            fps << frames;
            fps >> sFps;
            renderMgr->getDebugDisplay().setMessage("FPS", sFps);
            accum = 0;
            frames = 0;
        }
        frames++;

        conn->update(dt);
        engine->update(dt);
        renderMgr->renderLayers();
    }

    delete engine;

    return 0;
}

template <typename T>
T* componentCast(fsn::Component* c)
{
    return static_cast<T*>(c);
}

void bindSquirrel(HSQUIRRELVM vm)
{
    Sqrat::DerivedClass<fsn::Transform, fsn::Component,
        sqext::ConstAlloc<fsn::Transform, sf::Vector2f, float, sf::Vector2f>> transform(vm);
    transform.Func("setPosition", (void (fsn::Transform::*)(const sf::Vector2f&))&fsn::Transform::setPosition);
    transform.Func("getPosition", &fsn::Transform::getPosition);
    transform.Func("move", (void (fsn::Transform::*)(const sf::Vector2f&))&fsn::Transform::move);
    transform.Func("setOrigin", (void (fsn::Transform::*)(const sf::Vector2f&))&fsn::Transform::setOrigin);
    Sqrat::RootTable(vm).Bind("Transform", transform);

    Sqrat::DerivedClass<fsn::RenderComponent, fsn::Component> renderComp(vm);
    renderComp.Func("setLit", &fsn::RenderComponent::setLit);
    renderComp.Func("getLit", &fsn::RenderComponent::getLit);
    renderComp.Func("setLayer", &fsn::RenderComponent::setLayer);
    renderComp.Func("getLayer", &fsn::RenderComponent::getLayer);
    Sqrat::RootTable(vm).Bind("RenderComponent", renderComp);

    Sqrat::DerivedClass<fsn::Sprite, fsn::RenderComponent,
        sqext::ConstAlloc<fsn::Sprite, const std::string&, int, int>> sprite(vm);
    //sprite.Func("setTexture", &fsn::Sprite::setTexture);
    sprite.Func("setFrameLoop", &fsn::Sprite::setFrameLoop);
    sprite.Func("setFrameDir", &fsn::Sprite::setFrameDir);
    sprite.Func("setLoopAnim", &fsn::Sprite::setLoopAnim);
    Sqrat::RootTable(vm).Bind("Sprite", sprite);

    Sqrat::DerivedClass<fsn::Intent, fsn::Component, sqext::ConstAlloc<fsn::Intent>> intent(vm);
    intent.Func("mapKeyToIntent", &fsn::Intent::mapKeyToIntent);
    intent.Func("mapMouseBtnToEvent", &fsn::Intent::mapMouseBtnToIntent);
    intent.Func("isIntentActive", &fsn::Intent::isIntentActive);
    intent.Func("getMousePos", &fsn::Intent::getMousePos);
    Sqrat::RootTable(vm).Bind("Intent", intent);

    Sqrat::Class<phys::RigidBody, sqext::ConstAlloc<phys::RigidBody, phys::Shape*, int, float>> rigidbody(vm);
    rigidbody.Func("applyForce", &phys::RigidBody::applyForce);

    rigidbody.Func("setVelocity", &phys::RigidBody::setVelocity);
    rigidbody.Func("setVelocityX", &phys::RigidBody::setVelocityX);
    rigidbody.Func("setVelocityY", &phys::RigidBody::setVelocityY);
    rigidbody.Func("getVelocity", &phys::RigidBody::getVelocity);
    Sqrat::RootTable(vm).Bind("RigidBody", rigidbody);

    Sqrat::DerivedClass<RigidBody, fsn::Component, sqext::ConstAlloc<RigidBody, float, float, float>> rigidBody(vm);
    rigidBody.Func("getBody", &RigidBody::getBody);
    Sqrat::RootTable(vm).Bind("RigidBody", rigidBody);

    // Button states
    Sqrat::RootTable(vm).SetValue("BtnStateDown", fsn::Down);
    Sqrat::RootTable(vm).SetValue("BtnStateUp", fsn::Up);
    Sqrat::RootTable(vm).SetValue("BtnStatePressed", fsn::Pressed);
    Sqrat::RootTable(vm).SetValue("BtnStateReleased", fsn::Released);

    // Keyboard keys
    Sqrat::RootTable(vm).SetValue("KeyW", sf::Keyboard::W);
    Sqrat::RootTable(vm).SetValue("KeyS", sf::Keyboard::S);
    Sqrat::RootTable(vm).SetValue("KeyA", sf::Keyboard::A);
    Sqrat::RootTable(vm).SetValue("KeyD", sf::Keyboard::D);
    Sqrat::RootTable(vm).SetValue("KeyI", sf::Keyboard::I);
    Sqrat::RootTable(vm).SetValue("KeyK", sf::Keyboard::K);
    Sqrat::RootTable(vm).SetValue("KeyJ", sf::Keyboard::J);
    Sqrat::RootTable(vm).SetValue("KeyL", sf::Keyboard::L);

    // All the component types
    Sqrat::RootTable(vm).SetValue("RigidBodyType", RigidBody::Type());

    // All the component casting functions
    Sqrat::RootTable(vm).Func("castRigidBody", componentCast<RigidBody>);
}
