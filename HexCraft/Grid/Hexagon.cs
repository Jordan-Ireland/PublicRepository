using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public enum HexagonTypes { Forest, Quarry, Water, Blank, Crafting, Garden, None };
public class Hexagon : MonoBehaviour
{
    public List<WorldResource> resources = new List<WorldResource>();
    public List<Enemy> enemies = new List<Enemy>();
    public Vector2 myXY;
    public HexagonTypes type;
    public int typeIndex;

    List<Renderer> rends;
    public Vector3 GetPosition { get { return transform.position; } }

    public void Init(int x, int y, int index, HexagonTypes type)
    {
        myXY.x = x;
        myXY.y = y;
        typeIndex = index;
        this.type = type;
        WorldResource[] wrs = GetComponentsInChildren<WorldResource>();
        for (int i = 0; i < wrs.Length; i++)
        {
            this.resources.Add(wrs[i]);
        }
        AddHexagon();
    }

    public void Init(int x, int y, int index, float yPos, HexagonTypes type, ResourceSave[] resources, EnemySave[] enemies)
    {
        myXY.x = x;
        myXY.y = y;
        transform.position = new Vector3(transform.position.x, yPos, transform.position.z);
        typeIndex = index;
        this.type = type;
        WorldResource[] wrs = GetComponentsInChildren<WorldResource>();
        for (int i = 0; i < resources.Length; i++)
        {
            wrs[i].Init(resources[i].dead, resources[i].time, this);
            this.resources.Add(wrs[i]);
        }

        for (int i = 0; i < enemies.Length; i++)
        {
            EnemySave e = enemies[i];
            SpawnEnemy(e.enemyId, new Vector3(e.x, e.y, e.z));
            //Enemy enemy = SpawnHelper(e.enemyId);
            //enemy.Init(new Vector3(e.x, e.y, e.z), GameManager.enemies.Count - 1, this);
        }
        AddHexagon();
    }

    void AddHexagon()
    {
        if (type != HexagonTypes.Water &&
            type != HexagonTypes.Crafting)
        {
            GameManager.hexagons.Add(this);
        }
    }

    public void SpawnEnemy(int id, Vector3 position)
    {
        Enemy enemy = SpawnHelper(id);
        enemy.Init(position, GameManager.enemies.Count - 1, this);
    }

    public void SpawnEnemy(int id)
    {
        Enemy enemy = SpawnHelper(id);
        enemy.Init(GameManager.enemies.Count - 1, this);
    }

    Enemy SpawnHelper(int id)
    {
        Enemy enemy = Instantiate(GameManager.npcDatabase.GetById(id), transform).GetComponent<Enemy>();
        enemies.Add(enemy);
        GameManager.enemies.Add(enemy);
        return enemy;
    }

    // Start is called before the first frame update
    void Start()
    {
    }

    // Update is called once per frame
    void Update()
    {
    }
}
