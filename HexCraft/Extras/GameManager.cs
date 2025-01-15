using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;

public class GameManager : MonoBehaviour
{
    //Databases
    public static GameManager instance;
    public static ItemDatabase itemDatabase;
    public static HexagonDatabase hexagonDatabase;
    public static NPCDatabase npcDatabase;
    public static Player playerInstance;
    public static RecipeDatabase recipeDatabase;

    //Game Manager
    public enum GameState { Exploring, Building, MovingToExplore, MovingToBuild }
    public static GameState gameState = GameState.Exploring;
    public static List<Enemy> enemies = new List<Enemy>();
    public static float TimeOfDay = 0;
    public static int Day = 1;
    public static List<Hexagon> hexagons = new List<Hexagon>();
    public static WindController windContoller;

    //other vars
    public GameObject playerPrefab;
    public GameObject[] miningParticleEffects = new GameObject[0];
    public float timeSpeed = 5;
    public Material growingMaterial;
    public RecipeBook recipes = null;
    public RecipeBook standardRecipes;
    public LayerMask enemyWalkable;
    public LayerMask enemyIgnore;

    public static bool TutorialComplete = false;

    bool spawnEnemies = true;

    public static string Version = "0.4a";


    private void Awake()
    {
        instance = this;
        itemDatabase = Resources.Load("Databases/ItemDatabase") as ItemDatabase;
        hexagonDatabase = Resources.Load("Databases/HexagonDatabase") as HexagonDatabase;
        npcDatabase = Resources.Load("Databases/NPCDatabase") as NPCDatabase;
        recipeDatabase = Resources.Load("Databases/RecipeDatabase") as RecipeDatabase;
        recipes = null;

        standardRecipes = recipeDatabase.GetById(0);
    }

    // Start is called before the first frame update
    void Start()
    {
        //TODO Remove this when options are created
        //Screen.fullScreenMode = FullScreenMode.ExclusiveFullScreen;
        Cursor.lockState = CursorLockMode.Locked;
        Cursor.visible = false;
        if (Load.CheckForSave())
        {
            Grid.GridInstance.CreateWorld();
            Load.LoadGame();
            spawnEnemies = false;
            TutorialComplete = true;
            //Debug.Log("Loading Game")
            UIManager.instance.healthExpBarUI.UpdateUI();
        }
        else
        {
            //Debug.Log("No World Data");
            Grid.GridInstance.CreateWorld();
            spawnEnemies = true;
            TimeOfDay = 50;
            Day = 1;

            Grid.AddHexagon(HexagonTypes.Forest, 10, 10);
            Grid.AddHexagon(HexagonTypes.Quarry, 9, 11);
            Grid.AddHexagon(HexagonTypes.Garden, 10, 11);
            TutorialComplete = false;
            GameObject player = Instantiate(GameManager.instance.playerPrefab, new Vector3(0,3,0), Quaternion.identity);
        }
        if(TutorialComplete)
        {
            UIManager.instance.TutorialPanel.SetActive(false);
        }else
        {
            UIManager.instance.TutorialPanel.SetActive(true);
        }

        //Grid.grid[10, 10].SpawnEnemy(0);
    }

    float sunIntensity = 1;
    // Update is called once per frame
    void Update()
    {
        TimeOfDay += Time.deltaTime / timeSpeed;
        if (TimeOfDay >= 180 && TimeOfDay < 360)
        {
            if (spawnEnemies)
            {
                spawnEnemies = false;
                //night time
                for (int i = 0; i < hexagons.Count; i++)
                {
                    bool boss = true;
                    Enemy enemy;
                    int enemyCount = Random.Range(0, 4);
                    for (int x = 0; x < enemyCount; x++)
                    {
                        if (hexagons[i].enemies.Count < 4)
                        {
                            float bossPercent = Random.Range(0f, 1f);
                            if (bossPercent >= 0.95f && boss)
                            {
                                enemy = npcDatabase.GetBossEnemy().GetComponent<Enemy>();
                                boss = false;
                            }
                            else
                            {
                                enemy = npcDatabase.GetRandomEnemy().GetComponent<Enemy>();
                            }
                            hexagons[i].SpawnEnemy(enemy.enemy.id);
                        }
                        else
                        {
                            break;
                        }
                    }
                }
            }
        }
        else if (TimeOfDay >= 360)
        {
            //new day
            Day++;
            TimeOfDay = 0;
            spawnEnemies = true;
        }
        if (Input.GetKeyDown(InputManager.BuildingCamera) && !UIManager.ShowingMenu())
        {
            switch (gameState)
            {
                case GameState.Exploring:
                    gameState = GameState.MovingToBuild;
                    Cursor.lockState = CursorLockMode.Confined;
                    Cursor.visible = true;
                    break;
                case GameState.Building:
                    gameState = GameState.MovingToExplore;
                    Cursor.lockState = CursorLockMode.Locked;
                    Cursor.visible = false;
                    break;
            }
        }

        if (gameState != GameState.Building)
        {
            ChangeCursor();
        }
        else if (gameState == GameState.Building)
        {
            Cursor.lockState = CursorLockMode.None;
            Cursor.visible = true;
        }
        
    }

    void ChangeCursor()
    {
        if (UIManager.ShowingMenu())
        {
            Cursor.lockState = CursorLockMode.None;
            Cursor.visible = true;
        }
        else
        {
            Cursor.lockState = CursorLockMode.Locked;
            Cursor.visible = false;
        }
    }
}
