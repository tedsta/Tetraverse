function dirtBlock(grid, x, y)
{
	local dirt = Tile();
	dirt.mMat = 1;
	grid.setTile(x, y, dirt, -1);
}