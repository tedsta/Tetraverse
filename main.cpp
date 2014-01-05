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
#include <Fission/Rendering/SpriteComponent.h>
#include <Fission/Rendering/TransformComponent.h>

#include <Fission/Input/InputSystem.h>

#include <Fission/Network/Connection.h>
#include <Fission/Network/IntentSystem.h>
#include <Fission/Network/IntentComponent.h>

#include <Fission/Script/ScriptSystem.h>

#include "phys/PhysicsWorld.h"
#include "phys/RigidBody.h"
#include "phys/Shape.h"

#include "Components/PhysicsComponent.h"
#include "Components/EmitterComponent.h"

#include "Systems/PhysicsSystem.h"
#include "Systems/EmitterSystem.h"

#include "perlin/perlin.h"
#include <random>

void bindSquirrel(HSQUIRRELVM vm);

int main()
{
    std::cout << "Tetraverse\nTeam Fission\n2013\n\n";

    fsn::ResourceManager::init();

    fsn::ComponentTypeManager::add<fsn::TransformComponent>();
    fsn::ComponentTypeManager::add<fsn::SpriteComponent>();
    fsn::ComponentTypeManager::add<fsn::IntentComponent>();
    fsn::ComponentTypeManager::add<PhysicsComponent>();
    fsn::ComponentTypeManager::add<EmitterComponent>();

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

    auto player = entityMgr->createEntityRef(entityMgr->createEntity());
    player->addComponent(new fsn::TransformComponent(sf::Vector2f(50, 50)));
    player->addComponent(new fsn::SpriteComponent("Content/Textures/Tiles/dirt.png", 1, 1));

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
    Sqrat::DerivedClass<fsn::TransformComponent, fsn::Component,
        sqext::ConstAlloc<fsn::TransformComponent, sf::Vector2f, float, sf::Vector2f>> transform(vm);
    transform.Func("setPosition", (void (fsn::TransformComponent::*)(const sf::Vector2f&))&fsn::TransformComponent::setPosition);
    transform.Func("getPosition", &fsn::TransformComponent::getPosition);
    transform.Func("move", (void (fsn::TransformComponent::*)(const sf::Vector2f&))&fsn::TransformComponent::move);
    transform.Func("setOrigin", (void (fsn::TransformComponent::*)(const sf::Vector2f&))&fsn::TransformComponent::setOrigin);
    Sqrat::RootTable(vm).Bind("fsn::TransformComponent", transform);

    Sqrat::DerivedClass<fsn::RenderComponent, fsn::Component> renderComp(vm);
    renderComp.Func("setLit", &fsn::RenderComponent::setLit);
    renderComp.Func("getLit", &fsn::RenderComponent::getLit);
    renderComp.Func("setLayer", &fsn::RenderComponent::setLayer);
    renderComp.Func("getLayer", &fsn::RenderComponent::getLayer);
    Sqrat::RootTable(vm).Bind("RenderComponent", renderComp);

    Sqrat::DerivedClass<fsn::SpriteComponent, fsn::RenderComponent,
        sqext::ConstAlloc<fsn::SpriteComponent, const std::string&, int, int>> sprite(vm);
    //sprite.Func("setTexture", &fsn::SpriteComponent::setTexture);
    sprite.Func("setFrameLoop", &fsn::SpriteComponent::setFrameLoop);
    sprite.Func("setFrameDir", &fsn::SpriteComponent::setFrameDir);
    sprite.Func("setLoopAnim", &fsn::SpriteComponent::setLoopAnim);
    Sqrat::RootTable(vm).Bind("SpriteComponent", sprite);

    Sqrat::DerivedClass<fsn::IntentComponent, fsn::Component, sqext::ConstAlloc<fsn::IntentComponent>> intent(vm);
    intent.Func("mapKeyToIntent", &fsn::IntentComponent::mapKeyToIntent);
    intent.Func("mapMouseBtnToEvent", &fsn::IntentComponent::mapMouseBtnToIntent);
    intent.Func("isIntentActive", &fsn::IntentComponent::isIntentActive);
    intent.Func("getMousePos", &fsn::IntentComponent::getMousePos);
    Sqrat::RootTable(vm).Bind("IntentComponent", intent);

    Sqrat::Class<phys::RigidBody, sqext::ConstAlloc<phys::RigidBody, phys::Shape*, int, float>> rigidbody(vm);
    rigidbody.Func("applyForce", &phys::RigidBody::applyForce);

    rigidbody.Func("setVelocity", &phys::RigidBody::setVelocity);
    rigidbody.Func("setVelocityX", &phys::RigidBody::setVelocityX);
    rigidbody.Func("setVelocityY", &phys::RigidBody::setVelocityY);
    rigidbody.Func("getVelocity", &phys::RigidBody::getVelocity);
    Sqrat::RootTable(vm).Bind("RigidBody", rigidbody);

    Sqrat::DerivedClass<PhysicsComponent, fsn::Component, sqext::ConstAlloc<PhysicsComponent, float, float, float>> physics(vm);
    physics.Func("getBody", &PhysicsComponent::getBody);
    Sqrat::RootTable(vm).Bind("PhysicsComponent", physics);

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
    Sqrat::RootTable(vm).SetValue("PhysicsComponentType", PhysicsComponent::Type());

    // All the component casting functions
    Sqrat::RootTable(vm).Func("castPhysicsComponent", componentCast<PhysicsComponent>);
}
