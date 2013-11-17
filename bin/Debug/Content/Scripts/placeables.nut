class Door
{
    grid = null;
    x = 0;
    y = 0;

    constructor(_grid, _x, _y)
    {
        grid = _grid;
        x = _x;
        y = _y;
    }

    function update(dt)
    {
        //print(dt);
    }

    function interact()
    {
        local tile = grid.getTile(x, y);
        tile.mWire = 128;
        tile.mSignal = 128;
        grid.setTile(x, y, tile, -1);
    }
};
