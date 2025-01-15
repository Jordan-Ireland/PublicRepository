using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using TMPro;

public class UIManager : MonoBehaviour
{
    public static UIManager instance;

    //UI Components
    [Header("UI Vars")]
    public GameObject DateTime;
    public GameObject HealthExpBar;
    public GameObject Menu;
    public GameObject CraftingMenu;
    public GameObject InventoryMenu;
    public GameObject HandSlotMenu;
    public GameObject HexagonCraftingMenu;
    public HealthExpBarUI healthExpBarUI;
    public HandSlotUI[] HandSlots;
    public GameObject HandSelectionIcon;
    public ItemDetailsPanelUI ItemDetailsPanel;
    public GameObject DeletionMenu;

    //Imenu Components
    IMenu craftingMenu;
    IMenu menuMenu;
    IMenu inventoryMenu;
    IMenu hexagonMenu;
    IMenu deletionMenu;

    //booleans for UI 
    [Header("Menu bools")]
    public bool showInventory = false;
    public bool showBuildMenu = false;
    public bool showMenu = false;
    public bool showCraftingMenu = false;
    public bool moveCamera = true;

    [Header("Tutorial Vars")]
    public GameObject TutorialPanel;
    public TextMeshProUGUI tutorialText;

    private void Awake()
    {
        instance = this;
    }

    // Start is called before the first frame update
    void Start()
    {
        craftingMenu = CraftingMenu.GetComponent<IMenu>();
        menuMenu = Menu.GetComponent<IMenu>();
        inventoryMenu = InventoryMenu.GetComponent<IMenu>();
        hexagonMenu = HexagonCraftingMenu.GetComponent<IMenu>();
        deletionMenu = DeletionMenu.GetComponent<IMenu>();

        craftingMenu.Close();
        menuMenu.Close();
        inventoryMenu.Close();
        craftingMenu.Close();
        hexagonMenu.Close();
        deletionMenu.Close();

        ItemDetailsPanel.gameObject.SetActive(false);

        UpdateHandSlots();
        UpdateSelectionIcon(GameManager.playerInstance.currentItemBar);

        Load.LoadSettings();
    }

    public void OpenDeletionMenu()
    {
        deletionMenu.Open();
    }

    public bool IsDeletionMenuOpen()
    {
        if (DeletionMenu.activeSelf)
            return true;
        else
            return false;
    }

    // Update is called once per frame
    void Update()
    {
        if (!showMenu)
        {
            if (Input.GetKeyDown(InputManager.Inventory))
            {
                if (showInventory)
                    inventoryMenu.Close();
                else
                    inventoryMenu.Open();
            }
            if (Input.GetKeyDown(InputManager.CraftingMenu) && GameManager.gameState != GameManager.GameState.Building)
            {
                //showCraftingMenu = !showCraftingMenu;
                if (showCraftingMenu)
                    craftingMenu.Close();
                else
                    craftingMenu.Open();
            }
        }
        if(GameManager.gameState == GameManager.GameState.Building)
        {
            HealthExpBar.SetActive(false);
            //DateTime.SetActive(false);
            HandSlotMenu.SetActive(false);

        }else
        {
            HealthExpBar.SetActive(true);
            //DateTime.SetActive(true);
            HandSlotMenu.SetActive(true);
        }
        if (Input.GetKeyDown(InputManager.Menu))
        {
            //showMenu = !showMenu;
            inventoryMenu.Close();
            craftingMenu.Close();
            
            if (showMenu)
                menuMenu.Close();
            else
                menuMenu.Open();
        }
    }

    public void ChangeBuildMenu()
    {
        if (showBuildMenu)
            hexagonMenu.Close();
        else
            hexagonMenu.Open();
    }

    private void OnApplicationQuit()
    {
        Save.SavePlayerSettings();
        Save.SaveGame();
    }

    public void UpdateSelectionIcon(int index)
    {
        if(index < 3)
            HandSelectionIcon.GetComponent<RectTransform>().anchoredPosition = new Vector3(-195 + (index * 60), 0, 0);
        else
            HandSelectionIcon.GetComponent<RectTransform>().anchoredPosition = new Vector3(75 + ((index - 3) * 60), 0, 0);
    }

    public void UpdateHandSlots()
    {
        for (int i = 0; i < HandSlots.Length; i++)
        {
            Item item = GameManager.playerInstance.inventory.GetByIndex(i + Player.numSlots);
            if (item.Id != -1)
            {
                //set image to the icon
                HandSlots[i].UpdateUI(item.Icon, GameManager.playerInstance.inventory.GetAmountByIndex(i + Player.numSlots));
            }else
            {
                //set image to null & alpha to 0
                HandSlots[i].UpdateUI(null, -1);
            }
        }
    }

    public void OpenCrafting()
    {
        craftingMenu.Open();
    }

    public void CloseMenu()
    {
        showMenu = false;
        Menu.GetComponent<MenuUI>().Reset();
        Menu.SetActive(false);
    }

    public static bool ShowingMenu()
    {
        if (instance.showInventory)
            return true;
        if (instance.showBuildMenu)
            return true;
        if (instance.showMenu)
            return true;
        if (instance.showCraftingMenu)
            return true;

        return false;
    }
}
