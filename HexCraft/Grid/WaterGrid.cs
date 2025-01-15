using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class WaterGrid : MonoBehaviour
{
    public static GameObject[,] grid = new GameObject[41, 41];

    public int x = 41, y = 41;
    public GameObject waterPrefab;
    public float scale = 2;

    float hexWidth = 17.3f; //17.3
    float hexHeight = 20;   //20
    [Range(0, 5)] public float gap = 0.0f;
    public int[] offsets = new int[6] { 0, 9, 40, 30, 10, 29 };
    public bool refresh = false;

    Vector3 startPos;

    // Start is called before the first frame update
    void Start()
    {
        grid = new GameObject[x, y];
        hexWidth *= scale;
        hexHeight *= scale;

        AddGap();
        CalcStartPos();
        CreateGrid();
    }

    private void FixedUpdate()
    {
        if(refresh)
        {
            for (int y = 0; y < grid.GetLength(0); y++)
            {
                for (int x = 0; x < grid.GetLength(1); x++)
                {
                    Destroy(grid[x, y]);
                }
            }
            CreateGrid();
            refresh = false;
        }
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

    public Vector3 CalcWorldPos(Vector2 gridPos)
    {
        float offset = 0;
        if (gridPos.y % 2 != 0)
            offset = hexWidth / 2;

        float x = startPos.x + gridPos.x * hexWidth + offset;
        float z = startPos.z - gridPos.y * hexHeight * 0.75f;

        return new Vector3(x, 0, z);
    }

    void CreateGrid()
    {
        //height
        for (int y = 0; y < grid.GetLength(0); y++)
        {
            //width
            for (int x = 0; x < grid.GetLength(1); x++)
            {
                //y 0-9 | x 0-40
                //x 30-40 | y 10-29
                //y 30-40 | x 0-40
                //x 0-9 | y 10-29
                if ((y >= offsets[0] && y <= offsets[1] && x >= offsets[0] && x <= offsets[2]) ||
                    (x >= offsets[3] && x <= offsets[2] && y >= offsets[4] && y <= offsets[5]) ||
                    (y >= offsets[3] && y <= offsets[2] && x >= offsets[0] && x <= offsets[2]) || 
                    (x >= offsets[0] && x <= offsets[1] && y >= offsets[4] && y <= offsets[5]))
                {
                    AddHexagon(HexagonTypes.Water, x, y);
                }
            }
        }
    }

    public void AddHexagon(HexagonTypes type, int x, int y)
    {
        if (grid[x, y] != null)
        {
            Destroy(grid[x, y].gameObject);
        }

        GameObject go = Instantiate(waterPrefab);
        Vector2 gridPos = new Vector2(x, y);
        go.transform.position = CalcWorldPos(gridPos);
        grid[x, y] = go;
        go.transform.parent = transform;
        go.name = "Water Hexagon " + x + " | " + y;
    }
}
