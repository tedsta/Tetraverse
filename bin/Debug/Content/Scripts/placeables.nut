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
    on = true;

    constructor(_entity, _grid, _x, _y)
    {
        base.constructor(_entity, _grid, _x, _y);
    }

    function update(dt)
    {
        local light = castLightComponent(entity.getComponent(LightComponentType));
        local signal = castSignalComponent(entity.getComponent(SignalComponentType));
        if (signal.hasSignal())
        {
            local sig = signal.getInt();
            if (sig == 0)
                on = false;
            else if (sig == 1)
                on = true;
            light.setActive(on);
            signal.fireInt(sig);
        }
    }

    function interact()
    {
        local gridComp = castGridComponent(grid.getComponent(GridComponentType));
        local signal = castSignalComponent(entity.getComponent(SignalComponentType));

        on = !on;
        if (on)
            signal.fireInt(1);
        else
            signal.fireInt(0);
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
            physics.getBody().applyForce(Vector2f(0, -4000));
        if (intent.isIntentActive("down"))
            physics.getBody().applyForce(Vector2f(0, 4000));
        if (intent.isIntentActive("left"))
            physics.getBody().applyForce(Vector2f(-2000, 0));
        if (intent.isIntentActive("right"))
            physics.getBody().applyForce(Vector2f(2000, 0));
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
            signal.fireInt(1);
        else
            signal.fireInt(0);
    }
};
