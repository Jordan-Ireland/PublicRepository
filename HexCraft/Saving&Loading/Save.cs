using System.Collections.Generic;
using UnityEngine;
using Newtonsoft.Json;
using System;

public static class Save
{
    static SaveFile saveFile;
    static PlayerSettingsFile playerSettingsFile;
    const string keyWord = "07251994";

    public static void SaveGame()
    {
        saveFile = new SaveFile();
        SaveWorld(ref saveFile);
        Player player = GameManager.playerInstance;
        saveFile.player = new PlayerSave(player.transform.position, player.transform.eulerAngles.y, player.Level, player.CurrentExp, player.CurrentHealth, player.inventory.inventory);

        string json = JsonConvert.SerializeObject(saveFile);
        json = EncryptDecrypt(json);

        //Debug.Log(json);
        System.IO.File.WriteAllText(Application.persistentDataPath + "/save.afg", json);
    }

    static void SaveWorld(ref SaveFile saveFile)
    {
        for (int y = 0; y < Grid.grid.GetLength(0); y++)
        {
            for (int x = 0; x < Grid.grid.GetLength(1); x++)
            {
                if (Grid.grid[x, y].type != HexagonTypes.Water)
                {
                    Hexagon hexagon = Grid.grid[x, y];
                    List<ResourceSave> resSave = new List<ResourceSave>();
                    for (int i = 0; i < hexagon.resources.Count; i++)
                    {
                        WorldResource r = hexagon.resources[i];
                        
                        resSave.Add(new ResourceSave(r.IsDead, r.TimeSinceDead));
                    }
                    List<EnemySave> eSave = new List<EnemySave>();
                    for (int i = 0; i < hexagon.enemies.Count; i++)
                    {
                        Enemy e = hexagon.enemies[i];
                        eSave.Add(new EnemySave(e.transform.localPosition, e.enemy.id));
                    }
                    HexSave hex = new HexSave(hexagon.myXY, hexagon.transform.position.y, (int)hexagon.type, hexagon.typeIndex, resSave, eSave);
                    saveFile.hexagons.Add(hex);
                }
            }
        }
        saveFile.TimeOfDay = GameManager.TimeOfDay;
        saveFile.Day = GameManager.Day;
    }

    public static void SavePlayerSettings(bool loaded = true)
    {
        playerSettingsFile = new PlayerSettingsFile(loaded);


        string json = JsonConvert.SerializeObject(playerSettingsFile);

        //Debug.Log(json);
        System.IO.File.WriteAllText(Application.persistentDataPath + "/settings.afg", json);
    }

    public static string EncryptDecrypt(string data)
    {
        string result = "";
        for (int i = 0; i < data.Length; i++)
        {
            result += (char)(data[i] ^ keyWord[i % keyWord.Length]);
        }
        return result;
    }
}

[System.Serializable]
public class PlayerSettingsFile
{
    public InputsSave inputs;
    public Vector3 menuPosition;
    public Vector3 craftingPosition;
    public Vector3 inventoryPosition;
    public Vector3 hexagonCraftingPosition;
    public int graphics;
    public int screenType;

    public PlayerSettingsFile(bool loaded = true)
    {
        inputs = new InputsSave();
        if (loaded)
        {
            menuPosition = UIManager.instance.Menu.GetComponent<RectTransform>().position;
            craftingPosition = UIManager.instance.CraftingMenu.GetComponent<RectTransform>().position;
            inventoryPosition = UIManager.instance.InventoryMenu.GetComponent<RectTransform>().position;
            hexagonCraftingPosition = UIManager.instance.HexagonCraftingMenu.GetComponent<RectTransform>().position;
        }else
        {
            menuPosition = new Vector3(500, 282, 0);
            craftingPosition = new Vector3(500, 282, 0);
            inventoryPosition = new Vector3(500, 282, 0);
            hexagonCraftingPosition = new Vector3(500, 282, 0);
        }
        graphics = QualitySettings.GetQualityLevel();
        screenType = (int)Screen.fullScreenMode;
    }

    public void LoadMenuPositions()
    {
        UIManager.instance.Menu.GetComponent<RectTransform>().position = menuPosition;
        UIManager.instance.CraftingMenu.GetComponent<RectTransform>().position = craftingPosition;
        UIManager.instance.InventoryMenu.GetComponent<RectTransform>().position = inventoryPosition;
        UIManager.instance.HexagonCraftingMenu.GetComponent<RectTransform>().position = hexagonCraftingPosition;
    }

    public void LoadSettings()
    {
        QualitySettings.SetQualityLevel(graphics);
        Screen.fullScreenMode = (FullScreenMode)screenType;
    }
}

[System.Serializable]
public class SaveFile
{
    public List<HexSave> hexagons = new List<HexSave>();
    public float TimeOfDay;
    public int Day;
    public PlayerSave player;
}

[System.Serializable]
public class PlayerSave
{
    public int level;
    public int curExp;
    public int curHealth;
    public Vector3 position;
    public float yRot;
    public InventoryItem[] inventory;

    public PlayerSave(Vector3 position, float yRot, int level, int curExp, int curHealth, InventoryItem[] inventory)
    {
        this.position = position;
        this.yRot = yRot;
        this.level = level;
        this.curExp = curExp;
        this.curHealth = curHealth;
        this.inventory = inventory;
    }
}

[System.Serializable]
public class HexSave
{
    public int x;
    public int y;
    public float yPos;
    public int type;
    public int index;
    public List<ResourceSave> resources = new List<ResourceSave>();
    public List<EnemySave> enemies = new List<EnemySave>();

    public HexSave(Vector2 xy, float yPos, int type, int index, List<ResourceSave> resources, List<EnemySave> enemies)
    {
        this.resources = resources;
        this.x = (int)xy.x;
        this.y = (int)xy.y;
        this.yPos = yPos;
        this.type = type;
        this.index = index;
        this.enemies = enemies;
    }
}

[System.Serializable]
public class EnemySave
{
    public float x, y, z;
    public int enemyId;

    public EnemySave(Vector3 pos, int id)
    {
        x = pos.x;
        y = pos.y;
        z = pos.z;
        enemyId = id;
    }
}

[System.Serializable]
public class ResourceSave
{
    public bool dead;
    public float time;

    public ResourceSave(bool dead, float time)
    {
        this.dead = dead;
        this.time = time;
    }
}

public class InputsSave
{
    //Movement  Controls
    public string Left;
    public string Right;
    public string Up;
    public string Down;
    public string Jump;

    //Menu Controls
    public string BuildingCamera;
    public string Inventory;
    public string Menu;
    public string CraftingMenu;

    public float MouseXSpeed;
    public float MouseYSpeed;

    public InputsSave()
    {
        Left = InputManager.Left.ToString();
        Right = InputManager.Right.ToString();
        Up = InputManager.Up.ToString();
        Down = InputManager.Down.ToString();
        Jump = InputManager.Jump.ToString();

        BuildingCamera = InputManager.BuildingCamera.ToString();
        Inventory = InputManager.Inventory.ToString();
        Menu = InputManager.Menu.ToString();
        CraftingMenu = InputManager.CraftingMenu.ToString();

        MouseXSpeed = InputManager.MouseXSpeed;
        MouseYSpeed = InputManager.MouseYSpeed;
    }

    public void LoadInputs(string left, string right, string up, string down, string jump, string bc, string inv, string menu, string cm, float xSpeed, float ySpeed)
    {
        Debug.Log("Loading Keys");
        InputManager.Left = (KeyCode)Enum.Parse(typeof(KeyCode), left);
        InputManager.Right = (KeyCode)Enum.Parse(typeof(KeyCode), right);
        InputManager.Up = (KeyCode)Enum.Parse(typeof(KeyCode), up);
        InputManager.Down = (KeyCode)Enum.Parse(typeof(KeyCode), down);
        InputManager.Jump = (KeyCode)Enum.Parse(typeof(KeyCode), jump);

        InputManager.BuildingCamera = (KeyCode)Enum.Parse(typeof(KeyCode), bc);
        InputManager.Inventory = (KeyCode)Enum.Parse(typeof(KeyCode), inv);
        InputManager.Menu = (KeyCode)Enum.Parse(typeof(KeyCode), menu);
        InputManager.CraftingMenu = (KeyCode)Enum.Parse(typeof(KeyCode), cm);

        InputManager.MouseXSpeed = xSpeed;
        InputManager.MouseYSpeed = ySpeed;
    }

    public void LoadInputs()
    {
        //Debug.Log("Loading Keys");
        InputManager.Left = (KeyCode)Enum.Parse(typeof(KeyCode), Left);
        InputManager.Right = (KeyCode)Enum.Parse(typeof(KeyCode), Right);
        InputManager.Up = (KeyCode)Enum.Parse(typeof(KeyCode), Up);
        InputManager.Down = (KeyCode)Enum.Parse(typeof(KeyCode), Down);
        InputManager.Jump = (KeyCode)Enum.Parse(typeof(KeyCode), Jump);

        InputManager.BuildingCamera = (KeyCode)Enum.Parse(typeof(KeyCode), BuildingCamera);
        InputManager.Inventory = (KeyCode)Enum.Parse(typeof(KeyCode), Inventory);
        InputManager.Menu = (KeyCode)Enum.Parse(typeof(KeyCode), Menu);
        InputManager.CraftingMenu = (KeyCode)Enum.Parse(typeof(KeyCode), CraftingMenu);

        InputManager.MouseXSpeed = MouseXSpeed;
        InputManager.MouseYSpeed = MouseYSpeed;
    }
}
