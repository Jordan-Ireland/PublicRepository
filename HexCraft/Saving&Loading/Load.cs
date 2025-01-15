using UnityEngine;
using Newtonsoft.Json;

public static class Load
{
    static SaveFile saveFile;
    static PlayerSettingsFile playerSettingsFile;
    static void DeleteWorld()
    {
        var obj = new GameObject("Destroy helper");
        for (int i = 0; i < GameManager.hexagons.Count; i++)
        {
            UnityEngine.Object.Destroy(GameManager.hexagons[i]);
        }
        UnityEngine.Object.Destroy(obj);
    }

    public static void LoadGame()
    {
        DeleteWorld();
        string json = System.IO.File.ReadAllText(Application.persistentDataPath + "/save.afg");
        json = Save.EncryptDecrypt(json);
        saveFile = JsonConvert.DeserializeObject<SaveFile>(json);
        //init hexagons
        for (int i = 0; i < saveFile.hexagons.Count; i++)
        {
            HexSave hex = saveFile.hexagons[i];
            Grid.AddHexagon((HexagonTypes)hex.type, hex.x, hex.y, hex.yPos, hex.index, hex.resources.ToArray(), hex.enemies.ToArray());
        }

        //init player
        GameObject player = GameObject.Instantiate(GameManager.instance.playerPrefab, saveFile.player.position, Quaternion.Euler(0,saveFile.player.yRot, 0));
        //Player player = GameManager.playerInstance;
        player.GetComponent<Player>().Init(saveFile.player.position, saveFile.player.yRot, saveFile.player.level, saveFile.player.curExp, saveFile.player.curHealth, saveFile.player.inventory);
        //GameManager.playerInstance.transform.position = saveFile.player.position;
        //GameManager.playerInstance.transform.rotation = Quaternion.Euler(0, saveFile.player.yRot, 0);
        //GameManager.playerInstance.inventory.inventory = saveFile.player.inventory;

        //world setup
        GameManager.TimeOfDay = saveFile.TimeOfDay;
        GameManager.Day = saveFile.Day;
    }

    public static void LoadSettings(bool loaded = true)
    {
        if (CheckForSettings())
        {
            string json = System.IO.File.ReadAllText(Application.persistentDataPath + "/settings.afg");
            playerSettingsFile = JsonConvert.DeserializeObject<PlayerSettingsFile>(json);

            playerSettingsFile.inputs.LoadInputs();
            if(loaded)
                playerSettingsFile.LoadMenuPositions();
            playerSettingsFile.LoadSettings();
        }else
        {
            QualitySettings.SetQualityLevel(3);
            Screen.fullScreenMode = FullScreenMode.FullScreenWindow;
        }
    }

    public static bool CheckForSave()
    {

        if (System.IO.File.Exists(Application.persistentDataPath + "/save.afg"))
            return true;

        return false;
    }

    public static bool CheckForSettings()
    {
        if (System.IO.File.Exists(Application.persistentDataPath + "/settings.afg"))
            return true;

        return false;
    }
}
