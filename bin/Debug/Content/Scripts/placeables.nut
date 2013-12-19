class Placeable
{
    grid = null;
    entity = null;
    x = 0;
    y = 0;

    constructor(_entity, _grid, _x, _y)
    {
        entity = _entity;
        grid = _grid;
        x = _x;
        y = _y;
    }

    function setGrid(_grid)
    {
        grid = _grid;
    }

    function setGridPos(_x, _y)
    {
        x = _x;
        y = _y;
    }
};

class Door extends Placeable
{
    open = false;

    constructor(_entity, _grid, _x, _y)
    {
        base.constructor(_entity, _grid, _x, _y);

        castSpriteComponent(entity.getComponent(SpriteComponentType)).setFrameLoop(0, 0);
        castSpriteComponent(entity.getComponent(SpriteComponentType)).setLoopAnim(false);
    }

    function update(dt)
    {
        local light = castLightComponent(entity.getComponent(LightComponentType));
        local signal = castSignalComponent(entity.getComponent(SignalComponentType));
        if (signal.hasSignal())
        {
            local sig = signal.getInt();
            if (sig == 0)
            {
                open = false;
                castSpriteComponent(entity.getComponent(SpriteComponentType)).setFrameLoop(4, 0);
                castSpriteComponent(entity.getComponent(SpriteComponentType)).setLoopAnim(false);
            }
            else if (sig == 1)
            {
                open = true;
                castSpriteComponent(entity.getComponent(SpriteComponentType)).setFrameLoop(0, 4);
                castSpriteComponent(entity.getComponent(SpriteComponentType)).setLoopAnim(false);
            }

            light.setActive(on);
            signal.fireInt(sig);
        }
    }

    function interact()
    {
        local gridComp = castGridComponent(grid.getComponent(GridComponentType));

        open = !open;
        if (!open)
        {
            open = false;
            castSpriteComponent(entity.getComponent(SpriteComponentType)).setFrameLoop(4, 0);
            castSpriteComponent(entity.getComponent(SpriteComponentType)).setLoopAnim(false);
        }
        else
        {
            open = true;
            castSpriteComponent(entity.getComponent(SpriteComponentType)).setFrameLoop(0, 4);
            castSpriteComponent(entity.getComponent(SpriteComponentType)).setLoopAnim(false);
        }
    }
};

class Thruster extends Placeable
{
    constructor(_entity, _grid, _x, _y)
    {
        base.constructor(_entity, _grid, _x, _y);
    }

    function update(dt)
    {
        local gridComp = castGridComponent(grid.getComponent(GridComponentType));
        local physics = castPhysicsComponent(grid.getComponent(PhysicsComponentType));
        local intent = castIntentComponent(entity.getComponent(IntentComponentType));
        local mousePos = intent.getMousePos();

        if (intent.isIntentActive("up"))
            physics.getBody().applyForce(Vector2f(0, -100000));
        if (intent.isIntentActive("down"))
            physics.getBody().applyForce(Vector2f(0, 100000));
        if (intent.isIntentActive("left"))
            physics.getBody().applyForce(Vector2f(-100000, 0));
        if (intent.isIntentActive("right"))
            physics.getBody().applyForce(Vector2f(100000, 0));
    }

    function interact()
    {
    }
};

class Light extends Placeable
{
    constructor(_entity, _grid, _x, _y)
    {
        base.constructor(_entity, _grid, _x, _y);
    }

    function update(dt)
    {
        local light = castLightComponent(entity.getComponent(LightComponentType));
    }

    function interact()
    {
    }
};

class Swtch extends Placeable
{
    on = false;

    constructor(_entity, _grid, _x, _y)
    {
        base.constructor(_entity, _grid, _x, _y);

        castSpriteComponent(entity.getComponent(SpriteComponentType)).setFrameLoop(1, 1);
    }

    function update(dt)
    {
        local light = castLightComponent(entity.getComponent(LightComponentType));
        local signal = castSignalComponent(entity.getComponent(SignalComponentType));

    }

    function interact()
    {
        local gridComp = castGridComponent(grid.getComponent(GridComponentType));
        local signal = castSignalComponent(entity.getComponent(SignalComponentType));

        on = !on;
        if (on)
        {
            castSpriteComponent(entity.getComponent(SpriteComponentType)).setFrameLoop(0, 0);
            signal.fireInt(1);
        }
        else
        {
            castSpriteComponent(entity.getComponent(SpriteComponentType)).setFrameLoop(1, 1);
            signal.fireInt(0);
        }
    }
};
