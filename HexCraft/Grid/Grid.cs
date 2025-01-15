using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;

public class Grid : MonoBehaviour
{
    //HexagonGrid
    public static Grid GridInstance;
    public static Hexagon[,] grid = new Hexagon[20,20];
    public float hexagonScale;
    float hexWidth = 17.3f; //17.3
    float hexHeight = 20;   //20
    [Range(0, 5)] public float gap = 0.0f;
    Vector3 startPos;

    private void Awake()
    {
        GridInstance = this;
    }

    public void CreateWorld()
    {
        hexWidth *= hexagonScale;
        hexHeight *= hexagonScale;

        AddGap();
        CalcStartPos();
        CreateGrid();
    }

    // Start is called before the first frame update
    void Start()
    {
    }

    void AddGap()
    {
        hexWidth += hexWidth * gap;
        hexHeight += hexHeight * gap;
    }

    void CalcStartPos()
    {
        float offset = 0;
        if (grid.GetLength(0) / 2 % 2 != 0)
            offset = hexWidth / 2;

        float x = -hexWidth * (grid.GetLength(1) / 2) - offset;
        float z = hexHeight * 0.75f * (grid.GetLength(0) / 2);

        startPos = new Vector3(x, 0, z);
    }

    public static Vector3 CalcWorldPosNew(Vector2 gridPos)
    {
        float offset = 0;
        if (gridPos.y % 2 != 0)
            offset = GridInstance.hexWidth / 2;

        float x = GridInstance.startPos.x + gridPos.x * GridInstance.hexWidth + offset;
        float z = GridInstance.startPos.z - gridPos.y * GridInstance.hexHeight * 0.75f;

        return new Vector3(x, GetRandomY((int)gridPos.x, (int)gridPos.y), z);
    }
    public static Vector3 CalcWorldPosSaved(Vector2 gridPos)
    {
        float offset = 0;
        if (gridPos.y % 2 != 0)
            offset = GridInstance.hexWidth / 2;

        float x = GridInstance.startPos.x + gridPos.x * GridInstance.hexWidth + offset;
        float z = GridInstance.startPos.z - gridPos.y * GridInstance.hexHeight * 0.75f;

        return new Vector3(x, 0, z);
    }
    public static Vector3 CalcWaterPosition(Vector2 gridPos)
    {
        float offset = 0;
        if (gridPos.y % 2 != 0)
            offset = GridInstance.hexWidth / 2;

        float x = GridInstance.startPos.x + gridPos.x * GridInstance.hexWidth + offset;
        float z = GridInstance.startPos.z - gridPos.y * GridInstance.hexHeight * 0.75f;

        return new Vector3(x, -10, z);
    }

    static float GetRandomY(int x, int y)
    {
        if (grid[x, y].type == HexagonTypes.Water) return 0;
        (float, float) minMaxY = GetMaxY(x,y);
        if (minMaxY.Item1 <= 0) minMaxY.Item1 = 1;
        return Random.Range((int)minMaxY.Item1 - 1, (int)minMaxY.Item2 + 2);        
    }
    static (float, float) GetMaxY(int gridX, int gridY)
    {
        float maxY = 0;
        float minY = 100;
        //if even,even or odd,even
        if ((gridX % 2 == 0 && gridY % 2 == 0) ||
               (gridX % 2 != 0 && gridY % 2 == 0))
        {
            int[,] arr = new int[6,2] { { -1, 1 }, { -1, 0 }, { -1, -1 }, { 0, -1 }, { 1, 0 }, { 0, 1 } };
            for (int x = 0; x < arr.GetLength(0); x++)
            {
                float yPos = grid[gridX + arr[x, 0], gridY + arr[x, 1]].transform.position.y;
                if (yPos > maxY) maxY = yPos;
                if (yPos < minY) minY = yPos;
            }
        }
        //if even,odd or odd,odd
        if ((gridX % 2 == 0 && gridY % 2 != 0) ||
              (gridX % 2 != 0 && gridY % 2 != 0))
        {
            int[,] arr = new int[6, 2] { { 0, 1 }, { -1, 0 }, { 0, -1 }, { 1, -1 }, { 1, 0 }, { 1, 1 } };
            for (int x = 0; x < arr.GetLength(0); x++)
            {
                float yPos = grid[gridX + arr[x, 0], gridY + arr[x, 1]].transform.position.y;
                if (yPos > maxY) maxY = yPos;
                if (yPos < minY) minY = yPos;
            }
        }
        return (minY, maxY);
    }

    void CreateGrid()
    {
        
         //height
        for (int y = 0; y < grid.GetLength(0); y++)
        {
            //width
            for (int x = 0; x < grid.GetLength(1); x++)
            {
                AddHexagon(HexagonTypes.Water, x, y);
            }
        }
        
        //AddHexagon(HexagonTypes.Forest, grid.GetLength(0) / 2, grid.GetLength(1) / 2);
    }

    public static void AddHexagon(HexagonTypes type, int x, int y)
    {
        if (grid[x, y] != null)
        {
            Destroy(grid[x, y].gameObject);
        }

        (HexagonVariation, int) hex = GameManager.hexagonDatabase.GetHexagon(type).GetRandomHexagon();
        GameObject go = Instantiate(hex.Item1.hexagonPrefab);
        Vector2 gridPos = new Vector2(x, y);
        grid[x, y] = go.GetComponent<Hexagon>();
        if (type != HexagonTypes.Water)
            go.transform.position = CalcWorldPosNew(gridPos);
        else
            go.transform.position = CalcWaterPosition(gridPos);
        go.GetComponent<Hexagon>().Init(x, y, hex.Item2, type);
        go.transform.parent = GridInstance.transform;
        go.name = "Hexagon " + x + " | " + y;
    }
    public static void AddHexagon(HexagonTypes type, int index, int x, int y)
    {
        if (grid[x, y] != null)
        {
            Destroy(grid[x, y].gameObject);
        }

        //(HexagonVariation, int) hex = GameManager.hexagonDatabase.GetHexagon(type).GetRandomHexagon();
        HexagonVariation hex = GameManager.hexagonDatabase.GetHexagon(type).hexagonVariations[index];
        GameObject go = Instantiate(hex.hexagonPrefab);
        Vector2 gridPos = new Vector2(x, y);
        grid[x, y] = go.GetComponent<Hexagon>();
        if (type != HexagonTypes.Water)
            go.transform.position = CalcWorldPosNew(gridPos);
        else
            go.transform.position = CalcWaterPosition(gridPos);
        go.GetComponent<Hexagon>().Init(x, y, index, type);
        go.transform.parent = GridInstance.transform;
        go.name = "Hexagon " + x + " | " + y;
    }

    public static void AddHexagon(HexagonTypes type, int x, int y, float yPos, int index, ResourceSave[] resources, EnemySave[] enemies)
    {
        if (grid[x, y] != null)
        {
            Destroy(grid[x, y].gameObject);
        }

        GameObject go = Instantiate(GameManager.hexagonDatabase.GetHexagon(type).hexagonVariations[index].hexagonPrefab);
        Vector2 gridPos = new Vector2(x, y);
        go.transform.position = CalcWorldPosSaved(gridPos);
        grid[x, y] = go.GetComponent<Hexagon>();
        go.GetComponent<Hexagon>().Init(x, y, index, yPos, type, resources, enemies);
        go.transform.parent = GridInstance.transform;
        go.name = "Hexagon " + x + " | " + y;
    }

    public static bool CheckIfConnected(int x, int y)
    {
        //+1,0/+1+1/0+1/-1,0/0,-1/+1,-1
        if (x % 2 != 0 && y % 2 != 0)
        {

            if (CheckIfExists(x + 1, y) |
                CheckIfExists(x + 1, y + 1) |
                CheckIfExists(x, y + 1) |
                CheckIfExists(x - 1, y) |
                CheckIfExists(x, y - 1) |
                CheckIfExists(x + 1, y - 1)) {
                return true;
            }
        }
        else
        {
            //+1,0/0,+1/-1,+1/-1,0/-1,-1/0,-1
            if (CheckIfExists(x + 1, y) |
                CheckIfExists(x, y + 1) |
                CheckIfExists(x - 1, y + 1) |
                CheckIfExists(x - 1, y) |
                CheckIfExists(x - 1, y - 1) |
                CheckIfExists(x, y - 1)) {
                return true;
            }

        }

        return false;
    }

    static bool CheckIfExists(int x, int y)
    {
        if (x < grid.GetLength(1) && 
            y < grid.GetLength(0) &&
            x > 0 &&
            y > 0)
        {
            if (grid[x, y].type != HexagonTypes.Water)
                return true;
        }
        return false;
    }
}

public class SquareWorldCell
{
    public int x, y;
}
